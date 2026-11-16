///////////////////////////////////////////////////////////////////////////
// CLASS DIJKSTRA - METHOD IMPLEMENTATIONS
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// SOURCES:
// https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
///////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <limits.h>
#include <iomanip>
#include <memory>
#include <chrono>
#include <map>
#include "Dijkstra.h"

using namespace std;

#pragma hdrstop
#pragma argsused

	//
	Dijkstra::Dijkstra() :  graph(), dist(), path()
	{
		//
		this->ReadConfigFile("Algorithm.ini");
	}
	//
	Dijkstra::~Dijkstra()
	{
		 //
	}
	//
	int Dijkstra::ReadConfigFile(const char* fileName)
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
	vector<string> Dijkstra::StringSplit(const char* p_inputString, std::string p_delimiter, bool adjust)
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
		// Printing Array to see the results
		int outputArrSize = outputArr.size();
		for ( int i=0;i<outputArrSize;i++){
				//
				if ( adjust == true) {
					std::string coordinate = outputArr[i];
					coordinate.replace(coordinate.find("["),1,"");
					coordinate.replace(coordinate.find("]"),1,"");
					outputArr[i] = coordinate;
				}
		}
		//
		return outputArr;
	}
	//
	int Dijkstra::minDistance(vector<int> dist, vector<bool> sptSet, int p_vertexSize)
	{
		// Initialize min value
		int min = INT_MAX, min_index;

		for (int v = 0; v < p_vertexSize; v++)
			if (sptSet[v] == false && dist[v] <= min)
				min = dist[v], min_index = v;

		return min_index;
	}
	//
	string Dijkstra::GetDijkstra(vector<string> vertex, int p_vertexSize, int p_sampleSize, int p_sourcePoint)
	{
		//
		string status;
		// Driver Code
		SetDijkstra(p_sourcePoint, p_vertexSize);
		//
		string integerFormat = "00";
		//
		for (int index = 0; index < p_vertexSize; index++)
		{
			// CORREGIR VALORES
			if (dist[index] >= INT_MAX)
			{
				dist[index] = 0;
			}
			//
			string separator            = (index < (p_vertexSize - 1)) ? "<br/>" : "";
			//
			string vertexIndex          = vertex[index].replace(vertex[index].find(","),1,";");
			//
			std::ostringstream oss_index;
			oss_index << std::setw(2) << std::setfill('0') << index;
			std::string formattedIndex = oss_index.str();
			//
			std::ostringstream oss_dist;
			oss_dist << std::setw(2) << std::setfill('0') << dist[index];
			std::string formattedDist  = oss_dist.str();
			//
			stringstream  ss;
			ss     << formattedIndex
				   <<"&lt;"
				   << vertexIndex
				   << "&gt;-"
				   << formattedDist
				   << "-"
				   << path[index]
				   << separator;
			//
			string statusItem = ss.str();
			status += statusItem;
		}
		//
		return status;
	}
	// Function that implements Dijkstra's single source
	// shortest path algorithm for a graph represented using
	// adjacency matrix representation
	void Dijkstra::SetDijkstra(int src, int V)
	{
		
		try
		{
			vector<bool> sptSet; // sptSet[i] will be true if vertex i is
	
			// Initialize path
			// Initialize all distances as INFINITE and stpSet[] as false
			for (int i = 0; i < V; i++)
			{
				dist.push_back(INT_MAX);
				sptSet.push_back(false);
				path.push_back(string(""));
			}
	
			// Distance of source vertex from itself is always 0
			dist[src] = 0;
	
			// Find shortest path for all vertices
			for (int count = 0; count < V - 1; count++) {
				// Pick the minimum distance vertex from the set of
				// vertices not yet processed. u is always equal to
				// src in the first iteration.
				int u = minDistance(dist, sptSet, V);
	
				// Mark the picked vertex as processed
				sptSet[u] = true;
	

				// Update dist value of the adjacent vertices of the
				// picked vertex.
				for (int v = 0; v < V; v++)
				{
					if (u >= graph.size() || v >= graph[u].size()) {
				    	throw std::out_of_range("Index out of bounds in SetDijkstra");	
                    }

					// Update dist[v] only if is not in sptSet,
					// there is an edge from u to v, and total
					// weight of path from src to  v through u is
					// smaller than current value of dist[v]
					if (!sptSet[v] && graph[u][v]
						&& dist[u] != INT_MAX
						&& dist[u] + graph[u][v] < dist[v])
					{
						dist[v] = dist[u] + graph[u][v];
	
						//path[v] = path[u] + string.Format("[{0},{1}]≡", u,v);
						stringstream ss;
						string       pathSeparator  = "≡";
						//
						ss << "[" << u << ";" << v << "]" << pathSeparator;
						path[v] = path[u] + ss.str().c_str();
					}					
				}
			}
		}
		catch (const std::exception& e) {
	        std::cerr << "Error in SetDijkstra at line: " << __LINE__
	                  << ". Message: " << e.what() << std::endl;
	        throw;
    	}  

	}
	//
	float Dijkstra::Pitagorean(float coord_x, float coord_y)
	{
		//
		double pitagorean = 0;
		double power      = 2;

		pitagorean        = sqrt(
						std::pow(coord_x, power)
						+
						std::pow(coord_y, power)
		);

		//
		return pitagorean;
	}
	//
	float Dijkstra::GetHipotemuza(const char*vertexString, int index_x, int index_y)
	{

		
		//--------------------------------------------------------------
		//   string split
		//--------------------------------------------------------------
		std::string   p_delimiter = "|";
		vector<string> outputArr  = StringSplit(vertexString,p_delimiter,true);

		std::string  coord_source   = outputArr[index_y];
		std::string  coord_dest     = outputArr[index_x];

		std::vector<string> coord_source_array = StringSplit(coord_source.c_str(),",",false);
		std::vector<string> coord_dest_array   = StringSplit(coord_dest.c_str(),",",false);

		if (index_x >= outputArr.size() || index_y >= outputArr.size()) {
		    throw std::out_of_range("Index out of bounds in GetHipotemuza");
		}
		//
		float coord_source_x    = std::stof(coord_source_array[0]);
		float coord_source_y    = std::stof(coord_source_array[1]);
		float coord_dest_x      = std::stof(coord_dest_array[0]);
		float coord_dest_y      = std::stof(coord_dest_array[1]);
		//
		float coord_x = std::fabs(coord_dest_x - coord_source_x);
		float coord_y = std::fabs(coord_dest_y - coord_source_y);
		//
		float hipotemuza = Pitagorean(coord_x, coord_y);

		//
		return hipotemuza;
	}
	//
	string Dijkstra::GenerateRandomMatrix(const char* vertexString, int p_vertexSize)
	{
		try
		{
			//--------------------------------------------------------------
			// CREAR MATRIZ
			//--------------------------------------------------------------
			// Resize the matrix to the desired number of rows
			this->graph.resize(p_vertexSize);
			// Resize each row to the desired number of columns
			for (int i = 0; i < p_vertexSize; i++) {
				this->graph[i].resize(p_vertexSize);
			}
	
			//--------------------------------------------------------------
			// LA PARTE DIAGONAL DE LA MATRIZ SIEMPRE SERA 0
			// lA DISTANCIA ENTRE UN PUNTO Y EL MISMO SIEMPRE ES CERO
			//--------------------------------------------------------------
			this->graph.resize(p_vertexSize, std::vector<int>(p_vertexSize, 0));
			
			for (int index_x = 0; index_x < p_vertexSize; index_x++)
			{
				for (int index_y = 0; index_y < p_vertexSize; index_y++) {
					
				     if (index_x >= p_vertexSize || index_y >= p_vertexSize) {
            				throw std::out_of_range("Index out of bounds in GenerateRandomMatrix");
       				 }
					
					if (index_x < graph.size() && index_y < graph[index_x].size()) {
					    this->graph[index_x][index_y] = 0;
					} else {
					    throw std::out_of_range("Index out of bounds");
					}
				}
			}
	
			//--------------------------------------------------------------
			// LLENAR EL RESTO DE LA MATRIZ DE FORMA ALEATORIA
			//--------------------------------------------------------------
			//
			for (int index_x = 0; index_x < p_vertexSize; index_x++)
			{
				//
				for (int index_y = (index_x + 1); index_y < p_vertexSize; index_y++)
				{
					
					if (index_x >= p_vertexSize || index_y >= p_vertexSize) {
            			throw std::out_of_range("Index out of bounds in GenerateRandomMatrix");
      				}
					// Seed the generator with a random value om a hardware device
					std::mt19937        mt_3;
					std::random_device  rd_3;
					mt_3                 = std::mt19937(rd_3());
					std::uniform_int_distribution<int> distribution(0, 1);
					int randomValue      = distribution(mt_3);
					int hipotemuza       = 0;
	
					//--------------------------------------------------------------
					// EN VALORES POSITIVOS LLENAR LA MATRIZ CON DISTANCIAS
					//--------------------------------------------------------------
	
					if (randomValue == 1)
					{
						//
						hipotemuza = GetHipotemuza(vertexString, index_x, index_y);
					}
					//
					this->graph.resize(p_vertexSize, std::vector<int>(p_vertexSize, 0));
					//
					this->graph[index_x][index_y] = hipotemuza;
					this->graph[index_y][index_x] = hipotemuza;
				}
			}
	
			//----------------------------------------------------
			// GARANTIZAR CONECTIVIDAD DE AL MENOS UN PUNTO
			//----------------------------------------------------
			for (int index_x = 0; index_x < p_vertexSize; index_x++)
			{
				//
				int zeroCount = 0;
	
				//
				for (int index_y = 0; index_y < p_vertexSize; index_y++)
				{
					
					if (index_x >= p_vertexSize || index_y >= p_vertexSize) {
            			throw std::out_of_range("Index out of bounds in GenerateRandomMatrix");
      				}
					// DESCARTA DIAGONAL Y VERIFICAR EXISTENCIA DE VALOR "CERO"
					if ((index_x != index_y) && (graph[index_x][index_y] == 0))
					{
						//
						zeroCount++;
	
						// GARANTIZAR CONECTIVIDAD DE AL MENOS UN PUNTO
						if (zeroCount == (p_vertexSize - 1))
						{
							//
							int hipotemuza          = static_cast<int>(std::round(GetHipotemuza(vertexString, index_x, index_y)));
							this->graph.resize(p_vertexSize, std::vector<int>(p_vertexSize, 0));
							this->graph[index_x][index_y] = hipotemuza;
							this->graph[index_y][index_x] = hipotemuza;
						}
					}
				}
			}
			//--------------------------------------------------------------------
			// REPRESENTAR MATRIZ EN CADENA
			//--------------------------------------------------------------------
			//
			string statusMessage = "";
			//
			for (int index_x = 0; index_x < p_vertexSize; index_x++) {
	
				//
				string separator_1 = (index_x < p_vertexSize - 1) ? "|" : "";
				//
				string stringArray       = "";
				//
				for (int index_y = 0; index_y < p_vertexSize; index_y++)
				{
					//
					stringstream  ss_y;
					//
					string separator_2   = (index_y < p_vertexSize - 1)? "," : "";
					ss_y << graph[index_x][index_y]<< separator_2;
					string vertexItem_y  = ss_y.str();
					//
					stringArray          += vertexItem_y;
				}
				//
				stringstream  ss_x;
				ss_x          << "{" << stringArray << "}";
				//
				string stringArrayValues = ss_x.str();
				//
				statusMessage += stringArrayValues + separator_1;
			}
	
			//
			return statusMessage;			
	    } 
		catch (const std::exception& e) {
	        std::cerr << "Error in GetHipotemuza at line: " << __LINE__
	                  << ". Message: " << e.what() << std::endl;
	        throw;
		}

	}
	//
	string Dijkstra::GetRandomPoints(int p_vertexSize, int p_sourcePoint)
	{
		try
		{
			//
			int  p_sampleSize        = std::stoi(this->configMap["ARRAY_SIZE"]) - 1;
			p_sampleSize             = p_sampleSize - 2;    // remover extremos de coordenadas
			vector<int>  vertex_X;
			vector<int>  vertex_Y;
			//
			for (short i = 0; i < p_sampleSize; i++)
			{
				//
				vertex_X.push_back(i + 1); // remover extremos de coordenadas
				vertex_Y.push_back(i + 1); // remover extremos de coordenadas
			}
	
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::default_random_engine generator(seed);
			//
			std::shuffle ( vertex_Y.begin(), vertex_Y.end() , generator);
			std::shuffle ( vertex_X.begin(), vertex_X.end() , generator);
	
			//
			string _vertexArrayString = "";
			//
			for (int index = 0; index < p_vertexSize; index++) {
				//
				stringstream  ss;
				//
				string separator   = (index < p_vertexSize - 1)? "|" : "";
				ss << "[" << vertex_X[index] << "," << vertex_Y[index] << "]" << separator;
				string vertexItem  = ss.str();
				//
				_vertexArrayString += vertexItem;
			}
			//
			string _vertexMatrix = GenerateRandomMatrix(_vertexArrayString.c_str(), p_vertexSize);
			//
			string vertexList    = GetDijkstra(StringSplit(_vertexArrayString.c_str(),"|",false), p_vertexSize, p_sampleSize, p_sourcePoint);
			//
			string        separator_1("~");
			stringstream  ss_statusMessage;
			ss_statusMessage << _vertexArrayString << separator_1 << _vertexMatrix << separator_1 << vertexList;
			string s_statusMessage      = ss_statusMessage.str();
			//
			return s_statusMessage;		
		}
		catch (const std::exception& e) {
	        std::cerr << "Error in GetHipotemuza at line: " << __LINE__
	                  << ". Message: " << e.what() << std::endl;
	        throw;
		}
	}
