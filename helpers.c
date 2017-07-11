/**
 * helpers.c
 *
 * Helper functions for Problem Set 3.
 */
 
#include <cs50.h>
#include <stdio.h>
#include "helpers.h"

const int MAX_VALUES_LEN = 65536;

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n)
{
    //Validate values[] length
    if (n < 1)
    {
        return false;
    };
    
    /* 
        Perform binary search
        - O(log n)
        - Requires: sorted list
    */
    
    //Set current index to check and initial bounds
    int middleCurr = n/2, lowerB = 0, upperB = n;
    
    if (value == values[middleCurr])
    {
        //Value found
        return true;
    }
    else
    {
        do
        {
            //Adjust bounds
            if (value > values[middleCurr])
            {
                lowerB = middleCurr + 1;
            }
            else
            {
                upperB = middleCurr - 1;
            };
            
            //Get new middle
            middleCurr = (upperB + lowerB)/2;
            
            //Check for match
            if (value == values[middleCurr])
            {
                //Value found
                return true;
            };
        }
        while (upperB-lowerB > 2);      //(or loop may infinitely check at bound index in some cases, such as n=10 lowB=9 upB=10)
        
        //Last hope
        if (value == values[upperB] || value == values[lowerB])
        {
            //Value found
            return true;
        };
    };
    
    
    //***Note from video: middle of array was already checked, so next check uses middleIndex+1 if value is right of middle (or -1 if left of middle)
    //3:30 = pseudo-code frame
    
    //***insert code for binary search*** (see Hints)
    
    
    
    
    
    
    //---Start Temp - linear--- DELETE ---
    /*
    for (int i=0;i<n;i++)
    {
        if (values[i] == value)
        {
            //Value found
            return true;
        };
    };
    */
    //--End Temp --- DELETE ---
    
    
    //Value not found
    return false;
}

/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    //Init temp holding array
    int countAry[MAX_VALUES_LEN];
    int countIndexCurr = 0;
    
    //Counting sort - O(n)
    for (int i=0;i<n;i++)
    {
        //Add to holding array
        countAry[values[i]]++;
    };
    
    //Fill original array with sorted values
    for (int j=0;j<n;j++)
    {
        while (countAry[countIndexCurr] == 0)
        {
            countIndexCurr++;
        };
        
        //Add back into values[]
        values[j] = countIndexCurr;
        
        //Reduce count by 1
        countAry[countIndexCurr]--;
    };
    
    return;
}
