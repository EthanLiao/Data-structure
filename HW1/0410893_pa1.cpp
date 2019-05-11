#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
using namespace std;
int isPrimenumber(int);


int main(int argc, char const *argv[])
{
    string str_number;
    ifstream infile;
      infile.open(argv[1]);
    ofstream myfile;
      myfile.open(argv[2]);
    while(infile>>str_number)
    {
      myfile<<isPrimenumber(atoi(&str_number[0]))<<"\n";
    }
    infile.close();
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