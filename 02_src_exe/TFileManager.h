///////////////////////////////////////////////////////////////////////////
// FILE MANAGER
///////////////////////////////////////////////////////////////////////////

#ifndef TFileManager_H // include guard
#define TFileManager_H
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

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


class FileManager
{
	public :
		int     DeleteFile(const char* filePath);
		int     SaveLineToFile(string p_value, const char* filename);
	public :
		//
		template <typename T> int SaveVectorToFile(std::vector<T> array, const char* fileName)
		{
			// Open the file for appending
			std::ofstream outputFile;
			outputFile.open(fileName, std::ios::app);
			if (!outputFile.is_open()) {
				std::cerr << "Error opening file for appending." << std::endl;
				return 1;
			}
			// Write some content to the file
			for (auto _Item = array.begin(); _Item != array.end(); ++_Item)
			{
				//
				std::stringstream  ss;
				//
				ss << *_Item;
				//
				string Item     = ss.str();
				// Write some content to the file
				outputFile << Item << "\n";
			}
			// Close the file
			outputFile.close();
			return 0;
		};
};


