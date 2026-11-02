
/*
    // Remove Algorithm.dll dependency
    
    g++ -std=c++23 -shared -static -static-libgcc -static-libstdc++ -o tesseract.dll tesseractDll.cpp Algorithm.cpp -I"include"  -L"lib" -ltesseract55 -lleptonica              -lcurl -larchive -ltiff -lwebp -lsharpyuv -lgif -lopenjp2 -lssl -lcrypto -lpng -ljpeg -lz -lbz2 -llzma -llz4 -lzstd -lws2_32 -lbcrypt -lcrypt32 -Wl,--subsystem,windows -m64 

*/


#include "include/tesseractApp.h"
#include "include/Algorithm.h"

//
TesseractApp::TesseractApp()
{
    //
}

//
TesseractApp::~TesseractApp()
{
    //
}
//
const char* TesseractApp::GetTesseractOcrOutput()
{
   //
   const char* imagePath = "input.png";
   static std::string output;
   //
   tesseract::TessBaseAPI ocr;

   //
   if (ocr.Init("tessdata", "eng")) {
       output = "Could not initialize tesseract.";
       return output.c_str();
   }

   //
   Pix* image = pixRead(imagePath);
   if (!image) {
       output = "Could not open image.";
       return output.c_str();
   }

   //
   ocr.SetImage(image);
   char* result = ocr.GetUTF8Text();

   //
   output = result ? result : "No output.";
   delete[] result;
   pixDestroy(&image);
   ocr.End();

   //
   return output.c_str();
}
//
const char* TesseractApp::GetTesseractOcrOutputPath(const char* imagePath)
{
   //
   static std::string output;
   //
   tesseract::TessBaseAPI ocr;

   //
   if (ocr.Init("tessdata", "eng")) {
       output = "Could not initialize tesseract.";
       return output.c_str();
   }

   //
   Pix* image = pixRead(imagePath);
   if (!image) {
       output = "Could not open image.";
       return output.c_str();
   }

   //
   ocr.SetImage(image);
   char* result = ocr.GetUTF8Text();

   //
   output = result ? result : "No output.";
   delete[] result;
   pixDestroy(&image);
   ocr.End();

   //
   return output.c_str();
}
//
const char*  TesseractApp::GetTesseractVersion()
{
    // Get the Tesseract version
    return tesseract::TessBaseAPI::Version();
}
//
const char*  TesseractApp::GetTesseractAppVersion()
{
    //
    Algorithm* app = new Algorithm(true);
    app->ReadConfigFile("tesseract.ini");
    return app->configMap["DLL_VERSION"].c_str();
} 
//


