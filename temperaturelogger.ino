#include "OneWire/OneWire.h"
#include "temperaturereader.h"
#include "HttpClient.h"

// #include "AssignNode.h"

String coreID;

OneWire *ow;
byte addr[8];
int found=0;

String trackr_server_str = "192.168.1.21";
int trackr_port = 8080;
String trackr_path = "/api/temperature";

HttpClient http;

// Headers currently need to be set at init, useful for API keys etc.
http_header_t headers[] = {
    { "Cache-Control", "no-cache" },
   //  { "Content-Type", "application/json" },
    { "Content-Type", "application/x-www-form-urlencoded" },
   //  { "Accept" , "application/json" },
   { "Accept" , "*/*"},
   { NULL, NULL } // NOTE:  -Always terminate headers will NULL
};

http_request_t request;
http_response_t response;

// void ipArrayFromString(byte ipArray[], String ipString) {
//   int dot1 = ipString.indexOf('.');
//   ipArray[0] = ipString.substring(0, dot1).toInt();
//   int dot2 = ipString.indexOf('.', dot1 + 1);
//   ipArray[1] = ipString.substring(dot1 + 1, dot2).toInt();
//   dot1 = ipString.indexOf('.', dot2 + 1);
//   ipArray[2] = ipString.substring(dot2 + 1, dot1).toInt();
//   ipArray[3] = ipString.substring(dot1 + 1).toInt();
// }

unsigned long last_alarm = 0,
   refresh = /*min*/ 5 * /*sec*/ 60 * /*millis*/ 1000,
   // refresh = /*sec*/ 10 * /*millis*/ 1000,
   alarm = 1;

int resetAlarm() {
   last_alarm = alarm;
   alarm = millis() + refresh;
   return alarm;
}

bool next_interval() {
   return last_alarm > alarm /*overflow*/ || millis() > alarm /*time passed*/;
}

int sendTemperature(float temp, String unit){
   String nodeInfo = String("deviceType=spark&deviceId=" + coreID);
   String tempInfo = String("&value=" + String(temp) + "&unit="+ unit);
   String data = String(nodeInfo + tempInfo);
   request.body = data.c_str();

   http.post(request, response, headers);
   Serial.print("Application>\tResponse status: ");
   Serial.println(response.status);

   Serial.print("Application>\tHTTP Response Body: ");
   Serial.println(response.body);

   return 1;
}

void setup() {
   Serial.begin(9600);

   resetAlarm();

   ow = new OneWire(D0);
   // Get first sensor
   found = (*ow).search(addr);

   // http connection
   request.hostname = trackr_server_str;
   request.port = trackr_port;
   request.path = trackr_path;

   String temp = Spark.deviceID();
   coreID = temp;
}

void loop() {
   if(found != 1)
      found = (*ow).search(addr);

   if(next_interval()){
      Serial.print("coreID: ");
      Serial.println(coreID);
      resetAlarm();

      Serial.println(alarm);

      if(found == 1) {
         // Serial.print("Fahrenheit: ");
         float temp = TemperatureReader::getTemperature(*ow, addr, 1/*fahrenheit*/);
         // Serial.println(temp);
         sendTemperature(temp, "fahrenheit");
      } else
         Serial.println("Not Found");
   }
}