#include "SortBenchMark.h"

	//
	SortBenchMark::SortBenchMark(const char* p_randomValues, FormatType p_format) 
	{
		//
		this->ReadConfigFile("Algorithm.ini");
		//
		this->format         = p_format;
		this->randomValues   = p_randomValues;
		this->arraySize      = std::stoi(this->configMap["ARRAY_SIZE"]);

		std::cout << "array size     : " << this->arraySize << endl;
		std::cout << "random values  : " << p_randomValues  << endl;
		//-----------------------------------------------------
		// INICIAR ARREGLO
		//-----------------------------------------------------
		vector<string>   arreglo_str = StringSplit(p_randomValues,"|");
		//
		for (int i = 0; i < this->arraySize; i++) {
			//
			this->arreglo.push_back( std::stoi(arreglo_str[i] ) );
		}

		// TMP IS A TEMPORARY LINUX FOLDER WITH PERMISSIONS
		//this->_fileManager.DeleteFile("/tmp/ArrayUnsorted.txt");
		//this->_fileManager.SaveVectorToFile(this->arreglo,"/tmp/ArrayUnsorted.txt");

	};
	//
	SortBenchMark::~SortBenchMark()
	{
		 //
	};
	//
	int SortBenchMark::ReadConfigFile(const char* fileName)
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
	vector<string> SortBenchMark::StringSplit(const char* p_inputString, std::string p_delimiter)
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
	void SortBenchMark::SaveStep(int arr[])
	{
		//----------------------------------------------------
		// IMPRIMIR PASO INTERMEDIO
		//-----------------------------------------------------
		//
		string tempValues = "";
		for (int index = 0; index < this->arraySize; index++)
		{
		  //
		  stringstream  ss;
		  //
		  string separator = (index == 0) ? "" : ((this->format == FormatType::HTML)? "<br/>" : ",");
		  ss << separator << arr[index];
		  //
		  string _tempValues  = ss.str();
		  //
		  tempValues          += _tempValues;
		}
		//
		this->sortSteps.push_back(tempValues);
	}
	//
	string SortBenchMark::GetSort(int p_sortAlgorithm)
	{
		//
		switch (p_sortAlgorithm)
		{
			case 1: // BUBBLE SORT
				this->BubbleSort();
				break;
			case 2: // QUICK SORT
				this->QuickSort();
				break;
		};

		//
		string sortedList = (this->format==FormatType::HTML)? "" : "{";
		int stepNumber    = 0;
		// Using iterators
		for (auto sortStep = this->sortSteps.begin(); sortStep != this->sortSteps.end(); ++sortStep) {
				  //
				  stepNumber++;
				  //
				  stringstream  ss;
				  //
				  std::string separator = (this->format == FormatType::HTML) 
								    ? "~" 
								    : ( (stepNumber == 1)?  "" : ",") + ("\"step " + std::to_string(stepNumber) + " of " + std::to_string( this->sortSteps.size() )  + "\":");
    
				  ss << separator << ((this->format == FormatType::HTML)? "<br/>":"[") << *sortStep << ((this->format == FormatType::HTML)? "<br/>":"]") ;
				  //
				  string _tempValues  = ss.str();
				  //
				  sortedList          += _tempValues;

		} 
		sortedList += (this->format==FormatType::HTML)? "" : "}";
				
		// TMP FOLDER IS A DEFAULT LINUX TEMPOORARY FOLDER WITH PERMISSIONS
		/*
		switch (this->format) {
			case FormatType::HTML: // HTML
			{
				this->_fileManager.DeleteFile("/tmp/ArraySorted.txt");
				this->_fileManager.SaveLineToFile(sortedList,"/tmp/ArraySorted.txt");	
			}
			break;		
			case FormatType::JSON: // JSON
			{
				this->_fileManager.DeleteFile("/tmp/ArraySorted.json");
				this->_fileManager.SaveLineToFile(sortedList,"/tmp/ArraySorted.json");
			}
		};
		*/
		//
		return sortedList;
	};
	//
	void SortBenchMark::QuickSort()
	{
		//----------------------------------------------------
		// CONFIGURAR VALORES INICIALES
		//-----------------------------------------------------
		//
		int* _arreglo     = new int[this->arraySize];
		//
		for (int i = 0; i < this->arraySize; i++) {
			  _arreglo[i] = arreglo[i];
		}
		//----------------------------------------------------
		// INGRESAR A RECURSION
		//-----------------------------------------------------
		quickSort(_arreglo,0 , this->arraySize - 1);

		//----------------------------------------------------
		// IMPRIMIR VALOES FINALES
		//-----------------------------------------------------
		SaveStep(_arreglo);

		//----------------------------------------------------
		// REMOVER VALORES REPETIDOS
		//-----------------------------------------------------

		// Use std::unique to move duplicates to the end
		auto last = std::unique(this->sortSteps.begin(), this->sortSteps.end());

		// Erase the duplicates from the vector
		this->sortSteps.erase(last, this->sortSteps.end());
	}
	//
	int SortBenchMark::partition(int arr[],int low,int high)
	{
	  //choose the pivot
	  int pivot=arr[high];
	  //Index of smaller element and Indicate
	  //the right position of pivot found so far
	  int i=(low-1);
	  //
	  for(int j=low;j<=high;j++)
	  {
		  //If current element is smaller than the pivot
		  if(arr[j]<pivot)
		  {
			//Increment index of smaller element
			i++;
			std::swap(arr[i],arr[j]);
			SaveStep(arr);
		  }
	  }
	  //
	  std::swap(arr[i+1],arr[high]);
	  SaveStep(arr);
	  //
	  return (i+1);
	}
	//
	void SortBenchMark::quickSort(int arr[],int low,int high)
	{
	  // when low is less than high
	  if(low<high)
	  {
		// pi is the partition return index of pivot

		int pi=partition(arr,low,high);

		//Recursion Call
		//smaller element than pivot goes left and
		//higher element goes right
		quickSort(arr,low,pi-1);
		quickSort(arr,pi+1,high);
	  }
	}
	//
	void SortBenchMark::BubbleSort()
	{
	   //
	   int temp = 0;
	   //
	   for (int write = 0; write < this->arraySize; write++)
	   {
		   //
		   for (int sort = 0; sort < this->arraySize - 1; sort++)
		   {
			   //
			   if (arreglo[sort] > arreglo[sort + 1])
			   {
				   //
				   temp              = arreglo[sort + 1];
				   arreglo[sort + 1] = arreglo[sort];
				   arreglo[sort]     = temp;

				  //-----------------------------------------------------
				  // GUARDAR PASO DEL ARREGLO EN MATRIZ DE PASOS
				  //-----------------------------------------------------
				  //
				  string tempValues = "";
				  //
				  for (int j = 0; j < this->arraySize; j++)
				  {
					  //
					  stringstream  ss;
					  //
					  string separator = (j == 0)? "" : ((this->format==FormatType::HTML)?"<br/>":",");
					  ss << separator << arreglo[j];
					  //
					  string _tempValues  = ss.str();
					  //
					  tempValues          += _tempValues;
				  }
				  //
				  this->sortSteps.push_back(tempValues);
			   }
		   }
	   }
	}

