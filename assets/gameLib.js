var BA = BA || {};

BA.init = function() {
	/* Initialize game */
	
	//Cleanup
	$("#preload").remove();
	
	//Load top scores if available
	BA.data.topScoresInit();
	
	//Hijack keyboard arrow keys
	BA.ctrls.addArrowKeyCtrls();
	
	//Set game area
	BA.draw.createGameArea();
	
	//Add intro image
	BA.data.intro = new BA.draw.imageObj(600,400,0,0,BA.data.imgLoc.intro,"img");
	BA.data.intro.image.onload = function(){BA.data.intro.render();};
	
	$("#gameArea").on("click", function(){
		//Prevent multiple starts
		$("#gameArea").off("click");
		
		//Set refresh rate
		BA.draw.refreshRate = setInterval(BA.draw.refreshGameArea,10);
		
		//Create & add baddy and catcher
		BA.draw.createImgObjects();
		
		//Other area data
		BA.data.edgeRt = BA.data.canvas.width - BA.data.catcher.width;
		BA.data.edgeBtm = BA.data.canvas.height - 30;
		
		//Set level data
		BA.data.setLevel(1);
		
		//Start animation
		BA.draw.startTimers();
	});
};



BA.data = {
	moveLeft: false,
	moveRight: false,
	
	catcherSpeed: 8,
	baddySpeed: 0,
	baddyDirChangeProb: 20,		//Out of 1000
	bombRate: 2,
	
	level: 0,
	levelParams: {
		"bombQty": 0,		//Bombs to complete level
		"dropIntvl": 0,	//Bomb drop interval
		"bValue": 0,		//Score per bomb
		"created": 0,		//Track # created for current level
		"caught": 0			//Track # caught for current level
	},
	
	score: 0,
	
	imgLoc: {"baddy": "assets/img/baddy.png",
					 "bomb": "assets/img/bomb.png",
					 "boom": "assets/img/boom.png",
					 "catcher": "assets/img/catcher.png",
					 "background": "assets/img/back.jpg",
					 "intro": "assets/img/intro.jpg"},
	
	setLevel: function(lvl) {
		this.level = lvl;
		this.baddySpeed = Math.abs(this.baddySpeed) + 3;
		if (Math.floor(this.level/3) >= 1) {this.bombRate += 1;};
		this.levelParams.bombQty = this.level * 3 + 5;
		this.levelParams.bValue = this.level * 5;
		this.levelParams.created = 0;
		this.levelParams.caught = 0;
		this.levelParams.dropIntvl = 500 - this.level * 5;
	},
	
	topScoresInit: function() {
		if (typeof(Storage) !== "undefined") {
			BA.data.localStore = true;
			
			//Check if data is available
			var tempStr = localStorage.getItem("bombsAwayTopScores");
			
			if (tempStr == undefined)
			{
				//Init local scoreboard
				localStorage.setItem("bombsAwayTopScores","0");
				$("#topScoreList").html(BA.data.loadTopScores(["0"]));
			}
			else
			{
				//Data found, load top scores
				var topScoreList = tempStr.split(",");
				
				$("#topScoreList").html(BA.data.loadTopScores(topScoreList));
			};
		}
		else
		{
			//Local storage not supported
			BA.data.localStore = false;
			$("#topScoreList").append("Not supported by browser");
		};
	},
	
	loadTopScores: function(scoreList) {
		var newScrHtml = "<ol>";
		
		$.each(scoreList, function(){
			newScrHtml += "<li>"+this+"</li>";
		});
		
		newScrHtml += "</ul>";
		
		return newScrHtml;
	}
};



BA.draw = {
	baddyTimer: null,
	bombAddTimer: null,
	bombMoveTimer: null,
	bombList: [],
	refreshRate: null,
	
	refreshGameArea: function(){
		//Clear area
		BA.data.canvas.context.clear();
		
		//Move catcher if needed
		if (BA.data.moveLeft)
		{
			BA.move.left(BA.data.catcher,BA.data.catcherSpeed);
		}
		else if (BA.data.moveRight)
		{
			BA.move.right(BA.data.catcher,BA.data.catcherSpeed);
		};
		
		//Redraw
		BA.data.background.render();
		BA.data.scoreboard.text = "Level: "+BA.data.level+"          Score: "+BA.data.score;
		BA.data.scoreboard.render();
		BA.data.baddy.render();
		BA.data.catcher.render();
		$.each(BA.draw.bombList, function(i){
			this.render();
		});
	},
	
	createGameArea: function() {
		BA.data.canvas = $("#gameArea")[0];
		BA.data.canvas.context = BA.data.canvas.getContext("2d");
		BA.data.canvas.context.clear = function() {
			this.clearRect(0,0,BA.data.canvas.width,BA.data.canvas.height);
		};
	},
	
	createImgObjects: function() {
		//Create
		BA.data.background = new BA.draw.imageObj(600,400,0,0,BA.data.imgLoc.background,"img");
		BA.data.baddy = new BA.draw.imageObj(47,45,BA.data.canvas.width/2-23,35,BA.data.imgLoc.baddy,"img");
		BA.data.catcher = new BA.draw.imageObj(113,21,BA.data.canvas.width/2-56,BA.data.canvas.height-30,BA.data.imgLoc.catcher,"img");
		BA.data.scoreboard = new BA.draw.imageObj("14px","Times New Roman",20,20,"#000","text");
	
		//Render
		BA.data.background.image.onload = function(){BA.data.background.render();};
		BA.data.baddy.image.onload = function(){BA.data.baddy.render();};
		BA.data.catcher.image.onload = function(){BA.data.catcher.render();};
		BA.data.scoreboard.render();
	},
	
	updateBaddy: function() {
		if (BA.data.levelParams.created < BA.data.levelParams.bombQty)
		{
			//Change dir at edges, or check for random change
			if (BA.data.baddy.xPos < 5 || BA.data.baddy.xPos > BA.data.canvas.width-BA.data.baddy.width-5)
			{
				BA.data.baddySpeed *= -1;
			}
			else
			{
				var rNum = Math.floor(Math.random() * 1000);
				
				if (rNum < BA.data.baddyDirChangeProb)
				{
					BA.data.baddySpeed *= -1;
				};
			};
			
			//Update position
			BA.move.left(BA.data.baddy,BA.data.baddySpeed);
		}
		else
		{
			clearInterval(BA.draw.baddyTimer);
		};
	},
	
	addBombs: function() {
		if (BA.data.levelParams.created < BA.data.levelParams.bombQty)
		{
			//Calculate location to start at
			var bombLocX = BA.data.baddy.xPos + (BA.data.baddy.width-25)/2;
			var bombLocY = BA.data.baddy.yPos + BA.data.baddy.height;
			
			//Create bomb
			BA.draw.bombList.push(new BA.draw.imageObj(25,25,bombLocX,bombLocY,BA.data.imgLoc.bomb,"img"));
			BA.data.levelParams.created++;
		}
		else
		{
			//Stop creating bombs until new level starts
			clearInterval(BA.draw.bombAddTimer);
		};
	},
	
	updateBombs: function() {
		//Update existing bombs
		$.each(BA.draw.bombList, function(i){
			this.yPos += BA.data.bombRate;
			
			//Remove bomb if caught or hits ground
			if (this.yPos > BA.data.canvas.height-40)
			{
				//Check if caught
				var bmbRtEdge = this.xPos + this.width;
				var bmbLftEdge = this.xPos;
				var ctchRtEdge = BA.data.catcher.xPos + BA.data.catcher.width;
				var ctchLftEdge = BA.data.catcher.xPos;
				
				if (bmbRtEdge > ctchLftEdge && bmbLftEdge < ctchRtEdge)
				{
					//Caught, remove bomb, update score, move to next level if needed
					BA.data.levelParams.caught++;
					BA.data.score += BA.data.levelParams.bValue;
					BA.draw.bombList.splice(i,1);
					
					//Level up check
					if (BA.data.levelParams.caught == BA.data.levelParams.bombQty)
					{
						BA.data.setLevel(BA.data.level+1);
						
						//Reset timers to start next level
						clearInterval(BA.draw.baddyTimer);
						clearInterval(BA.draw.bombAddTimer);
						clearInterval(BA.draw.bombMoveTimer);
						BA.draw.startTimers();
					};
				}
				else
				{
					//Not caught, explode!
					BA.draw.stopAll();
					//BA.data.canvas.context.clearRect(this.xPos,this.yPos-10,25,35);
					this.image.src = BA.data.imgLoc.boom;
					this.width = 30;
					this.height = 30;
					this.yPos -= 5;
					BA.draw.refreshGameArea();
					
					//End game
					BA.draw.endGame();
				};
			};
		});
	},
	
	imageObj: function(w,h,xPos,yPos,imgLoc,type) {
		this.type = type;
		if (this.type == "img")
		{
			this.image = new Image();
			this.image.src = imgLoc;
		}
		else
		{
			this.fill = imgLoc;
		};
		
		this.width = w;
		this.height = h;
		this.xPos = xPos;
		this.yPos = yPos;
		
		this.render = function() {
			var drawArea = BA.data.canvas.context;
			
			if (this.type == "text")
			{
				drawArea.font = this.width + " " + this.height;
				drawArea.fillStyle = this.fill;
				drawArea.fillText(this.text,
													this.xPos,
													this.yPos);
			}
			else
			{
				drawArea.drawImage(this.image,
													 this.xPos,
													 this.yPos,
													 this.width,
													 this.height);
			};
    };
	},
	
	endGame: function() {
		//Draw Game Over text
		var drawArea = BA.data.canvas.context;
		
		drawArea.fillStyle = "#000";
		drawArea.fillRect(0,BA.data.canvas.height/2-50,BA.data.canvas.width,70);
		
		drawArea.font = "40px Impact";
		drawArea.fillStyle = "#FFF";
		drawArea.fillText("Game Over",BA.data.canvas.width/2-90,BA.data.canvas.height/2);
		
		drawArea.font = "12px Times New Roman";
		drawArea.fillStyle = "#FFF";
		drawArea.fillText("[Click to replay]",BA.data.canvas.width-100,BA.data.canvas.height/2);
		
		//Update local top scores
		var tempStr = localStorage.getItem("bombsAwayTopScores");
		
		if (tempStr == "0")
		{
			//--No other scores found--
			//Update top score UI
			$("#topScoreList").html(BA.data.loadTopScores([BA.data.score.toString()]));
			
			//Save new data
			localStorage.setItem("bombsAwayTopScores",BA.data.score.toString());
		}
		else
		{
			//Get top scores and compare to current game
			var scoreList = tempStr.split(",");
			var added = -1;
			
			$.each(scoreList, function(i){
				if (BA.data.score >= parseInt(this))
				{
					scoreList.splice(i,0,BA.data.score.toString());
					added = i+1;
					return false;
				};
			});
			
			//Check if room to add as last place if not already added
			if (scoreList.length < 10 && added == -1)
			{
				scoreList.push(BA.data.score.toString());
				added = scoreList.length;
			};
			
			//Only if top scores have changed...
			if (added != -1)
			{
				//Limit to 10 scores
				if (scoreList.length > 10) {scoreList.splice(10);};
				
				//Update top score UI
				$("#topScoreList").html(BA.data.loadTopScores(scoreList));
				
				//Highlight current score
				$("#topScoreList li:nth-of-type("+added+")").css("background","#ffe6cc");
				
				//Save new data
				localStorage.setItem("bombsAwayTopScores",scoreList.join(","));
			};
			
			//Restart game
			$("#gameArea").on("click", function(){window.location="index.html"});
		};
	},
	
	startTimers: function() {
		//Start baddy
		this.baddyTimer = setInterval(BA.draw.updateBaddy,20);
		
		//Start bombs
		this.bombAddTimer = setInterval(BA.draw.addBombs,BA.data.levelParams.dropIntvl);
		this.bombMoveTimer = setInterval(BA.draw.updateBombs,20);
	},
	
	stopAll: function() {
		clearInterval(BA.draw.refreshRate);
		clearInterval(BA.draw.baddyTimer);
		clearInterval(BA.draw.bombAddTimer);
		clearInterval(BA.draw.bombMoveTimer);
	}
};



BA.move = {
	left: function(elem,rate) {
		elem.xPos -= rate;
		
		//Limits check
		if (elem.xPos < 0) {elem.xPos = 0;};
	},

	right: function(elem,rate) {
		elem.xPos += rate;
		
		//Limits check
		if (elem.xPos > BA.data.edgeRt) {elem.xPos = BA.data.edgeRt;};
	},
	
	fall: function(elem,rate) {
		elem.yPos += rate;
	},
	
	stop: function(elem) {
		BA.data.moveLeft = false;
		BA.data.moveRight = false;
	}
};



BA.ctrls = {
	addArrowKeyCtrls: function() {
		//For catcher
		$(document).on("keydown",function(event){
			if (event.which == 37)	//Up = 38, Down = 40 (special items/events?)
			{
				//Left arrow
				event.preventDefault();
				BA.data.moveLeft = true;
				
			}
			else if (event.which == 39)
			{
				//Right arrow
				event.preventDefault();
				BA.data.moveRight = true;
				BA.move.right(BA.data.catcher,BA.data.catcherSpeed);
			}
			else
			{
				return;
			};
		});
		
		$(document).on("keyup",function(event){
			if (event.which == 37 || event.which == 39)
			{
				//Stop moving
				event.preventDefault();
				BA.move.stop(BA.data.catcher);
			};
		});
		
		//Specials
	}
}





//-----------FOR TESTING------------
function cl(a) {
	console.log(a);
}