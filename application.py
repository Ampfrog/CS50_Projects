from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session, url_for
from flask_session import Session
from passlib.apps import custom_app_context as pwd_context
from tempfile import mkdtemp

from helpers import *

# configure application
app = Flask(__name__)

# ensure responses aren't cached
if app.config["DEBUG"]:
    @app.after_request
    def after_request(response):
        response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
        response.headers["Expires"] = 0
        response.headers["Pragma"] = "no-cache"
        return response

# custom filter
app.jinja_env.filters["usd"] = usd

# configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

@app.route("/")
@login_required
def index():
    """Show portfolio"""
    
    # get cash balance
    cashAvail = db.execute("SELECT cash FROM users WHERE id = :userId", userId=session["user_id"])
    
    # get stock portfolio data
    stocks = db.execute("SELECT sName, sSymbol, SUM(qty) FROM portfolio WHERE userId = :userId GROUP BY sName", userId=session["user_id"])
    
    # confirm successful query
    if len(cashAvail) > 0 and stocks is not None:
        cash = cashAvail[0]["cash"]
        
        # get current stock prices, format
        priceDict = {}
        stockBal = 0
        
        for i in range(len(stocks)):
            stockSym = stocks[i]["sSymbol"]
            quote = lookup(stockSym)
            
            # validate stock symbol (ensure not removed from market)
            if quote is None:
                stocks[i]["price"] = 0.00
            else:
                stocks[i]["price"] = quote["price"]
            
            # calculate total value for current stock, and portfolio stock total
            stocks[i]["total"] = stocks[i]["price"] * float(stocks[i]["SUM(qty)"])
            stockBal += stocks[i]["total"]
            
            # format
            stocks[i]["total"] = usd(stocks[i]["total"])
            stocks[i]["price"] = usd(stocks[i]["price"])
        
        return render_template("index.html", cashBal=usd(cash), stockBal=usd(stockBal), totalBal=usd(cash+stockBal), stocks=stocks)
    else:
        return apology("account data currently not available","","/")

@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock."""
    
    # if user reached route via POST
    if request.method == "POST":
        
        # ensure stock symbol was submitted
        if not request.form.get("stock"):
            return apology("must provide stock symbol","","buy")
        
        # ensure stock symbol was submitted
        elif not request.form.get("qty"):
            return apology("must provide a quantity","","buy")
        
        # get cash balance
        cashAvail = db.execute("SELECT cash FROM users WHERE id = :userId", userId=session["user_id"])
        
        # confirm successful query
        if len(cashAvail) > 0:
            cash = cashAvail[0]["cash"]
            
            # check for $0 balance (don't need to check stock price if True)
            if cash <= 0:
                return apology("no cash available in account","","buy")
            
            # lookup quote
            stock = request.form.get("stock")
            qty = request.form.get("qty")
            quote = lookup(stock)
            
            # validate stock symbol
            if quote is None:
                return apology("invalid stock symbol","","buy")
            
            # affordability check
            totalCost = quote["price"] * float(qty)
            
            if totalCost > cash:
                return apology("total cost is more","than cash available","buy")
            else:
                # update cash balance
                cash -= totalCost
                db.execute("UPDATE users SET cash=:cash WHERE id=:userId", cash=cash, userId=session["user_id"])
                
                # update portfolio
                db.execute("INSERT INTO portfolio (userId, sName, sSymbol, price, qty, total, date, action) VALUES (:userId, :sName, :sSymbol, :price, :qty, :total, datetime('now'), 1)",
                                    userId=session["user_id"],
                                    sName=quote["name"],
                                    sSymbol=quote["symbol"],
                                    price=quote["price"],
                                    qty=qty,
                                    total=totalCost)
            
                return render_template("bought.html", stockName=quote["name"], stockSym=quote["symbol"], stockPrice=usd(quote["price"]),
                                                      stockQty=qty, stockTotal=usd(totalCost), stockCash=usd(cash))
        else:
            return apology("account data currently not available","","buy")
        
    # else if user reached route via GET
    else:
        return render_template("buy.html")

@app.route("/history")
@login_required
def history():
    """Show history of transactions."""
    
    # get stock portfolio data
    stocks = db.execute("SELECT sName, sSymbol, price, qty, total, date, actionType FROM portfolio JOIN actionTypes ON portfolio.action=actionTypes.id WHERE userId = :userId", userId=session["user_id"])
    
    # confirm successful query
    if stocks is not None:
        for i in range(len(stocks)):
            # format
            stocks[i]["total"] = usd(stocks[i]["total"])
            stocks[i]["price"] = usd(stocks[i]["price"])
            stocks[i]["qty"] = abs(stocks[i]["qty"])
            
        return render_template("history.html", stocks=stocks)
    else:
        return apology("account data currently not available","","history")

@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in."""

    # forget any user_id
    session.clear()

    # if user reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username","","login")

        # ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password","","login")

        # query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))

        # ensure username exists and password is correct
        if len(rows) != 1 or not pwd_context.verify(request.form.get("password"), rows[0]["hash"]):
            return apology("invalid username and/or password","","login")

        # remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # redirect user to home page
        return redirect(url_for("index"))

    # else if user reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")

@app.route("/logout")
def logout():
    """Log user out."""

    # forget any user_id
    session.clear()

    # redirect user to login form
    return redirect(url_for("login"))

@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    
    # if user reached route via POST
    if request.method == "POST":
        
        # ensure stock symbol was submitted
        if not request.form.get("stock"):
            return apology("must provide stock symbol","","quote")
        
        # lookup quote
        stock = request.form.get("stock")
        quote = lookup(stock)
        
        # validate stock symbol
        if quote is None:
            return apology("invalid stock symbol","","quote")
        
        return render_template("quoted.html", stockName=quote["name"], stockSym=quote["symbol"], stockPrice=usd(quote["price"]))
        
    # else if user reached route via GET
    else:
        return render_template("quote.html")

@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user."""
    
    # if user reached route via POST
    if request.method == "POST":
        
        # ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username","","register")
        
        # ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password","","register")
        
        # ensure password confirmation was submitted
        elif not request.form.get("passwordConf"):
            return apology("must confirm password","","register")
        
        # ensure password was submitted
        elif request.form.get("passwordConf") != request.form.get("password"):
            return apology("password confirmation incorrect","","register")
        
        # Validated, add user to database
        result = db.execute("SELECT * FROM users WHERE username = :username", username=request.form.get("username"))
        
        # ensure username is unique
        if len(result) == 1:
            return apology("username already exists","","register")
        else:
            idAdded = db.execute("INSERT INTO users (username, hash) VALUES (:username, :hash)",
                                    username=request.form.get("username"),
                                    hash=pwd_context.hash(request.form.get("password")))
            
            # remember which user has logged in
            session["user_id"] = idAdded
    
            # redirect user to home page
            return redirect(url_for("index"))
            

    # else if user reached route via GET
    else:
        return render_template("register.html")
    
    

@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock."""
    
    # if user reached route via POST
    if request.method == "POST":
        # ensure stock symbol was submitted
        if not request.form.get("stock"):
            return apology("must provide stock symbol","","sell")
        
        # ensure stock symbol was submitted
        elif not request.form.get("qty"):
            return apology("must provide a quantity","","sell")
        
        # get cash balance
        cashAvail = db.execute("SELECT cash FROM users WHERE id = :userId", userId=session["user_id"])
        
        # confirm successful query
        if len(cashAvail) > 0:
            cash = cashAvail[0]["cash"]
            
            # inputs
            stock = request.form.get("stock").upper()
            qty = request.form.get("qty")
            
            # get stock portfolio data
            stocks = db.execute("SELECT SUM(qty) FROM portfolio WHERE userId=:userId AND sSymbol=:sSymbol GROUP BY sName", userId=session["user_id"], sSymbol=stock)
            
            if len(stocks) != 1:
                # stock not found
                return apology("stock is not in portfolio","","sell")
            elif float(qty) > stocks[0]["SUM(qty)"]:
                # stock qty request > stock qty available
                return apology("qty is too large","","sell")
            else:
                # lookup quote
                quote = lookup(stock)
                
                # validate stock symbol (make sure biz is still in market) and calculate value
                if quote is None:
                    totalCost = 0.0
                else:
                    totalCost = quote["price"] * float(qty)
                
                # update cash balance
                cash += totalCost
                db.execute("UPDATE users SET cash=:cash WHERE id=:userId", cash=cash, userId=session["user_id"])
                
                # update portfolio
                db.execute("INSERT INTO portfolio (userId, sName, sSymbol, price, qty, total, date, action) VALUES (:userId, :sName, :sSymbol, :price, :qty, :total, datetime('now'), 2)",
                                    userId=session["user_id"],
                                    sName=quote["name"],
                                    sSymbol=quote["symbol"],
                                    price=quote["price"],
                                    qty=int(qty) * -1,
                                    total=totalCost)
                
                return render_template("sold.html", stockName=quote["name"], stockSym=quote["symbol"], stockPrice=usd(quote["price"]),
                                                      stockQty=qty, stockTotal=usd(totalCost), stockCash=usd(cash))
        else:
            return apology("account data currently not available","","sell")
    
    # else if user reached route via GET
    else:
        return render_template("sell.html")

"""Personal Touch - Add cash to account"""
@app.route("/deposit", methods=["GET", "POST"])
@login_required
def deposit():
    """Add cash to account."""
    
    # if user reached route via POST
    if request.method == "POST":
        # ensure stock symbol was submitted
        if not request.form.get("amount"):
            return apology("must provide amount","","deposit")
        
        cashAmt = float(request.form.get("amount"))
        
        # get cash balance
        cashAvail = db.execute("SELECT cash FROM users WHERE id = :userId", userId=session["user_id"])
        
        # confirm successful query
        if len(cashAvail) > 0:
            # get total
            cash = cashAvail[0]["cash"]
            cashTtl = cashAmt + cash
            
            # save to account
            db.execute("UPDATE users SET cash = :cash WHERE id = :userId", cash=cashTtl, userId=session["user_id"])
            
            return render_template("deposited.html", cashAmt=usd(cashAmt), cashTtl=usd(cashTtl))
        else:
            return apology("account data currently not available","","deposit")
    
    # else if user reached route via GET
    else:
        return render_template("deposit.html")

""" For testing only """
#return render_template("test.html", test=salesData)