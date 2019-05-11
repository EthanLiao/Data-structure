#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
using namespace std;

struct Node
{
  int key;
  int leftSize;
  Node *leftchild;
  Node *rightchild;
  Node *parent;
};

class BST
{

  Node *root;
  int sizeofTree;

public:
  BST();
Node *searchNode(int num);
int searchLevel(int num);
void insert(int num);
void Delete(int num);
Node *findMax(Node* node);
Node *searchRank(int rank);
void modifyLeftsize(Node* targetnode);
int getsizeofTree();
};

BST::BST(){root=nullptr; sizeofTree=0;}
Node *BST::searchNode(int num)
{
  Node *node;
  node=root;
  while(node!=nullptr)
  {
    if(node->key==num) return node;
    else
    {
      if(num<node->key) node=node->leftchild;
      else node=node->rightchild;
    }
  }
  //not found
  return node;
}

int BST::searchLevel(int num)
{
  Node *node=root;
  int level=0;
  while(node!=nullptr)
  {
    if(node->key==num) return level;
    else
    {
      if(num<node->key) {node=node->leftchild; level++;}
      else { node=node->rightchild; level++;}
    }
  }
  //not found
  return -1;
}


void BST::insert(int num)
{
  //cout<<"insert";
  if(searchNode(num)!=nullptr) return;

  Node *newnode,*current,*prev;
  newnode=new Node;
  newnode->key=num;
  newnode->leftSize=1;
  newnode->leftchild=newnode->rightchild=newnode->parent=nullptr;
  sizeofTree++;

  /*Insert a node*/
  //find the insert position
  if(root==nullptr) root=newnode;
  else
  {
    current=root;
    // Search the node position
    while(current!=nullptr)
    {
      prev=current;
      if(num>current->key) current=current->rightchild;
      else {current->leftSize++;  current=current->leftchild;}
    }
    //insert the node
    newnode->parent=prev;
    if(num>prev->key) prev->rightchild=newnode;
    else prev->leftchild=newnode;
  }
}
void BST::Delete(int num)
{
  // No data in BST
  if(root==nullptr) return;
  Node *delnode;
  //BST doesn't have the key
  if((delnode=searchNode(num))==nullptr) return;

  //Case1 : delnode is leaf
  if(delnode->rightchild==nullptr && delnode->leftchild==nullptr)
  {
    if(delnode==root)
    {
      delete delnode;
      root=nullptr;
    }
    else
    {
      if(delnode->key>delnode->parent->key)
      {
        modifyLeftsize(delnode);
        delnode->parent->rightchild=nullptr;
      }
      else if(delnode->key<delnode->parent->key)
      {
        modifyLeftsize(delnode);
        delnode->parent->leftchild=nullptr;
      }
      delete delnode;
    }
    sizeofTree--;
  }
  //Case2 : delnode has one-child
  else if(delnode->rightchild==nullptr && delnode->leftchild!=nullptr)
  {

    if(delnode==root)
    {
      root=delnode->leftchild;
      root->parent=nullptr;
    }
    else
    {
      modifyLeftsize(delnode);
      if(delnode->key>delnode->parent->key)
      {
        delnode->parent->rightchild=delnode->leftchild;
        delnode->leftchild->parent=delnode->parent;
      }
      if(delnode->key<delnode->parent->key)
      {
        delnode->parent->leftchild=delnode->leftchild;
        delnode->leftchild->parent=delnode->parent;
      }
    }
    delete delnode;
    sizeofTree--;
  }
  else if(delnode->rightchild!=nullptr && delnode->leftchild==nullptr)
  {

    if(delnode==root)
    {
      root=delnode->rightchild;
      root->parent=nullptr;
    }
    else
    {
      modifyLeftsize(delnode);
      if(delnode->key>delnode->parent->key)
      {
       delnode->parent->rightchild=delnode->rightchild;
       delnode->rightchild->parent=delnode->parent;
      }
      if(delnode->key<delnode->parent->key)
      {
       delnode->parent->leftchild=delnode->rightchild;
       delnode->rightchild->parent=delnode->parent;
      }
    }
    delete delnode;
    sizeofTree--;
  }
  //Case3 : delnode has two-child
  else if(delnode->rightchild!=nullptr && delnode->leftchild!=nullptr)
  {

    if(delnode==root && delnode->leftchild->leftchild==nullptr && delnode->leftchild->rightchild==nullptr&& delnode->rightchild->leftchild==nullptr && delnode->rightchild->rightchild==nullptr)
    {
      Node* temp;
      delnode->key=delnode->leftchild->key;
      delnode->leftchild=nullptr;
      delnode->leftSize=1;
      delete delnode->leftchild;
    }
    else
    {
      Node *alternode;
      alternode=findMax(delnode->leftchild);
      if(alternode->key>alternode->parent->key)
      {
        modifyLeftsize(alternode);
        alternode->parent->rightchild=alternode->leftchild;
      }
      else if(alternode->key<alternode->parent->key)
      {
        modifyLeftsize(alternode);
        alternode->parent->leftchild=alternode->leftchild;
      }
      delnode->key=alternode->key;
      delete alternode;
    }

      sizeofTree--;
  }
}

Node* BST::findMax(Node* innode)
{
  Node* node;
  node=innode;
  while(node!=nullptr&&node->rightchild!=nullptr) node=node->rightchild;
  return node;
}

Node* BST::searchRank(int rank)
{
  for(Node* node=root;node;)
  {
    if(rank==node->leftSize) return node;
    if(rank<node->leftSize) node=node->leftchild;
    else
    {
      rank-=node->leftSize;
      node=node->rightchild;
    }
  }
  return 0;
}

void BST::modifyLeftsize(Node* targetnode)
{
  Node* target=targetnode;
  Node* current=root;
  while (current!=target)
  {
    if(target->key < current->key){current->leftSize--; current=current->leftchild;}
    if(target->key > current->key) current=current->rightchild;
  }
}

int BST::getsizeofTree()
{
return sizeofTree;
}


int main(int argc, char const *argv[]) {

  BST tree;
  fstream srcfile;
  ofstream outfile;

  srcfile.open(argv[1]);
  outfile.open(argv[2]);
  int action,key;
  //cout<<"test";
  while(srcfile>>action>>key)
  {
    int count=0;
    if(action==1)
    {
      //cout<<"test";
      tree.insert(key);
    }

    else if(action==2)
    {
      //cout<<"test";
      tree.Delete(key);
    }
    else if(action==3)
    {
      //cout<<"test";
      outfile<<tree.searchLevel(key)<<"\n";
    }

    else if(action==4)
    {
      //cout<<"test";
      if(key>tree.getsizeofTree()) continue;
      outfile<<(tree.searchRank(key))->key<<" "<<tree.searchLevel((tree.searchRank(key))->key)<<"\n";
    }
  }
  srcfile.close();
  outfile.close();



  return 0;
}
