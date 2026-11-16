///////////////////////////////////////////////////////////////////////////
// REGEX MANAGER
///////////////////////////////////////////////////////////////////////////

#include "RegExManager.h"

//
RegExManager::RegExManager() 
{
	//
	this->ReadConfigFile("Algorithm.ini");
	//
	this->GetXml();
}
//
RegExManager::~RegExManager()
{
	//
}
//
int RegExManager::ReadConfigFile(const char* fileName)
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
//
string RegExManager::HtmlEncode(const std::string& input)
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
string RegExManager::StringTrim(const std::string& str)
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
int RegExManager::GetXml()
{
	// UNABLE TO READ FILENAME FROM CONFIG FILE
	//const char* filePath        = this->configMap["XMLPATH"].c_str();
	const char* filePath        = "CdCatalog.xml";

	std::ifstream inputFile(filePath);
	if (!inputFile.is_open()) {
	    std::cerr << "Failed to open [" << filePath << "]" << std::endl;
	}
	// Read and print the contents of the file
	std::string line;
	while (std::getline(inputFile, line))
	{
		//
		this->xmlItems.push_back(line);
	}

	// Close the file
	inputFile.close();

	//
	return 0;
}
//
string RegExManager::RegExEval(char* p_tagSearch, char* p_textSearch)
{
		//
		int matchCount        = 0;
		string textMatch      = "";
		string _p_tagSearch   = StringTrim(p_tagSearch);
		string _p_textSearch  = StringTrim(p_textSearch);
		//
		std::ostringstream oss_regex;
		//
		oss_regex << "((<" << _p_tagSearch << ".*>)(.*" << _p_textSearch << ".*)(<\\/" << _p_tagSearch << ">))";
		//
		string strRegex    = oss_regex.str();
		//
		std::regex regexExp(strRegex, std::regex_constants::icase);
		//
		std::smatch regexMatch;
		//
		for (auto _xmlItem = this->xmlItems.begin(); _xmlItem != this->xmlItems.end(); ++_xmlItem) {
			//
			stringstream  ss;
			//
			ss << *_xmlItem;
			//
			string xmlItem     = ss.str();
			//
			if (std::regex_search(xmlItem, regexMatch, regexExp)) {
				//
				string openingTag = "<"  + _p_tagSearch + ">";
				string closingTag = "</" + _p_tagSearch + ">";
				//
				xmlItem.replace(xmlItem.find(openingTag) ,openingTag.length(), "[" + openingTag);
				xmlItem.replace(xmlItem.find(closingTag) ,closingTag.length(), closingTag + "]");
				//
				matchCount++;
			}
			//
			textMatch += (xmlItem + '\n');
		}
	//
	string encodedTextMatch = this->HtmlEncode(textMatch);
	//
	std::ostringstream oss_result;
	//
	oss_result << std::to_string(matchCount) << "|" << encodedTextMatch << "|" << strRegex << std::endl;
	//
	return oss_result.str();
}
