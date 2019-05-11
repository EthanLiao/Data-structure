#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <stack>
#include <queue>
using namespace std;







typedef struct
{
	short int row,col,dir;
} element;

typedef struct
{
    short int vert,horiz;
} offsets;

offsets move_long[4]={{0,1},{1,0},{0,-1},{-1,0}};
                /*right down  left   upper  */
 // Establish a tow-dimesional array for the maze
char **maze;
char **mark;


class MatMaze
{
private :
 int row,column;    // index for the whole maze
 int start_row,start_column; //index for the start
 int end_row,end_column;     //indext foe the exist
public:
	 ~MatMaze();

void TypeError(void);
void readFile(string);
void solveMaze(void);
void outputFile(string);
};


MatMaze :: ~MatMaze()
{
  delete [] maze;
  delete [] mark;
}

void MatMaze :: TypeError()
{
   cout<<"plz restart";
}

void MatMaze :: readFile(string srcFilename)
{
   fstream srcfile;
   srcfile.open(&srcFilename[0]);
   char single_ch;
   int count=0;
   int row_count=0,column_count=0;
   srcfile>>column>>row;

    maze=new char*[row];
    mark=new char*[row];
      for(int k=0;k<row;k++)
      {
         maze[k]=new char[column];
         mark[k]=new char[column];
      }
    for(int i=0;i<row;i++)
      for(int j=0;j<column;j++)
       {
        srcfile>>maze[i][j];
        if(maze[i][j]=='S')
          {
                start_row=i;
                start_column=j;

          }

        if (maze[i][j]=='E')
          {
            end_row=i;
            end_column=j;
          }
       }
  srcfile.close();

}

void MatMaze :: solveMaze()
{

 //copy the maze to the mark map
 for(int i=0;i<row;i++)
    for(int j=0;j<column;j++)
      mark[i][j]=maze[i][j];

 int found=0; // If the Mat reach to the exist,we will set the found as 1
 int row,col,dir;
 int nextRow,nextCol;
 // maze entrance & direction east
 element position;
 position.row=start_row;
 position.col=start_column;
 position.dir=0; // intial direction is the east
 // initialize stack
 stack<element> s;
 s.push(position);

 while(!s.empty())
 {
 	if(found)
    {
      mark[row][col]='1';
      break;
    }
 	//cordinate and direcrion pop from stack
 	position=s.top();
 	row=position.row;
 	col=position.col;
 	dir=position.dir;
 	s.pop();
 	 while(dir<4) // if there exist more direction to go
 	 {
 	 	//cordinate of the next move
 	 	nextRow=row+move_long[dir].vert;
 	 	nextCol=col+move_long[dir].horiz;

        // if the next positon is the exit
 	 	if((nextRow==end_row) && (nextCol==end_column))
 	 	{
 	 		found=1;
 	 		break;
 	 	}

        //if the next postion don't encounter with the wall and havn't been visted
 	 	else if(maze[nextRow][nextCol]!='2' && mark[nextRow][nextCol]!='1')
 	 	{
 	 		mark[nextRow][nextCol]='1'; // mark the next position

 	 		// store the current position
 	 		position.row=row;
 	 		position.col=col;
 	 		position.dir=++dir;// store the next direction to try into stack
 	 		s.push(position);

 	 		//move to the next position
 	 		row=nextRow; col=nextCol; dir=0; // we will set the
 	 		                                 //default direction to try for the next position
 	 	}
 	 	//if the next position is the wall or have been here
 	 	else ++dir;
 	 }
 }
}

void MatMaze :: outputFile(string out)
{
  ofstream outfile;
  outfile.open(&out[0]);
  for(int i=0;i<row;i++)
    for(int j=0;j<column;j++)
    {
      outfile<<mark[i][j]<<" ";
      if(j==column-1)
        outfile<<"\n";
    }
    outfile.close();
}

/*Method two*/


typedef struct
{
  int row,col;
  int dist,dir; //distance for the queue node
}Point;


typedef struct
{
  int vert,horiz;
}deviate;

deviate move_short[4]={{0,1},{1,0},{0,-1},{-1,0}};
                    /* right  dowwn  left   upper*/

char **maze_short,**mark_short;
int **maze_short_final; //For the path distacne record

class MatMaze_short_Dist
{
private :
 int row,column;    // index for the whole maze
 int start_row,start_column; //index for the start
 int end_row,end_column;     //indext foe the exist
public:
	~MatMaze_short_Dist();
	void outputFile(string);
    void readFile(string);
	void solveMaze(void);
	void traceBack(void);
};

MatMaze_short_Dist :: ~MatMaze_short_Dist()
{
  delete [] maze_short;
  delete [] maze_short_final;
  delete [] mark_short;
}


void MatMaze_short_Dist :: readFile(string srcFilename)
{
   fstream srcfile;
   srcfile.open(&srcFilename[0]);
   char single_ch;
   int count=0;
   int row_count=0,column_count=0;
   srcfile>>column>>row;

    maze_short=new char*[row];
    mark_short=new char*[row];
    maze_short_final=new int*[row];
      for(int k=0;k<row;k++)
      {
         maze_short[k]=new char[column];
         mark_short[k]=new char[column];
         maze_short_final[k]=new int[column];
      }
    for(int i=0;i<row;i++)
      for(int j=0;j<column;j++)
       {
        srcfile>>maze_short[i][j];
        if(maze_short[i][j]=='S')
         {
                start_row=i;
                start_column=j;
          }

        if (maze_short[i][j]=='E')
          {
            end_row=i;
            end_column=j;
          }
       }
  srcfile.close();



}

void MatMaze_short_Dist :: solveMaze()
{
 //copy the maze to the mark map

  for(int i=0;i<row;i++)
    for(int j=0;j<column;j++)
    {
      mark_short[i][j]=maze_short[i][j];
      maze_short_final[i][j]=-1;
    }





 int found=0; // If the Mat reach to the exist,we will set the found as 1
 int row,col,dist,dir;
 int nextRow,nextCol;
 // maze entrance & distance
 Point position,nextPosition;
 position.row=start_row;
 position.col=start_column;
 position.dist=0; // intial the distance of start position is 0
 maze_short_final[start_row][start_column]=position.dist;
 position.dir=0;
 // initialize queue
 queue<Point> q;
 q.push(position);

 while(!q.empty())
 {
 	//+if(found) mark[row][col]='E';
 	//cordinate and direcrion pop from stack
 	position=q.front();
 	row=position.row;
 	col=position.col;
  dir=position.dir;
  //if(nextRow==end_row && nextCol==end_column)
      //maze_short_final[nextRow][nextRow]=dist;
  q.pop();
 	 while(dir<4) // if there exist more direction to go
 	 {
 	 //cordinate of the next move
 	 nextRow=row+move_short[dir].vert;
 	 nextCol=col+move_short[dir].horiz;

      //if the next postion don't encounter with the wall and havn't been visted
   if((mark_short[nextRow][nextCol]!='1') && (mark_short[nextRow][nextCol]!='2'))
 	 	{
 	 	  mark_short[nextRow][nextCol]='1'; // mark the next position
 	 	  // enqueue the current position
      // we will set the default direction to try for the next position */
      nextPosition.row=nextRow;
      nextPosition.col=nextCol;
      nextPosition.dist=position.dist+1;
      maze_short_final[nextRow][nextCol]=nextPosition.dist;
      nextPosition.dir=0;
      q.push(nextPosition);
      dir++;
 	 	}
 	 	//if the next position is the wall or have been here
 	 	else ++dir;
 	 }
 }
}

deviate trace[4]={{0,-1},{-1,0},{0,1},{1,0},};
                /* left  upper  right  down */
void MatMaze_short_Dist :: traceBack()
{
 int row,col,dir,dist;
 int nextRow,nextCol;
 // maze entrance & distance
   Point position;
   position.row=end_row;
   position.col=end_column;
   position.dir=0;
   position.dist=maze_short_final[end_row][end_column];

   //cordinate and direcrion pop from stack
   row=position.row;
   col=position.col;
   dist=position.dist;
   dir=position.dir;
   while(dir<4) // if there exist more direction to go
   {
    //cordinate of the next move
    nextRow=row+trace[dir].vert;
    nextCol=col+trace[dir].horiz;
    // if the next positon is the exit
    if((nextRow==start_row) && (nextCol==start_column))
    {
      maze_short[row][col]='1';
      break;
    }
    //if the next postion don't encounter with the wall and havn't been visted
    else if(
            ( (nextRow!=row-1 || nextRow!=0 || nextCol!=column-1 || nextCol!=0))
            && maze_short_final[nextRow][nextCol]==dist-1
           )
    {
      maze_short[nextRow][nextCol]='1'; // mark the next position
      row=nextRow; col=nextCol; dir=0; dist--; // For the next position,we set default direction to try
    }
    //if the next position is the wall or have been here
    else ++dir;
  }
}

void MatMaze_short_Dist :: outputFile(string out)
{
	traceBack();
  ofstream outfile;
	outfile.open(&out[0]);
	for(int i=0;i<row;i++)
		for(int j=0;j<column;j++)
		{
      outfile<<maze_short[i][j]<<" ";
		  if(j==column-1)
		  	outfile<<"\n";
		}
  outfile.close();
}

int main(int argc, char const *argv[])
{

   MatMaze obj_1;
   MatMaze_short_Dist obj_2;

   obj_1.readFile(argv[1]);
   obj_1.solveMaze();
   obj_1.outputFile(argv[2]);

   obj_2.readFile(argv[1]);
   obj_2.solveMaze();
   obj_2.outputFile(argv[3]);

	return 0;
}
