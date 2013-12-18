var http = require('http');
var url = require('url');
var query = require('querystring');

var server = http.createServer(function(request, response) {
    //console.log("got request: ");
    //console.log("url " + request.url);

    var parts = url.parse(request.url);
    //console.log("got request: ", parts);

    if(parts.pathname === '/test') {
        var params = query.parse(parts.query);
        console.log("params: ", params);

        response.writeHead(200);
        response.write('hello');
        response.end();

    } else {
        response.writeHead(500);
        response.end();
    }


});

server.listen(9000);
console.log("server started");