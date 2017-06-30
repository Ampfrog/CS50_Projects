#include <stdio.h>
#include <cs50.h>

void printIntro();
int getBlockQty();
void drawLeftSpc(int sQty);
void drawBlocks(int bQty);


int main (void)
{
  //Intro
  printIntro();
  
  //Input base count
  int baseBlockCount = getBlockQty();
  
  //Create output
  if (baseBlockCount > 0)
  {
    for (int i=1;i<baseBlockCount+1;i++)
    {
      //Space to left of blocks (if needed)
      drawLeftSpc(baseBlockCount-i);
      
      //Blocks for current row
      drawBlocks(i);
    };
  };
}



void printIntro()
{
  printf("--[ Mario Half-Pyramid Builder with Gap to Jump]--\n\n"
    "Overview:\n"
    "Enter the number of blocks required at the base of each half-pyramid\n"
    "to set the overall size. Allowable range is 0 to 23 blocks.\n\n");
}

int getBlockQty()
{
  int i;
  
  do {
    printf("Number of blocks in each base: ");
    i = get_int();
  }
  while (i < 0 || i > 23);
  
  return i;
}

void drawLeftSpc(int sQty)
{
  for (int i=0;i<sQty;i++)
  {
    printf(" ");
  };
}

void drawBlocks(int bQty)
{
  for (int j=0;j<2;j++)
  {
    for (int i=0;i<bQty;i++)
    {
      printf("#");
    };
    
    //Add space between blocks & break after blocks
    if (j == 0)
    {
      //Gap
      printf("  ");
    }
    else
    {
      //Line break
      printf("\n");
    };
  };
}
