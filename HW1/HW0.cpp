#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <sstream>
#include <fstream>
using namespace std;
int isPrimenumber(int);
void printTofile(int);
int main(int argc, char const *argv[])
{
    /*Open the file and then read the number to the str_number*/
    string str_number,str;
    ifstream infile;
    infile.open(argv[1]);

    while(!infile.eof())
      infile>>str_number;
   // getline(infile,str);
     	//while(!infile.eof())
     // {
          //str_number+=str;
        //  getline(infile,str_number);
      //}
     infile.close();

    /*Token the number then transfer it into int_type*/   
      stringstream ss_number(str_number);
      vector<string> token;
      string intermediate;
      int *int_answer;

      int i=0;
      ofstream myfile;
       myfile.open(argv[2]);

      while(getline(ss_number,intermediate,' '))
      {


      	token.push_back(intermediate);
      	string final_str_number(token[i]);
        cout<<final_str_number;
      	*(int_answer+i)=isPrimenumber(atoi(&final_str_number[0]));
         
          myfile<<*(int_answer+i)<<"\n";

        //printTofile(*(int_answer+i));
      	i++;
      }
                myfile.close();

	return 0;
}

int isPrimenumber(int number)
{
    if(number%2==0 && number!=2) return 0;
        unsigned i=0;
    for(i=3; i*i<=number; i+=2) if(number%i==0) return 0;
        return 1;
};


/*void printTofile(int answer)
{
    ofstream myfile;
      myfile.open(argv[2]);
      myfile<<answer<<"\n";
      myfile.close();
};*/
