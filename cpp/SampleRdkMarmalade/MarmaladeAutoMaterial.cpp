
#include "stdafx.h"
#include "MarmaladeAutoMaterial.h"
#include "MarmaladePlugIn.h"
#include "MarmaladeColorSection.h"

// Automatic materials such as Orange, Satin and Lemon Curd are based on the various shaders
// and use the RDK Automatic UI instead of a hard-coded UI. This makes it possible to use the
// same material code for many different shaders without having to produce a UI for each one.

static const wchar_t* wszColor          = L"color";
static const wchar_t* wszTransparency   = L"transparency";
static const wchar_t* wszTextureOn      = L"color-on";
static const wchar_t* wszTextureAmount  = L"color-amount";
static const wchar_t* wszColorChildSlot = wszColor;

//---- CMarmaladeAutoMaterialFactory

CMarmaladeAutoMaterialFactory::CMarmaladeAutoMaterialFactory(const UUID& uuidShader)
	:
	m_uuid(uuidShader)
{
}

CRhRdkMaterial* CMarmaladeAutoMaterialFactory::NewMaterial(void) const
{
	return new CMarmaladeAutoMaterial(m_uuid);
}

//---- CMarmaladeAutoMaterial

CMarmaladeAutoMaterial::CMarmaladeAutoMaterial(const UUID& uuidShader)
{
	m_pShader = Shaders().FindShader(uuidShader);

	ASSERT(nullptr != m_pShader);
}

bool CMarmaladeAutoMaterial::Initialize(void)
{
	m_ParamBlock = m_pShader->ParamBlock();

	return CRhRdkContent::Initialize();
}

UUID CMarmaladeAutoMaterial::RenderEngineId(void) const
{
	return MarmaladePlugIn().PlugInID();
}

UUID CMarmaladeAutoMaterial::PlugInId(void) const
{
	return MarmaladePlugIn().PlugInID();
}

UUID CMarmaladeAutoMaterial::TypeId(void) const
{
	return m_pShader->Uuid();
}

ON_wString CMarmaladeAutoMaterial::TypeName(void) const
{
	return m_pShader->FriendlyName();
}

ON_wString CMarmaladeAutoMaterial::TypeDescription(void) const
{
	return m_pShader->Description();
}

ON_wString CMarmaladeAutoMaterial::InternalName(void) const
{
	return m_pShader->Name();
}

bool CMarmaladeAutoMaterial::TextureOn(void) const
{
	const auto* pParam = m_ParamBlock.FindParameter(wszTextureOn);
	if (nullptr == pParam)
		return false;

	return pParam->m_vValue;
}

void CMarmaladeAutoMaterial::SetTextureOn(bool b)
{
	auto* pParam = m_ParamBlock.FindParameter(wszTextureOn);
	if (nullptr == pParam)
		return;

	if (pParam->m_vValue.AsBool() != b)
	{
		pParam->m_vValue = b;
		Changed();
	}
}

double CMarmaladeAutoMaterial::TextureAmount(void) const
{
	const auto* pParam = m_ParamBlock.FindParameter(wszTextureAmount);
	if (nullptr == pParam)
		return 0.0;

	return pParam->m_vValue;
}

void CMarmaladeAutoMaterial::SetTextureAmount(double d)
{
	auto* pParam = m_ParamBlock.FindParameter(wszTextureAmount);
	if (nullptr == pParam)
		return;

	if (pParam->m_vValue.AsDouble() != d)
	{
		pParam->m_vValue = d;
		Changed();
	}
}

void CMarmaladeAutoMaterial::SimulateMaterial(ON_Material& mat, CRhRdkTexture::TextureGeneration tg, int iSimulatedTextureSize, const CRhinoObject* pObject) const
{
	const auto* pParam = m_ParamBlock.FindParameter(L"color");
	if (nullptr != pParam)
	{
		mat.SetDiffuse(pParam->m_vValue.AsRdkColor().ColorRefAlpha());
	}

	pParam = m_ParamBlock.FindParameter(L"transparency");
	if (nullptr != pParam)
	{
		mat.SetTransparency(pParam->m_vValue);
	}

	const CRhRdkContent* pChild = nullptr;
	if (TextureOn() && (TextureAmount() > 0.0) && (nullptr != (pChild = FindChild(wszColorChildSlot))))
	{
		// IsFactoryProductAcceptableAsChild should ensure that the child is a texture.
		const auto* pTexture = dynamic_cast<const CRhRdkTexture*>(pChild);
		if (nullptr != pTexture)
		{
			CRhRdkSimulatedTexture onTexture(DocumentAssoc());
			pTexture->SimulateTexture(onTexture, tg);
			mat.AddTexture(onTexture.Filename(), ON_Texture::TYPE::bitmap_texture);

			if (1 == mat.m_textures.Count())
			{
				ON_Texture& tex = mat.m_textures[0];
				tex.m_bOn = TextureOn();
				tex.m_magfilter = tex.m_minfilter = ON_Texture::FILTER::linear_filter;
				tex.m_blend_constant_A = TextureAmount();
				tex.m_mode = (tex.m_blend_constant_A < 1.0) ? ON_Texture::MODE::blend_texture : ON_Texture::MODE::decal_texture;
			}
		}
	}
}

static const int idParameters1 = 0;
static const int idParameters2 = 1;

void CMarmaladeAutoMaterial::AddUISections(IRhRdkExpandableContentUI& ui)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ui.AddSection(new CMarmaladeColorSection);

	AddAutomaticUISection(ui, L"Parameters 1", L"Parameters 1", idParameters1);
	AddAutomaticUISection(ui, L"Parameters 2", L"Parameters 2", idParameters2);

	CRhRdkMaterial::AddUISections(ui);
}

void CMarmaladeAutoMaterial::AddAutoParameters(IRhRdkParamBlock& paramBlock, int iId) const
{
	for (int i = 0; i < m_ParamBlock.ParameterCount(); i++)
	{
		const auto* p = m_ParamBlock.Parameter(i);

		const bool bColor = (0 == p->m_sName.CompareNoCase(wszColor));

		if ( bColor && (iId == CRhRdkCoreContent::TextureSummary) ||
			 bColor && (iId == idParameters1) ||
			!bColor && (iId == idParameters2))
		{
			if (bColor && IsTexturingSupported())
			{
				paramBlock.Add(p->m_sName, wszColorChildSlot, p->m_sFriendlyName, p->m_vValue,
							   p->m_vMin, p->m_vMax, IRhRdkParamBlock::UiHints::None, RDK_TEXTURING, this);
			}
			else
			{
				paramBlock.Add(p->m_sName, L"", p->m_sFriendlyName, p->m_vValue,
							   p->m_vMin, p->m_vMax, IRhRdkParamBlock::UiHints::None, RDK_DECIMAL_PLACES, this);
			}
		}
	}
}

void CMarmaladeAutoMaterial::GetAutoParameters(const IRhRdkParamBlock& paramBlock, int iId)
{
	// Because the values are stored in a param block (not fields)
	// it is important to detect when a value changes and call Changed().
	// If this is not done, the material preview will not update.

	auto* pIterator = paramBlock.NewIterator();

	ON_wString sParamName;
	CRhRdkVariant vParamValue;

	while (pIterator->Next(sParamName, vParamValue))
	{
		auto* pParam = m_ParamBlock.FindParameter(sParamName);
		if (nullptr != pParam)
		{
			if (pParam->m_vValue != vParamValue)
			{
				pParam->m_vValue = vParamValue;
				Changed();
			}
		}
	}

	pIterator->DeleteThis();
}

bool CMarmaladeAutoMaterial::WriteParametersToSection(IRhRdk_XMLSection& section, WriteParamsContext context) const
{
	for (int i = 0; i < m_ParamBlock.ParameterCount(); i++)
	{
		const auto* pParam = m_ParamBlock.Parameter(i);
		section.SetParam(pParam->m_sName, pParam->m_vValue);
	}

	return true;
}

bool CMarmaladeAutoMaterial::ReadParametersFromSection(const IRhRdk_XMLSection& section, ReadParamsContext context)
{
	auto* pIterator = section.NewIterator();

	ON_wString sParamName;
	CRhRdkVariant vParamValue;

	while (section.NextParam(pIterator, sParamName, vParamValue))
	{
		auto* pParam = m_ParamBlock.FindParameter(sParamName);
		if (nullptr != pParam)
		{
			pParam->m_vValue = vParamValue;
		}
	}

	delete pIterator;

	return true;
}

CRhRdkVariant CMarmaladeAutoMaterial::GetParameter(const wchar_t* wszName) const
{
	const auto* pParam = m_ParamBlock.FindParameter(wszName);
	if (nullptr == pParam)
	{
		return CRhRdkMaterial::GetParameter(wszName);
	}

	return pParam->m_vValue;
}

bool CMarmaladeAutoMaterial::SetParameter(const wchar_t* wszName, const CRhRdkVariant& vValue)
{
	auto* pParam = m_ParamBlock.FindParameter(wszName);
	if (nullptr != pParam)
	{
		if (pParam->m_vValue != vValue)
		{
			pParam->m_vValue = vValue;

			Changed();
		}
	}

	return true;
}

void* CMarmaladeAutoMaterial::GetShader(const UUID& uuidRenderEngine, void* pvData) const
{
	if (!IsCompatible(uuidRenderEngine))
		return nullptr;

	auto* pShader = m_pShader->Clone();
	if (nullptr != pShader)
	{
		pShader->ParamBlock() = m_ParamBlock;
	}

	return reinterpret_cast<void*>(pShader);
}

bool CMarmaladeAutoMaterial::IsTexturingSupported(void) const
{
	return (nullptr != m_ParamBlock.FindParameter(wszColor)) &&
	       (nullptr != m_ParamBlock.FindParameter(wszTextureOn)) &&
	       (nullptr != m_ParamBlock.FindParameter(wszTextureAmount));
}

unsigned int CMarmaladeAutoMaterial::BitFlags(void) const
{
	auto flags = CRhRdkMaterial::BitFlags(); // | bfSharedUI; // Shared UI is now mandatory.

	if (!IsTexturingSupported())    // If texturing is not supported then
		flags &= ~bfTextureSummary; // no texture summary is required.

	return flags;
}

const wchar_t* CMarmaladeAutoMaterial::Category(void) const
{
	return RDK_CAT_GENERAL;
}

bool CMarmaladeAutoMaterial::IsFactoryProductAcceptableAsChild(const CRhRdkContentFactory& f, const wchar_t* wszChildSlotName) const
{
	if (f.Kind() == CRhRdkContent::Kinds::Texture)
		return true; // Factory produces textures.

	return false; // Factory produces something "unpalatable".
}

bool CMarmaladeAutoMaterial::GetExtraRequirementParameter(const wchar_t* wszParamName, const wchar_t* wszExtraRequirementName, CRhRdkVariant& vValueOut) const
{
	if (CRhRdkMaterial::GetExtraRequirementParameter(wszParamName, wszExtraRequirementName, vValueOut))
		return true;

	const ON_wString sParamName = wszParamName;

	const ON_wString sExtraRequirementName = wszExtraRequirementName;

	if (sExtraRequirementName == RDK_DECIMAL_PLACES_AMOUNT)
	{
		if (sParamName == wszTransparency)
		{
			vValueOut = 2;
		}
		else
		{
			vValueOut = 3;
		}

		return true;
	}

	// Handle texturing extra requirements for the color parameter.
	if (sParamName == wszColor)
	{
		if (sExtraRequirementName == RDK_TEXTURE_ON)
		{
			vValueOut = TextureOn();
			return true;
		}

		if (sExtraRequirementName == RDK_TEXTURE_AMOUNT)
		{
			vValueOut = TextureAmount() * 100.0;
			return true;
		}
	}

	return false;
}

bool CMarmaladeAutoMaterial::SetExtraRequirementParameter(const wchar_t* wszParamName,
                                                          const wchar_t* wszExtraRequirementName,
	                                                      const CRhRdkVariant& vValue, SetContext sc)
{
	const ON_wString sParamName = wszParamName;

	if (sParamName == wszColor)
	{
		const ON_wString sExtraRequirementName = wszExtraRequirementName;

		if (sExtraRequirementName == RDK_TEXTURE_ON)
		{
			SetTextureOn(vValue);
			return true;
		}

		if (sExtraRequirementName == RDK_TEXTURE_AMOUNT)
		{
			SetTextureAmount(vValue.AsDouble() * 0.01);
			return true;
		}
	}

	return false;
}

class CMarmaladeAutoMaterialCSI : public CRhRdkContent::CChildSlotIterator
{
public:
	CMarmaladeAutoMaterialCSI(const CMarmaladeAutoMaterial* pMaterial, CRhRdkContent::CSIContext context)
		: CRhRdkContent::CChildSlotIterator(pMaterial, context) { m_iIndex = -1; }

	virtual bool NextChildSlot(ON_wString& sInternalNameOut, ON_wString& sChildSlotNameOut, ON_wString& sDisplayNameOut);

private:
	int m_iIndex;
};

bool CMarmaladeAutoMaterialCSI::NextChildSlot(OUT ON_wString& sInternalName, OUT ON_wString& sChildSlotName, OUT ON_wString& sDisplayName)
{
	m_iIndex++;

	switch (m_iIndex)
	{
	case 0:
		sInternalName  = wszColor;
		sDisplayName   = L"Color";
		sChildSlotName = wszColorChildSlot;
		return true;
	}

	return false;
}

CRhRdkContent::CChildSlotIterator* CMarmaladeAutoMaterial::NewChildSlotIterator(CSIContext context) const
{
	if (!IsTexturingSupported())
		return nullptr;

	return new CMarmaladeAutoMaterialCSI(this, context);
}
