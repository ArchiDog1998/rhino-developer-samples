#include "stdafx.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleLeader command
//

class CCommandSampleLeader : public CRhinoCommand
{
public:
  CCommandSampleLeader() = default;
  ~CCommandSampleLeader() = default;
  UUID CommandUUID() override
  {
    // {7D5E43E9-D737-49CB-BD44-39E1C6957B54}
    static const GUID SampleLeaderCommand_UUID =
    { 0x7D5E43E9, 0xD737, 0x49CB, { 0xBD, 0x44, 0x39, 0xE1, 0xC6, 0x95, 0x7B, 0x54 } };
    return SampleLeaderCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleLeader"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSampleLeader object
static class CCommandSampleLeader theSampleLeaderCommand;

CRhinoCommand::result CCommandSampleLeader::RunCommand(const CRhinoCommandContext& context)
{
  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  ON_3dPointArray points;
  points.Append(ON_3dPoint(1.0, 1.0, 0.0));
  points.Append(ON_3dPoint(5.0, 1.0, 0.0));
  points.Append(ON_3dPoint(5.0, 5.0, 0.0));
  points.Append(ON_3dPoint(9.0, 5.0, 0.0));

  const CRhinoLeader* dim_obj = doc->AddLeaderObject(
    L"Leader",
    ON_Plane::World_xy,
    points.Count(),
    points.Array()
  );

  if (nullptr != dim_obj)
    doc->Redraw();

  return CRhinoCommand::success;
}

//
// END SampleLeader command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
