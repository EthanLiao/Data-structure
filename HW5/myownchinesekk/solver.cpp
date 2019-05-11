#include "Graph.cpp"
#include "ChinesePostman.cpp"
#include<fstream>
#include<iostream>
#include<string>
#include <utility>
#include <map>
using namespace std;

//map for the original graph
//independent int represent the mark of vertex
//in the pair,each int type represent the position of the vertex
map<int,pair<int,int> > vertex;

pair<Graph,vector<double> > buildGrapgh(string srcfile)
{
  ifstream infile;
  infile.open(srcfile);
  int column,row;
  infile>>column;
  infile>>row;
  int **map=new int*[row];
  int **mark=new int*[row];
  for(int i=0;i<row;i++)
  {
    mark[i]=new int[column];
    map[i]=new int[column];
  }
  //vector for the mark map
  //in the pair,each int type represent the mark of vertex
  vector<pair<int,int> > edge;


  int vertexMark=0;
  int amtEdges=0,amtVertices=0;
  for(int r=row-1;r>=0;r--)
   for(int c=0;c<column-1;c++)
   {
       int tmp;
       infile>>tmp;
       if(tmp==0)
       {
         map[r][c]=tmp;
         mark[r][c]=vertexMark;
         amtVertices++;
         vertexMark++;
         vertex[vertexMark]=make_pair(r,c);
         // check whether exist edge
         if(map[r-1][c]==0)
         {
           edge.push_back(make_pair(mark[r-1][c],mark[r][c]));
           amtEdges++;
         }
         if(map[r][c-1]==0)
         {
           edge.push_back(make_pair(mark[r][c-1],mark[r][c]));
           amtEdges++;
         }
       }
       else
        {
          map[r][c]=tmp;
          mark[r][c]=-1;
        }
     }
     // build graph
     Graph G(amtVertices);
     vector<double> weight(amtEdges);
     for(int i=0;i<amtEdges;i++)
     {
       G.AddEdge(edge[i].first,edge[i].second);
       weight[G.GetEdgeIndex(edge[i].first,edge[i].second)]=1.0;
     }
     infile.close();
     return make_pair(G, weight);
}


int main(int argc, char const *argv[]) {


  pair<Graph,vector<double> > p=buildGrapgh(argv[1]);
  Graph g=p.first;
  vector<double> weight=p.second;

  //list<int> shoud be modified
  pair<list<int>,double> sol= ChinesePostman(g,weight);
  list<int> s=sol.first;
  ofstream outfile;
  outfile.open(argv[2]);
  for(list<int>::iterator it=s.begin();it!=s.end();it++)
   outfile<<vertex.at(*it).first<<" "<<vertex.at(*it).second<<"\n";
  outfile.close();

  return 0;
}
