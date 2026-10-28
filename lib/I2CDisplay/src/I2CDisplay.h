#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// Define the entries and their values in main.cpp and keep them alive.
// Types: f = float, i = int, b = bool, s = null-terminated char array,
// l = int[8]. Boolean entries toggle on a long ENTER press by default.
// Supply an optional callback for custom long-press behavior (e.g. strings).
struct DisplayVariable {
  const char *name;
  void *value;
  char type;
  void (*onLongPress)(void *);

  DisplayVariable(const char *label, void *pointer, char valueType,
                  void (*callback)(void *) = nullptr)
      : name(label), value(pointer), type(valueType), onLongPress(callback) {}
};
class I2CDisplay {
public:
  // Buttons connect to GND; begin() enables INPUT_PULLUP.
  I2CDisplay(const DisplayVariable *variables, uint8_t variableCount, uint8_t up = 2,
                      uint8_t down = 3, uint8_t enter = 4, uint8_t back = 5,
                      TwoWire &wire = Wire);
  bool begin(uint8_t address = 0x3C);
  void update(); // Call frequently from loop() for button handling.
  void refresh(); // Force a redraw after application data changes.

private:
  enum ButtonIndex : uint8_t { UP, DOWN, ENTER, BACK, BUTTON_COUNT };

  static constexpr unsigned long DEBOUNCE_MS = 30;

  Adafruit_SSD1306 display;
  const DisplayVariable *variables;
  uint8_t variableCount;
  uint8_t buttonPins[BUTTON_COUNT];
  bool previousReading[BUTTON_COUNT] = {HIGH, HIGH, HIGH, HIGH};
  bool stableState[BUTTON_COUNT] = {HIGH, HIGH, HIGH, HIGH};
  unsigned long lastChange[BUTTON_COUNT] = {};
  uint8_t selectedVariable = 0;
  bool showingDetails = false;
  bool ready = false;
  unsigned long lastValueCheck = 0;
  unsigned long enterDownAt = 0;
  bool enterHandled = false;

  bool wasPressed(ButtonIndex button);
  void printVariableValue(const DisplayVariable &variable);
  void toggleSelectedValue();
  void drawScreen();
};

