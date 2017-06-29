#include <stdio.h>
#include <cs50.h>

int main (void)
{
  printf("--[ Bottled Shower Water ]--\n\n"
    "Overview:\n"
    "Enter the number of minutes spent in the shower, and the\n"
    "equivalent in 16oz bottles of water will be calculated.\n\n");
  
  //Input minutes
  printf("Minutes in shower: ");
  int minutes = get_int();
  
  //Calculate and display result
  printf("\n%i minutes equals %i bottles of water.\n", minutes, minutes*12);
}