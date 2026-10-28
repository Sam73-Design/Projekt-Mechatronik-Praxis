# Linetracker

SunFounder I2C module with exactly eight sensors, address 0x09.
Reads 16 bytes as eight uint16_t values, high byte first.

Use in main.cpp:

    #include <Arduino.h>
    #include <Linetracker.h>

    Linetracker linetracker;

    void setup() {
      linetracker.begin();
    }

    void loop() {
      static unsigned long lastRead = 0;
      if (millis() - lastRead >= 500) {
        lastRead = millis();
        if (linetracker.read()) {
          uint16_t firstSensor = linetracker.values()[0];
          // Use readings at indices 0 through 7.
          (void)firstSensor;
        }
      }
    }

begin() initializes Wire at 100 kHz; it does not probe the module.
read() returns true on a complete read. On failure it drains remaining bytes
and retains the last readings (initially zero). connected() reports the last
read's success, and is false until a successful read. bytesReceived() returns
the last request's byte count.

The application chooses the read interval. The library has no Serial output
or deliberate delays; Wire transactions are synchronous.
Optional bus/address: Linetracker sensors(Wire, 0x09).

Values are uint16_t. I2CDisplay's current 'l' type expects int[8], so do not
pass this pointer directly as an 'l' entry on AVR, where int is signed.

Build the Serial example: pio run -e Arduino_Uno_Linetracker
