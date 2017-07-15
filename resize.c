#include <cs50.h>
#include <stdio.h>
#include "bmp.h"

bool checkArgs (int argcTemp, char *argvTemp[], int argQty, char *progName);
void setFileNames(char *argvTemp[]);
int validateFiles(void);
bool checkFileInHeaders(void);
void createResizedImage(void);
void cleanUp(int i);
int getPadAmount(int imgWidth);

int rVal;   //For resize value

//Create file-related data container
typedef struct
{
	char *name;
	FILE *file;
	BITMAPFILEHEADER bFH;
	BITMAPINFOHEADER bIH;
}
fileObj;

fileObj fileData[2];



int main(int argc, char *argv[])
{
  //Validate args (argc should equal 4, argv[1] should be int 1-100)
  if (!checkArgs(argc,argv,4,"resize"))
  {
    return 1;   //Error out of main
  };
  
  //--Initialize Files--
  //Set file list
  setFileNames(argv);
  
  //Verify files can be opened/created
  int fileStatus = validateFiles();
  
  if (fileStatus != 0)
  {
    return fileStatus;
  };
  
  //Get file headers, verify file type
  if(!checkFileInHeaders())
  {
    return 4;
  };
  
  //Create resized image
  createResizedImage();
  
  return 0;
}




//-------------------------------
bool checkArgs (int argcTemp, char *argvTemp[], int argQty, char *progName)
{
  //Check for correct arg count
  if (argcTemp != argQty)
  {
    fprintf(stderr, "[Error] Usage: ./%s <resize value> <file_in> <file_out>\n",progName);
    return false;
  };
  
  //Check resize value is valid
  rVal = atoi(argvTemp[1]);
  
  if (rVal < 1 || rVal > 100)
  {
    fprintf(stderr, "[Error] Resize value must be between 1 and 100.\n");
    return false;
  };
  
  return true;
}

//-------------------------------
void setFileNames(char *argvTemp[])
{
  fileData[0].name = argvTemp[2];
  fileData[1].name = argvTemp[3];
}

//-------------------------------
int validateFiles(void)
{
  //Test input
  fileData[0].file = fopen(fileData[0].name, "r");
  if (fileData[0].file == NULL)
  {
    fprintf(stderr, "[Error] Could not open \'%s\'.\n", fileData[0].name);
    return 2;
  };
  
  //Test output
  fileData[1].file = fopen(fileData[1].name, "w");
  if (fileData[1].file == NULL)
  {
    //Clean up
    cleanUp(1);
    
    fprintf(stderr, "[Error] Could not create \'%s\'.\n", fileData[1].name);
    return 3;
  };
  
  return 0;
}
//-------------------------------
bool checkFileInHeaders(void)
{
  fread(&fileData[0].bFH, sizeof(BITMAPFILEHEADER), 1, fileData[0].file);
  fread(&fileData[0].bIH, sizeof(BITMAPINFOHEADER), 1, fileData[0].file);
  
  if (fileData[0].bFH.bfType != 0x4d42 || fileData[0].bFH.bfOffBits != 54 || 
      fileData[0].bIH.biSize != 40 || fileData[0].bIH.biBitCount != 24 || fileData[0].bIH.biCompression != 0)
  {
    //Clean up
    cleanUp(2);
    
    fprintf(stderr, "[Error] Unsupported file format for input.\n");
    return false;
  };
  
  return true;
}
//-------------------------------
void createResizedImage(void)
{
  //Image padding
  int paddingOut;
  int padding = getPadAmount(fileData[0].bIH.biWidth);
  
  //Copy header data to otuput file
  fileData[1].bFH = fileData[0].bFH;
  fileData[1].bIH = fileData[0].bIH;
  
  //Update for resized image (if rVal>1, else will have same headers)
  if (rVal > 1)
  {
    fileData[1].bIH.biWidth *= rVal;
    fileData[1].bIH.biHeight *= rVal;
    
    paddingOut = getPadAmount(fileData[1].bIH.biWidth);
    
    fileData[1].bIH.biSizeImage = ((sizeof(RGBTRIPLE) * fileData[1].bIH.biWidth) + paddingOut) * abs(fileData[1].bIH.biHeight);
    fileData[1].bFH.bfSize = fileData[1].bIH.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  }
  else
  {
    paddingOut = padding;
  };
  
  //Write BITMAPFILEHEADER & BITMAPINFOHEADER
  fwrite(&fileData[1].bFH, sizeof(BITMAPFILEHEADER), 1, fileData[1].file);
  fwrite(&fileData[1].bIH, sizeof(BITMAPINFOHEADER), 1, fileData[1].file);
  
  //Temporary storage
  RGBTRIPLE triple[fileData[0].bIH.biWidth];
  
  // iterate over input height
  for (int i = 0, biHeight = abs(fileData[0].bIH.biHeight); i < biHeight; i++)
  {
      // iterate over pixels in scanline
      for (int j = 0; j < fileData[0].bIH.biWidth; j++)
      {
        // read RGB triple from input
        fread(&triple[j], sizeof(RGBTRIPLE), 1, fileData[0].file);
      };
      
      //Skip over input padding if needed
      fseek(fileData[0].file, padding, SEEK_CUR);
          
      //Loop to resize height
      for (int h=0;h<rVal;h++)
      {
        //Loop to resize width & write pixels to output, pad
        for (int k=0;k<fileData[0].bIH.biWidth;k++)
        {
          for (int w=0;w<rVal;w++)
          {
            fwrite(&triple[k], sizeof(RGBTRIPLE), 1, fileData[1].file);
          };
        };
        
        //Add padding if needed
        for (int l=0;l<paddingOut;l++)
        {
            fputc(0x00, fileData[1].file);
        };
      };
  };
  
  //Clean up
  cleanUp(2);
}
//-------------------------------
void cleanUp(int i)
{
  fclose(fileData[0].file);
  
  if (i==2)
  {
    fclose(fileData[1].file);
  };
}
//-------------------------------
int getPadAmount(int imgWidth)
{
  return ((4 - (imgWidth * sizeof(RGBTRIPLE)) % 4) % 4);
}