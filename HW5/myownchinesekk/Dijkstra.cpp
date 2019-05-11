#include "Graph.cpp"
#include "MinHeap.cpp"
#include <limits>
using namespace std;

pair<vector<int>,vector<double>> Dijkstra(Graph &G,int start_vertex,vector<double> &cost)
{
  MinHeap M;
  int n=G.getamtVertexes();

  vector<int> initial(n,-1);

}
