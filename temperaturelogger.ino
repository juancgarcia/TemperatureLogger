#include "OneWire/OneWire.h"
#include "temperaturereader.h"

// TCPClient client;

// void ipArrayFromString(byte ipArray[], String ipString) {
//   int dot1 = ipString.indexOf('.');
//   ipArray[0] = ipString.substring(0, dot1).toInt();
//   int dot2 = ipString.indexOf('.', dot1 + 1);
//   ipArray[1] = ipString.substring(dot1 + 1, dot2).toInt();
//   dot1 = ipString.indexOf('.', dot2 + 1);
//   ipArray[2] = ipString.substring(dot2 + 1, dot1).toInt();
//   ipArray[3] = ipString.substring(dot1 + 1).toInt();
// }

// int connectToMyServer(String ip) {
//   byte serverAddress[4];
//   ipArrayFromString(serverAddress, ip);

//   if (client.connect(serverAddress, 9000)) {
//     return 1; // successfully connected
//   } else {
//     return -1; // failed to connect
//   }
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

// byte trackr_server[] = { 192, 168, 1, 10 };
// int postTemperature(float temp, String unit){
//     if (client.connect(trackr_server, 80)) {
//         Serial.println("connected");
//         client.println("POST /api/temperature HTTP/1.0");
//         client.println("Host: 192.168.1.10:8080");
//         client.println("Content-Length: 0");
//         client.println();
//     }
//     return 0;
// }

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
    // Spark.function("connect", connectToMyServer);
    Spark.function("setNodeName", setNodeName);
    Spark.variable("nodeName", &nodeName, STRING);
    resetAlarm();
    ow = new OneWire(D0);
    // Get first sensor
    found = (*ow).search(addr);

    // pinMode(D7, OUTPUT);
}

// void getFirstDevice(byte *addr){
//     // int deviceItr = 0;
//     int searchStatus;
    
//     // while(deviceItr < maxDevices || searchStatus == 0){
//         searchStatus = !ow.search(addr);
//         // Serial.println(addr);
//         // deviceItr++;
//     // }
//     // return addr;
// }

void loop() {
    if(found != 1)
        found = (*ow).search(addr);
        
    if(next_interval()){
        // Serial.print(found);
        // Serial.println("blah");
        resetAlarm();
        
        // Serial.print("Tweaky");
        
        // Serial.print(coreID);
        Serial.print(" - ");
        Serial.println(alarm);
        
        if(found == 1) {
            Serial.print("Fahrenheit: ");
            float temp = TemperatureReader::getTemperature(*ow, addr, 1);
            Serial.println(temp);
            Serial.println("");
        } else
            Serial.println("Not Found");
    }
//   if (client.connected()) {
//     if (client.available()) {
//       char pin = client.read() - '0' + D0;
//       char level = client.read();
//       if ('h' == level) {
//         digitalWrite(pin, HIGH);
//       } else {
//         digitalWrite(pin, LOW);
//       }
//     }
//   }
}