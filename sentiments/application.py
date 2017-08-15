import os
import sys

from flask import Flask, redirect, render_template, request, url_for

import helpers
from analyzer import Analyzer

app = Flask(__name__)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/search")

def search():
    #Validate screen_name
    screen_name = request.args.get("screen_name", "")
    if not screen_name:
        return redirect(url_for("index"))

    #Get screen_name's tweets
    tweets = helpers.get_user_timeline(screen_name)
    
    """ Analyze tweets """
    #Absolute paths to lists
    positives = os.path.join(sys.path[0], "positive-words.txt")
    negatives = os.path.join(sys.path[0], "negative-words.txt")
    
    #Instantiate analyzer
    analyzer = Analyzer(positives, negatives)
    
    #Catch errors
    if tweets is None:
        return redirect(url_for("index"))
    
    #Get tweet count
    tweetLen = len(tweets);
    
    if tweetLen == 0:
        return render_template("search.html", chart="-- No tweets found --", screen_name=screen_name)
    else:
        #Limit to most recent 100 tweets (tweetList is newest -> oldest)
        if tweetLen > 100:
            tweetLen = 100
    
        #Init counters
        countList = {"pos":0, "neg":0, "neut":0}
    
        #Loop through tweets
        for i in range(tweetLen):
            score = analyzer.analyze(tweets[i])
            
            if score > 0.0:
                countList["pos"] += 1
            elif score < 0.0:
                countList["neg"] += 1
            else:
                countList["neut"] += 1
    
        #Generate chart
        chart = helpers.chart(countList["pos"], countList["neg"], countList["neut"])
    
        #Render results
        return render_template("search.html", chart=chart, screen_name=screen_name)
