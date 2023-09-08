#include "StdAfx.h"
#include "SampleOptionsListCtrlPageDialog.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleOptionsListCtrl command
//

#pragma region SampleOptionsListCtrl command

class CCommandSampleOptionsListCtrl : public CRhinoCommand
{
public:
  CCommandSampleOptionsListCtrl() = default;
  ~CCommandSampleOptionsListCtrl() = default;
  UUID CommandUUID() override
  {
    // {2539983D-243E-49AE-8558-CDE83DF4485F}
    static const GUID SampleOptionsListCtrlCommand_UUID =
    { 0x2539983D, 0x243E, 0x49AE,{ 0x85, 0x58, 0xCD, 0xE8, 0x3D, 0xF4, 0x48, 0x5F } };
    return SampleOptionsListCtrlCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleOptionsListCtrl"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext&);
};

static class CCommandSampleOptionsListCtrl theSampleOptionsListCtrlCommand;

CRhinoCommand::result CCommandSampleOptionsListCtrl::RunCommand(const CRhinoCommandContext& context)
{
  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  ON_UUID tabId = CSampleOptionsListCtrlPageDialog::ID();

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
// END SampleOptionsListCtrl command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
