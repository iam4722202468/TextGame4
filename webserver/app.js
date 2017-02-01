var express = require('express');
var path = require('path');
var favicon = require('static-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var fs = require("fs");

var findGames = require('./routes/findGames');
var playGame = require('./routes/playGame')
var misc = require('./routes/misc');

var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.use(favicon());
app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded());
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, 'html'));

app.use('/', misc);
app.use('/', findGames);

app.use('/contact', misc);
app.use('/about', misc);

var mainGameServer = require('child_process').spawn('./game')

require('./routes/manageSocket')(io, mainGameServer);
require('./routes/getGames')(mainGameServer);

app.use('/playGame', playGame);

/// catch 404 and forward to error handler
app.use(function(req, res, next) {
    var err = new Error('Not Found');
    err.status = 404;
    next(err);
});

/// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
    app.use(function(err, req, res, next) {
        res.status(err.status || 500);
        res.send('error: ' + err.message);
    });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.send('error: ' + err.message);
});

app.set('port', process.env.PORT || 3000);

if(fs.existsSync('../info')) {
    http.listen(app.get('port'), function() {
        console.log('Server listening on port ' + app.get('port'));
    });
} else {
    console.log("Error: info file not found");
}

function closeServer(callback_)
{
    console.log('Stopping ...');
    mainGameServer.kill('SIGINT');
    callback_();
}

process.on('SIGINT', function() {
    closeServer(function() {
        process.exit();
    });
});