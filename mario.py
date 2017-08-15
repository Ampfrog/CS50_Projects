import cs50

def main():
	#Print introduction
	printIntro()
	
	#Input base count
	baseBlockCount = getBlockQty();
	
	#Create output
	for i in range(1,baseBlockCount+1):
		#Space to left of blocks (if needed)
		drawLeftSpc(baseBlockCount-i);
		
		#Blocks for current row
		drawBlocks(i);
	

def printIntro():
	intro = ("--[ Mario Half-Pyramid Builder with Gap to Jump ]--\n\n"
			"Overview:\n"
			"Enter the number of blocks required at the base of each half-pyramid\n"
			"to set the overall size. Allowable range is 0 to 23 blocks.\n\n");
	
	print("{}".format(intro));


def getBlockQty():
	while True:
		print("Number of blocks in each base: ", end="");
		i = cs50.get_int();
		
		#Validate
		if i > 0 and i < 24:
			break;
	
	return i;


def drawLeftSpc(sQty):
	for i in range(sQty):
		print(" ", end="");


def drawBlocks(bQty):
	for j in range(2):
		for i in range(bQty):
		  print("#", end="");
	
		#Add space between blocks & break after blocks
		if j == 0:
		  #Gap
		  print("  ", end="");
		else:
		  #Line break
		  print("");


if __name__ == "__main__":
	main()