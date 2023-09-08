#include "StdAfx.h"
#include "SampleTabbedDockBarDialog.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleTabbedDockBar command
//

#pragma region SampleTabbedDockBar command

class CCommandSampleTabbedDockBar : public CRhinoCommand
{
public:
  CCommandSampleTabbedDockBar() = default;
  ~CCommandSampleTabbedDockBar() = default;
  UUID CommandUUID() override
  {
    // {241B6E5D-ADEA-4FF0-90F6-2CF14B49BDF4}
    static const GUID SampleTabbedDockBarCommand_UUID =
    { 0x241B6E5D, 0xADEA, 0x4FF0, { 0x90, 0xF6, 0x2C, 0xF1, 0x4B, 0x49, 0xBD, 0xF4 } };
    return SampleTabbedDockBarCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleTabbedDockBar"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSampleTabbedDockBar object.  
static class CCommandSampleTabbedDockBar theSampleTabbedDockBarCommand;

CRhinoCommand::result CCommandSampleTabbedDockBar::RunCommand(const CRhinoCommandContext& context)
{
  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  ON_UUID tabId = CSampleTabbedDockBarDialog::ID();

  if (context.IsInteractive())
  {
    CRhinoTabbedDockBarDialog::OpenDockbarTab(*doc, tabId);
  }
  else
  {
    bool bVisible = CRhinoTabbedDockBarDialog::IsTabVisible(*doc, tabId);

    ON_wString str;
    str.Format(L"%ls is %ls. New value", LocalCommandName(), bVisible ? L"visible" : L"hidden");

    CRhinoGetOption go;
    go.SetCommandPrompt(str);
    int h_option = go.AddCommandOption(RHCMDOPTNAME(L"Hide"));
    int s_option = go.AddCommandOption(RHCMDOPTNAME(L"Show"));
    int t_option = go.AddCommandOption(RHCMDOPTNAME(L"Toggle"));
    go.GetOption();
    if (go.CommandResult() != CRhinoCommand::success)
      return go.CommandResult();

    const CRhinoCommandOption* option = go.Option();
    if (0 == option)
      return CRhinoCommand::failure;

    int option_index = option->m_option_index;

    if (h_option == option_index && bVisible)
      CRhinoTabbedDockBarDialog::ShowDockbarTab(*doc, tabId, false, false, nullptr);
    else if (s_option == option_index && !bVisible)
      CRhinoTabbedDockBarDialog::ShowDockbarTab(*doc, tabId, true, true, nullptr);
    else if (t_option == option_index)
      CRhinoTabbedDockBarDialog::ShowDockbarTab(*doc, tabId, !bVisible, !bVisible, nullptr);
  }

  return CRhinoCommand::success;
}

#pragma endregion

//
// END SampleTabbedDockBar command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
