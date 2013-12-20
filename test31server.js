var http = require('http');
var url = require('url');
var query = require('querystring');
var crypto = require('crypto');

var alg = 'sha1';
var blocksize = 64;

function hash(data) {
    var h = crypto.createHash(alg);

    var digest;

    if(h.write) {
        h.write(data);
        h.end();
        digest = h.read();
    } else {
        h.update(data);
        digest = new Buffer(h.digest(), 'binary');
    }
    return digest;
}

function generate_hmac(message, key) {
    var i, zeros, o_key_pad, i_key_pad;
    
    if(key.length > blocksize) {
        key = hash(key);
    }
    if(key.length < blocksize) {
        zeros = new Buffer(blocksize - key.length);
        zeros.fill(0);
        key = Buffer.concat([key, zeros]);
    }
    //console.log("key: " + key.toString('hex'));

    o_key_pad = new Buffer(blocksize);
    i_key_pad = new Buffer(blocksize);
    for(i = 0; i < blocksize; i++) {
        o_key_pad[i] = 0x5c ^ key[i];
        i_key_pad[i] = 0x36 ^ key[i];
    }
    
    return hash( Buffer.concat([o_key_pad, hash(Buffer.concat([i_key_pad, message])) ]) );
}

function insecure_compare(str1, str2, cb) {
    var n = 0;

    function nextchar() {
        if(n < str1.length && n < str2.length) {
            if(str1[n] === str2[n]) {
                n++;
                setTimeout(nextchar, 10); 
            } else {
                cb(false);
            }
        } else {
            cb(str1.length === str2.length);
        }
    }

    nextchar();

}

crypto.randomBytes(32, function(err, key) {
    var server; 

    if(err) {
        throw(err);
        return;
    }

// example http://localhost:9000/test?file=foo&signature=46b4ec586117154dacd49d664e5d63fdc88efb51

    var logged = false;

    server = http.createServer(function(request, response) {
        var hmac;
        //console.log("got request: ");
        //console.log("url " + request.url);
        
        var parts = url.parse(request.url);
        //console.log("got request: ", parts);
        
        if(parts.pathname === '/test') {
            var params = query.parse(parts.query);
            //console.log("params: ", params);

            if(params.file && params.signature) {

                hmac = generate_hmac(new Buffer(params.file), key);

                insecure_compare(hmac.toString('hex'), params.signature, function(valid) {
                    if(valid) {
                        response.writeHead(200);
                        response.write('OK');
                        response.end();
                    } else {
                        if(!logged) {
                            logged = true;
                            console.log("invalid signature, did not match: " + hmac.toString('hex'));
                        }
                        response.writeHead(500);
                        response.write("Invalid signature");
                        response.end();
                    }
                });
            } else {
                response.writeHead(500);
                response.write("Invalid request, expected file and signature");
                response.end();
            }

        } else {
            response.writeHead(500);
            response.write("Invalid request: " + parts.pathname);
            response.end();
        }
    });

    // test hmac

    //var hmac = generate_hmac(new Buffer("hello"), key);
    //console.log("hmac: " + hmac.toString('hex'));
    //var refmac = crypto.createHmac('sha1', key);
    //refmac.update(new Buffer("hello"));
    //console.log("refmac: " + refmac.digest('hex'));
    
    server.listen(9000);
    console.log("server started");
});


