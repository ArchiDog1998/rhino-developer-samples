#include "stdafx.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleDimStyleTextHeight command
//

#pragma region SampleDimStyleTextHeight command

class CCommandSampleDimStyleTextHeight : public CRhinoCommand
{
public:
  CCommandSampleDimStyleTextHeight() = default;
  ~CCommandSampleDimStyleTextHeight() = default;
  UUID CommandUUID() override
  {
    // {905F3A7A-518-442A-9637-74D407528DCE}
    static const GUID SampleDimStyleTextHeightCommand_UUID =
    { 0x905F3A7A, 0x518, 0x442A, { 0x96, 0x37, 0x74, 0xD4, 0x07, 0x52, 0x8D, 0xCE } };
    return SampleDimStyleTextHeightCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleDimStyleTextHeight"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSampleDimStyleTextHeight object
static class CCommandSampleDimStyleTextHeight theSampleDimStyleTextHeightCommand;

CRhinoCommand::result CCommandSampleDimStyleTextHeight::RunCommand(const CRhinoCommandContext& context)
{
  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  // Get the current dimstyle
  const CRhinoDimStyle& dimstyle = doc->m_dimstyle_table.CurrentDimStyle();

  ON_wString prompt;
  prompt.Format(L"New text height for \"%ls\" dimension style", static_cast<const wchar_t*>(dimstyle.Name()));

  // Prompt for a new text height value
  CRhinoGetNumber gn;
  gn.SetCommandPrompt(prompt);
  gn.SetDefaultNumber(dimstyle.TextHeight());
  gn.SetLowerLimit(0.0, TRUE);
  gn.AcceptNothing();
  gn.GetNumber();
  if (gn.CommandResult() != CRhinoCommand::success)
    return gn.CommandResult();

  // New text height value
  double height = gn.Number();

  // Validate new value
  if (height != dimstyle.TextHeight() && ON_IsValid(height) && height > ON_SQRT_EPSILON)
  {
    int style_index = dimstyle.Index();

    // Copy the dimstyle
    ON_DimStyle modified_dimstyle(doc->m_dimstyle_table[style_index]);

    // Modify the text height
    modified_dimstyle.SetTextHeight(height);

    // Modify the dimension style
    if (doc->m_dimstyle_table.ModifyDimStyle(modified_dimstyle, style_index))
      doc->Redraw();
  }

  return CRhinoCommand::success;
}

#pragma endregion

//
// END SampleDimStyleTextHeight command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
