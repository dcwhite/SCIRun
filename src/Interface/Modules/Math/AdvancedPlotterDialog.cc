/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2015 Scientific Computing and Imaging Institute,
   University of Utah.

   License for the specific language governing rights and limitations under
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

#include <Interface/Modules/Math/AdvancedPlotterDialog.h>
#include <Modules/Math/BasicPlotter.h>
#include <Modules/Math/AdvancedPlotter.h>
#include <Core/Datatypes/DenseMatrix.h>
#include <Core/Algorithms/Base/AlgorithmVariableNames.h>
#include <QtGui>

#ifndef Q_MOC_RUN
#include <Core/Utils/StringUtil.h>
#endif

using namespace SCIRun::Gui;
using namespace SCIRun::Dataflow::Networks;
using namespace SCIRun::Modules::Math;
using namespace SCIRun::Core::Algorithms;
using namespace SCIRun::Core::Algorithms::Math;
using namespace SCIRun::Core::Datatypes;

AdvancedPlotterDialog::AdvancedPlotterDialog(const std::string& name, ModuleStateHandle state,
	QWidget* parent/* = 0*/)
	: ModuleDialogGeneric(state, parent)
{
	setupUi(this);
	setWindowTitle(QString::fromStdString(name));
	fixSize();

	addDoubleSpinBoxManager(verticalAxisSpinBox_, Parameters::VerticalAxisPosition);
  addDoubleSpinBoxManager(horizontalAxisSpinBox_, Parameters::HorizontalAxisPosition);
	//addCheckBoxManager(verticalAxisGroupBox_, Parameters::VerticalAxisVisible);
  //addCheckBoxManager(horizontalAxisGroupBox_, Parameters::HorizontalAxisVisible);
	addLineEditManager(titleLineEdit_, Parameters::PlotTitle);
	addLineEditManager(dataLineEdit_, Parameters::DataTitle);
	addLineEditManager(xAxisLineEdit_, Parameters::XAxisLabel);
	addLineEditManager(yAxisLineEdit_, Parameters::YAxisLabel);
	addCheckBoxManager(showPointsCheckBox_, Parameters::ShowPointSymbols);

	connect(showPlotPushButton_, SIGNAL(clicked()), this, SLOT(showPlot()));
	connect(exportPlotPushButton_, SIGNAL(clicked()), this, SLOT(exportPlot()));
	connect(dataColorPushButton_, SIGNAL(clicked()), this, SLOT(assignDataColor()));

	dataColors_.push_back(Qt::red);
}

AdvancedPlotterDialog::~AdvancedPlotterDialog()
{
	delete plotDialog_;
}

void AdvancedPlotterDialog::pullSpecial()
{
	if (plotDialog_ && plotDialog_->isVisible())
		updatePlot();
}

void AdvancedPlotterDialog::showPlot()
{
  if (!plotDialog_)
	{
		plotDialog_ = new PlotDialog(this);
	}

	updatePlot();
	plotDialog_->show();
	plotDialog_->raise();
}

void AdvancedPlotterDialog::updatePlot()
{
  plotData();

  plotDialog_->updatePlot(titleLineEdit_->text(), xAxisLineEdit_->text(), yAxisLineEdit_->text(),
    boost::make_optional(horizontalAxisGroupBox_->isChecked(), horizontalAxisSpinBox_->value()),
    boost::make_optional(verticalAxisGroupBox_->isChecked(), verticalAxisSpinBox_->value()));
}

void AdvancedPlotterDialog::plotData()
{
  auto independents = transient_value_cast<std::vector<DenseMatrixHandle>>(state_->getTransientValue(Parameters::IndependentVariablesVector));
  auto dependents = transient_value_cast<std::vector<DenseMatrixHandle>>(state_->getTransientValue(Parameters::DependentVariablesVector));
  auto plot = plotDialog_->plot();
  auto showPoints = showPointsCheckBox_->isChecked();
  plot->clearCurves();
  bool addLegend = true;
  for (auto&& tup : zip(independents, dependents))
  {
    DenseMatrixHandle x, y;
    boost::tie(x, y) = tup;
    for (int c = 0; c < y->ncols(); ++c)
      plot->addCurve(x->col(0), y->col(c), "data " + QString::number(c), "red", c < 5, showPoints);
    if (y->ncols() > 5)
      addLegend = false;
  }
  if (addLegend)
    plot->addLegend();
}

void AdvancedPlotterDialog::assignDataColor()
{
  auto newColor = QColorDialog::getColor(dataColors_[0], this, "Choose data color");
  if (newColor.isValid())
  {
		dataColors_[0] = newColor;
		updatePlot();
  }
}

void AdvancedPlotterDialog::exportPlot()
{
	plotDialog_->plot()->exportPlot();
}
