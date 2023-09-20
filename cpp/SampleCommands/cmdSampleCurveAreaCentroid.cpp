#include "stdafx.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleCurveAreaCentroid command
//

#pragma region SampleCurveAreaCentroid command

class CGetClosedPlanarCurve : public CRhinoGetObject
{
public:
  bool CustomGeometryFilter(const CRhinoObject* obj, const ON_Geometry* geom, ON_COMPONENT_INDEX ci) const;
};

bool CGetClosedPlanarCurve::CustomGeometryFilter(const CRhinoObject* obj, const ON_Geometry* geom, ON_COMPONENT_INDEX ci) const
{
  UNREFERENCED_PARAMETER(obj);
  UNREFERENCED_PARAMETER(ci);

  bool rc = false;
  const ON_Curve* crv = ON_Curve::Cast(geom);
  if (crv && crv->IsClosed() && crv->IsPlanar(0, ON_ZERO_TOLERANCE))
    rc = true;
  return rc;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

class CCommandSampleCurveAreaCentroid : public CRhinoCommand
{
public:
  CCommandSampleCurveAreaCentroid() = default;
  ~CCommandSampleCurveAreaCentroid() = default;
  UUID CommandUUID() override
  {
    // {5B5B5B5A-C7AB-4B4B-AF6C-2F64D64AC135}
    static const GUID SampleCurveAreaCentroidCommand_UUID =
    { 0x5B5B5B5A, 0xC7AB, 0x4B4B, { 0xAF, 0x6C, 0x2F, 0x64, 0xD6, 0x4A, 0xC1, 0x35 } };
    return SampleCurveAreaCentroidCommand_UUID;
  }
  const wchar_t* EnglishCommandName() override { return L"SampleCurveAreaCentroid"; }
  CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override ;
};

// The one and only CCommandSampleCurveAreaCentroid object
static class CCommandSampleCurveAreaCentroid theSampleCurveAreaCentroidCommand;

static int RhinoClosedCurveOrientation(const ON_Curve& curve, const ON_Plane& plane)
{
  ON_Xform xform(plane.Origin(), plane.Xaxis(), plane.Yaxis(), plane.Normal());
  return ON_ClosedCurveOrientation(curve, &xform);
}

CRhinoCommand::result CCommandSampleCurveAreaCentroid::RunCommand(const CRhinoCommandContext& context)
{
  CRhinoDoc* doc = context.Document();
  if (nullptr == doc)
    return CRhinoCommand::failure;

  CGetClosedPlanarCurve go;
  go.SetCommandPrompt(L"Select closed, planar curves for centroid calculation");
  go.EnableSubObjectSelect(FALSE);
  go.GetObjects(1, 0);
  if (go.CommandResult() != CRhinoCommand::success)
    return go.CommandResult();

  ON_SimpleArray<const ON_Curve*> curves(go.ObjectCount());
  for (int i = 0; i < go.ObjectCount(); i++)
  {
    const ON_Curve* crv = go.Object(i).Curve();
    if (crv)
      curves.Append(crv);
  }

  if (0 == curves.Count())
    return CRhinoCommand::nothing;

  ON_MassProperties total_mp;
  for (int i = 0; i < curves.Count(); i++)
  {
    const ON_Curve* crv = curves[i];
    if (crv)
    {
      ON_Plane plane;
      if (crv->IsPlanar(&plane, ON_ZERO_TOLERANCE))
      {
        // Check the orienation of the curve and flip it's plane if necessary
        if (RhinoClosedCurveOrientation(*crv, plane) < 0)
          plane.Flip();

        ON_BoundingBox bbox = crv->BoundingBox();
        ON_3dPoint point = plane.ClosestPointTo(bbox.Center());

        ON_MassProperties mp;
        if (crv->AreaMassProperties(point, plane.Normal(), mp))
          total_mp.Sum(1, &mp, true);
      }
    }
  }

  ON_3dPoint centroid = total_mp.Centroid();
  doc->AddPointObject(centroid);
  doc->Redraw();

  ON_wString str;
  RhinoFormatPoint(centroid, str, L"%g");

  if (1 == curves.Count())
    RhinoApp().Print(L"Area Centroid = %ls\n", static_cast<const wchar_t*>(str));
  else
    RhinoApp().Print(L"Cumulative Area Centroid = %ls for %d planar curves\n", static_cast<const wchar_t*>(str), curves.Count());

  return CRhinoCommand::success;
}

#pragma endregion

//
// END SampleCurveAreaCentroid command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
