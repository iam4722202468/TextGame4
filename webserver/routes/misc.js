var express = require('express');
var router = express.Router();
var ejs = require('ejs');
var fs = require('fs');

var baseFilePath = '/../html/'

router.get('/contact', function(req, res) {
    ejs.renderFile(__dirname + baseFilePath + 'contact.html', {active:"Contact"}, function(err, result) {
        if (!err) {
            res.end(result);
        } else {
            res.end(err.toString());
            console.log(err);
        }
    });
});

router.get('/about', function(req, res) {
    ejs.renderFile(__dirname + baseFilePath + 'about.html', {active:"About"}, function(err, result) {
        if (!err) {
            res.end(result);
        } else {
            res.end(err.toString());
            console.log(err);
        }
    });
});

router.get('/', function(req, res) {    
    ejs.renderFile(__dirname + baseFilePath + 'index.html', {active:"Home"}, function(err, result) {
        if (!err) {
            res.end(result);
        } else {
            res.end(err.toString());
            console.log(err);
        }
    });
});

module.exports = router;