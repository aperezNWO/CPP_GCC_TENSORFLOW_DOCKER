///////////////////////////////////////////////////////////////////////////
// DIJKSTRA ALGORITHM
///////////////////////////////////////////////////////////////////////////

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <map>

using std::string;
using std::map;
using std::vector;

class Dijkstra  
{
public:
    Dijkstra();
    ~Dijkstra();
    int                      ReadConfigFile(const char* fileName);
    std::vector<std::string> StringSplit(const char* p_inputString, std::string p_delimiter, bool adjust);
    int                      minDistance(std::vector<int> dist, std::vector<bool> sptSet, int p_vertexSize);
    std::string              GetDijkstra(std::vector<std::string> vertex, int p_vertexSize, int p_sampleSize, int p_sourcePoint);
    void                     SetDijkstra(int src, int vertexSize);
    float                    Pitagorean(float coord_x, float coord_y);
    float                    GetHipotemuza(const char* vertexString, int index_x, int index_y);
    std::string              GenerateRandomMatrix(const char* vertexString, int p_vertexSize);
    std::string              GetRandomPoints(int p_vertexSize, int p_sourcePoint);

public:
    std::vector<int>              dist;
    std::vector<std::string>      path;
    std::vector<std::vector<int>> graph;
    std::map<string, string> configMap;
};

#endif // DIJKSTRA_H