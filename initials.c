#include <cs50.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

string getUserInputStr();
void extractInitials(string fullName);


int main (void)
{
  //Extract, format, and display
  extractInitials(getUserInputStr());
  
  return 0;
}


//-------------------------------------
string getUserInputStr()
{
  return get_string();
}


void extractInitials(string fullName)
{
  //Find first letter (in case of leading spaces)
  int firstLetterIndex = 0;
  
  while (fullName[firstLetterIndex] == ' ')
  {
    firstLetterIndex++;
  };
  
  printf("%c",toupper(fullName[firstLetterIndex]));
  
  
  //Get balance of initials   (note: start @ firstLetterIndex+2 since index 1 will always be letter or space, neither an initial)
  for (int i=firstLetterIndex+2, n=strlen(fullName); i<n; i++)
  {
    if (fullName[i] != ' ' && fullName[i-1] == ' ')
    {
      //Initial found
      printf("%c",toupper(fullName[i]));
    };
  };
  
  printf("\n");
}
