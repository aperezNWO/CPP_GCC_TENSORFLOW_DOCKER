/*

    // WINDOWS
    g++ -std=c++17 -O3 CppWebServer.cpp Algorithm.cpp tensorFlowApp.cpp tictactoe.cpp -Iinclude -Llib -ltensorflow -o cpp_web_server.exe -mconsole -lws2_32
    
    // LINUX -- DOCKER (NO .EXE EXTENSION FOR DOCKER!!!)
    	// EDIT DOCKER FILE TO ADD NEEDED FILES
	
   			 COPY --from=builder /app/tensorflow.ini /app/tensorflow.ini
			 COPY --from=builder /app/cpp_web_server /app/cpp_web_server
        // COPY C++ COMMAND TO DOCKER FILE	
			 RUN  <COMMAND_LINE>		 

*/

#include "include/httplib.h"
#include "include/tensorFlowApp.h"
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
	    {"HEALTH_ENDPOINT"                        , {"/health"                        , "Print all endpoints"}},
	    {"PING_ENDPOINT"                          , {"/zero"                          , "Render workaround"}},
	    {"APP_VERSION_ENDPOINT"                   , {"/getAppVersion"                 , "Get Application Version"}},
	    {"STD_VERSION_ENDPOINT"                   , {"/getSTDVersion"                 , "Get Standard C++ Version"}},
	    {"SERVER_VERSION_ENDPOINT"                , {"/getServerVersion"              , "Get Http Server Version"}},
	    {"TENSORFLOW_VERSION_ENDPOINT"            , {"/api/tensorflow/getAPIVersion"  , "Get Tensorflow API Version"}},
	    {"TENSORFLOW_TICTACTOE_PLAY_ENDPOINT"     , {"/api/tictactoe/play"            , "Auto Play TicTacToe with a.i."}},    
	    {"LINEAR_REGRESSION_PREDICT_ENDPOINT"     , {"/api/linearregression/predict"  , "Predict the next hypothetic Apollo Mission data"}},    
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
    
    // Get Http Server Version
    svr.Get(endpointDictionary["SERVER_VERSION_ENDPOINT"].endpointName.c_str(), [](const httplib::Request&, httplib::Response& res) {
    		//
			std::string jsonResponse  = CppWebServerApp::GetCPPHttpVersion();
            res.set_content(jsonResponse, "application/json");
            res.status = 200;
    });

    // Get Application Version
    svr.Get(endpointDictionary["APP_VERSION_ENDPOINT"].endpointName.c_str(), [](const httplib::Request&, httplib::Response& res) {
    		//
 	        std::unique_ptr<Algorithm> uniquePtr = std::make_unique<Algorithm>(true);   
			//
    		std::string jsonResponse  = uniquePtr->GetDLLVersion(); 
    		//
            res.set_content(jsonResponse, "application/json");
            res.status = 200;
    });
    
    // Get API Version
    svr.Get(endpointDictionary["TENSORFLOW_VERSION_ENDPOINT"].endpointName.c_str(), [](const httplib::Request&, httplib::Response& res) {
  		  	//
		    std::unique_ptr<TensorFlowApp> uniquePtr = std::make_unique<TensorFlowApp>();
			//
    		std::string jsonResponse  = uniquePtr->GetTensorFlowAPIVersion(); 
    		//
            res.set_content(jsonResponse, "application/json");
            res.status = 200;
    });
    // TIC TAC TOE PLAY
    // http://localhost:8080/api/tictactoe/play?aiMode=4&temperature=1.5
	svr.Get(endpointDictionary["TENSORFLOW_TICTACTOE_PLAY_ENDPOINT"].endpointName.c_str(), [](const httplib::Request& req, httplib::Response& res) {
	    
	    // 1. Allocate using std::make_unique
	    auto result = std::make_unique<TicTacToeResultOnline>(); 
	    
	    int    aiMode            = 1;
	    bool   hasTemperature    = false;
	    double temperature       = 0.0;
	    std::string jsonResponse = "";
	    
	    // Check and parse aiMode from query string
	    if (req.has_param("aiMode")) {
	        try {
	            aiMode = std::stoi(req.get_param_value("aiMode"));
	            std::cout << "aiMode: " << aiMode << std::endl;
	        } catch (const std::exception& e) {
	            std::cout << "[Error occurred] " << e.what() << std::endl;    
	        }
	    }
	
	    // Check and parse temperature from query string
	    if (req.has_param("temperature")) {
	        try {
	            temperature = std::stod(req.get_param_value("temperature"));
	            hasTemperature = true;
	            std::cout << "temperature: " << temperature << std::endl;
	        } catch (const std::exception& e) {
	            std::cout << "[Error occurred] " << e.what() << std::endl;    
	        }
	    }
	  
	    try {
	        double temp = hasTemperature ? temperature : ((aiMode == 1) ? 1.5 : 0.1);
	        
	        // 2. Dereference the smart pointer using *result when passing to function
	        bool boolResponse = RunTicTacToeSelfPlay(*result, aiMode, temp);
	        
	        std::stringstream ss;
	        ss << "{";
	        
	        // 3. Access members using the arrow operator -> just like a raw pointer
	        ss << "\"finalBoard\":[";
	        for (int i = 0; i < 9; ++i) {
	            ss << result->finalBoard[i];
	            if (i < 8) ss << ",";
	        }
	        ss << "],";
	        
	        ss << "\"moves\":[";
	        int maxMoves = (result->moveCount < 9) ? result->moveCount : 9;
	        for (int i = 0; i < maxMoves; ++i) {
	            ss << result->moves[i];
	            if (i < maxMoves - 1) ss << ",";
	        }
	        ss << "],";
	        
	        std::string winnerStr = "Draw";
	        if (result->winner == 1) winnerStr = "X";
	        else if (result->winner == -1) winnerStr = "O";
	        ss << "\"winner\":\"" << winnerStr << "\",";
	        
	        ss << "\"moveCount\":" << result->moveCount << ",";
	        ss << "\"historyCount\":" << result->historyCount << ",";
	        
	        ss << "\"history\":[";
	        int maxHistory = (result->historyCount < 10) ? result->historyCount : 10;
	        for (int i = 0; i < maxHistory; ++i) {
	            ss << "[";
	            for (int j = 0; j < 9; ++j) {
	                ss << result->history[i][j];
	                if (j < 8) ss << ",";
	            }
	            ss << "]";
	            if (i < maxHistory - 1) ss << ",";
	        }
	        ss << "]";
	        ss << "}";
	        jsonResponse = ss.str();
	
	    } catch (const std::exception& e) {
	        std::cout << "[Error occurred] " << e.what() << std::endl;    
	    }
	
	    // No need for 'delete result;' here! 
	    // std::unique_ptr automatically cleans up when it goes out of scope.
	
	    res.set_content(jsonResponse, "application/json");
	    res.status = 200;
	});
 
 
	// PREDICT THE NEXT HYPOTHETIC APOLLO MISISION USILNG LINERAR REGRESSION
	// https://tensorflownetcore64.tryasp.net/api/linearregression/predict?missionNumberToPredict=18
	// {"input_mission_number":18,"predicted_total_time_hours":302.2483870967742,"predicted_duration_days":12.593682795698925}
	svr.Get(endpointDictionary["LINEAR_REGRESSION_PREDICT_ENDPOINT"].endpointName.c_str(), [](const httplib::Request& req, httplib::Response& res) {
	    std::string jsonResponse = "{}"; // Default fallback
	    
	    if (req.has_param("missionNumberToPredict")) {
	        try {
	            // 1. Parse input mission number from query parameters
	            double missionNumberToPredict = std::stod(req.get_param_value("missionNumberToPredict"));
	            int inputMissionNumber = static_cast<int>(missionNumberToPredict);
	            
	            // 2. Run linear regression prediction using a smart pointer for memory safety
	            auto linearRegression = std::make_unique<LinearRegression>();
	            double predictedTotalTimeHours = linearRegression->predict(missionNumberToPredict);
	            
	            // 3. Calculate derived metrics
	            double predictedDurationDays = predictedTotalTimeHours / 24.0;
	            
	            std::cout << "predictedTotalTimeHours: " << predictedTotalTimeHours << std::endl;
	            
	            // 4. Build the structured JSON response with precision formatting
	            std::stringstream ss;
	            ss << std::fixed << std::setprecision(15);
	            
	            ss << "{";
	            ss << "\"input_mission_number\":" << inputMissionNumber << ",";
	            ss << "\"predicted_total_time_hours\":" << predictedTotalTimeHours << ",";
	            ss << "\"predicted_duration_days\":" << predictedDurationDays;
	            ss << "}";
	            
	            jsonResponse = ss.str();
	            res.status = 200;
	
	        } catch (const std::exception& e) {
	            std::cout << "[Error occurred] " << e.what() << std::endl;
	            jsonResponse = "{\"error\": \"" + std::string(e.what()) + "\"}";
	            res.status = 500;
	        }
	    } else {
	        jsonResponse = "{\"error\": \"Missing parameter 'missionNumberToPredict'\"}";
	        res.status = 400;
	    }
	
	    res.set_content(jsonResponse, "application/json");
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
