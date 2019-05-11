#include <vector>
#define larger(A,B)((A)-(B)>0)
using namespace std;

class MinHeap
{
private:
  vector<int> heap; // the heap will kepp the indexes of the edges
  vector<int> pos; // the pos will keep the position of heap element
  vector<int> key; // the key will keep the length of edges
  int size;

public :
  MinHeap() :size(0),heap(1);
  void clear();
  void insert(int value);
  int deleteMin();
  int getSize(){return size;}
};

void MinHeap:: clear()
{
  heap.clear();
}

void MinHeap:: insert(int length,int indexes)
{
    size++;
    // midify the structure to fit a new data
    if(indexes>=pos.size())
    {
     heap.resize(s+2);
     pos.resize(s+1,-1);
     key.resize(s+1);
    }
    
    int i=size-1;
    for(i;i!=1;i/=2)
    {
      if(value>=heap[i/2]) break;
      heap[i]=heap[i/2];
    }
    heap[i]=value;
}

void MinHeap:: deleteMin()
{
  if(!size) throw "heap is empty";

  int minkey=heap[1];
  int lastNode=heap[size-1];
  size--;

  int i=1;
  for(int j=2;j<=size;i=j,j*=2)
  {
    if((j<n) && heap[j]<heap[j+1]) j++;
    if(lastNode<=heap[j]) break;
    heap[i]=heap[j];
  }

  heap[i]=lastNode;
  return minkey;
}
