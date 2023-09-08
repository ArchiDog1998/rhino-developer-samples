#include "stdafx.h"
#include "SamplePickObjectsDialog.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SamplePickObjects command
//

#pragma region SamplePickObjects command

class CCommandSamplePickObjects : public CRhinoCommand
{
public:
  CCommandSamplePickObjects() = default;
  ~CCommandSamplePickObjects() = default;
  UUID CommandUUID() override
  {
    // {C1E761F2-BED6-44CC-8D2D-5AD9EEF2AB81}
    static const GUID SamplePickObjectsCommand_UUID =
    { 0xC1E761F2, 0xBED6, 0x44CC, { 0x8D, 0x2D, 0x5A, 0xD9, 0xEE, 0xF2, 0xAB, 0x81 } };
    return SamplePickObjectsCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SamplePickObjects"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSamplePickObjects object
static class CCommandSamplePickObjects theSamplePickObjectsCommand;

CRhinoCommand::result CCommandSamplePickObjects::RunCommand(const CRhinoCommandContext& context)
{
  CWnd* pParentWnd = CWnd::FromHandle(RhinoApp().MainWnd());
  CSamplePickObjectsDialog dialog(pParentWnd, context.m_rhino_doc_sn);
  dialog.DoModal();
  return CRhinoCommand::success;
}

#pragma endregion

//
// END SamplePickObjects command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
