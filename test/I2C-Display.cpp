#include <Arduino.h>
#include <Wire.h>
#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Change these pins to match your wiring. Buttons connect the pin to GND
// when pressed; INPUT_PULLUP is used.
constexpr uint8_t BUTTON_UP = 2;
constexpr uint8_t BUTTON_DOWN = 3;
constexpr uint8_t BUTTON_ENTER = 4;
constexpr uint8_t BUTTON_BACK = 5;

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr uint8_t OLED_RESET =  -1;
constexpr uint8_t OLED_ADDRESS = 0x3C;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables to display
float temperatureC = 30.0f; // Convert analog reading to Celsius
float motorSpeedRpm = 1250.0f;
int batteryPercent = 82;
bool systemActive = true;
char mode[5] = "Line";
int sensorValues[8] = {};

// Structure to hold variable information
struct DisplayVariable {
  const char *name;
  const void *value;
  char type;
};

// &variable stores the address
DisplayVariable variables[] = {
    {"Temperature", &temperatureC, 'f'},
    {"Motor speed", &motorSpeedRpm, 'f'},
    {"Battery", &batteryPercent, 'i'},
    {"System active", &systemActive, 'b'},
    {"Mode", mode, 's'},
    {"Linetrackers", nullptr, 'l'},
};
constexpr uint8_t VARIABLE_COUNT = sizeof(variables) / sizeof(variables[0]);

bool variableValuesChanged() {
  static uint8_t previousValues[VARIABLE_COUNT][sizeof(mode)];
  static bool initialized = false;
  bool changed = false;

  for (uint8_t i = 0; i < VARIABLE_COUNT; ++i) {
    uint8_t valueSize = variables[i].type == 'f' ? sizeof(float) :
                        variables[i].type == 'i' ? sizeof(int) :
                        variables[i].type == 'b' ? sizeof(bool) : sizeof(mode);
    if (initialized && memcmp(previousValues[i], variables[i].value, valueSize) != 0) {
      changed = true;
    }
    memcpy(previousValues[i], variables[i].value, valueSize);
  }
  initialized = true;
  return changed;
}

uint8_t selectedVariable = 0;
bool showingDetails = false;
uint8_t sensorScroll = 0;

enum ButtonIndex : uint8_t { UP, DOWN, ENTER, BACK, BUTTON_COUNT };
const uint8_t buttonPins[BUTTON_COUNT] = {
    BUTTON_UP, BUTTON_DOWN, BUTTON_ENTER, BUTTON_BACK};
bool previousReading[BUTTON_COUNT] = {HIGH, HIGH, HIGH, HIGH};
bool stableState[BUTTON_COUNT] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastChange[BUTTON_COUNT] = {0, 0, 0, 0};
constexpr unsigned long DEBOUNCE_MS = 30;

bool wasPressed(ButtonIndex button) {
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

void printVariableValue(const DisplayVariable &variable) {
  switch (variable.type) {
    case 'f': display.print(*static_cast<const float *>(variable.value), 2); break;
    case 'i': display.print(*static_cast<const int *>(variable.value)); break;
    case 'b': display.print(*static_cast<const bool *>(variable.value) ? "ON" : "OFF"); break;
    case 's': display.print(static_cast<const char *>(variable.value)); break;
    case 'l': break;
  }
}

void toggleSelectedValue() {
  DisplayVariable &variable = variables[selectedVariable];
  if (variable.type == 'b') {
    bool *value = const_cast<bool *>(static_cast<const bool *>(variable.value));
    *value = !*value;
  } else if (variable.type == 's') {
    strcpy(mode, strcmp(mode, "Line") == 0 ? "Sort" : "Line");
  }
}

void drawScreen() {
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
          uint8_t index = (sensorScroll + line * 2 + column) % 8;
          display.setCursor(column * 64, 16 + line * 12);
          display.print(index + 1);
          display.print(": ");
          display.print(sensorValues[index]);
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
      uint8_t i = (selectedVariable + VARIABLE_COUNT + line - 1) % VARIABLE_COUNT;
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

void setup() {
  for (uint8_t i = 0; i < BUTTON_COUNT; ++i) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    while (true) delay(100);
  }
  display.clearDisplay();
  drawScreen();
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
}

void loop() {
  static unsigned long lastValueCheck = 0;
  static unsigned long enterDownAt = 0;
  static bool enterHandled = false;
  temperatureC = 0.48828125f * analogRead(A0); // Update the temperature reading
  sensorValues[0] = analogRead(A1);
  sensorValues[1] = analogRead(A2);
  sensorValues[2] = analogRead(A3);
  sensorValues[3] = analogRead(A1)*2;
  sensorValues[4] = analogRead(A2)*2;
  sensorValues[5] = analogRead(A3)*2;
  sensorValues[6] = analogRead(A1)*3;
  sensorValues[7] = analogRead(A2)*3;

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
    changed = variableValuesChanged();
  }

  if (!showingDetails) {
    if (wasPressed(UP)) {
      selectedVariable = (selectedVariable + VARIABLE_COUNT - 1) % VARIABLE_COUNT;
      changed = true;
    }
    if (wasPressed(DOWN)) {
      selectedVariable = (selectedVariable + 1) % VARIABLE_COUNT;
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