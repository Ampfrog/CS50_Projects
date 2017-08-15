import nltk

class Analyzer():
    """Implements sentiment analysis."""

    def __init__(self, positives, negatives):
        """Initialize Analyzer. Load pos/neg words."""
        
        #Set word list file locations
        self.files = [positives,negatives]
        
        #Create dictionary
        self.wordList = self.loadWordList()

    def analyze(self, text):
        """Analyze text for sentiment, returning its score. (neg=-1,pos=1,else 0)"""
        
        if " " in text:
            tokenizer = nltk.tokenize.TweetTokenizer()
            textList = tokenizer.tokenize(text)
        else:
            textList = [text]
            
        totalScore = 0;
        
        for i in textList:
            if i.lower() in self.wordList:
                totalScore += self.wordList[i.lower()]
        
        return totalScore
    
    def loadWordList(self):
        """Load words from files into dictionary"""
        
        tempList = {}
        
        for i in self.files:
            if "negative" in str(i):
                score = -1
            else:
                score = 1
            
            with open(i) as lines:
                for line in lines:
                    #Append to list if not a comment or blank
                    line = line.strip()
                    
                    if line.startswith(";") is False and len(line) > 0:
                        tempList[line] = score
        
        return tempList