#include "stdafx.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleBoundingBoxAll command
//

#pragma region SampleBoundingBoxAll command

class CCommandSampleBoundingBoxAll : public CRhinoCommand
{
public:
  CCommandSampleBoundingBoxAll() = default;
  ~CCommandSampleBoundingBoxAll() = default;
  UUID CommandUUID() override
  {
    // {5B3D320B-DD65-4B8B-A95A-96AE9B7B51E7}
    static const GUID SampleBoundingBoxAllCommand_UUID =
    { 0x5B3D320B, 0xDD65, 0x4B8B,{ 0xA9, 0x5A, 0x96, 0xAE, 0x9B, 0x7B, 0x51, 0xE7 } };
    return SampleBoundingBoxAllCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleBoundingBoxAll"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSampleBoundingBoxAll object
static class CCommandSampleBoundingBoxAll theSampleBoundingBoxAllCommand;

CRhinoCommand::result CCommandSampleBoundingBoxAll::RunCommand(const CRhinoCommandContext& context)
{
  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  ON_SimpleArray<const CRhinoObject*> objects;

  CRhinoObjectIterator it(*doc, CRhinoObjectIterator::all_objects);
  const CRhinoObject* obj = 0;
  for (obj = it.First(); 0 != obj; obj = it.Next())
    objects.Append(obj);

  ON_BoundingBox bbox;
  RhinoGetTightBoundingBox(objects, bbox);

  ON_wString str0, str1;
  ON_wString dimx, dimy, dimz;

  RhinoFormatPoint(bbox.m_min, str0);
  RhinoFormatPoint(bbox.m_max, str1);
  ON_3dVector dir(bbox.m_max - bbox.m_min);

  ON_3dmUnitsAndTolerances doc_units = (ON::page_space == RhinoApp().ActiveSpace()) ?
    doc->Properties().PageUnitsAndTolerances() :
    doc->Properties().ModelUnitsAndTolerances();

  ON_3dmUnitsAndTolerances display_units = doc_units;
  RhinoFormatNumber(dir.x, doc_units, display_units, dimx, false);
  RhinoFormatNumber(dir.y, doc_units, display_units, dimy, false);
  RhinoFormatNumber(dir.z, doc_units, display_units, dimz, true);

  RhinoApp().Print(L"World coordinates:\n");
  RhinoApp().Print(L"min = %ls\n", static_cast<const wchar_t*>(str0));
  RhinoApp().Print(L"max = %ls\n", static_cast<const wchar_t*>(str1));
  RhinoApp().Print(L"dimensions = %ls, %ls, %ls\n", 
    static_cast<const wchar_t*>(dimx),
    static_cast<const wchar_t*>(dimy),
    static_cast<const wchar_t*>(dimz)
  );

  return CRhinoCommand::success;
}

#pragma endregion

//
// END SampleBoundingBoxAll command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
