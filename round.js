/* 
  Tool to generate ROUND operations in C by calling ROUNDx macros 

  usage, cat "[ABCD k s] ..." | node round.js ROUNDx 
  
  That is, [ABCD k s]...  are the 16 round operations as cut and paste from RFC1320, 
  ROUNDx is the round generated, e.g ROUND1, ..., ROUND3.

*/

var indata = "";

var round = process.argv[2];

function processIndata(data) {
    console.log("Data:\n" + data);

    var ops = data.match(/\[[^\]]+\]/g);

    var line = "";

    //console.log("ops:", ops);
    for(i = 0; i < ops.length; i++) {
        var str = ops[i].match(/\[(.+)\]/)[1];
        //console.log("str: ", str[1]);
        var parts = str.split(/ +/);
//        console.log("parts: ", parts);
        var letters = parts[0].split("");
        var k = ("  " + parts[1]).slice(-2);
        var s = ("  " + parts[2]).slice(-2);

        line += round + "( " + letters.join(",") + ", " + k + ", " + s + " ); "

        if(i % 4 === 3) {
            console.log(line);
            line = "";
        }
        //console.log(round + "( " + letters.join(",") + ", " + k + ", " + s + " );");
    }
}


process.stdin.on('data', function (data) { indata += data.toString(); } );
process.stdin.on('end', function () { processIndata(indata); } );
process.stdin.resume();