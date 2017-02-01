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
            res.end("Cookie set");
        });
    }
});

router.get('/resume', function(req, res) {
    if("sessionID" in req.cookies) {
        getGames.getUser(req.cookies['sessionID'], function(data) {
            findGames(function(games) {
                ejs.renderFile(__dirname + baseFilePath + 'resumeGame.html', {active:"Resume", sessions:data, games:games}, function(err, result) {
                    if (!err) {
                        res.end(result);
                    } else {
                        res.end(err.toString());
                        console.log(err);
                    }
                });
            });
        });
    } else {
        getGames.createUser(res, function() {
            res.end("Cookie set");
        });
    }
});

module.exports = router;
module.exports.findGames = findGames;




















