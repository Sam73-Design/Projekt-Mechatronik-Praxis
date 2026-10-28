# I2CDisplay

128x64 SSD1306 menu. Define the menu entries in main.cpp:

```cpp
#include <Arduino.h>
#include <I2CDisplay.h>

float temperature = 25.0f;
bool active = true;
char mode[5] = "Line";
int sensors[8] = {};

DisplayVariable variables[] = {
    {"Temperature", &temperature, 'f'},
    {"Active", &active, 'b'},
    {"Mode", mode, 's'},
    {"Sensors", sensors, 'l'},
};
I2CDisplay menu(variables, sizeof(variables) / sizeof(variables[0]));

void setup() {
  if (!menu.begin()) {
    // Handle invalid entries or OLED initialization failure.
  }
}

void loop() {
  // Update your variables from the application.
  menu.update();
}
```

Each entry contains a name, pointer and type:
- 'f': float pointer
- 'i': int pointer
- 'b': bool pointer
- 's': null-terminated char array (pass the array directly)
- 'l': int array with exactly eight sensor values (pass the array directly)

The pointer must match the declared type. Keep the entries, names and values
alive while the display uses them. The menu supports 1 to 255 entries.
Values refresh every second; refresh() forces an immediate redraw.

UP/DOWN selects an entry, ENTER opens details, and BACK returns to the menu.
Holding ENTER toggles bool entries. For other actions, supply an optional
fourth field: {"Mode", mode, 's', toggleMode}, where the application defines
void toggleMode(void *value). A callback overrides the default bool toggle.
See test/I2C-Display.cpp for a complete example including the mode callback.

Buttons default to pins 2, 3, 4, 5 and connect to GND (INPUT_PULLUP).
Custom wiring: I2CDisplay menu(variables, count, up, down, enter, back);
An optional final TwoWire & argument selects the I2C bus.
begin(0x3C) accepts a different OLED address and returns false on failure.
Call update() frequently for button debounce and long-press detection.

Build the example with pio run -e Arduino_Uno_Display.
Requires the Adafruit SSD1306 and GFX dependencies configured in platformio.ini.
