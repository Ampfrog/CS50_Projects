/**
 * Implements a dictionary's functionality.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dictionary.h"


typedef struct nodeTrie
{
    bool isWord;
    struct nodeTrie *children[27];
}
node;

void freeNode(node *thisNode);
int getIndex(char c);

//Track root node
node *root = NULL;
node *nodeCurr = NULL;

//Track dictionary size
int dSize = 0;



//-------------------------------------------
/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char *dictionary)
{
    //Open dictionary
    FILE *dFile = fopen(dictionary, "r");
    if (dFile == NULL)
    {
        return false;
    };
    
    //Init
    char wordCurr[LENGTH+1];
    root = malloc(sizeof(node));
    
    if (root == NULL)
    {
        //Out of mem
        return false;
    };
    
    //Load words into trie
    while(fscanf(dFile, "%s", wordCurr) != EOF)
    {
        //Add to size tracking
        dSize++;
        
        //Reset "current node" tracker
        nodeCurr = root;
        
        //Loop through word, create path for letters
        for (int i=0, l=strlen(wordCurr); i<l; i++)
        {
            //Get index of current char
            int indx = getIndex(wordCurr[i]);
            
            //Check if node exists at index
            if (nodeCurr->children[indx] == NULL)
            {
                nodeCurr->children[indx] = malloc(sizeof(node));
                
                if (nodeCurr->children[indx] == NULL)
                {
                    //Out of mem
                    return false;
                };
                
                nodeCurr = nodeCurr->children[indx];
                nodeCurr->isWord = false;
            }
            else
            {
                nodeCurr = nodeCurr->children[indx];  
            };
            
            if (i == (l-1))
            {
                nodeCurr->isWord = true;
            };
        };
    };
    
    return true;
}
//-------------------------------------------
/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return dSize;
}
//-------------------------------------------
/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char *word)
{
    //Reset "current node" tracker
    nodeCurr = root;
    
    //Start travelling path (if exists)
    for (int i=0,wLen=strlen(word);i<wLen;i++)
    {
        //Get index of current char
        int indx = getIndex(word[i]);
        
        if (nodeCurr->children[indx] != NULL)
        {
            nodeCurr = nodeCurr->children[indx];
        }
        else
        {
            return false;
        };
    };
    
    //Full path found, check if a valid word
    if (nodeCurr->isWord)
    {
        return true;
    }
    else
    {
        return false;
    };
}
//-------------------------------------------
/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool unload(void)
{
    //Reset "current node" tracker
    nodeCurr = root;
    
    //Start travelling all paths
    freeNode(nodeCurr);
    
    return true;
}
//-------------------------------------------
//Recursive function to check down all paths, start freeing each in reverse once end of path is reached
void freeNode(node *thisNode)
{
    for (int i=0;i<27;i++)
    {
        if (thisNode->children[i] != NULL)
        {
            freeNode(thisNode->children[i]);
        };
    };
    
    free(thisNode);
}
//-------------------------------------------
//Calculate index of letter, case insensitive
int getIndex(char c)
{
    int indTemp = tolower(c) - 97;
    
    if (indTemp < 0)
    {
        //Apostrophe
        indTemp = 26;
    };
    
    return indTemp;
}
//-------------------------------------------