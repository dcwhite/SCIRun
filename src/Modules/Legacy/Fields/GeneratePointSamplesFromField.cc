/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2015 Scientific Computing and Imaging Institute,
   University of Utah.


   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
   */

///
///@file  GeneratePointSamplesFromField.cc
///
///@author
///   Robert Van Uitert
///   Diagnostic Radiology Department
///   National Institutes of Health
///@date  November 2004
///

#include <Modules/Legacy/Fields/GeneratePointSamplesFromField.h>
#include <Modules/Legacy/Fields/GenerateSinglePointProbeFromField.h>
#include <Core/Datatypes/Legacy/Field/Mesh.h>
#include <Core/Datatypes/Legacy/Field/VMesh.h>
#include <Core/Datatypes/Legacy/Field/VField.h>
#include <Core/Datatypes/Legacy/Field/Field.h>
#include <Core/Datatypes/Legacy/Field/FieldInformation.h>
#include <Core/GeometryPrimitives/Point.h>
#include <Core/GeometryPrimitives/BBox.h>
#include <Graphics/Widgets/SphereWidget.h>

using namespace SCIRun;
using namespace Core;
using namespace Datatypes;
using namespace Dataflow::Networks;
using namespace Modules::Fields;
using namespace Geometry;
using namespace Algorithms::Fields;
using namespace Graphics::Datatypes;

ALGORITHM_PARAMETER_DEF(Fields, NumSeeds);
ALGORITHM_PARAMETER_DEF(Fields, ProbeScale);

MODULE_INFO_DEF(GeneratePointSamplesFromField, NewField, SCIRun)

namespace SCIRun
{
  namespace Modules
  {
    namespace Fields
    {

      class GeneratePointSamplesFromFieldImpl
      {
      public:
        BBox last_bounds_;
        std::vector<size_t> widget_id_;
        std::vector<SphereWidgetHandle> pointWidgets_;
        double l2norm_;

        FieldHandle makePointCloud()
        {
          //when push send button--TODO: for now, always update seed mesh
          FieldInformation fi("PointCloudMesh", 1, "double");
          auto ofield = CreateField(fi);
          auto mesh = ofield->vmesh();
          auto field = ofield->vfield();

          for (int i = 0; i < pointWidgets_.size(); i++)
          {
            const Point location = pointWidgets_[i]->position();

            VMesh::Node::index_type pcindex = mesh->add_point(location);
            field->resize_fdata();
            field->set_value(static_cast<double>(i), pcindex);
          }
          return ofield;
        }
      };
    }
  }
}


GeneratePointSamplesFromField::GeneratePointSamplesFromField()
  : GeometryGeneratingModule(staticInfo_), impl_(new GeneratePointSamplesFromFieldImpl)
{
  INITIALIZE_PORT(InputField);
  INITIALIZE_PORT(GeneratedWidget);
  INITIALIZE_PORT(GeneratedPoints);
}

void GeneratePointSamplesFromField::setStateDefaults()
{
  auto state = get_state();
  state->setValue(Parameters::NumSeeds, 1);
  state->setValue(Parameters::ProbeScale, 0.23);
}

void GeneratePointSamplesFromField::execute()
{
  sendOutput(GeneratedPoints, GenerateOutputField());

  auto geom = WidgetFactory::createComposite(impl_->pointWidgets_.begin(), impl_->pointWidgets_.end());
  sendOutput(GeneratedWidget, geom);
}

FieldHandle GeneratePointSamplesFromField::GenerateOutputField()
{
  auto ifieldhandle = getRequiredInput(InputField);

  auto bbox = ifieldhandle->vmesh()->get_bounding_box();

  Point center;
  Point bmin = bbox.get_min();
  Point bmax = bbox.get_max();

  if (!bbox.is_similar_to(impl_->last_bounds_))
  {
    // Fix degenerate boxes.
    const double size_estimate = std::max((bmax - bmin).length() * 0.01, 1.0e-5);
    if (fabs(bmax.x() - bmin.x()) < 1.0e-6)
    {
      bmin.x(bmin.x() - size_estimate);
      bmax.x(bmax.x() + size_estimate);
    }
    if (fabs(bmax.y() - bmin.y()) < 1.0e-6)
    {
      bmin.y(bmin.y() - size_estimate);
      bmax.y(bmax.y() + size_estimate);
    }
    if (fabs(bmax.z() - bmin.z()) < 1.0e-6)
    {
      bmin.z(bmin.z() - size_estimate);
      bmax.z(bmax.z() + size_estimate);
    }

    center = bmin + Vector(bmax - bmin) * 0.5;
    impl_->l2norm_ = (bmax - bmin).length();
    impl_->last_bounds_ = bbox;
  }

  auto state = get_state();
  auto numSeeds = state->getValue(Parameters::NumSeeds).toInt();
  auto scale = state->getValue(Parameters::ProbeScale).toDouble();

  if (impl_->widget_id_.size() != numSeeds)
  {
    if (numSeeds < impl_->widget_id_.size())
    {
      // remove current composite widget
      impl_->widget_id_.resize(numSeeds);
      impl_->pointWidgets_.resize(numSeeds);
    }
    else
    {
      for (size_t i = impl_->widget_id_.size(); i < numSeeds; i++)
      {
        auto seed = boost::dynamic_pointer_cast<SphereWidget>(WidgetFactory::createSphere(*this,
          scale * impl_->l2norm_ * 0.003, "Color(0.5,0.5,0.5)", center, bbox));
        impl_->pointWidgets_.push_back(seed);
        impl_->widget_id_.push_back(i);
        //seed->setPosition(center);
        //seed->setScale(scale * impl_->l2norm_ * 0.003);
      }
    }
  }

  // for (auto& point : impl_->pointWidgets_)
  // {
  //   point->setScale(scale * impl_->l2norm_ * 0.003);
  // }

  return impl_->makePointCloud();
}
