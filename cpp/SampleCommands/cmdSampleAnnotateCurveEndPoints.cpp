#include "stdafx.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleAnnotateCurveEndPoints command
//

class CCommandSampleAnnotateCurveEndPoints : public CRhinoCommand
{
public:
  CCommandSampleAnnotateCurveEndPoints() = default;
  ~CCommandSampleAnnotateCurveEndPoints() = default;
  UUID CommandUUID() override
  {
    // {DAA00BF1-5763-4498-AD95-C1DFDD04122F}
    static const GUID SampleAnnotateCurveEndPointsCommand_UUID =
    { 0xDAA00BF1, 0x5763, 0x4498, { 0xAD, 0x95, 0xC1, 0xDF, 0xDD, 0x04, 0x12, 0x2F } };
    return SampleAnnotateCurveEndPointsCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleAnnotateCurveEndPoints"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSampleAnnotateCurveEndPoints object
static class CCommandSampleAnnotateCurveEndPoints theSampleAnnotateCurveEndPointsCommand;

CRhinoCommand::result CCommandSampleAnnotateCurveEndPoints::RunCommand(const CRhinoCommandContext& context)
{
  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  CRhinoGetObject go;
  go.SetCommandPrompt(L"Select curves to annotate");
  go.SetGeometryFilter(CRhinoGetObject::curve_object);
  if (go.GetObjects(1, 0) != CRhinoGet::object)
    return CRhinoCommand::cancel;

  for (int i = 0; i < go.ObjectCount(); i++)
  {
    const CRhinoObjRef& objref = go.Object(i);
    const ON_Curve* curve = objref.Curve();
    if (0 == curve)
      continue;

    ON_TextDot start_dot;
    ON_3dPoint start_pt = curve->PointAtStart();
    start_dot.SetCenterPoint(start_pt);

    ON_wString str;
    RhinoFormatPoint(start_pt, str);
    start_dot.SetPrimaryText(str);

    CRhinoTextDot* pStartDot = new CRhinoTextDot();
    pStartDot->SetDot(start_dot);
    doc->AddObject(pStartDot);

    if (curve->IsClosed())
      continue;

    ON_TextDot end_dot;
    ON_3dPoint end_pt = curve->PointAtEnd();
    end_dot.SetCenterPoint(end_pt);

    RhinoFormatPoint(end_pt, str);
    end_dot.SetPrimaryText(str);

    CRhinoTextDot* pEndDot = new CRhinoTextDot();
    pEndDot->SetDot(end_dot);

    doc->AddObject(pEndDot);
  }

  doc->Redraw();

  return CRhinoCommand::success;
}

//
// END SampleAnnotateCurveEndPoints command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
