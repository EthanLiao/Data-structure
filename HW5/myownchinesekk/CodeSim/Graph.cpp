#include <list>
#include <vector>
using namespace std;

class Graph
{
  private :
  //number of vertexes,n
  int amtVertexes;

  //nunber of edges,m
  int amtEdges;

  //Adjacency list for graph
  vector<list<int>> adjList;

  //Adjacency matrix for graph (use two dimensional vector for strage the graph)
  vector<vector<int>> adjMat;

  //One dimesional Array for edges
  vector<pair<int,int>> edges;

  //Two dimensional Indexes of edges(use two dimensional vector for edgeIndexes)
  vector<vector<int>> edgeIndexes;


  public :
  // n is number of vertexes
  // edges are element of pair represent two nodes,default = empty list
  Graph(int n,list<pair<int,int>> &edges=list<pair<int,int>>());
                                 // inintialize as a empty list
  //initialize a empty graph : deefault constructor
  Graph():amtVertexes(0),amtEdges(0){};
  // add a new vertex to graph
  //void addVertex();

  // add a new edge to graph
  void addEdges(int u,int v);

  //given a endpoint ,return the index of edges
  int getedgeIndexes(int u,int v){return edgeIndexes[u][v];};

  // given edge index(mark evry edge if use),return the edge endpoint as type of pair
  pair<int,int> getEdge(int i){return pair<int,int> (edges[i].first,edges[i].second);};


  // get the number of edges
  int getamtEdges(){return amtEdges;};

  // get the number of vertexes
  int getamtVertexes(){return amtVertexes;};


  // give a vertex and then return the list of node
  list<int>& adjList(int v){return adjList[v];};

  // return all the graph matrix
  vector<vector<int>> & adjMatrix{return adjMat};

};



Graph::Graph(int V,list<pair<int,int>> &edges):
amtVertexes(V),
amtEdges(edges.size()),
adjList(V),
adjMat(V,vector<int>(V,false)),
edges(),
edgeIndexes(V,vector<int>(V,-1))
{
  for(list<pair<int,int>> :: iterator it=edges.begin();it!=edges.end();it++)
      {
        int u=*it.first;
        int v=*it.second;
        addEdges(u,v);
      }
}

void Graph:: addEdges(int u,int v)
{

  //if the edge exist,don't do anything
  if(adjMat[u][v]) return;

  // for adjList
  adjList[u].push_back(v);
  adjList[v].push_back(u);

  //for adjMat
  adjMat[u][v]=adjMat[v][u]=true;

  // for array of edges record
  edges.push_back(pair<int,int> (u,v));

  //for edge Indexes
  edgeIndexes[u][v]=edgeIndexes[v][u]=amtEdges++;

}
