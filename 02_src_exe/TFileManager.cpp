	#include "TFileManager.h"

   	//
	int FileManager::DeleteFile(const char* filePath)
	{
		// Attempt to delete the file
		if (std::remove(filePath) != 0) {
			std::cerr << "Error deleting file: " << filePath << std::endl;
			return 1; // Return an error code
		}
		//
		//cout << "File deleted successfully: " << filePath << std::endl;
		//
		return 0;
	}
	//
	int FileManager::SaveLineToFile(string p_value, const char* filename)
	{
			// Open the file for appending
			ofstream outputFile;
			outputFile.open(filename, std::ios::app);

			if (!outputFile.is_open()) {
				std::cerr << "Error opening file for appending." << std::endl;
				return 1;
			}

			// Write some content to the file
			outputFile << p_value << "\n";

			// Close the file
			outputFile.close();

			return 0;
	}

