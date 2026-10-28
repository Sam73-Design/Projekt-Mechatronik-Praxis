#include "I2CDisplay.h"
#include <string.h>

I2CDisplay::I2CDisplay(const DisplayVariable *entries, uint8_t count,
                       uint8_t up, uint8_t down, uint8_t enter,
                       uint8_t back, TwoWire &wire)
    : display(128, 64, &wire, -1), variables(entries), variableCount(count),
      buttonPins{up, down, enter, back} {}
bool I2CDisplay::begin(uint8_t address) {
  ready = false;
  selectedVariable = 0;
  showingDetails = false;
  if (!variables || variableCount == 0) return false;
  for (uint8_t i = 0; i < variableCount; ++i) {
    if (!variables[i].name || !variables[i].value) return false;
    if (!strchr("fibsl", variables[i].type) || variables[i].type == '\0')
      return false;
  }
  for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    previousReading[i] = stableState[i] = digitalRead(buttonPins[i]);
    lastChange[i] = millis();
  }
  ready = display.begin(SSD1306_SWITCHCAPVCC, address);
  if (ready) drawScreen();
  return ready;
}

void I2CDisplay::refresh() {
  drawScreen();
}
bool I2CDisplay::wasPressed(ButtonIndex button) {
  bool reading = digitalRead(buttonPins[button]);
  unsigned long now = millis();

  if (reading != previousReading[button]) {
    previousReading[button] = reading;
    lastChange[button] = now;
  }
  if (now - lastChange[button] >= DEBOUNCE_MS &&
      reading != stableState[button]) {
    stableState[button] = reading;
    return stableState[button] == LOW;
  }
  return false;
}

void I2CDisplay::printVariableValue(const DisplayVariable &variable) {
  switch (variable.type) {
    case 'f': display.print(*static_cast<const float *>(variable.value), 2); break;
    case 'i': display.print(*static_cast<const int *>(variable.value)); break;
    case 'b': display.print(*static_cast<const bool *>(variable.value) ? "ON" : "OFF"); break;
    case 's': display.print(static_cast<const char *>(variable.value)); break;
    case 'l': break;
  }
}

void I2CDisplay::toggleSelectedValue() {
  const DisplayVariable &variable = variables[selectedVariable];
  if (variable.onLongPress) {
    variable.onLongPress(variable.value);
  } else if (variable.type == 'b') {
    bool *value = static_cast<bool *>(variable.value);
    *value = !*value;
  }
}
void I2CDisplay::drawScreen() {
  if (!ready) return;
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  if (showingDetails) {
    const DisplayVariable &variable = variables[selectedVariable];
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(variable.name);
    if (variable.type == 'l') {
      for (uint8_t line = 0; line < 4; ++line) {
        for (uint8_t column = 0; column < 2; ++column) {
          uint8_t index = (line * 2 + column) % 8;
          display.setCursor(column * 64, 16 + line * 12);
          display.print(index + 1);
          display.print(": ");
          display.print(static_cast<const int *>(variable.value)[index]);
        }
      }
    } else {
      display.setCursor(0, 24);
      printVariableValue(variable);
      display.setCursor(0, 48);
      display.print("BACK: menu");
    }
  } else {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Variables  UP/DOWN");
    for (uint8_t line = 0; line < 3; ++line) {
      uint8_t i = (selectedVariable + variableCount + line - 1) % variableCount;
      uint8_t y = 16 + line * 16;
      display.setCursor(0, y);
      display.print(line == 1 ? "> " : "  ");
      display.print(variables[i].name);
      display.print(": ");
      printVariableValue(variables[i]);
    }
    
  }

  display.display();
}

void I2CDisplay::update() {
  if (!ready) return;
  bool changed = false;

  unsigned long now = millis();
  bool enterEvent = wasPressed(ENTER);
  if (enterEvent) {
    enterDownAt = now;
    enterHandled = false;
  }
  if (stableState[ENTER] == LOW && !enterHandled && now - enterDownAt >= 700) {
    toggleSelectedValue();
    enterHandled = true;
    changed = true;
  }
  if (now - lastValueCheck >= 1000) {
    lastValueCheck = now;
    changed = true; // Refresh application values once per second.
  }

  if (!showingDetails) {
    if (wasPressed(UP)) {
      selectedVariable = (selectedVariable + variableCount - 1) % variableCount;
      changed = true;
    }
    if (wasPressed(DOWN)) {
      selectedVariable = (selectedVariable + 1) % variableCount;
      changed = true;
    }
    if (enterEvent && !enterHandled) {
      showingDetails = true;
      changed = true;
    }
    wasPressed(BACK); // Consume the button event while on the menu.
  } else {

    if (wasPressed(BACK)) {
      showingDetails = false;
      changed = true;
    }
  }

  if (changed) drawScreen();
}

