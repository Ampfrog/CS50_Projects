#include <stdio.h>
#include <cs50.h>

void printIntro();
long long getCCNumber();
int * sliceCC(long long ccTemp);
void validateCard(int *ccPartsTemp);



int main (void)
{
  //Intro
  printIntro();
  
  //Get User Input
  long long cc = getCCNumber();
  
  //Get digits to work with
  int *ccParts;
  ccParts = sliceCC(cc);
  
  //Validate
  validateCard(ccParts);
  
  return 0;
}

//-------------------------

void printIntro()
{
  printf("--[ Credit Card Type & Validity Checker ]--\n\n"
    "Overview:\n"
    "Enter a credit card number (numbers only, no dashes) to determine the brand and check if valid.\n\n"
    "- If output is a card brand, number is valid for that brand\n"
    "- If output is 'INVALID', the type or number (or both) are invalid\n\n");
}

//-------------------------

long long getCCNumber()
{
  long long ll;
  
  do {
    printf("Enter a credit card number: ");
    ll = get_long_long();
  }
  while (ll < 0);
  
  return ll;
}

//-------------------------

int * sliceCC(long long ccTemp)
{
  static int ccDigits[16];
  
  for (int i=15;i>-1;i--)
  {
    if (ccTemp < 1)
    {
      //Set placeholder
      ccDigits[i] = -9;
    }
    else
    {
      ccDigits[i] = ccTemp % 10;
      
      //Divide by 10 to get next digit
      ccTemp /= 10;
    };
  };
  
  //Check if longer than 16 digits (max allowed)
  if (ccTemp >= 1)
  {
    //Flag as INVALID
    ccDigits[0] = -1;
  };
  
  return ccDigits;
}

//-------------------------

void validateCard(int *ccPartsTemp)
{
  if (ccPartsTemp[0] == -1)
  {
    //Too many digits, determined in sliceCC()
    printf("INVALID\n");
  }
  else
  {
    //Determine type
    string ccType = "";
    bool validType = false;
    
    if (ccPartsTemp[1] == 3 && (ccPartsTemp[2] == 4 || ccPartsTemp[2] == 7))
    {
      ccType = "AMEX";
      validType = true;
    }
    else if (ccPartsTemp[0] == 5 && (ccPartsTemp[1] >= 1 && ccPartsTemp[1] <= 5))
    {
      ccType = "MASTERCARD";
      validType = true;
    }
    else if (ccPartsTemp[0] == 4 || ccPartsTemp[3] == 4)
    {
      ccType = "VISA";
      validType = true;
    }
    else
    {
      //Invalid prefix
      printf("INVALID\n");
    };
    
    if (validType)
    {
      //Valid type & length, now run checksum algorithm
      int j = 14;   //Second-to-last digit
      int luhnTtl = 0;
      int multiple;
      
      //Luhn’s Step 0
      while (ccPartsTemp[j] != -9 && j > -1)
      {
        multiple = 2 * ccPartsTemp[j];
        
        //Get digits & sum, max digit qty will be 2 (9*9)
        for (int k=0;k<2;k++)
        {
          luhnTtl += multiple % 10;
          
          multiple /= 10;
        };
        
        j -= 2;
      };
      
      //Luhn’s Step 1
      j = 15;
      
      while (ccPartsTemp[j] != -9 && j > -1)
      {
        luhnTtl += ccPartsTemp[j];
        
        j -= 2;
      };
      
      //Luhn's Step 2
      if (luhnTtl % 10 != 0)
      {
        validType = false;
      };
      
      //Final output
      //printf("Luhn's Total: %d\n",luhnTtl);
      
      if (validType)
      {
        //printf("[Valid per Luhn's]\n");
        printf("%s\n",ccType);
      }
      else
      {
        //printf("[Invalid per Luhn's]\n");
        printf("INVALID\n");
      }
    };
  };
}
