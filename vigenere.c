#include <cs50.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool checkArgs (int argcTemp, string argvTemp[]);
string getUserInputStr(string promptTxt);
void encryptUserStr(string plainStr, string key);


int main(int argc, string argv[])
{
  //Validate args (argc = 2, 0 < argv[1] < 2^31-26)
  if (!checkArgs(argc,argv))
  {
    return 1;   //Error out of main
  };
  
  //Encrypt user string
  encryptUserStr(getUserInputStr("plaintext:  "),argv[1]);
  
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
  
  //Key arg is present, check if alpha-beta
  string keyStr = argvTemp[1];
  
  for (int i=0, l=strlen(keyStr); i<l; i++)
  {
    if (!isalpha(keyStr[i]))
    {
      printf("Key must contain uppercase or lowercase letters only.\n");
      return false;
    };
  };
  
  return true;
}


string getUserInputStr(string promptTxt)
{
  printf("%s",promptTxt);
  return get_string();
}


void encryptUserStr(string plainStr, string key)
{
  printf("ciphertext: ");
  
  //Encrypt user string
  char currPlain, currKeyChar;
  int currCiph,
      currOffset,
      keyPos = 0,
      keyLastIndex = strlen(key)-1;
  
  for (int i=0,l=strlen(plainStr); i<l; i++)
  {
    //Current char
    currPlain = plainStr[i];
    
    if (isalpha(currPlain))
    {
      //Letter found, get offset from key
      currKeyChar = key[keyPos];
      
      //Increment key position, jump to beginning of key if needed
      keyPos++;
      
      if (keyPos > keyLastIndex)
      {
        keyPos = 0;
      };
      
      //Adjust to alpha-indexed based on case type
      if (isupper(currKeyChar))
      {
        currOffset = currKeyChar - 65;
      }
      else
      {
        currOffset = currKeyChar - 97;
      };
      
      //Encrypt
      currCiph = currPlain+currOffset;
      
      if (!isalpha(currCiph) || ((currPlain+currOffset) >= 97 && currPlain <= 90))
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
