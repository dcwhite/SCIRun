
/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2009 Scientific Computing and Imaging Institute,
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

#include <Modules/Legacy/Bundle/GetFieldsFromBundle.h>
#include <Core/Datatypes/Legacy/Bundle/Bundle.h>
#include <Core/Datatypes/Legacy/Field/Field.h>

using namespace SCIRun;
using namespace SCIRun::Core::Datatypes;
using namespace SCIRun::Modules::Bundles;
using namespace SCIRun::Dataflow::Networks;
using namespace SCIRun::Core::Algorithms;

ModuleLookupInfo GetFieldsFromBundle::staticInfo_("GetFieldsFromBundle", "Bundle", "SCIRun");
AlgorithmParameterName GetFieldsFromBundle::FieldNameList("FieldNameList");
const AlgorithmParameterName GetFieldsFromBundle::FieldNames[] = {
  AlgorithmParameterName("field1-name"), 
  AlgorithmParameterName("field2-name"), 
  AlgorithmParameterName("field3-name"), 
  AlgorithmParameterName("field4-name"), 
  AlgorithmParameterName("field5-name"), 
  AlgorithmParameterName("field6-name")
};

GetFieldsFromBundle::GetFieldsFromBundle() : Module(staticInfo_)
{
  INITIALIZE_PORT(InputBundle);
  INITIALIZE_PORT(OutputBundle);
  INITIALIZE_PORT(field1);
  INITIALIZE_PORT(field2);
  INITIALIZE_PORT(field3);
  INITIALIZE_PORT(field4);
  INITIALIZE_PORT(field5);
  INITIALIZE_PORT(field6);
}

void GetFieldsFromBundle::setStateDefaults()
{
  auto state = get_state();
  state->setValue(FieldNameList, std::string());
  
  for (int i = 0; i < NUM_BUNDLE_OUT; ++i)
  {  
    state->setValue(FieldNames[i], "field" + boost::lexical_cast<std::string>(i));
  }
}

void GetFieldsFromBundle::execute()
{
  auto bundle = getRequiredInput(InputBundle);
  
#ifdef SCIRUN4_CODE_TO_BE_ENABLED_LATER
  if (inputs_changed_ || guifield1name_.changed() || 
      guifield2name_.changed() || guifield3name_.changed() ||
      guifield4name_.changed() || guifield5name_.changed() ||
      guifield6name_.changed() || !oport_cached("bundle") || 
      !oport_cached("field1")  || !oport_cached("field2") || 
      !oport_cached("field3")  || !oport_cached("field4") ||
      !oport_cached("field5")  || !oport_cached("field6"))
#endif
  if (needToExecute())
  {
    update_state(Executing);

    auto state = get_state();
    state->setValue(FieldNameList, makeFieldNameList(*bundle));

    for (int i = 0; i < NUM_BUNDLE_OUT; ++i)
    {
      auto fieldName = state->getValue(FieldNames[i]).getString();
      if (bundle->isField(fieldName))
      {
        auto field = bundle->getField(fieldName);
#ifdef SCIRUN4_CODE_TO_BE_ENABLED_LATER
        sendOutput("field1",field);
#endif
      } 
    }
        
    sendOutput(OutputBundle, bundle);
  }
}

std::string GetFieldsFromBundle::makeFieldNameList(const Bundle& bundle) const
{
  /*
  std::string fieldlist;

  int numFields = bundle->numFields();
  for (int p = 0; p < numFields; p++)
  {
  fieldlist += "{" + bundle->getFieldName(p) + "} ";
  }
  */
  return "fpp";
}