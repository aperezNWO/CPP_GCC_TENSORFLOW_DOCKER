///////////////////////////////////////////////////////////////////////////
// TESSERACT.DLL
///////////////////////////////////////////////////////////////////////////

#ifndef TESSERACTAPP_H // include guard
#define TESSERACTAPP_H
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

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#define DLL_EXPORT extern "C" __declspec(dllexport) __stdcall

using namespace std;

#pragma hdrstop
#pragma argsused
#pragma once

class TesseractApp 
{
   public :
        //
        TesseractApp();
        ~TesseractApp();
        //
        const char*  GetTesseractOcrOutput(); 
        const char*  GetTesseractOcrOutputPath(const char* imagePath);
        const char*  GetTesseractVersion();
        const char*  GetTesseractAppVersion(); 
        //
        //int          ReadConfigFile();
     public :
        //
        //map<string, string> configMap;

};
