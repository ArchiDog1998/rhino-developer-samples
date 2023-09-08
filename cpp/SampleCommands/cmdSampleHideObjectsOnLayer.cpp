#include "stdafx.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleHideObjectsOnLayer command
//

#pragma region SampleHideObjectsOnLayer command

class CCommandSampleHideObjectsOnLayer : public CRhinoCommand
{
public:
  CCommandSampleHideObjectsOnLayer() = default;
  ~CCommandSampleHideObjectsOnLayer() = default;
  UUID CommandUUID() override
  {
    // {4326C435-2835-4C0D-BDE9-2C80D632E113}
    static const GUID SampleHideObjectsOnLayerCommand_UUID =
    { 0x4326C435, 0x2835, 0x4C0D,{ 0xBD, 0xE9, 0x2C, 0x80, 0xD6, 0x32, 0xE1, 0x13 } };
    return SampleHideObjectsOnLayerCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleHideObjectsOnLayer"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSampleHideObjectsOnLayer object
static class CCommandSampleHideObjectsOnLayer theSampleHideObjectsOnLayerCommand;

CRhinoCommand::result CCommandSampleHideObjectsOnLayer::RunCommand(const CRhinoCommandContext& context)
{
  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  const CRhinoLayerTable& layer_table = doc->m_layer_table;
  const ON_wString& current_layer_name = layer_table.CurrentLayer().Name();

  CRhinoGetString gs;
  gs.SetCommandPrompt(L"Layer to hide objects");
  gs.SetDefaultString(current_layer_name);
  gs.GetString();
  if (gs.CommandResult() != CRhinoCommand::success)
    return gs.CommandResult();

  ON_wString layer_name = gs.String();
  layer_name.TrimLeftAndRight();
  if (layer_name.IsEmpty())
    return CRhinoCommand::nothing;

  const int layer_index = layer_table.FindLayerFromFullPathName(layer_name, -1);
  if (layer_index < 0)
  {
    RhinoApp().Print(L"The \"%ls\" layer was not found.\n", static_cast<const wchar_t*>(layer_name));
    return CRhinoCommand::nothing;
  }

  const CRhinoLayer& layer = layer_table[layer_index];
  ON_SimpleArray<CRhinoObject*> objects;
  const int object_count = doc->LookupObject(layer, objects);

  for (int i = 0; i < object_count; i++)
  {
    const CRhinoObject* obj = objects[i];
    if (obj && obj->IsVisible())
      doc->HideObject(CRhinoObjRef(obj));
  }

  doc->Redraw();

  return CRhinoCommand::success;
}

#pragma endregion

//
// END SampleHideObjectsOnLayer command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
