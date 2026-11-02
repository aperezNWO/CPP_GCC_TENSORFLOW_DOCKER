///////////////////////////////////////////////////////////////////////////
// ALGORITHM.H
///////////////////////////////////////////////////////////////////////////

#ifndef Algorithm_H // include guard
#define Algorithm_H
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
#include <cctype>
#include <map>
#include <cstring>

#define DLL_EXPORT extern "C" __declspec(dllexport) __stdcall

#ifdef _WIN32
    #ifdef ALGORITHM_EXPORTS
        #define ALGORITHM_API __declspec(dllexport)
    #else
        #define ALGORITHM_API __declspec(dllimport)
    #endif
#else
    #define ALGORITHM_API // Non-Windows platforms don't need this
#endif

//
using namespace std;

#pragma hdrstop
#pragma argsused
#pragma once

// Define the enum
enum class FormatType {
    HTML = 1,  // Represents HTML format
    JSON = 2   // Represents JSON format
};

//
class Algorithm
{
	   public :
			//
			Algorithm(bool readConfigFile);
			~Algorithm();
			//
			static string                      HtmlEncode(const std::string& input);
			static string                      StringTrim(const std::string& str);
			static vector<string>              StringSplit(const char* p_inputString, std::string p_delimiter);
			int                                ReadConfigFile(const char* fileName);
            static void                        ReplaceAll(std::string &str, const std::string &from, const std::string &to);
            const char*                        GetDLLVersion();
            const char*                        GetCPPSTDVersion(long int cppVersion);
	   public :
		  //
		  map<string, string> configMap;
};


