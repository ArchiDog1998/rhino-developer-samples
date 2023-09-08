#include "stdafx.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleDisplayPrecision command
//

class CCommandSampleDisplayPrecision : public CRhinoCommand
{
public:
  CCommandSampleDisplayPrecision() = default;
  ~CCommandSampleDisplayPrecision() = default;
  UUID CommandUUID() override
  {
    // {6E4FEF6B-C6A3-49C9-886D-A1337CE74EFD}
    static const GUID SampleDisplayPrecisionCommand_UUID =
    { 0x6E4FEF6B, 0xC6A3, 0x49C9, { 0x88, 0x6D, 0xA1, 0x33, 0x7C, 0xE7, 0x4E, 0xFD } };
    return SampleDisplayPrecisionCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleDisplayPrecision"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSampleDisplayPrecision object
static class CCommandSampleDisplayPrecision theSampleDisplayPrecisionCommand;

CRhinoCommand::result CCommandSampleDisplayPrecision::RunCommand(const CRhinoCommandContext& context)
{
  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  ON_3dmUnitsAndTolerances units = doc->Properties().ModelUnitsAndTolerances();

  CRhinoGetInteger gi;
  gi.SetCommandPrompt(L"New display precision");
  gi.SetDefaultInteger(units.m_distance_display_precision);
  gi.SetLowerLimit(0);
  gi.SetUpperLimit(6);
  gi.GetInteger();
  if (gi.CommandResult() == CRhinoCommand::success)
  {
    int distance_display_precision = gi.Number();
    if (distance_display_precision != units.m_distance_display_precision)
    {
      units.m_distance_display_precision = distance_display_precision;
      doc->Properties().SetModelUnitsAndTolerances(units, false);
    }
  }

  return CRhinoCommand::success;
}

//
// END SampleDisplayPrecision command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
