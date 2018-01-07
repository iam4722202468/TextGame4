var getGames = require('./getGames');
var findGames = require('./findGames');

//make sure right user is playing right game
function checkGame(cookieObject, callback_)
{
    getGames.getUser(cookieObject['sessionID'], function(data) {
        if(data.length > 0)
        {
            findingGame = data[0]['GameKeys'].filter(function(v){ return v["key"] == cookieObject['currentGame'];});
            
            if(findingGame.length > 0)
                callback_("ok")
            else
                callback_("Error: game not associated with user"); 
        }
        else
            callback_("Error: sessionID not found");
    });
}

module.exports = function(io, mainGameServer)
{
    var sockets = {};
    
    //data sends faster than client connects
    var sendBuffer = {};
    
    //store past user text
    var scrollback = {};
    
    function readBuffer() {
        for(x in sendBuffer)
        {
            if(sendBuffer[x].length > 0)
            {
                //console.log(sendBuffer[x])
                
                if(x in sockets)
                {
                    if(sendBuffer[x][sendBuffer[x].length-1] == "DEATH") {
                        sockets[x]['Socket'].emit('in', sendBuffer[x].slice(0,sendBuffer[x].length-1).join("<br>"));
                        delete scrollback[x]
                        delete sockets[x]
                    } else {
                        sockets[x]['Socket'].emit('in', sendBuffer[x].join("<br>"));
                    }
                    
                    sendBuffer[x] = [];
                    //console.log(sendBuffer);
                }
            }
        }
    }
    //send buffer every x milliseconds
    setInterval(readBuffer, 200);
    
    mainGameServer.stdout.on('data', function(data) {
        data = data.toString();
        datum = data.split('\n');
        
        //on death
        
        for(x in datum)
        {
            w = datum[x].split(' ');
            gameKey = w.slice(w.length-1, w.length)[0];
            
            if(w[0] == "DEATH")
            {
                getGames.dropGame(sockets[gameKey]['sessionID'], gameKey);
            }
            
            toReturn = w.slice(0,w.length-1).join(" ");
            
            if(!(gameKey in sendBuffer))
                sendBuffer[gameKey] = [];
            
            if(toReturn != [''])
            {
                sendBuffer[gameKey].push(toReturn.split(" ").join("&nbsp;"));
                
                if(!(gameKey in scrollback))
                    scrollback[gameKey] = []
                
                scrollback[gameKey].push(toReturn.split(" ").join("&nbsp;"));
            }
        }
    });
    
    io.on('connection', function(socket){
        cookies = socket.request.headers.cookie;
        
        if(cookies !== undefined)
        {
            sections = cookies.split('; ');
            cookieObject = {}
            for (x in sections)
            {
                isSplit = sections[x].split('=')
                if(!(isSplit[0] in cookieObject))
                    cookieObject[isSplit[0]] = isSplit[1]
            }
            
            checkGame(cookieObject, function(data) {
                if(data == "ok")
                {
                    //returning user
                    if(cookieObject['currentGame'] in scrollback)
                    {
                        if(scrollback[cookieObject['currentGame']].length > 0) {
                            sendBuffer[cookieObject['currentGame']] = scrollback[cookieObject['currentGame']]
                        }
                    } else {
                        scrollback[cookieObject['currentGame']] = [];
                    }
                } else {
                    console.log(data);
                }
            });
            
            sockets[cookieObject['currentGame']] = {'Socket':socket, 'sessionID':cookieObject['sessionID']};
        }
        
        console.log("Connected");
        
        socket.on('restart', function(cookies) {
            console.log(cookies);
            sections = cookies.split('; ');
            cookieObject = {}
            for (x in sections)
            {
                isSplit = sections[x].split('=')
                if(!(isSplit[0] in cookieObject))
                    cookieObject[isSplit[0]] = isSplit[1]
            }
            
            getGames.getUser(cookieObject['sessionID'], function(data) {
                if(data.length > 0) {
                    findGames.findGames(function(games) {
                        
                        gamePath = unescape(cookieObject['currentGamePath']);
                    
                        subFound = games['Games'].filter(function(v){ return v["Path"] == gamePath;});
                        
                        if(subFound.length > 0) {
                            getGames.createGame(cookieObject['sessionID'], subFound[0], function(newKey) {
                                socket.emit("newID", newKey);
                                
                                //on restart
                                
                                if (cookieObject['currentGame'] in scrollback)
                                {
                                    delete scrollback[cookieObject['currentGame']];
                                    delete sockets[cookieObject['currentGame']];
                                    getGames.dropGame(cookieObject['sessionID'], cookieObject['currentGame']);
                                }
                                
                                mainGameServer.stdin.write("DELETE " + cookieObject['currentGame'] + '\n');
                                scrollback[cookieObject['currentGame']] = [];
                                
                                sockets[newKey] = {'Socket':socket, 'sessionID':cookieObject['sessionID']};
                            });
                        }
                    });
                } else {
                    console.log("Error: sessionID not found");
                }
            });
        });
        
        socket.on('out', function(msg, cookies){
            console.log(cookies);
            
            sections = cookies.split('; ');
            cookieObject = {}
            for (x in sections)
            {
                isSplit = sections[x].split('=')
                if(!(isSplit[0] in cookieObject))
                    cookieObject[isSplit[0]] = isSplit[1]
            }
            
            console.log(msg);
            console.log(cookieObject);
            //console.log(sockets);
            
            checkGame(cookieObject, function(data) {
                if(data == "ok")
                {
                    toSend = msg + " " + cookieObject['currentGame'] + "|\n";
                    console.log(toSend);
                    mainGameServer.stdin.write(toSend);
                    
                    scrollback[cookieObject['currentGame']].push("<b>>&nbsp;&nbsp;" + msg + "</b>");
                    
                } else {
                    console.log(data);
                }
            });
            

            
        });
    });
}