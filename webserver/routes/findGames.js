var express = require('express');
var router = express.Router();
var ejs = require('ejs');
var fs = require('fs');
var getGames = require('./getGames');

var baseFilePath = '/../html/'

function findGames(callback_)
{
    fs.readFile('../info', function (err, data) {
        if (err) throw err;
            callback_(JSON.parse(data));
    });
}

router.post('/deleteGame', function(req, res) {
    getGames.dropGame(req.cookies['sessionID'], req.body.gameID);
});

router.get('/findgame', function(req, res) {
    if("sessionID" in req.cookies) {
        findGames(function(games) {
            ejs.renderFile(__dirname + baseFilePath + 'findGame.html', {active:"Find", games:games}, function(err, result) {
                if (!err) {
                    res.end(result);
                } else {
                    res.end(err.toString());
                    console.log(err);
                }
            });
        });
    } else {
        getGames.createUser(res, function() {
            res.redirect(req.originalUrl);
        });
    }
});

router.get('/resume', function(req, res) {
    if("sessionID" in req.cookies) {
        getGames.getUser(req.cookies['sessionID'], function(data) {
            if(data.length > 0) {
                findGames(function(games) {
                    ejs.renderFile(__dirname + baseFilePath + 'resumeGame.html', {active:"Resume", sessions:data[0]['GameKeys'], games:games}, function(err, result) {
                        if (!err) {
                            res.end(result);
                        } else {
                            res.end(err.toString());
                            console.log(err);
                        }
                    });
                });
            } else {
                res.cookie('sessionID', '', { expires: new Date(1), path: '/' })
                res.cookie('currentGame', '', { expires: new Date(1), path: '/' })
                res.cookie('currentGamePath', '', { expires: new Date(1), path: '/' })
                
                res.redirect(req.originalUrl);
            }
        });
    } else {
        getGames.createUser(res, function() {
            ejs.renderFile(__dirname + baseFilePath + 'resumeGame.html', {active:"Resume", sessions:{}, games:{}}, function(err, result) {
                if (!err) {
                    res.end(result);
                } else {
                    res.end(err.toString());
                    console.log(err);
                }
            });
        });
    }
});

module.exports = router;
module.exports.findGames = findGames;
