#include <cs50.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool checkArgs (int argcTemp, string argvTemp[]);
string getUserInputStr();
void encryptUserStr(string plainStr, int key);


int main(int argc, string argv[])
{
  //Validate args (argc = 2, 0 < argv[1] < 2^31-26)
  if (!checkArgs(argc,argv))
  {
    return 1;   //Error out of main
  };
  
  //Encrypt user string
  encryptUserStr(getUserInputStr("plaintext:  "),atoi(argv[1]));
  
  return 0;
}


//-------------------------------
bool checkArgs (int argcTemp, string argvTemp[])
{
  //Check for correct arg count
  if (argcTemp != 2)
  {
    printf("[Error] Usage: ./caesar <key>\n");
    return false;
  };
  
  //Key arg is present, convert and validate int
  long long ll = atoll(argvTemp[1]);
  
  if (ll < 2147483621)
  {
    //Within upper limit, convert to int
    int userArg = atoi(argvTemp[1]);
    
    //Check lower limit
    if (userArg < 1)
    {
      printf("Key must be greater than 0.\n");
      return false;
    };
  }
  else
  {
    printf("Key must be less than 2147483621.\n");
    return false;
  };
  
  return true;
}


string getUserInputStr(string promptTxt)
{
  printf("%s",promptTxt);
  return get_string();
}


void encryptUserStr(string plainStr, int key)
{
  printf("ciphertext: ");
  
  //Account for wrapping
  int actualOffset = key % 26;
  
  //Encrypt user string
  char currPlain;
  int currCiph;
  
  for (int i=0,l=strlen(plainStr); i<l; i++)
  {
    //Current char
    currPlain = plainStr[i];
    
    if (isalpha(currPlain))
    {
      //Letter found, encrypt
      currCiph = currPlain+actualOffset;
      
      if (!isalpha(currCiph))
      {
        //Wrap
        currCiph -= 26;
      };
      
      printf("%c",currCiph);
    }
    else
    {
      //Not a letter, do not encrypt
      printf("%c",currPlain);
    };
  };
  
  printf("\n");
}
