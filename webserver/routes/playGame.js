var ejs = require('ejs');
var fs = require('fs');
var findGames = require('./findGames');
var getGames = require('./getGames');

var express = require('express')
var router = express.Router();

var baseFilePath = '/../html/'

function renderPage(callback_)
{
    ejs.renderFile(__dirname + baseFilePath + 'playGame.html', {active:"Play"}, function(err, result) {
        if (!err) {
            callback_(result);
        } else {
            callback_(err.toString());
            console.log(err);
        }
    });
}

router.get('/*', function(req, res) {
    findGames.findGames(function(data) {
        
        newData = data['Games'].filter(function(v){ return v["Path"] == req.url;});
        
        if(newData.length > 0)
        {
            if("sessionID" in req.cookies) {
                getGames.getUser(req.cookies['sessionID'], function(data) {
                    if(data.length > 0) {
                        if("currentGame" in req.cookies && req.cookies['currentGamePath'] == req.url) {
                            renderPage(function(data) {
                                res.end(data);
                            });
                            //res.end(JSON.stringify(newData[0]) + req.cookies['sessionID']);
                        } else {
                            getGames.createGame(req.cookies['sessionID'], newData[0], function(newKey) {
                                res.cookie('currentGame', newKey, {expires: new Date(2147483647000)});
                                res.cookie('currentGamePath', req.url, {expires: new Date(2147483647000)});
                                
                                renderPage(function(data) {
                                    res.end(data);
                                });
                                //res.end(JSON.stringify(newData[0]) + req.cookies['sessionID'] + " " + newKey);
                            });
                        }
                    } else {
                        res.cookie('sessionID', '', { expires: new Date(1), path: '/' })
                        res.cookie('currentGame', '', { expires: new Date(1), path: '/' })
                        res.cookie('currentGamePath', '', { expires: new Date(1), path: '/' })
                        
                        res.redirect(req.originalUrl);
                    }
                });
            } else {
                getGames.createUser(res, function(returnedKey) {
                    getGames.createGame(returnedKey, newData[0], function(newKey) {
                        res.cookie('currentGame', newKey, {expires: new Date(2147483647000)});
                        res.cookie('currentGamePath', req.url, {expires: new Date(2147483647000)})
                        
                        res.redirect(req.originalUrl);
                    });
                });
            }
        } else {
            res.end("Error: Game path not found");
        }
    });
});

/*
    ejs.renderFile(__dirname + baseFilePath + 'contact.html', {active:"Contact"}, function(err, result) {
        if (!err) {
            res.end(result);
        } else {
            res.end(err.toString());
            console.log(err);
        }
    });
*/

module.exports = router;