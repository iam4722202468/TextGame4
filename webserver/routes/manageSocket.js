var getGames = require('./getGames');
var findGames = require('./findGames');

module.exports = function(io, mainGameServer)
{
    var sockets = {};
    
    //data sends faster than client connects
    var sendBuffer = {};
    
    function readBuffer() {
        for(x in sendBuffer)
        {
            if(sendBuffer[x].length > 0)
            {
                //console.log(sendBuffer[x])
                
                if(x in sockets)
                {
                    sockets[x].emit('in', sendBuffer[x].join("<br>"));
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
        
        for(x in datum)
        {
            w = datum[x].split(' ');
            gameKey = w.slice(w.length-1, w.length)[0];
            
            if(w[0] == "DEATH")
            {
                sockets[gameKey].emit('restarti');
            }
            
            toReturn = w.slice(0,w.length-1).join(" ");
            
            console.log(toReturn)
            
            if(!(gameKey in sendBuffer))
                sendBuffer[gameKey] = [];
            
            if(toReturn != [''])
                sendBuffer[gameKey].push(toReturn.split("    ").join("&nbsp;&nbsp;&nbsp;&nbsp;"));
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
            
            sockets[cookieObject['currentGame']] = socket;
        }
        
        console.log("Connected");
        
        socket.on('restarto', function(cookies) {
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
                if(data.length > 0)
                {
                    findingGame = data[0]['GameKeys'].filter(function(v){ return v["key"] == cookieObject['currentGame'];});
                    
                    if(findingGame.length > 0)
                    {
                        findGames.findGames(function(data2) {
                            subFound = data2['Games'].filter(function(v){ return v["Path"] == findingGame[0]['path'];});
                            getGames.createGame(cookieObject['sessionID'], subFound[0], function(newKey) {
                                socket.emit("newID", newKey);
                                sockets[newKey] = socket;
                            });
                        });
                    }
                    else
                        console.log("Error: game not associated with user"); 
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
            
            //make sure right user is playing right game
            getGames.getUser(cookieObject['sessionID'], function(data) {
                if(data.length > 0)
                {
                    findingGame = data[0]['GameKeys'].filter(function(v){ return v["key"] == cookieObject['currentGame'];});
                    
                    if(findingGame.length > 0)
                    {
                        toSend = msg + " " + cookieObject['currentGame'] + "|\n";
                        mainGameServer.stdin.write(toSend);
                    }
                    else
                        console.log("Error: game not associated with user"); 
                }
                else
                    console.log("Error: sessionID not found");
            });
            
        });
    });
}