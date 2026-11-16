///////////////////////////////////////////////////////////////////////////
// REGEX MANAGER
///////////////////////////////////////////////////////////////////////////

#ifndef RegExManager_H // include guard
#define RegExManager_H
#endif


#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <random>
#include <regex>
#include <map>

using namespace std;

#pragma hdrstop
#pragma argsused

class RegExManager 
{
	public :
		 RegExManager();
		 ~RegExManager();
		 int    ReadConfigFile(const char*);
		 string HtmlEncode(const std::string& input);
		 string StringTrim(const std::string& str);
		 int    GetXml();
		 string RegExEval(char* p_tagSearch, char* p_textSearch);
	public :
		 vector<string>           xmlItems;
		 std::map<string, string> configMap;
};
