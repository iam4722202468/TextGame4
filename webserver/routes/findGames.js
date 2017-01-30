var express = require('express');
var router = express.Router();
var ejs = require('ejs');
var fs = require('fs');

var baseFilePath = '/../html/'

function findGames()
{
    fs.readFile('../info', function (err, data) {
        if (err) throw err;
            console.log(JSON.parse(data));
    });
    
    return "MOO"
}

router.get('/findgame', function(req, res) {
    games = findGames();
    
    ejs.renderFile(__dirname + baseFilePath + 'selectGame.html', {active:"Find"}, function(err, result) {
        if (!err) {
            res.end(result);
        } else {
            res.end(err.toString());
            console.log(err);
        }
    });
});

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

module.exports = router;
