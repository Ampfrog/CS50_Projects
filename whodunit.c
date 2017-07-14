#include <cs50.h>
#include <stdio.h>
#include "bmp.h"

bool checkArgs (int argcTemp, int argQty, char *progName);
void setFileNames(char *argvTemp[]);
int validateFiles(void);
bool checkFileInHeaders(void);
void createOutput(void);
void cleanUp(void);

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
  //Validate args (argc should equal 3)
  if (!checkArgs(argc,3,"whodunit"))
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
  
  //Remove noise
  createOutput();
  
  return 0;
  
  //Answer: It was Colonel Mustard with the candlestick in the library.
}


//-------------------------------
bool checkArgs (int argcTemp, int argQty, char *progName)
{
  //Check for correct arg count
  if (argcTemp != argQty)
  {
    fprintf(stderr, "[Error] Usage: ./%s <file_in> <file_out>\n",progName);
    return false;
  };
  
  return true;
}

//-------------------------------
void setFileNames(char *argvTemp[])
{
  fileData[0].name = argvTemp[1];
  fileData[1].name = argvTemp[2];
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
  
  //Test Output
  fileData[1].file = fopen(fileData[1].name, "w");
  if (fileData[1].file == NULL)
  {
    //Close previously opened input file (prevent leak)
    fclose(fileData[0].file);
    
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
    cleanUp();
    
    fprintf(stderr, "[Error] Unsupported file format for input.\n");
    return false;
  };
  
  return true;
}
//-------------------------------
void createOutput(void)
{
  //Write BITMAPFILEHEADER & BITMAPINFOHEADER
  fwrite(&fileData[0].bFH, sizeof(BITMAPFILEHEADER), 1, fileData[1].file);
  fwrite(&fileData[0].bIH, sizeof(BITMAPINFOHEADER), 1, fileData[1].file);
  
  // determine padding for scanlines
  int padding =  (4 - (fileData[0].bIH.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
  
  // iterate over infile's scanlines
  for (int i = 0, biHeight = abs(fileData[0].bIH.biHeight); i < biHeight; i++)
  {
      // iterate over pixels in scanline
      for (int j = 0; j < fileData[0].bIH.biWidth; j++)
      {
          // temporary storage
          RGBTRIPLE triple;
  
          // read RGB triple from infile
          fread(&triple, sizeof(RGBTRIPLE), 1, fileData[0].file);
          
          //Update color
          if (triple.rgbtRed == 0xff)
          {
            triple.rgbtRed = 0x00;
          }
          else
          {
            triple.rgbtRed = 0x00;
            triple.rgbtBlue = 0x00;
            triple.rgbtGreen = 0x00;
          };
          
          // write RGB triple to outfile
          fwrite(&triple, sizeof(RGBTRIPLE), 1, fileData[1].file);
      };
  
      // skip over padding, if any
      fseek(fileData[0].file, padding, SEEK_CUR);
  
      // then add it back (to demonstrate how)
      for (int k = 0; k < padding; k++)
      {
          fputc(0x00, fileData[1].file);
      };
  };
  
  //Clean up
  cleanUp();
}
//-------------------------------
void cleanUp(void)
{
  fclose(fileData[0].file);
  fclose(fileData[1].file);
}
//-------------------------------