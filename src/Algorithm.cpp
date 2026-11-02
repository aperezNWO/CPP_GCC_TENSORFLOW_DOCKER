#include "include/Algorithm.h"

	Algorithm::Algorithm(bool readConfigFile)
	{
		 //
		 if (readConfigFile)
		 {
		 	this->ReadConfigFile("tesseract.ini");
		 }

	}
	//
	Algorithm::~Algorithm()
	{
		//
	}
	//
	string Algorithm::HtmlEncode(const std::string& input)
	{
		//
		std::ostringstream encoded;
		//
		for (char c : input) {
			//
			if (std::isspace(c))
			{
				encoded << "&nbsp;";
			}
			//
			switch (c) {
				case '&': encoded << "&amp;";
					break;
				case '<': encoded << "&lt;";
					break;
				case '>': encoded << "&gt;";
					break;
				case '\"': encoded << "&quot;";
					break;
				case '\'': encoded << "&apos;";
					break;
				case '\n': encoded << "<br/>";
					break;
				case '\t': encoded << "&nbsp;";
					break;
				case '[': encoded << "<mark>";
					break;
				case ']': encoded << "</mark>";
					break;
				default: encoded << c;
			}
		}
		return encoded.str();
	}
	//
	string Algorithm::StringTrim(const std::string& str)
	{
		//
		size_t start = str.find_first_not_of(" \t\n\r");   // Find the first non-whitespace character
		size_t end   = str.find_last_not_of(" \t\n\r");    // Find the last non-whitespace character
		//
		if (start == std::string::npos || end == std::string::npos) {
			// The string is empty or contains only whitespaces
			return "";
		}
		//
		return str.substr(start, end - start + 1);
	};
	//
	vector<string> Algorithm::StringSplit(const char* p_inputString, std::string p_delimiter)
	{
		//
		std::string s(p_inputString);
		//
		std::vector<std::string> outputArr;
		size_t pos = 0;
		std::string token;
		//
		while ((pos = s.find(p_delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			s.erase(0, pos + p_delimiter.length());
			outputArr.push_back(token);
		}
		outputArr.push_back(s);
		//
		return outputArr;
	};
	//
	int Algorithm::ReadConfigFile(const char* fileName)
	{
		 // Open the configuration file
		std::ifstream configFile(fileName);

		// Check if the file is opened successfully
		if (!configFile.is_open()) {
			std::cerr << "Error opening the configuration file." << std::endl;
			return 1;
		}

		// Read the file line by line
		std::string line;
		while (std::getline(configFile, line)) {
			// Skip empty lines or lines starting with '#' (comments)
			if (line.empty() || line[0] == '#') {
				continue;
			}

			// Split the line into key and value
			std::istringstream iss(line);
			std::string key, value;
			if (std::getline(iss, key, '=') && std::getline(iss, value))
			{
				// Trim leading and trailing whitespaces from key and value
				key.erase(0, key.find_first_not_of(" \t"));
				key.erase(key.find_last_not_of(" \t") + 1);
				value.erase(0, value.find_first_not_of(" \t"));
				value.erase(value.find_last_not_of(" \t") + 1);

				// Insert key-value pair into the map
				this->configMap[key] = value;
			}
		}

		// Close the configuration file
		configFile.close();

		//
		return 0;
	}

	void Algorithm::ReplaceAll(std::string &str, const std::string &from, const std::string &to) 
	{
		//
		size_t startPos = 0;
		//
		while ((startPos = str.find(from, startPos)) != std::string::npos) {
			str.replace(startPos, from.length(), to);
			startPos += to.length(); // Move to the next position after replacement
		}
	}
	//
	const char* Algorithm::GetDLLVersion()
	{
		//
		return this->configMap["DLL_VERSION"].c_str();
	}
	//
	const char* Algorithm::GetCPPSTDVersion(long int cppVersion)	
	{
		//
		cout << "cpp Version short : " << cppVersion << endl;
		//
		switch (cppVersion) {
		        case 199711L: return "C++98/C++03";
		        case 201103L: return "C++11";
		        case 201402L: return "C++14";
		        case 201703L: return "C++17";
		        case 202002L: return "C++20";
			    case 202100L: return "C++23 Prerelase";
		        case 202302L: return "C++23";
		        default: {
		        	if ((cppVersion >   202100L)
		        	     && 
					    (cppVersion <=  202302L))
					return "C++23 Prerelease";
		        	
		        	return "Unknown C++ Standard";
					break;
				}
		}
	}
	///////////////////////////////////////////////////////////////////////////
