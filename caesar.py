import cs50
import sys

def main():
    #Validate args
    if checkArgs(sys.argv) is False:
        sys.exit(1)   #Error out of main, bad execution
    
    #Encrypt user string
    encryptUserStr(input("plaintext:  "), int(sys.argv[1]));
    
    sys.exit(0)


#-------------------------
def checkArgs(argvTemp):
    #Check for correct arg count
    if len(argvTemp) != 2:
        print("[Error] Usage: caesar.py <key>");
        return False;
  
    #Key arg is present, convert and validate int
    keyIn = int(argvTemp[1]);
    
    #Check lower limit
    if (keyIn < 1):
        print("[Error] Key must be an integer greater than 0.");
        return False;

    return True;

def encryptUserStr(plainStr, key):
    print("ciphertext: ", end="");
    
    #Account for wrapping
    actualOffset = key % 26;
    
    #Encrypt user string
    for i in plainStr:
        #Current char
        currPlain = i;

        if currPlain.isalpha() is True:
            #Letter found, encrypt
            currCiph = ord(currPlain)+actualOffset;
            
            if chr(currCiph).isalpha() is False:
                #Wrap
                currCiph -= 26;
            
            print(chr(currCiph), end="");
        else:
            #Not a letter, do not encrypt
            print(currPlain, end="");
    
    print("")

#-------------------------
if __name__ == "__main__":
	main()