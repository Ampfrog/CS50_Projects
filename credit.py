import cs50
import sys


def main():
    #Print introduction
    printIntro();
    
    #Get User Input
    cc = getCCNumber();
    
    #Get digits to work with
    ccParts = sliceCC(cc);
    
    #Validate
    validateCard(ccParts);
    
    sys.exit(0);


#-------------------------
def printIntro():
	intro = ("--[ Credit Card Type & Validity Checker ]--\n\n"
            "Overview:\n"
            "Enter a credit card number (numbers only, no dashes) to determine the brand and check if valid.\n\n"
            "- If output is a card brand, number is valid for that brand\n"
            "- If output is 'INVALID', the type or number (or both) are invalid\n\n");
	
	print(intro);


def getCCNumber():
    while True:
    	print("Enter a credit card number: ", end="");
    	i = cs50.get_int();
    	
    	#Validate
    	if i > 0:
    		break;
    
    return i;


def sliceCC(ccTemp):
    ccStr = str(ccTemp)
    ccDigits = [-1]
    
    #Check if longer than 16 digits (max allowed)
    if len(ccStr) > 16:
        return ccDigits;
    else:
        ccDigits.pop(0);
    
    for i in ccStr:
        ccDigits.append(int(i))
    
    return ccDigits


def validateCard(ccPartsTemp):
    if ccPartsTemp[0] == -1:
        #Too many digits, determined in sliceCC()
        print("INVALID");
    else:
        #Determine type
        ccType = "";
        ccLen = len(ccPartsTemp);
        validType = False;
    
        if ccPartsTemp[0] == 3 and (ccPartsTemp[1] == 4 or ccPartsTemp[1] == 7) and ccLen == 15:
            ccType = "AMEX";
            validType = True;
        elif ccPartsTemp[0] == 5 and (ccPartsTemp[1] >= 1 and ccPartsTemp[1] <= 5) and ccLen == 16:
            ccType = "MASTERCARD";
            validType = True;
        elif ccPartsTemp[0] == 4 and (ccLen == 16 or ccLen == 13):
            ccType = "VISA";
            validType = True;
        else:
            #Invalid prefix
            print("INVALID");
    
        if validType:
            #Valid type & length, now run checksum algorithm
            j = len(ccPartsTemp)-2   #Second-to-last digit
            luhnTtl = 0
            
            #Luhn’s Step 0
            while j > -1:
                multiple = 2 * ccPartsTemp[j];
                
                #Get digits & sum
                for i in str(multiple):
                    luhnTtl += int(i);
                
                #Set next index
                j -= 2;
          
            #Luhn’s Step 1: add unused digits from cc number to balance
            j = len(ccPartsTemp) - 1;
          
            while j > -1:
                luhnTtl += ccPartsTemp[j];
                j -= 2;
          
            #Luhn's Step 2
            if (luhnTtl % 10 != 0):
                validType = False;
            
            #Final output - for testing only
            #print("Luhn's Total: {}".format(luhnTtl));
            
            if validType:
                print(ccType);
            else:
                print("INVALID");


#-------------------------
if __name__ == "__main__":
	main()