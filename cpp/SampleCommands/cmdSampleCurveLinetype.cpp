#include "stdafx.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleCurveLinetype command
//

#pragma region SampleCurveLinetype command

class CCommandSampleCurveLinetype : public CRhinoCommand
{
public:
  CCommandSampleCurveLinetype() = default;
  ~CCommandSampleCurveLinetype() = default;
  UUID CommandUUID() override
  {
    // {24668EBF-7D79-45BE-BA4D-81863B306B2F}
    static const GUID SampleCurveLinetypeCommand_UUID =
    { 0x24668EBF, 0x7D79, 0x45BE, { 0xBA, 0x4D, 0x81, 0x86, 0x3B, 0x30, 0x6B, 0x2F } };
    return SampleCurveLinetypeCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleCurveLinetype"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSampleCurveLinetype object
static class CCommandSampleCurveLinetype theSampleCurveLinetypeCommand;

CRhinoCommand::result CCommandSampleCurveLinetype::RunCommand(const CRhinoCommandContext& context)
{
  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  CRhinoGetObject go;
  go.SetCommandPrompt(L"Select curve to change linetype");
  go.SetGeometryFilter(CRhinoGetObject::curve_object);
  go.EnableSubObjectSelect(FALSE);
  go.GetObjects(1, 1);
  if (go.CommandResult() != CRhinoCommand::success)
    return go.CommandResult();

  const CRhinoObjRef& obj_ref = go.Object(0);
  const CRhinoObject* obj = obj_ref.Object();
  if (0 == obj)
    return CRhinoCommand::failure;

  const int bylayer_linetype_index = -2;

  int old_linetype_index = obj->Attributes().m_linetype_index;
  ON::object_linetype_source linetype_source = obj->Attributes().LinetypeSource();
  if (linetype_source == ON::linetype_from_layer)
    old_linetype_index = bylayer_linetype_index;

  int new_linetype_index = old_linetype_index;

  BOOL rc = RhinoLinetypeDialog(RhinoApp().MainWnd(), new_linetype_index, true);
  if (rc && new_linetype_index != old_linetype_index)
  {
    CRhinoObjectAttributes new_attributes = obj->Attributes();
    if (new_linetype_index == bylayer_linetype_index)
      new_attributes.SetLinetypeSource(ON::linetype_from_layer);
    else
    {
      new_attributes.m_linetype_index = new_linetype_index;
      new_attributes.SetLinetypeSource(ON::linetype_from_object);
    }

    doc->ModifyObjectAttributes(obj_ref, new_attributes);
    doc->Redraw();
  }

  return CRhinoCommand::success;
}

#pragma endregion

//
// END SampleCurveLinetype command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
