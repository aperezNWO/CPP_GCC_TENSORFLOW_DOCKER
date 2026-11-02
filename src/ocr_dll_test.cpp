
//////////////////////////////////////////
// CONFIGURATION
//////////////////////////////////////////
/*

1) TOOL CHAIN : C:\msys64\mingw64\bin
2) g++ -std=c++11 -Wall -Wextra  ocr_dll_test.cpp -o ocr_dll_test.exe -m64
3) TERMINAL > RUN > BUILD TASK > (1) C/C++: g++.exe compilar archivo activo
4) Copy *.exe file to __dist folder.
5) execute ".\ocr_dll_test.exe"

*/

#include <windows.h>
#include <iostream>
#include <conio.h>
#include <string>

// Define function pointer types for the DLL functions
typedef        const char* (__cdecl *GetTesseractOcrOutputFunc)();
typedef        const char* (__cdecl *GetTesseractVersionFunc)();
typedef        const char* (__cdecl *GetTesseractAppVersionFunc)();
typedef        const char* (__cdecl *GetTesseract_CPPSTDVersionFunc)();

//
int main() {
    // Load the DLL
    HINSTANCE hDll = LoadLibraryA("tesseract.dll");
    if (!hDll) {
        DWORD errorCode = GetLastError(); // Capture the error immediately
        std::cerr << "Failed to load TensorFlowApp64_CPP.dll. Error code: " << errorCode << std::endl;

        // Optional: Format the error message for more detail
        LPVOID errorMsg;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&errorMsg,
            0,
            NULL
        );
        std::cerr << "Error message: " << (LPSTR)errorMsg << std::endl;
        LocalFree(errorMsg);

        return 1;
    }
 
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // GetTesseractOcrOutput
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    GetTesseractOcrOutputFunc getOcrOutput = (GetTesseractOcrOutputFunc)GetProcAddress(hDll, "GetTesseractOcrOutput");

    // Check if the functions were found
    if (!getOcrOutput) {
        std::cerr << "Failed to get function addresses. Error code: " << GetLastError() << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    // Test GetTensorFlowOcrOutput (uses hardcoded "Input.png")
    const char* ocrOutput = getOcrOutput();
    std::cout << std::endl << "'GetTesseractOcrOutput' result: " << ocrOutput /* << std::endl*/;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // GetTesseractVersion
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    GetTesseractVersionFunc getTesseractVersion = (GetTesseractVersionFunc)GetProcAddress(hDll, "GetTesseractVersion");

    // Check if the functions were found
    if (!getTesseractVersion) {
        std::cerr << "Failed to get function addresses. Error code: " << GetLastError() << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    // GetTesseractVersion
    const char* tesseractVersion = getTesseractVersion();
    std::cout << std::endl<< "'GetTesseractAPIVersion'  result: " << tesseractVersion << std::endl;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // GetTesseractAppVersion
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    GetTesseractAppVersionFunc getAppVersion = (GetTesseractAppVersionFunc)GetProcAddress(hDll, "GetTesseractAppVersion");

    // Check if the functions were found
    if (!getAppVersion) {
        std::cerr << "Failed to get function addresses 'GetTesseractAppVersion'. Error code: " << GetLastError() << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    // Test GetTensorFlowOcrOutput (uses hardcoded "Input.png")
    const char* appVersion = getAppVersion();
    std::cout << std::endl << "'GetTesseractAppVersion' result: " << appVersion << std::endl;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // GetTesseract_CPPSTDVersion
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    GetTesseract_CPPSTDVersionFunc getCPPSTDVersion = (GetTesseract_CPPSTDVersionFunc)GetProcAddress(hDll, "GetTesseract_CPPSTDVersion");

    // Check if the functions were found
    if (!getCPPSTDVersion) {
        std::cerr << "Failed to get function addresses 'GetTesseract_CPPSTDVersion'. Error code: " << GetLastError() << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    // Get STD c++ VERSION
    const char* cppSTDVersion = getCPPSTDVersion();
    std::cout << std::endl << "'GetTesseract_CPPSTDVersion' result: " << cppSTDVersion << std::endl;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CLEANUP AND FINISING TASKS
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
 
    // Pause and wait for a keypress
    std::cout << "\nPress any key to exit..." << std::endl;
    _getch();

    
    FreeLibrary(hDll);
    return 0;
}
