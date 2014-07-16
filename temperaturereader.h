#include "OneWire/OneWire.h"

class TemperatureReader {
    
    public:
    static float getTemperature(OneWire ow, byte *addr, int unit);
};