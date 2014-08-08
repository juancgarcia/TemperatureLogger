#include "OneWire/OneWire.h"
#include "temperaturereader.h"
#include "HttpClient.h"

HttpClient http;

// Headers currently need to be set at init, useful for API keys etc.
http_header_t headers[] = {
   //  { "Content-Type", "application/json" },
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

char nodeName[20] = "NoName";
unsigned long last_alarm = 0,
   // refresh = /*min*/ 10 * /*sec*/ 60 * /*millis*/ 1000,
   refresh = /*sec*/ 10 * /*millis*/ 1000,
   alarm = 1;

int setNodeName(String name) {
   strcpy(nodeName, name.c_str());
   return 1;
}

int resetAlarm() {
   last_alarm = alarm;
   alarm = millis() + refresh;
   return alarm;
}

bool next_interval() {
   return last_alarm > alarm /*overflow*/ || millis() > alarm /*time passed*/;
}

String trackr_server_str = "192.168.1.10";
int trackr_port = 8080;

int sendTemperature(float temp){
   String nodeInfo = String("nodeName=" + String(nodeName));
   String tempInfo = String("&temperature=" + String(temp));
   String data = String(nodeInfo + tempInfo);

   // Serial.println("Data Payload:");
   // Serial.println(data);

   return postData(trackr_server_str, trackr_port, String("/api/temperature"), data);
}

int postData(String server, int port, String path, String payload){
   Serial.println("good connection");
   
   request.hostname = server;
   request.port = port;
   request.path = path;

   request.body = payload.c_str();

   http.post(request, response, headers);
   Serial.print("Application>\tResponse status: ");
   Serial.println(response.status);

   Serial.print("Application>\tHTTP Response Body: ");
   Serial.println(response.body);
   return 1;
}

// char coreID[12];

// String getCoreID()
// {
//   String coreIdentifier = "";
//   char id[12];
//   memcpy(id, (char *)ID1, 12);
//   char hex_digit;
//   for (int i = 0; i < 12; ++i)
//   {
//     hex_digit = 48 + (id[i] >> 4);
//     if (57 < hex_digit)
//      hex_digit += 39;
//      coreIdentifier = coreIdentifier + hex_digit;
//     hex_digit = 48 + (id[i] & 0xf);
//   if (57 < hex_digit)
//      hex_digit += 39;
//   coreIdentifier = coreIdentifier + hex_digit;
//  }
//  return coreIdentifier;
// }

OneWire *ow;
byte addr[8];
int found=0;

void setup() {
   Serial.begin(9600);
   // strcpy(coreID, getCoreID().c_str());
   Spark.function("setNodeName", setNodeName);
   Spark.variable("nodeName", &nodeName, STRING);
   resetAlarm();
   ow = new OneWire(D0);
   // Get first sensor
   found = (*ow).search(addr);
}

void loop() {
   if(found != 1)
      found = (*ow).search(addr);

   if(next_interval()){
      // Serial.println(found);
      resetAlarm();

      // Serial.print(coreID);
      Serial.println(alarm);

      if(found == 1) {
         // Serial.print("Fahrenheit: ");
         float temp = TemperatureReader::getTemperature(*ow, addr, 1);
         // Serial.println(temp);
         sendTemperature(temp);
      } else
         Serial.println("Not Found");
   }
}