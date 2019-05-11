/*
Datastructure Homework2
*/
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <fstream>
#include <cstring>
#include <stack>
using namespace std;


typedef struct
{
	int pow;
    int coef;
} polynode;

bool is_number(string);
void buidPoly(list<polynode> &,int ,int);
void addpolyAndAssigntoPolyA(list<polynode> &,list<polynode> &);
void subpolyAndAssigntoPolyA(list<polynode> &,list<polynode> &);
void multiplyTwoPolyResult(list<polynode>&,list<polynode>&);
void mulPolyAndAssigntoPolyA(list<polynode> &,list<polynode> &);
void printResultToFile(string);


list<polynode> polyA,polyB;
int main(int argc, char *argv[])
{

	stack<string> operation;
	int coef,pow;
	fstream srcfile; 
	srcfile.open(argv[1]);
	string tmp;
    int turn_for_polyX=0;

	while(srcfile>>tmp)
	{
       if(is_number(tmp))
       {	
       	if((turn_for_polyX)==0)
       	{ 
          int count=0;
          while(count<atoi(&tmp[0]))
          {
            srcfile>>coef>>pow;
            //cout<<coef<<" "<<pow<<"\n";
            buidPoly(polyA,coef,pow);
          	count++;
          }
          turn_for_polyX++;
        }

        else
        {
          int count=0;
          while(count<atoi(&tmp[0]))
          {
            srcfile>>coef>>pow;
            //cout<<coef<<" "<<pow<<"\n";
            buidPoly(polyB,coef,pow);
          	count++;
          }

          if(operation.top()=="+") addpolyAndAssigntoPolyA(polyA,polyB);
          else if (operation.top()=="-") subpolyAndAssigntoPolyA(polyA,polyB);
          else if(operation.top()=="*")  mulPolyAndAssigntoPolyA(polyA,polyB);

         /*cout<<polyA.size()<<"\n";
           for(list<polynode> :: iterator it=polyA.begin();it!=polyA.end();it++)
            cout<<(*it).coef<<" "<<(*it).pow<<"\n";
             cout<<"\n";
         
           cout<<polyB.size()<<"\n";
           for(list<polynode> :: iterator it=polyB.begin();it!=polyB.end();it++)
           cout<<(*it).coef<<" "<<(*it).pow<<"\n"; */

          turn_for_polyX++;
          operation.pop();
          polyB.clear();
        }
       }

       else 
        {
          operation.push(&tmp[0]); 
          //cout<<tmp;  
        }      
	}
	srcfile.close();
	printResultToFile(argv[2]);
	return 0;
}

bool is_number(std::string s)
{
    return !s.empty() && s.find_first_not_of("0123456789") == std::string::npos;
}

void buidPoly(list<polynode> &polylist,int coef,int pow )
{

    polynode *listnode=new polynode;
    (*listnode).coef=coef;
    (*listnode).pow=pow;
    polylist.push_back(*listnode);
}


void addpolyAndAssigntoPolyA(list<polynode> &listA, list<polynode> &listB)
{   

	list<polynode> polyAns;
    polynode *listnode=new polynode;
    polyAns.push_back(*listnode);
    list<polynode> :: iterator listAIt=listA.begin(),listBIt=listB.begin(),listAnsIt=polyAns.begin();
    
    while(listAIt!=listA.end() && listBIt!=listB.end())
    {
    	if((*listAIt).pow>(*listBIt).pow) 
    	{
          (*listAnsIt).pow=(*listAIt).pow;
          (*listAnsIt).coef=(*listAIt).coef;
          //cout<<"test";
          //cout<<(*listnode).coef<<" "<<(*listnode).pow<<"\n";
    		listAIt++;
    	}

    	else if((*listBIt).pow>(*listAIt).pow)
    	{
          (*listAnsIt).coef=(*listBIt).coef;
          (*listAnsIt).pow=(*listBIt).pow;
          listBIt++;
    	}

    	else
    	{
          if((*listBIt).coef+(*listAIt).coef!=0)
          {    //cout<<(*listAIt).coef<<" "<<(*listAIt).pow<<"\n";
            (*listAnsIt).pow=(*listBIt).pow;
            (*listAnsIt).coef=(*listBIt).coef+(*listAIt).coef;
            //cout<<(*listAnsIt).coef<<" "<<(*listAnsIt).pow<<"\n";
          }
          else 
          {
            polyAns.pop_back();
            listAnsIt--;
          }

            listBIt++;
            listAIt++;
    	}
      //cout<<(*listAnsIt).coef<<" "<<(*listAnsIt).pow<<"\n";
      polynode* listnode=new polynode;
      polyAns.push_back(*listnode);
      listAnsIt++;
    }

    while((listAIt!=listA.end()) || (listBIt!=listB.end()))
    {
    	if(listAIt!=listA.end())
    	{
          if((*listAIt).coef!=0)
          {
           (*listAnsIt).coef=(*listAIt).coef;
           (*listAnsIt).pow=(*listAIt).pow;
            //cout<<(*listnode).coef<<" "<<(*listnode).pow<<"\n";
          }
           else 
          {
            polyAns.pop_back();
            listAnsIt--;
          }
           listAIt++;
        }

      if(listBIt!=listB.end())
      {
        if((*listBIt).coef!=0)
        {
          (*listAnsIt).coef=(*listBIt).coef;
          (*listAnsIt).pow=(*listBIt).pow;
        }
        else 
        {
            polyAns.pop_back();
            listAnsIt--;
        }
        listBIt++;
      }

      polynode* listnode=new polynode;
      polyAns.push_back(*listnode);
      listAnsIt++;
    } 
    listAnsIt--;
    polyAns.pop_back();

    polyA=polyAns;
   // cout<<"\n";
    /**cout<<polyA.size()<<"\n";
    for(list<polynode> :: iterator it=polyA.begin();it!=polyA.end();it++)
    cout<<(*it).coef<<" "<<(*it).pow<<"\n";**/
}


void subpolyAndAssigntoPolyA(list<polynode> &listA, list<polynode> &listB)
{   

    list<polynode> polyAns;
    polynode* listnode=new polynode;
    polyAns.push_back(*listnode);
    list<polynode> :: iterator listAIt=listA.begin(),listBIt=listB.begin(),listAnsIt=polyAns.begin();
    while(listAIt!=listA.end() && listBIt!=listB.end())
    {
      if((*listAIt).pow>(*listBIt).pow) 
      {
          (*listAnsIt).pow=(*listAIt).pow;
          (*listAnsIt).coef=(*listAIt).coef;
        //cout<<"test";
        //cout<<(*listnode).coef<<" "<<(*listnode).pow<<"\n";
        listAIt++;
      }

      else if((*listBIt).pow>(*listAIt).pow)
      {
          (*listAnsIt).coef=-(*listBIt).coef;
          (*listAnsIt).pow=(*listBIt).pow;
          listBIt++;
      }

      else
      {
        if((*listAIt).coef-(*listBIt).coef!=0)
        {   
            //cout<<(*listAIt).coef<<" "<<(*listAIt).pow<<"\n";
            (*listAnsIt).pow=(*listBIt).pow;
            (*listAnsIt).coef=(*listAIt).coef-(*listBIt).coef;
            //cout<<(*listAnsIt).coef<<" "<<(*listAIt).pow<<"\n";
        }
        else 
          {
            polyAns.pop_back();
            listAnsIt--;
          }
            listBIt++;
            listAIt++;
      }
     // cout<<(*listAnsIt).coef<<" "<<(*listAnsIt).pow<<"\n";
      polynode* listnode=new polynode;
      polyAns.push_back(*listnode);
      listAnsIt++;
    }

    while((listAIt!=listA.end()) || (listBIt!=listB.end()))
    {
      if(listAIt!=listA.end())
      {
        if((*listAIt).coef!=0)
        {
         (*listAnsIt).coef=(*listAIt).coef;
         (*listAnsIt).pow=(*listAIt).pow;
            //cout<<(*listnode).coef<<" "<<(*listnode).pow<<"\n";
        }
        else 
        {
            polyAns.pop_back();
            listAnsIt--;
        }
        listAIt++;
      }

      if(listBIt!=listB.end())
      {
        if((*listBIt).coef!=0)
        {
        (*listAnsIt).coef=-(*listBIt).coef;
        (*listAnsIt).pow=(*listBIt).pow;
        }
        else 
         {
            polyAns.pop_back();
            listAnsIt--;
         }
        listBIt++;
      }

      polynode* listnode=new polynode;
      polyAns.push_back(*listnode);
      listAnsIt++;
    } 
    listAnsIt--;
    polyAns.pop_back();

    polyA=polyAns;
    //cout<<"\n";
    /**cout<<polyA.size()<<"\n";
    for(list<polynode> :: iterator it=polyA.begin();it!=polyA.end();it++)
    cout<<(*it).coef<<" "<<(*it).pow<<"\n";**/
}

void mulPolyAndAssigntoPolyA(list<polynode> &listA,list<polynode> &listB)
{
   
   list<polynode> polyAns;
   
   // If the two polymials are all empty
   if(listA.size()==0 && listB.size()==0)
   {
     polyA=polyAns;
   	 return;
   }


   if(listA.size()==0) //listA is empty
   {
   	 polyA=polyAns;
   	 return;
   }

   else //listA is not empty
   {
     if (listB.size()==0) //listA is not empty,but listB is empty
     {
        polyA=polyAns;
        return;
     }
     
     else  //listA is not empty and listB is not empty
     {
      //cout<<"test";

             	multiplyTwoPolyResult(listA,listB);
              //cout<<"test";
     }
   }
}

void multiplyTwoPolyResult(list<polynode>& listA,list<polynode>& listB)
{ 
  int coef,pow;
  list<polynode> listAns;
  polynode* listnode=new polynode;
    (*listnode).coef=0;
    (*listnode).pow=0;
    listAns.push_front(*listnode);
  for(list<polynode>::iterator  itA=listA.begin();itA!=listA.end();itA++)
    for(list<polynode> :: iterator  itB=listB.begin();itB!=listB.end();itB++)
    {
      coef=(*itA).coef * (*itB).coef;
      pow=(*itA).pow+(*itB).pow;
      if(coef!=0)
       { 
          list<polynode> :: iterator it=listAns.begin();
          while(pow<(*it).pow&&it!=listAns.end()) it++;
            // cout<<(*it).coef<<" "<<(*it).pow<<"\n";
          if(it==listAns.end())
          { 
           polynode *listnode=new polynode;
           (*listnode).coef=coef;
           (*listnode).pow=pow;
          listAns.push_back(*listnode);
          }

          else if(pow==(*it).pow) (*it).coef=(*it).coef+coef;
          else
          {
             (*listnode).coef=coef;
             (*listnode).pow=pow;
             listAns.insert(it,*listnode); 
          }
        }
    }
  listAns.pop_back();
  polyA=listAns;
}

void printResultToFile(string FileName)
{
	ofstream destFile;
	destFile.open(&FileName[0]);
  if(polyA.size()==0)
   {
   	list<polynode> :: iterator it=polyA.begin();
   	destFile<<polyA.size()+1<<"\n"<<(*it).coef<<" "<<(*it).coef;
   }
   

  else
  {
    destFile<<polyA.size()<<"\n";
    for(list<polynode> :: iterator it=polyA.begin();it!=polyA.end();it++)
    destFile<<(*it).coef<<" "<<(*it).pow<<"\n";
  }

	destFile.close();
}