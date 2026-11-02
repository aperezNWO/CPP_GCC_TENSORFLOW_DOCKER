/*

//
TOOLCHAIN : C:\msys64\uctr64\bin (CHANGE PATH)

// INSTALLERS
https://www.tensorflow.org/install/lang_c?hl=es-419


// NON STATIC - REFERENCE - INHERITANCE

g++ -std=c++20 -I"include" -L"lib" -shared -m64 -o TensorFlowAppCPP.dll tensorFlowApp.cpp -ltensorflow -lAlgorithm -Wl,--subsystem,windows -DALGORITHM_EXPORTS

// COMPILE FROM ROOT (above _tensorflow folder)

g++ -std=c++20 -I"include" -L"lib" -shared -m64 -o "__dist/TensorFlowAppCPP.dll" "_tensorflow/tensorFlowApp.cpp"  -ltensorflow -lAlgorithm -Wl,--subsystem,windows -DALGORITHM_EXPORTS   

*/

#include "../include/tensorFlowApp.h"

//
TensorFlowApp::TensorFlowApp(): Algorithm(false)
{
     //
     this->ReadConfigFile("tensorflow.ini");
}
//
TensorFlowApp::~TensorFlowApp()
{
    //
}

//
const char*  TensorFlowApp::GetTensorFlowAPIVersion()
{
	//
  	return TF_Version(); // Return the TensorFlow version directly;
}

//
std::string TensorFlowApp::GetTensorFlowAppVersion()
{
    auto it = this->configMap.find("DLL_VERSION");
    if (it != this->configMap.end()) {
        return it->second;
    }
    return "UNKNOWN"; 
}

/////////////////////////////////////////////////////////////////////
// DLL ENTRY POINTS
/////////////////////////////////////////////////////////////////////
//
DLL_EXPORT const char* GetTensorFlowAPIVersion() 
{
    static std::string versionCache;
    {
        std::unique_ptr<TensorFlowApp> app = std::make_unique<TensorFlowApp>();
        versionCache = app->GetTensorFlowAPIVersion(); // TF_Version()
    }
    return versionCache.c_str();
}
//
DLL_EXPORT const char* GetTensorFlowAppVersion() {
    static std::string version;
    if (version.empty()) {
        TensorFlowApp app;
        version = app.GetTensorFlowAppVersion();
    }
    return version.c_str();
}
//
DLL_EXPORT const char* GetCPPSTDVersion()
{
    static std::string version;
    if (version.empty()) {
        TensorFlowApp app;
        version = app.GetCPPSTDVersion(__cplusplus);
    }
    return version.c_str();
}
//
DLL_EXPORT bool PlayTicTacToeGameWithHistory(TicTacToeResultOnline* result, int aiMode, double temperature) 
{
    try {
    	//
        if (!result) return false;
        //
        return RunTicTacToeSelfPlay(*result, aiMode, temperature);
    } catch (...) {
        return false;
    }
}

//
DLL_EXPORT double Predict( double missionNumberToPredict )
{
	//
    LinearRegression *linearRegression = new LinearRegression();
    
    //
    double predictedTotalTime          = linearRegression->predict(missionNumberToPredict);
      
    //
    return predictedTotalTime;
}

