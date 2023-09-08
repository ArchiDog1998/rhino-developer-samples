#include "stdafx.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleScripter command
//

class CSampleScriptCancelHelper : public CRhinoEventWatcher
{
public:
  CSampleScriptCancelHelper();
  ~CSampleScriptCancelHelper();

  CRhinoCommand::result LastCommandResult() const;

public:
  void OnEndCommand(const CRhinoCommand& command, const CRhinoCommandContext& context, CRhinoCommand::result rc);

private:
  CRhinoCommand::result m_result;
};

CSampleScriptCancelHelper::CSampleScriptCancelHelper()
{
  m_result = CRhinoCommand::success;
  Register();
  Enable(TRUE);
}

CSampleScriptCancelHelper::~CSampleScriptCancelHelper()
{
  Enable(FALSE);
  UnRegister();
}

void CSampleScriptCancelHelper::OnEndCommand(const CRhinoCommand& command, const CRhinoCommandContext& context, CRhinoCommand::result rc)
{
  UNREFERENCED_PARAMETER(command);
  UNREFERENCED_PARAMETER(context);
  m_result = rc;
}

CRhinoCommand::result CSampleScriptCancelHelper::LastCommandResult() const
{
  return m_result;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

class CCommandSampleScripter : public CRhinoScriptCommand
{
public:
  CCommandSampleScripter() = default;
  ~CCommandSampleScripter() = default;
  UUID CommandUUID() override
  {
    // {D68FBC91-E0BC-470B-A31A-B3539F70E08E}
    static const GUID SampleScripterCommand_UUID =
    { 0xD68FBC91, 0xE0BC, 0x470B, { 0xA3, 0x1A, 0xB3, 0x53, 0x9F, 0x70, 0xE0, 0x8E } };
    return SampleScripterCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleScripter"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSampleScripter object
static class CCommandSampleScripter theSampleScripterCommand;

CRhinoCommand::result CCommandSampleScripter::RunCommand(const CRhinoCommandContext& context)
{
  UNREFERENCED_PARAMETER(context);

  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  CSampleScriptCancelHelper helper;

  RhinoApp().RunScript(doc->RuntimeSerialNumber(), L"_Line", 1);
  if (helper.LastCommandResult() != CRhinoCommand::success)
    return helper.LastCommandResult();

  RhinoApp().RunScript(doc->RuntimeSerialNumber(), L"_Circle", 1);
  if (helper.LastCommandResult() != CRhinoCommand::success)
    return helper.LastCommandResult();

  RhinoApp().RunScript(doc->RuntimeSerialNumber(), L"_Arc", 1);
  if (helper.LastCommandResult() != CRhinoCommand::success)
    return helper.LastCommandResult();

  return CRhinoCommand::success;
}

//
// END SampleScripter command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
