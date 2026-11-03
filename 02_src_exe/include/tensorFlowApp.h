
#ifndef TENSORFLOWAPP_H // include guard
#define TENSORFLOWAPP_H
#endif


#include "Algorithm.h"
#include "tictactoe.h"
#include "LinearRegression.h"

#ifdef _WIN32
    #define DLL_EXPORT extern "C" __declspec(dllexport) __stdcall
#else
    #define DLL_EXPORT extern "C" __attribute__((visibility("default")))
#endif

using namespace std;

//
class TensorFlowApp :
	public Algorithm
{
    public :
        //
        TensorFlowApp();
        ~TensorFlowApp();
        //
        const char*  GetTensorFlowAPIVersion();
        std::string  GetTensorFlowAppVersion(); 
};

