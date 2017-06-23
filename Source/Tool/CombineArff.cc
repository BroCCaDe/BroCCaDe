#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_set>

int main(int argc, char* argv[])
{
    if (argc < 2) {printf("not enough parameters\n"); return 0;}

	std::string line;
    std::unordered_set<std::string> current_attribute_names;
    std::unordered_set<std::string> last_attribute_names;
    std::unordered_set<std::string> class_names;
    std::vector<std::string> data_str;
    std::string relation_name;
    char sep = ' ';
	int i = 2;    

    relation_name.clear();
    current_attribute_names.clear();
    last_attribute_names.clear();
    class_names.clear();
    data_str.clear();

    for (i = 2; i < argc; i++)
	{
		// for one file
		std::ifstream f;
		f.open(argv[i]);
        bool data_reached = false;
		while( getline(f, line) )
		{
            if (data_reached)                       // we've reached data section, so just combine
            {
                data_str.push_back(line);
            }
            else
            {
                // remove \t
                size_t tab = 0;
                while( (tab = line.find('\t', tab)) != std::string::npos)
                    line.replace(tab, 1, 1, ' ');

                std::istringstream stm(line) ;
    		    std::string token ;
                if( std::getline( stm, token, sep ) )
                {
                    if (token.empty()) continue;
                    // at the moment we don't care about case
                    if (token == "@RELATION")
                    {
                        std::getline(stm, token, sep);
                        if (relation_name.empty()) relation_name = token;
                        else if (relation_name != token)
                        {
                            printf("relation names are not the same (%s != %s) in %s\n", 
                                relation_name.c_str(), token.c_str(), argv[i]);
                            return 1;
                        }
                    }
                    else if (token == "@ATTRIBUTES")
                    {
                        std::getline(stm, token, sep);
                        current_attribute_names.insert(token);
                        if (token == "class")
                        {
                            std::getline(stm, token, '{');
                            std::getline(stm, token, '}');
                            std::istringstream stm_token(token) ;
                            std::string class_token;
                            while(std::getline(stm_token, class_token, ','))
                            {
                                class_names.insert(class_token);
                            }
                        }
                    }
                    else if (token == "@DATA")
                    {
                        data_reached = true;
                        // compare attributes
                        if (last_attribute_names.size() == 0)
                        {
                            last_attribute_names.swap(current_attribute_names);
                        }
                        else
                        {
                            if (last_attribute_names.size() != current_attribute_names.size())
                            {
                                printf("attributes number of %s is of different size (%lu %lu)\n", argv[i], 
                                    last_attribute_names.size(), current_attribute_names.size());
                                return 1;
                            }
                            for (std::unordered_set<std::string>::iterator it = last_attribute_names.begin();
                                it != last_attribute_names.end(); it++)
                            {
                                if (current_attribute_names.find(*it) == current_attribute_names.end())
                                {
                                    printf("attribute %s is missing from %s\n", it->c_str(), argv[i]);
                                    return 1;
                                }
                            }
                        }
                        //current_attribute_names.clear();
                    }
                }
            }
        }
		f.close();
	}

    std::ofstream of;
    of.open(argv[1]);
    of << "@RELATION \t" << relation_name << "\n\n";
    for (std::unordered_set<std::string>::iterator it = last_attribute_names.begin(); 
        it != last_attribute_names.end(); it++)
    {
        of << "@ATTRIBUTES " << *it;
        if (*it == "class")
        {
            of << "\t{";
            // assume that there will be at least one class
            std::unordered_set<std::string>::iterator itc = class_names.begin();
            of << *itc;
            for (++itc; itc != class_names.end(); itc++)
                of << "," << *itc;
            
            of << "}";
        }
        else of << "\tNUMERIC";
        of << "\n";
    }
    of << "\n@DATA\n";
    for (std::vector<std::string>::iterator it = data_str.begin(); it != data_str.end(); it++)
        of << *it << "\n";
    
    of.close(); 

	return 0;
}
