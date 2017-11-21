/*******************************************************************************\
* Copyright (c) 2017 Murdoch University                                         *
*                                                                               *
* Written by Hendra Gunadi (Hendra.Gunadi@murdoch.edu.au)                       *
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
* This work was supported by a grant from the Comcast Innovation Fund.          *
*                                                                               *
* CombineArff.cc : combining ARFF files for Weka input. It is assumed that the  *
*       inputs are in the correct ARFF format otherwise undefined behaviours    *
*                                                                               *
* Usage : <output> [<file1>.arff [<file1>.arff [ ... ] ] ]                      *
\*******************************************************************************/

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <algorithm>

int main(int argc, char* argv[])
{
    // check if users have passed the output file name
    if (argc < 2) {printf("Usage : %s <output> [<file1>.arff [<file1>.arff [ ... ] ] ]\n", argv[0]); return 0;}

	std::string line;                                           // temporary string read from the input files
    std::vector<std::string> attribute_names;                   // attribute names arff files
    std::unordered_set<std::string> class_names;                // accummulated class names
    std::vector<std::string> data_str;                          // string of the data section
    std::string relation_name;                                  // relation name (has to be the same across all files)
    std::unordered_set<std::string>::iterator itc;
    char sep = ' ';                                             // token separator
	int i = 2;                                                  // starting from the second argument (first arff file)

    // initialize
    relation_name.clear();
    attribute_names.clear();
    class_names.clear();
    data_str.clear();


    for (i = 2; i < argc; i++)
	{
		std::ifstream f;                                        // open file
		f.open(argv[i]);
        bool data_reached = false;                              // we haven't seen the @DATA token yet
        int att_count = 0;                                      // attribute counter
		while( getline(f, line) )
		{
            if (data_reached)                                   // we've reached data section, so just combine
            {
                data_str.push_back(line);
            }
            else
            {
                // replace '\t' with the common separator ' '
                size_t tab = 0;
                while( (tab = line.find('\t', tab)) != std::string::npos)
                    line.replace(tab, 1, 1, ' ');

                std::istringstream stm(line) ;
    		    std::string token ;
                // get the first token
                if( std::getline( stm, token, sep ) )
                {
                    // if it's an empty line then keep going
                    if (token.empty()) continue;
                    std::transform(token.begin(), token.end(), token.begin(), toupper); // upper case the token
                    // if it starts with @RELATION, grab the relation name
                    if (token == "@RELATION")
                    {
                        std::getline(stm, token, sep);
                        // then check if it's consistent across all arff files
                        if (relation_name.empty()) relation_name = token;
                        else if (relation_name != token)
                        {
                            printf("relation names are not the same (%s != %s) in %s\n", 
                                relation_name.c_str(), token.c_str(), argv[i]);
                            return 1;
                        }
                    }
                    // if it starts with @ATTRIBUTES, grab the attribute name. We can assume that the type will
                    // be NUMERIC except for "class" attribute
                    else if (token == "@ATTRIBUTES")
                    {
                        std::getline(stm, token, sep);
                        if (i == 2) attribute_names.push_back(token);
                        else if (attribute_names[att_count].compare(token) != 0)
                        {
                            printf("inconsistent attribute %s in %s\n", token.c_str(), argv[i]);
                                    return 1;
                        }
                        // if it is of "class" attribute then we need to grab the class names, delimited by '{' '}'
                        // and is separated by ',' with no spaces in between
                        if (token == "class")
                        {
                            std::getline(stm, token, '{');
                            std::getline(stm, token, '}');
                            std::istringstream stm_token(token) ;
                            std::string class_token;
                            while(std::getline(stm_token, class_token, ','))
                            {
                                class_names.insert(class_token);    // accummulate all the class names
                            }
                        }
                        att_count++;
                    }
                    // if it reached @DATA, then check whether the attributes are consistent across all files
                    else if (token == "@DATA")
                    {
                        data_reached = true;
                    }
                }
            }
        }
		f.close();                                              // close the file
	}

    // open the output file
    std::ofstream of;
    of.open(argv[1]);
    // output the relation name directly
    of << "@RELATION \t" << relation_name << "\n\n";
    for (std::vector<std::string>::iterator it = attribute_names.begin(); 
        it != attribute_names.end(); it++)
    {
        if (*it == "class") continue;
        // output attribute name and its type. All of the attributes will be numeric except
        // "class" which will be a collection of class names, e.g. "{c1,c2,c3}"
        of << "@ATTRIBUTE " << *it << "\tNUMERIC\n";
    }

    // output class
    of << "@ATTRIBUTE class \t{";
    // assume that there will be at least one class
    itc = class_names.begin();
    of << *itc;
    for (++itc; itc != class_names.end(); itc++)
        of << "," << *itc;
    of << "}\n";
        
    // output the data section verbatim
    of << "\n@DATA\n";
    for (std::vector<std::string>::iterator it = data_str.begin(); it != data_str.end(); it++)
        of << *it << "\n";
    
    of.close();                                                 // close the file

	return 0;
}
