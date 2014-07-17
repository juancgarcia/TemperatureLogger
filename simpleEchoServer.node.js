var
http = require('http'),
qs = require('querystring');

var serverPort = 8888;

http.createServer(function(req, resp){
   if(req.method == "GET"){
      resp.writeHead(200, {'Content-Type': 'text/json'});
      resp.end(JSON.stringify({code: 200, success: true, msg: "post some data to echo"}));
   } else if(req.method == "POST") {
      var requestBody = '';
      req.on('data', function(data){
         requestBody += data;
         if(requestBody.length > 1e7) {
            msg = "Request Entity Too Large";
            resp.writeHead(413, msg, {'Content-Type': 'text/json'});
            resp.end(JSON.stringify({success: false, code: 413, error_message: msg}));
         }
      });
      req.on('end', function(){
         var formData = qs.parse(requestBody);
         resp.writeHead(200, {'Content-Type': 'text/json'});
         resp.end(JSON.stringify({success: true, code: 200, data_echo: formData}));
      });
   }
}).listen(serverPort);
console.log("listening on "+ serverPort);