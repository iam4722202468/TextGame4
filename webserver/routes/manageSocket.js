var getGames = require('./getGames');

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
    setInterval(readBuffer, 1000);
    
    mainGameServer.stdout.on('data', function(data) {
        data = data.toString();
        datum = data.split('\n');
        
        for(x in datum)
        {
            w = datum[x].split(' ');
            gameKey = w.slice(w.length-1, w.length)[0];
            toReturn = w.slice(0,w.length-1).join(" ");
            //console.log(gameKey)
            
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
                cookieObject[isSplit[0]] = isSplit[1]
            }
            
            sockets[cookieObject['currentGame']] = socket;
        }
        
        console.log("Connected");
        
        socket.on('out', function(msg){
            
            cookies = socket.request.headers.cookie;
            console.log(cookies);
            
            sections = cookies.split('; ');
            cookieObject = {}
            for (x in sections)
            {
                isSplit = sections[x].split('=')
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