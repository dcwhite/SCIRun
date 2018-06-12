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

/// \author James Hughes
/// \date   December 2012
/// \brief  Not sure this file should go in Modules/Render. But it is an
///         auxiliary file to the ViewScene render module.

#include <Interface/Modules/Render/GLContext.h>
#include <QOpenGLContext>
#include <QDebug>

using namespace SCIRun::Gui;

GLContext::GLContext(QOpenGLWidget* glWidget) :
    mGLWidget(glWidget)
{
}

void GLContext::makeCurrent()
{
  // Be sure to call XInitThreads() before creating the QApplication object on
  // X11 based systems. Otherwise, these calls are NOT thread safe.
  mGLWidget->makeCurrent();
}

void GLContext::swapBuffers()
{
	//dummy commit to save this branch in my fork.
  //no longer a function
  //mGLWidget->swapBuffers();
  // qDebug() << "mGLWidget" << mGLWidget;
  // if (mGLWidget)
  // {
  //   qDebug() << "mGLWidget->context()" << mGLWidget->context();
  //   if (mGLWidget->context())
  //     qDebug() << "mGLWidget->context()->surface()" << mGLWidget->context()->surface();
  // }
  if (mGLWidget->context() && mGLWidget->context()->surface())
    mGLWidget->context()->swapBuffers(mGLWidget->context()->surface());

}
