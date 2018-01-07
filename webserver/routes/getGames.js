var mongodb = require('mongodb'),
    MongoClient = mongodb.MongoClient,
    url = 'mongodb://localhost:27017/textgame';

function generateHash()
{
    var hash = "";
    for(var x = 0; x <= 9; x++)
    {
        currentChar = Math.floor(Math.random() * 36);
        if(currentChar > 9)
        {
            hash += String.fromCharCode(currentChar + 55);
        } else {
            hash += currentChar;
        }
        
        if(x == 9)
            return hash;
    }
}

function createUser(res, callback_)
{
    var key = generateHash();
    
    MongoClient.connect(url, function (err, db) {
        if (err) {
            console.log('Unable to connect to the mongoDB server. Error:', err);
        } else {
            res.cookie('sessionID', key, {expires: new Date(2147483647000)});
            
            db.collection("Users", function(error, collection) {
                collection.insert({
                GameKeys:[],
                UserID: key
            }, function() {
                console.log("successfully added key " + key);
                db.close();
                callback_(key);
            });
            });
        }
    });
}

function getUser(key, callback_)
{
    MongoClient.connect(url, function (err, db) {
        if (err) {
            console.log('Unable to connect to the mongoDB server. Error:', err);
            callback_('error');
        } else {
            var collection = db.collection("Users");
            console.log(key);
            collection.find({UserID:key}).toArray(function(err, docs){
                db.close();
                callback_(docs);
            });
        }
    });
}

function dropDatabase()
{
    MongoClient.connect(url, function (err, db) {
        if (err) {
            console.log('Unable to connect to the mongoDB server. Error:', err);
            callback_('error');
        } else {
            db.dropDatabase();
        }
    });
}

module.exports = function(mainGameServer)
{
    function createGame(key, gameInfo, callback_)
    {
        var gameKey = generateHash();
        MongoClient.connect(url, function (err, db) {
            if (err) {
                console.log('Unable to connect to the mongoDB server. Error:', err);
                callback_('error');
            } else {
                var collection = db.collection("Users");
                collection.update({'UserID': key},{$push: { 'GameKeys': {key:gameKey, path:gameInfo['Path']} }}, function(err) {
                    console.log("NEW " + gameKey + " " + gameInfo['File'] + " " + gameInfo['Start'] +'\n');
                    mainGameServer.stdin.write("NEW " + gameKey + " " + gameInfo['File'] + " " + gameInfo['Start'] +'\n');
                    
                    db.close();
                    callback_(gameKey);
                });
            }
        });
    }
    
    function dropGame(userID, gameID)
    {
        //Does not delete scrollback for game and socket**
        //Need to do that
        //(Later)
        
        MongoClient.connect(url, function (err, db) {
            if (err) {
                console.log('Unable to connect to the mongoDB server. Error:', err);
                callback_('error');
            } else {
                var collection = db.collection("Users");
                collection.update({'UserID': userID},{$pull: { 'GameKeys': {key:gameID} }}, function(err) {
                    mainGameServer.stdin.write("DELETE " + gameID + '\n');
                    db.close();
                });
            }
        });
    }
    
    module.exports.createGame = createGame;
    module.exports.dropGame = dropGame;
}

module.exports.dropDatabase = dropDatabase;
module.exports.createUser = createUser;
module.exports.getUser = getUser;