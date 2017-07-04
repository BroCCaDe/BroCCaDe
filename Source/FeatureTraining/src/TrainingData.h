/*******************************************************************************\
* Copyright (c) 2017 by Hendra Gunadi (Hendra.Gunadi@murodch.edu.au)            *
*                                                                               *
* Redistribution and use in source and binary forms, with or without            *
* modification, are permitted provided that the following conditions are met:   *
*                                                                               *
* (1) Redistributions of source code must retain the above copyright            *
*     notice, this list of conditions and the following disclaimer.             *
*                                                                               *
* (2) Redistributions in binary form must reproduce the above copyright         *
*     notice, this list of conditions and the following disclaimer in           *
*     the documentation and/or other materials provided with the                *
*     distribution.                                                             *
*                                                                               *
* (3) Neither the name of Hendra Gunadi and/or Murdoch University, nor          *
*     the names of contributors may be used to endorse or promote               *
*     products derived from this software without specific prior written        *
*     permission.                                                               *
*                                                                               *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"   *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE     *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE    *
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF          *
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS      *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       *
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE    *
* POSSIBILITY OF SUCH DAMAGE.                                                   *
*                                                                               *
* TrainingData.h : the content of a particular training data                    *
* Contents :                                                                    *
*   * class DataRow         : the content of the data section of the ARFF file  *
*   * class FeatureTraining : Contains the information needed for each ARFF file*
\*******************************************************************************/

#ifndef BRO_PLUGIN_AUX_FEATURE_TRAINING
#define BRO_PLUGIN_AUX_FEATURE_TRAINING

#include<vector>
#include<memory>
#include<string>
#include<unordered_set>

// each data row in the ARFF file is a comma delimited values with the class string
// at the end. So we store the row as a vector of values and string name.
class DataRow
{
public:
    DataRow(std::vector<double> features, std::string name) : 
        _values(features), _class_name(name) {};
    virtual ~DataRow() {}

    // put into the ARFF format
    std::string to_string() {
        std::string temp;
        temp.clear();
        for (unsigned int i = 0; i < _values.size(); i++)
        {
            temp = temp + std::to_string(_values[i]) + ",";
        }
        temp = temp + _class_name + "\n";
        return temp;
    }
private:
    std::vector<double> _values;
    std::string _class_name;
};

// we assume that the attributes is always of the form NUMERIC
class FeatureTraining
{
public:
    // initialize
    FeatureTraining() {
        _relation.clear();
        _attributes.clear();
        _class_names.clear();
        _data.clear();
    }
    virtual ~FeatureTraining() {}
    
    // change the relation string
    void change_relation(std::string relation_name) {_relation = relation_name;}
    // add an attribute of name @attribute_name
    void add_attribute(std::string attribute_name) {_attributes.insert(attribute_name);}
    // add a class
    void add_class(std::string class_name) {_class_names.insert(class_name);}
    // add a data row
    void add_data_row(std::vector<double> features, std::string name) {
        std::unique_ptr<DataRow> row(new DataRow(features, name));
        _data.resize(_data.size() + 1);
        _data[_data.size()-1].swap(row);
    }

    // get the information already in the print format
    std::string get_relation() {return "@RELATION " + _relation + "\n\n";}
    std::string get_attributes() {
        std::string temp;
        temp.clear();
        for (std::unordered_set<std::string>::iterator it = _attributes.begin(); 
            it != _attributes.end(); ++it) 
                temp = temp + "@ATTRIBUTES " + *it + " NUMERIC\n";
        return temp;
    }
    std::string get_classes() {
        std::unordered_set<std::string>::iterator it = _class_names.begin();
        std::string temp = "@ATTRIBUTES class {" + *it;
        for (++it; it != _attributes.end(); ++it)  
            temp = temp + "," + *it;
        temp = temp + "}\n\n";
        return temp;
    }
    std::string get_data() {
        std::string temp = "@DATA\n";
        for (unsigned int i = 1; i < _data.size(); i++) 
            temp = temp + _data[i]->to_string();
        return temp;
    }
private:
    std::string _relation;
    std::unordered_set<std::string> _attributes;
    std::unordered_set<std::string> _class_names;
	std::vector<std::unique_ptr<DataRow> > _data;
};

#endif
