#include "OneWire/OneWire.h"
#include "temperaturereader.h"


/*
    Adapted from Tim's snippet http://pastebin.com/D2yjgYEG
    via: https://community.spark.io/t/can-i-use-1-wire-devices-with-spark-core/401/89
    
    ow: the OneWire bus instance
    addr: the sensor's address which we'll be querying
    unit: 0 -> celsius; 1 -> fahrenheit
*/
float TemperatureReader::getTemperature(OneWire ow, byte *addr, int unit) { //return (float) 0;
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
//   byte addr[8];
  float celsius = -1337, fahrenheit = -1337;
 
  // the first ROM byte indicates which chip
  // includes debug output of chip type
  switch (addr[0]) {
    case 0x10:
    //   Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
    //   Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
    //   Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    case 0x26:
    //   Serial.println("  Chip = DS2438");
      type_s = 2;
      break;
    default:
    //   Serial.println("Device is not a DS18x20/DS1822/DS2438 device. Skipping...");
      return -300;
  }
  
//   if ( !ow.search(addr)) {
//     Serial.println("No more addresses.");
//     Serial.println();
//     ow.reset_search();
//     delay(5000);
    ow.reset();
    ow.skip();
    ow.write(0x44); 
//     Serial.println("Converting temp on all probes");
//     Serial.println();
    delay(1000);
//     return -500;
//   }
  
//   Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    // Serial.write(' ');
    // Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    //   Serial.println("CRC is not valid!");
      return -400;
  }
//   Serial.println();
                        
    // start conversion, with parasite power on at the end
  
  present = ow.reset();
  ow.select(addr);
  ow.write(0xBE);         // Read Scratchpad

//   Serial.print("  Data = ");
//   Serial.print(present, HEX);
//   Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ow.read();
    // Serial.print(data[i], HEX);
    // Serial.print(" ");
  }
//   Serial.print(" CRC=");
//   Serial.print(OneWire::crc8(data, 8), HEX);
//   Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    if (type_s==1) {    // DS18S20
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
      celsius = (float)raw / 16.0;
    }else{ // type_s==2 for DS2438
      if (data[2] > 127) data[2]=0;
      data[1] = data[1] >> 3;
      celsius = (float)data[2] + ((float)data[1] * .03125);
    }
  } else {  // DS18B20 and DS1822
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  celsius = (float)raw / 16.0;
  }
  fahrenheit = celsius * 1.8 + 32.0;
  // end of test code
  
  // debug output
//   Serial.print("  Temperature = ");
//   Serial.print(celsius);
//   Serial.print(" Celsius, ");
//   Serial.print(fahrenheit);
//   Serial.println(" Fahrenheit");
  
  return (unit == 1)? fahrenheit : celsius;
}