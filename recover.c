#include <cs50.h>
#include <stdint.h>
#include <stdio.h>

bool checkArgs (int argcTemp, int argQty, char *progName);
void setFileNames(char *argvTemp[]);
int validateFiles(void);
void findJpgs(void);
void cleanUp(FILE *fileName);

//Create file-related data container
typedef struct
{
	char *name;
	FILE *file;
}
fileObj;

fileObj fileData[1];



int main(int argc, char *argv[])
{
  //Validate args
  if (!checkArgs(argc,2,"recover"))
  {
    return 1;   //Error out of main
  };
  
  //--Initialize Files--
  //Set file list
  setFileNames(argv);
  
  //Verify input file can be opened
  int fileStatus = validateFiles();
  
  if (fileStatus != 0)
  {
    return fileStatus;
  };
  
  //Recover images
  findJpgs();
  
  return 0;
}


//-------------------------------
bool checkArgs (int argcTemp, int argQty, char *progName)
{
  //Check for correct arg count
  if (argcTemp != argQty)
  {
    fprintf(stderr, "[Error] Usage: ./%s <file_in>\n",progName);
    return false;
  };
  
  return true;
}

//-------------------------------
void setFileNames(char *argvTemp[])
{
  fileData[0].name = argvTemp[1];
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
  
  return 0;
}
//-------------------------------
void findJpgs(void)
{
  //Create buffer & init vars
  typedef uint8_t  BYTE;
  BYTE buffer[512];
  
  int jpgCount = 0;
  bool writeNow = false;
  
  FILE *imgCurr = NULL;
  char filename[8];
  
  //Read through file
  while(true)
  {
    //Read & save result for EOF check
    if (fread(&buffer, 1, 512, fileData[0].file) != 512)
    {
      //EOF found
      break;
    };
    
    if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
    {
      //Store file name
      sprintf(filename, "%03i.jpg", jpgCount);
      
      //Close prev img file if needed
      if (imgCurr != NULL)
      {
        cleanUp(imgCurr);
      };
      
      //Open new file
      imgCurr = fopen(filename, "w");
      
      //Update count
      jpgCount++;
      
      //Set flag to allow writing
      if (!writeNow)
      {
        writeNow = true;
      };
    };
    
    //Only write after initial .raw buffer is passed
    if (writeNow)
    {
      fwrite(&buffer, 1, 512, imgCurr);
    };
  };
  
  
  //Close files
  cleanUp(fileData[0].file);
}
//-------------------------------
void cleanUp(FILE *fileName)
{
  fclose(fileName);
}
//-------------------------------
