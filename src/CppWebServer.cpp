/*

    // WINDOWS
  
    g++ -std=c++23 -shared -static -static-libgcc -static-libstdc++ -o tesseract.exe tesseractDll.cpp Algorithm.cpp -I"include"  -L"lib" -ltesseract55 -lleptonica              -lcurl -larchive -ltiff -lwebp -lsharpyuv -lgif -lopenjp2 -lssl -lcrypto -lpng -ljpeg -lz -lbz2 -llzma -llz4 -lzstd -lws2_32 -lbcrypt -lcrypt32 -Wl,--subsystem,windows -m64 

    g++ -std=c++17 -O3 CppWebServer.cpp -o cpp_web_server.exe -mconsole -lws2_32

    g++ -std=c++23  -static -static-libgcc -static-libstdc++ -o cpp_web_server.exe CppWebServer.cpp Algorithm.cpp tesseractDll.cpp -I"include"  -L"lib" -ltesseract55 -lleptonica              -lcurl -larchive -ltiff -lwebp -lsharpyuv -lgif -lopenjp2 -lssl -lcrypto -lpng -ljpeg -lz -lbz2 -llzma -llz4 -lzstd -lws2_32 -lbcrypt -lcrypt32 -mconsole -m64 
  
    // LINUX -- DOCKER (NO .EXE EXTENSION FOR DOCKER!!!)
    	// EDIT DOCKER FILE TO ADD NEEDED FILES
	
   			 COPY --from=builder /app/Algorithm.ini /app/Algorithm.ini
			 COPY --from=builder /app/cpp_web_server /app/cpp_web_server
        // COPY C++ COMMAND TO DOCKER FILE	
			 RUN  <COMMAND_LINE>		 
        g++ -std=c++17 -O3 CppWebServer.cpp -I.  -o cpp_web_server -lpthread 

        g++ -std=c++23 -O3 CppWebServer.cpp Algorithm.cpp tesseractDll.cpp -I. -Iinclude -o cpp_web_server -lpthread -ltesseract -llept

*/

#include "httplib.h"
#include "include/tesseractApp.h"
#include "include/Algorithm.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <filesystem>

//////////////////////////////////////////////////////////////////////
// CLASS DECLARATION
//////////////////////////////////////////////////////////////////////

class CppWebServerApp
{
	//
	public :
		//
		struct EndpointInfo {
		    std::string endpointName;
		    std::string endpointDescription;
		};
		//
		std::unordered_map<std::string, EndpointInfo> endpointDictionary;
		//
	    httplib::Server svr;
	    //
	    const char* cppWebServerVersion;
    //
    public :
		//
		CppWebServerApp();
		//
		~CppWebServerApp();	
		//
		static const char* GetCPPHttpVersion();
    	//
    	int Run();
    
};

//////////////////////////////////////////////////////////////////////
// CLASS IMPLEMENTATION
//////////////////////////////////////////////////////////////////////

//
CppWebServerApp::CppWebServerApp()
{	
	//
	this->endpointDictionary = {
	    {"HEALTH_ENDPOINT"            , {"/health"                     , "Print all endpoints"}},
	    {"PING_ENDPOINT"              , {"/zero"                       , "Render workaround"}},
	    {"APP_VERSION_ENDPOINT"       , {"/getAppVersion"              , "Get Application Version"}},
	    {"STD_VERSION_ENDPOINT"       , {"/getSTDVersion"              , "Get Standard C++ Version"}},
	    {"SERVER_VERSION_ENDPOINT"    , {"/getServerVersion"           , "Get Http Server Version"}},
	    {"TESSERACT_VERSION_ENDPOINT"    , {"/api/ocr/getAPIVersion"   , "Get Tesseract API Version"}},
	    {"TESSERACT_UPLOAD_ENDPOINT"     , {"/api/ocr/upload"          , "Upload image from html form via HTTP POST"}},    
	    {"TESSERACT_OCR_IMAGE_ENDPOINT"  , {"/api/ocr/getImage"        , "UNITARY TEST"}},       
	};
	//
	this->cppWebServerVersion = this->GetCPPHttpVersion();
}
//
CppWebServerApp::~CppWebServerApp()
{	
	//
}
//
const char* CppWebServerApp::GetCPPHttpVersion()
{
    #ifdef CPPHTTPLIB_VERSION
        return CPPHTTPLIB_VERSION;
    #else
        return "Unknown cpp-httplib Version";
    #endif
}
//
int CppWebServerApp::Run()
{
	// HTTP Interceptor & Global CORS Middleware
    svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        // 1. Request Logging Interceptor
        std::cout << "[Interceptor] " << req.method << " " << req.path << std::endl;

        // 2. Global CORS Header Policy
        res.set_header("Access-Control-Allow-Origin"     , "*");
        res.set_header("Access-Control-Allow-Methods"    , "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers"    , "Origin, Content-Type, Accept, Authorization");
        res.set_header("Access-Control-Allow-Credentials", "true");

        // 3. Handle browser preflight OPTIONS requests automatically
        if (req.method == "OPTIONS") {
            res.status = 204; // No Content
            return httplib::Server::HandlerResponse::Handled;
        }

        // Proceed to route handlers
        return httplib::Server::HandlerResponse::Unhandled;
    });
    
       // Ping / Zero Endpoint
    svr.Get(endpointDictionary["PING_ENDPOINT"].endpointName.c_str(), [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    // Server Diagnostics (Health Endpoint listing all routes)
    svr.Get(endpointDictionary["HEALTH_ENDPOINT"].endpointName.c_str(), [&](const httplib::Request&, httplib::Response& res) {
    	//
		std::string webServerVersion = GetCPPHttpVersion();
        //
		std::string jsonResponse     = "{\n  \"server\": \"C++ Http Server 'cpp-httplib' - v[" + webServerVersion  + "] Working!\",\n  \"endpoints\": [\n";
        //
        size_t count = 0;
        for (const auto& pair : endpointDictionary) {
            const std::string& key = pair.first;
            const EndpointInfo& info = pair.second;
            
            std::string comma = (++count < endpointDictionary.size()) ? "," : "";

            jsonResponse += "    {\n";
            jsonResponse += "      \"key\": \"" + key + "\",\n";
            jsonResponse += "      \"path\": \"" + info.endpointName + "\",\n";
            jsonResponse += "      \"description\": \"" + info.endpointDescription + "\"\n";
            jsonResponse += "    }" + comma + "\n";
        }
        
        jsonResponse += "  ]\n}";
        
        res.set_content(jsonResponse, "application/json");
        res.status = 200;
    });
    
    // Get Standard C++ Version
    svr.Get(endpointDictionary["STD_VERSION_ENDPOINT"].endpointName.c_str(), [](const httplib::Request&, httplib::Response& res) {
		 	//
	        std::unique_ptr<Algorithm> uniquePtr = std::make_unique<Algorithm>(true);   
    		//
			std::string jsonResponse             = uniquePtr->GetCPPSTDVersion(__cplusplus); 
            //
			res.set_content(jsonResponse, "application/json");
            res.status = 200;
    });
    
        
    // Get Application Version
    svr.Get(endpointDictionary["APP_VERSION_ENDPOINT"].endpointName.c_str(), [](const httplib::Request&, httplib::Response& res) {
    		//
 	        std::unique_ptr<Algorithm> uniquePtr = std::make_unique<Algorithm>(true);   
			//
    		std::string jsonResponse  = uniquePtr->configMap["DLL_VERSION"]; 
    		//
            res.set_content(jsonResponse, "application/json");
            res.status = 200;
    });
    
    
    // Get API Version
    svr.Get(endpointDictionary["TESSERACT_VERSION_ENDPOINT"].endpointName.c_str(), [](const httplib::Request&, httplib::Response& res) {
  		  	//
		    std::unique_ptr<TesseractApp> uniquePtr = std::make_unique<TesseractApp>();
			//
    		std::string jsonResponse  = uniquePtr->GetTesseractVersion(); 
    		//
            res.set_content(jsonResponse, "application/json");
            res.status = 200;
    });
    
	// Get Http Server Version
    svr.Get(endpointDictionary["SERVER_VERSION_ENDPOINT"].endpointName.c_str(), [](const httplib::Request&, httplib::Response& res) {
    		//
			std::string jsonResponse  = CppWebServerApp::GetCPPHttpVersion();
            res.set_content(jsonResponse, "application/json");
            res.status = 200;
    });
    
    // UNIT TEST 
    svr.Get(endpointDictionary["TESSERACT_OCR_IMAGE_ENDPOINT"].endpointName.c_str(), [](const httplib::Request&, httplib::Response& res) {
   	    //
	    std::unique_ptr<TesseractApp> uniquePtr = std::make_unique<TesseractApp>();
		
		//
		std::string jsonResponse  = uniquePtr->GetTesseractOcrOutput();
        res.set_content(jsonResponse, "application/json");
        res.status = 200;
    });
    
	// UPLOAD IMAGE FROM HTML FORM VIA POST
    svr.Post("TESSERACT_UPLOAD_ENDPOINT", [](const httplib::Request& req, httplib::Response& res) {
	    if (req.body.empty()) {
	        res.status = 400;
	        res.set_content("{\"error\": \"No image data received\"}", "application/json");
	        return;
	    }
	
	    // req.body holds the raw binary bytes of the uploaded image
	    std::string savedPath = "/app/uploaded_image.png"; 
	    std::ofstream outfile(savedPath, std::ios::binary);
	    outfile.write(req.body.data(), req.body.size());
	    outfile.close();
	
	    // Pass savedPath using .c_str() to match const char*
	    std::unique_ptr<TesseractApp> uniquePtr = std::make_unique<TesseractApp>();
	    
	    // Convert std::string to const char* via .c_str() and capture result
	    std::string jsonResponse = uniquePtr->GetTesseractOcrOutputPath(savedPath.c_str());
	    
        res.set_content(jsonResponse, "application/json");
        res.status = 200;
	});
	
	//
    int port = 8080;
    if (getenv("PORT")) {
        port = std::stoi(getenv("PORT"));
    }

	//
    std::cout << "C++ REST Server listening on port " << port << "...\n";
    svr.listen("0.0.0.0", port);

	//
	return 0;
};

////////////////////////////////////////////////////////////
// MAIN PROGRAM ENTRY
////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	//
	try
	{
		//
		CppWebServerApp* app = new CppWebServerApp();
	
		//
		app->Run();
	} 
	catch(exception &e)
	{
		cout << "Error occurred : [" << e.what() << "]" << endl;
 	}
	
	//
    return 0;
}
