///////////////////////////////////////////////////////////////////////////
// ALGORITHM.LIBRARY.DLL
///////////////////////////////////////////////////////////////////////////

#ifndef SortBenchMark_H // include guard
#define SortBenchMark_H
#endif

#include "TFileManager.h"
#include <unordered_map>
#include <map>

#include <algorithm>

using namespace std;

#pragma hdrstop
#pragma argsused

class SortBenchMark	 
{
	public  :
	   SortBenchMark(const char* p_randomValues, FormatType p_format);
	   ~SortBenchMark();
	   int            ReadConfigFile(const char* fileName);
	   vector<string> StringSplit(const char* p_inputString, std::string p_delimiter);
	   void           SaveStep(int arr[]);
	   string	      GetSort(int p_sortAlgoritm);
	   void           QuickSort();
	   int            partition(int arr[],int low,int high);
	   void           quickSort(int arr[],int low,int high);
	   void           BubbleSort();
	public  :
	   const char*      randomValues;
	   FormatType       format;
	   int              arraySize;
	   vector<int>      arreglo;
	   vector<string>   sortSteps;
	   FileManager      _fileManager;
	   std::map<string, string> configMap;
};
