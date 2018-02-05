//Which server to connect to
let SERVER = "thefeeltra.in:8082";

var server,

    //Audio handler, keeps audio objects together
    audio = {
        synth: new Audio("audio/synth.mp3"),
        hover: new Audio("audio/ui-hover.wav"),
        click: new Audio("audio/ui-click.wav"),
        blue: new Audio("audio/blue.wav"),
        pink: new Audio("audio/pink.wav"),
        purple: new Audio("audio/purple.wav"),
        orange: new Audio("audio/orange.wav"),
        green: new Audio("audio/green.wav")
    },

    //Game object
    Game = {
        //Settings related to the game, can be changed on the "Create Game" screen
        settings: {
            maxplayers: 4,
            visibility: 1,
            linusmode: 0,
            scoretowin: 15
        },
        players: {}
    },

    //Keep track of last sync
    previous = Game,

    //Player object
    Player = {

        //Ask for name on load, could be done in the UI but easier on time to do it here
        "name": prompt("Enter your name (Max 8 characters)", "New001").substr(0, 8),

        //Set initial view to the main menu
        "view": "main-menu",

        //If the player has started the game
        "started": false,

        //Total number of points the player has made
        //Pulled from localStorage to save time
        "scores": localStorage.getItem("totalScores") || 0

    },

    //List of keys that are pressed
    Keys = {},

    //Update at glorious 60fps
    GameLoop = setInterval(loop, 1000 / 60);

function initialize() {

    //Detect if key is being pressed
    window.onkeyup = function(e) {
        Keys[e.keyCode] = false;
    }
    window.onkeydown = function(e) {
        Keys[e.keyCode] = true;
    }

    $('.chat-input').keypress(function (e) {

        if (e.which == 13) {
            sendMessage($(this).val());
            $(this).val("");
        }

    });

    //Show stat(s)
    //I plan on adding more stats in the future
    //This one is just proof of concept
    $('[data-stat="totalScores"]').text(Player.scores);

    //Connect to server, temp use localhost
    server = new WebSocket("ws://"+SERVER);

    //Let user know if socket cannot connect
    server.onerror = function() {
        alert("Unable to connect to server.");
    }

    //Initialize the connection to the server
    server.onopen = function(event) {
        let msg = {
            type: "connect",
            name: Player.name
        };
        server.send(JSON.stringify(msg));
    };

    //Recieve data from server
    server.onmessage = function(event) {

        //Parse as JSON
        let msg = JSON.parse(event.data);

        if (msg.type == "init") {

            //Initialize player with generated ID
            Player.ID = msg.id;

            //Show game code in lobby screen
            $('.game-code').text(msg.id);

        }

        else if(msg.type == "chat") {
            $('.chat-content').append("<div class=\"chat-message\">"+msg.sender+": "+msg.message+"</div>");
            $('.chat-content').scrollTop($('.chat-content')[0].scrollHeight);
        }

        //When the user successfully joins a lobby
        else if (msg.type == "lobbyJoined") {

            //Set lobby as the joined lobby
            Player.lobby = msg.lobbyID;

            //Reset hidden elements
            $('li[data-player]').show();

            //Hide unused elements
            $('li[data-player]').each(function() {
                let p = $(this).attr('data-player');
                if (p > msg.Game.maxplayers) {
                    $(this).hide();
                }
            });

            //Set game code
            $('.game-code').text(msg.lobbyID);

            //Remove start button, host only
            $('.start.host').remove();

            //Hide current menu and show lobby
            $('.showing').removeClass('showing');
            $('.lobby').addClass('showing');
            Player.view = "lobby";

        }

        //Recieve server list
        else if (msg.type == "list") {

            //Clear current list of servers
            $('.server-browser').empty();

            //Loop through each server in the list
            for (var s in msg.servers) {

                //Get server info and add element to the physical list
                let server = msg.servers[s];
                $('.server-browser').append('<div class="server" data-server="' + s + '"><div class="name">' + server.players[Object.keys(server.players)[0]].name + '</div><div class="players">' + server.current + '/' + server.maxplayers + '</div></div>')

            }

            //Bind hover sound effect
            $('.server').mouseover(function() {
                audio.hover.currentTime = 0;
                audio.hover.play();
            });

            //Bind click
            $('.server').click(function() {

                //Join lobby from data attribute
                joinLobby($(this).attr('data-server'));

                //Play success sound
                audio.click.currentTime = 0;
                audio.click.play();

            });

        }

        else if(msg.type == "score") {

            //If player has been scored on, add it up
            if(msg.playerID == Player.ID) {
                Player.scores++;
                //Save in localStorage so it can be retrieved later
                //Ideally this would be in a database
                localStorage.setItem("totalScores",Player.scores);
                $('[data-stat="totalScores"]').text(Player.scores);
            }

            audio[msg.color].currentTime = 0;
            audio[msg.color].play();

        }

        //Add player to their own lobby
        else if (msg.type == "lobbyCreated") {

            Player.lobby = Player.ID;

            //Update game code
            $('.game-code').text(Player.ID);

            //Add start button for host
            $('<span class="start host gray">Start</span>').click(startGame).appendTo('.view.lobby');

        }

        //Sync with the server
        else if (msg.type == "sync") {

            try {

                //Get new data
                Game = msg.Game;

                //If lobby has changed, sync it up
                if (!Object.is(Game, previous)) {
                    sync();
                    previous = Game;
                }

            } catch (e) {

                //Remove from lobby if connection is lost
                leaveLobby();

                //Return to main menu
                $('.showing').removeClass('showing');
                $('.main-menu').addClass('showing');
                Player.view = "main-menu";

                //Let the player know what happened
                alert("Lost connection to host.");
                console.log(e);
            }

        }

        //Catch any data sent for testing
        else {
            console.log(msg);
        }

    }

    // Main music loop, volume is temp while settings are made
    audio.synth.loop = 1;
    audio.synth.volume = 0.25;
    audio.synth.play();

    // Bind UI hover sound effect to hover
    audio.hover.volume = 0.1;
    $('.main-menu > ul > li, .setting > ul > li, .start, .back').mouseover(function() {

        //Restart and play sound effect
        audio.hover.currentTime = 0;
        audio.hover.play();

    });

    // Bind UI click sound effect to click
    audio.click.volume = 0.1;
    $('.setting > ul > li').click(function() {

        //Restart and play sound effect
        audio.click.currentTime = 0;
        audio.click.play();

    });

    //Color volumes
    audio.blue.volume = 0.25;
    audio.pink.volume = 0.25;
    audio.purple.volume = 0.25;
    audio.orange.volume = 0.25;
    audio.green.volume = 0.25;

    // Bind main menu view swaps
    $('*[data-view]').click(function() {

        //Get view to change to
        let to = $(this).attr('data-view');

        // Hide current view, show new view
        $('.showing').removeClass('showing');
        $('.' + to).addClass('showing');
        Player.view = to;

    });

    // Get list of servers
    $('*[data-action="browse-servers"]').click(browseServers);

    // Create lobby on click
    $('*[data-action="start-lobby"]').click(createLobby);

    // Leave lobby on click
    $('*[data-action="leave-lobby"]').click(leaveLobby);

    // Bind quit to click event
    $('*[data-action="quit"]').click(quit);

    //Game creation settings
    $('.setting > ul > li').click(function() {

        //Get which setting and what to change it to
        let s = $(this).parents('.setting').attr('data-setting'),
            to = $(this).attr('data-value');

        //Change variable in the Game object
        Game.settings[s] = to;

        //Show change to the user by removing and readding class
        $(this).siblings().removeClass('selected');
        $(this).addClass('selected');

    });

}

function loop() {

    //If the player is in a lobby, sync with server
    if (Player.lobby) {
        let msg = {
            type: "sync",
            id: Player.ID,
            lobbyID: Player.lobby,
            time: new Date().getTime()
        };
        server.send(JSON.stringify(msg));
    }

}

//Updates using server data
function sync() {

    //Before the game starts
    if (Game.status == "lobby") {

        var keys = Object.keys(Game.players);

        //List players
        for (var i = 0; i < 4; i++) {

            if (typeof keys[i] != "undefined") {
                $('li[data-player=' + (i + 1) + ']').text(Game.players[Object.keys(Game.players)[i]].name);
            } else {
                $('li[data-player=' + (i + 1) + ']').text("Waiting...");
            }

        }

        //Update button text if lobby isn't full
        $('.start.host').text(Game.current == Game.maxplayers ? "Start" : "Need " + Game.maxplayers + " players to start").mouseover(function() {
            audio.hover.currentTime = 0;
            audio.hover.play();
        });

        //Player count in the top right
        $('.player-count').text(Game.current + "/" + Game.maxplayers);

    }

    //During the game
    if (Game.status == "playing") {

        var keys = Object.keys(Game.players);

        //Make sure player is on the game screen
        if (Player.view != "tabletop") {
            $('.showing').removeClass('showing');
            $('.tabletop').addClass('showing');
            Player.view = "tabletop";
        }


        for(var p in Game.players) {

            //If one of the players reaches the target score
            if(Game.players[p].score == Game.scoretowin) {

                //End the game
                endGame(Game.players[p].name);
                Game.status = "ended";

            }

        }

        //Runs the first loop
        if (!Player.started) {

            Player.started = true;

            //Reset player rectangles
            $('.player').removeClass('two-players').show();

            if (Game.maxplayers == "2") {

                //Link players to their respective rectangle and scorebox
                $('.player.blue, .score.blue').attr('data-player', keys[0]);
                $('.player.purple, .score.purple').attr('data-player', keys[1]);

                //Move scoreboxes to center if there are two players
                $('.score.blue, .score.purple').addClass('two-players');

                //Hide unused elements
                $('.player.pink, .score.pink').hide();
                $('.player.orange, .score.orange').hide();
            } else if (Game.maxplayers == "3") {

                //Link players to their respective rectangle and scorebox
                $('.player.blue, .score.blue').attr('data-player', keys[0]);
                $('.player.purple, .score.purple').attr('data-player', keys[1]);
                $('.player.pink, .score.pink').attr('data-player', keys[2]);

                //Hide unused elements
                $('.player.orange, .score.orange').hide();

            } else {

                //Link players to their respective rectangle and scorebox
                $('.player.blue, .score.blue').attr('data-player', keys[0]);
                $('.player.purple, .score.purple').attr('data-player', keys[1]);
                $('.player.pink, .score.pink').attr('data-player', keys[2]);
                $('.player.orange, .score.orange').attr('data-player', keys[3]);

            }

            //Meme machine
            if(Game.linusmode == 1) {
                $('.ball').addClass('linus');
            } else {
                $('.ball').removeClass('linus');
            }

        }

        //Done for each player in the game
        for (var i = 0; i < keys.length; i++) {

            //Refer to player object as p from now on
            let p = Game.players[keys[i]];

            //Update scoreboxes
            $('.score[data-player=' + keys[i] + '] .name').text(p.name);

            //padStart adds a zero to the front if score is one digit
            $('.score[data-player=' + keys[i] + '] .points').text(p.score.toString().padStart(2, 0));

            if(typeof p.x == "undefined") {

                //If player 1 set X coord to 60
                if(i == 0) {
                    p.x = 60;
                }

                //If player 2 set X coord to 640
                if(i == 1) {
                    p.x = 640;
                }

                //If player 3 or 4 set X coord to 300
                if(i == 2 || i == 3) {
                    p.x = 300;
                }

            }

            //If Y coord has not been set yet
            if(typeof p.y == "undefined") {

                //If player 1 or 2 set Y coord to 300
                if(i == 0 || i == 1) {
                    p.y = 300;
                }

                //If player 3 set Y coord to 60
                if(i == 2) {
                    p.y = 60;
                }

                //If player 4 set Y coord to 640
                if(i == 3) {
                    p.y = 640;
                }

            }

            //If coords have been updated, translate to position
            if (p.x && p.y) {
                $('.player[data-player=' + keys[i] + ']').css({
                    "transform": "translate(" + p.x + "px," + p.y + "px)"
                });
            }

        }

        //Bind keys to move functions
        //Only bind vertical if player 1 or 2
        if (Player.ID == keys[0] || Player.ID == keys[1]) {

            //If key is up arrow or W
            if (Keys[38] || Keys[87]) {
                move("up");
            }

            //If key is down arrow or S
            else if (Keys[40] || Keys[83]) {
                move("down");
            }

        }

        //Only bind horizontal if player 3 or 4
        if (Player.ID == keys[2] || Player.ID == keys[3]) {

            //If key is left arrow or A
            if (Keys[37] || Keys[65]) {
                move("left");
            }

            //If key is right arrow or D
            else if (Keys[39] || Keys[68]) {
                move("right");
            }

        }

        //Host updates ball and scores
        if (Player.ID == Game.host) {

            var ball = Game.ball,
                speed = 6;

            //If ball is brand new give it a random direction
            if (ball.sx == 0) {
                ball.sx = randomBetween(-speed,speed);
            }
            if (ball.sy == 0) {
                ball.sy = randomBetween(-speed,speed);

                //Prevent bouncing on the exact corners
                //Super annoying bug
                while(ball.sy == ball.sx) {
                    ball.sy = randomBetween(-speed,speed);
                }

            }

            if (ball.x > 720) {

                //Scored on purple player 2
                if(ball.last != "purple") {
                    score(ball.last,"purple");
                }


                //Reset ball
                ball.x = 350;
                ball.y = 350;
                ball.last = "green";

                //Send towards opposite player
                ball.sy = 0;
                ball.sx = -speed;

            }

            if (ball.x < -20) {

                //Scored on blue player 1
                if(ball.last != "blue") {
                    score(ball.last,"blue");
                }

                //Reset ball
                ball.x = 350;
                ball.y = 350;
                ball.last = "green";

                //Send towards opposite player
                ball.sy = 0;
                ball.sx = speed;

            }

            var p1 = Game.players[keys[0]],
                p2 = Game.players[keys[1]];

            //Hard coding all of these values for simplicity
            //With more time I would develop some kind of proper collison system

            //If ball hits a left corner, turn around
            if (ball.x <= 80 && (ball.y <= 78 || ball.y >= 622)) {
                ball.sx = -ball.sx;
            }

            //If ball hits a right corner, turn around
            if (ball.x >= 620 && (ball.y <= 78 || ball.y >= 622)) {
                ball.sx = -ball.sx;
            }

            //If ball hits a top corner, turn around
            if (ball.y <= 80 && (ball.x <= 78 || ball.x >= 622)) {
                ball.sy = -ball.sy;
            }

            //If ball hits a bottom corner, turn around
            if (ball.y >= 620 && (ball.x <= 78 || ball.x >= 622)) {
                ball.sy = -ball.sy;
            }

            if (ball.x <= 80 && ball.x >= 60 && (ball.y >= p1.y-20 && ball.y <= (p1.y + 120))) {
                //Hits player 1's rectangle
                ball.sx = -ball.sx;
                ball.last = "blue";

                //For stat collection
                countHit("blue");

                //Play sound effect
                audio.blue.currentTime = 0;
                audio.blue.play();
            }

            if (ball.x >= 620 && ball.x <= 640 && (ball.y >= p2.y-20 && ball.y <= (p2.y + 120))) {
                //Hits player 2's rectangle
                ball.sx = -ball.sx;
                ball.last = "purple";

                //For stat collection
                countHit("purple");

                //Play sound effect
                audio.purple.currentTime = 0;
                audio.purple.play();
            }

            //Goal conditions for 2 players
            if (Game.maxplayers == 2) {

                //If ball hits the top, turn it around
                if (ball.y < 0) {
                    ball.sy = -ball.sy;
                }

                //If ball hits the bottom, turn it around
                if (ball.y > 700) {
                    ball.sy = -ball.sy;
                }

            }

            if (Game.maxplayers == 3) {

                if (ball.y < -20) {

                    //Scored on pink player 3
                    if(ball.last != "pink") {
                        score(ball.last,"pink");
                    }

                    //Reset ball
                    ball.x = 350;
                    ball.y = 350;
                    ball.last = "green";

                    //Send towards other player
                    ball.sy = speed;
                    ball.sx = 0;

                }

                var p3 = Game.players[keys[2]];


                if (ball.y <= 80 && ball.y >= 60 && (ball.x >= p3.x-20 && ball.x <= (p3.x + 100))) {
                    //Hits player 3's rectangle
                    ball.sy = -ball.sy;
                    ball.last = "pink";

                    //For stat collection
                    countHit("pink");

                    //Play sound effect
                    audio.pink.currentTime = 0;
                    audio.pink.play();
                }

                //If ball hits the bottom, turn it around
                if (ball.y > 700) {
                    ball.sy = -ball.sy;
                }

            }

            if (Game.maxplayers == 4) {

                if (ball.y < -20) {

                    //Scored on pink player 3
                    if(ball.last != "pink") {
                        score(ball.last,"pink");
                    }

                    //Reset ball
                    ball.x = 350;
                    ball.y = 350;
                    ball.last = "green";

                    //Send towards opposite player
                    ball.sy = speed;
                    ball.sx = 0;

                }

                if (ball.y > 720) {

                    //Scored on orange player 4
                    if(ball.last != "orange") {
                        score(ball.last,"orange");
                    }

                    //Reset ball
                    ball.x = 350;
                    ball.y = 350;
                    ball.last = "green";

                    //Send towards opposite player
                    ball.sy = -speed;
                    ball.sx = 0;

                }

                var p3 = Game.players[keys[2]],
                    p4 = Game.players[keys[3]];


                if (ball.y <= 80 && ball.y >= 60 && (ball.x >= p3.x-20 && ball.x <= (p3.x + 100))) {
                    //Hits player 3's rectangle
                    ball.sy = -ball.sy;
                    ball.last = "pink";

                    //For stat collection
                    countHit("pink");

                    //Play sound effect
                    audio.pink.currentTime = 0;
                    audio.pink.play();
                }

                if (ball.y >= 620 && ball.y <= 640 && (ball.x >= p4.x-20 && ball.x <= (p4.x + 100))) {
                    //Hits player 4's rectangle
                    ball.sy = -ball.sy;
                    ball.last = "orange";

                    //For stat collection
                    countHit("orange");

                    //Play sound effect
                    audio.orange.currentTime = 0;
                    audio.orange.play();
                }

            }

            //Prevent the ball from going too fast
            ball.sx = ball.sx.clamp(-10, 10);
            ball.sy = ball.sy.clamp(-10, 10);

            //Update ball position
            ball.x += ball.sx;
            ball.y += ball.sy;

            server.send(JSON.stringify({
                "type": "updateBall",
                "lobbyID": Player.ID,
                "ball": ball
            }));

        }

        //Easy color access
        var colors = {
            green: "#00d900",
            blue: "#00ccff",
            purple: "#8c00ff",
            pink: "#ff0091",
            orange: "#ff8c00"
        };


        //Check for Linus mode
        if(Game.linusmode == 1) {

            //Just memes
            //Removes the border and shadow from the ball
            $('.ball').css({
                "transform": "translate(" + Game.ball.x + "px," + Game.ball.y + "px) scale(3)",
                "border": 0,
                "box-shadow": "none"
            });

        } else {

            //Update ball position normally
            $('.ball').css({
                "transform": "translate(" + Game.ball.x + "px," + Game.ball.y + "px)",
                "border": "2px solid " + colors[Game.ball.last],
                "box-shadow": "0px 0px 32px 2px " + colors[Game.ball.last] + ", inset 0px 0px 32px 2px " + colors[Game.ball.last]
            });

        }

    }

    if(Game.status == "ended") {

        //Show winner's name
        $('.winner span').text(Game.winner+'!');

        //Make sure player is on the post-game screen
        if (Player.view != "post-game") {
            $('.showing').removeClass('showing');
            $('.post-game').addClass('showing');
            Player.view = "post-game";
        }

    }

}

function score(color,on) {

    audio[color].currentTime = 0;
    audio[color].play();

    //Do nothing if nobody actually scored
    if (color != "green") {

        let keys = Object.keys(Game.players),
            colorID;

        //Get player by color
        colorID = playerFromColor(color);

        //Let the server know about the score
        server.send(JSON.stringify({
            "type": "score",
            "lobbyID": Player.lobby,
            "playerID": colorID,
            "color": color
        }));

    } else {

        //Send green for sound effect only
        server.send(JSON.stringify({
            "type": "score",
            "lobbyID": Player.lobby,
            "color": "green"
        }));

    }


}

//Gets the player's ID from their color
function playerFromColor(c) {
    var colorID,
        keys = Object.keys(Game.players);
    switch (c) {
        case "blue":
            colorID = keys[0];
            break;
        case "purple":
            colorID = keys[1];
            break;
        case "pink":
            colorID = keys[2];
            break;
        case "orange":
            colorID = keys[3];
            break;
    }
    return colorID;
}

function move(d) {

    //Player movement
    //Will have lag when implemented this way but that's not the point
    server.send(JSON.stringify({
        type: "move",
        lobbyID: Player.lobby,
        playerID: Player.ID,
        direction: d
    }));

    //Refer to player as p
    var p = Game.players[Player.ID],
        //Amount moved per tick
        a = 4;

    //Client side movement to help reduce visual lag
    switch (d) {
        case "up":
            p.y -= a;
            break;
        case "down":
            p.y += a;
            break;
        case "left":
            p.x -= a;
            break;
        case "right":
            p.x += a;
            break;
    }

    //Actually move the element, more efficient with a translate
    //Position gets overridden later by sync
    $('.player[data-player=' + Player.ID + ']').css({
        "transform": "translate(" + p.x + "px," + p.y + "px)"
    });

}

function browseServers() {

    //Ask server for list
    server.send(JSON.stringify({
        type: "list"
    }));

}

function joinLobby(i) {

    //Join lobby with the code 'i'

    let msg = {
        type: "joinLobby",
        id: Player.ID,
        lobbyID: i
    };
    server.send(JSON.stringify(msg));

}

function leaveLobby() {

    let msg = {
        type: "leaveLobby",
        id: Player.ID,
    };
    server.send(JSON.stringify(msg));

    //Remove player from lobby client-side
    delete Player.lobby;

    //Reset Game object to default
    Game = {
        settings: {
            maxplayers: 4,
            visibility: 1,
            linusmode: 0
        },
        players: {}
    };

}

function createLobby() {

    //Let the server know this ID wants to create a lobby
    let msg = {
        type: "createLobby",
        id: Player.ID,
        settings: Game.settings
    };

    server.send(JSON.stringify(msg));

    //Set game host as yourself
    Game.host = Player.ID;

    //Create new player object in lobby
    Game.players[Player.ID] = {
        name: Player.name
    };


    //Reset hidden elements
    $('li[data-player]').show();

    //Hide unused elements
    $('li[data-player]').each(function() {
        let p = $(this).attr('data-player');
        if (p > Game.settings.maxplayers) {
            $(this).hide();
        }
    });

    //Add player to list
    $('li[data-player=1]').text(Player.ID);

}

function startGame() {

    //Only start if all players are present
    if (Game.current == Game.maxplayers) {

        let msg = {
            type: "startGame",
            //Use host's ID as lobbyID
            //Could be done better
            lobbyID: Player.ID,
        };

        server.send(JSON.stringify(msg));

    }

}

function countHit(c) {

    //Stat collection here

}

function endGame(w) {

    server.send(JSON.stringify({
        type: "endGame",
        lobbyID: Player.lobby,
        winner: w
    }));

}

function sendMessage(m) {
    server.send(JSON.stringify({
        type: "message",
        lobbyID: Player.lobby,
        sender: Player.name,
        message: m
    }));
}

function quit() {

    //Close connection to server
    server.close();

}

//Random number between
function randomBetween(min,max) {
    return Math.floor(Math.random()*(max-min+1)+min);
}

//Keep a number within a specified range
Number.prototype.clamp = function(min, max) {
    return Math.min(Math.max(this, min), max);
};

// Runs initialize when the page loads
window.onload = initialize;

//Runs when the page is unloaded
window.onunload = quit;
