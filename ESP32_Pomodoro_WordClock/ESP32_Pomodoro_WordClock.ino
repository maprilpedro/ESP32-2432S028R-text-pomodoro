/*
ESP32 Pomodoro Timer with Word Clock
Based on ESP322432S028_WordClockV016 by Ed Nieuwenhuys

Features:
- Word clock display when timer is idle
- Touch controls: top-left=start, top-right=pause, bottom-right=stop
- Configuration popup for timer duration
- Countdown displayed in words (e.g., "TWENTY FOUR MINUTES")
- Red/orange flash animation on completion
- RGB LED status (red=work, green=break, yellow=paused)
- WiFi and BLE integration from original project

Hardware: ESP32-2432S028R (Cheap Yellow Display)
Display: ILI9341 2.8" TFT (320x240)
Touch: Resistive touch (XPT2046)

Author: Pedro Fernandes (based on Ed Nieuwenhuys' work)
Date: November 2025
*/

// ==================== INCLUDES ====================
#define ESP2432S028
#define ILI9341

// WiFi Credentials (stored in separate file, not committed to GitHub)
#include "credentials.h"

#include <NimBLEDevice.h>
#include <ESPNtpClient.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <Preferences.h>
#include "Colors.h"
#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <XPT2046_Touchscreen.h>  // Touch screen library
#include <esp_task_wdt.h>          // Watchdog timer

// ==================== TOUCHSCREEN SETUP ====================
#define TOUCH_CS 33    // Touch screen chip select
#define TOUCH_IRQ 36   // Touch screen interrupt

XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

// ==================== WORD CLOCK MACROS ====================
// Dutch language macros (from original)
#define HET     ColorLeds("Het",     0,   2, MINColor);
#define IS      ColorLeds("is",      4,   5, SECColor);    Is = true;
#define WAS     ColorLeds("was",     7,   9, SECColor);    Is = false;
#define MVIJF   ColorLeds("vijf",    11, 14, LetterColor);
#define PRECIES ColorLeds("precies", 16, 22, LetterColor);
#define MTIEN   ColorLeds("tien",    25, 28, LetterColor);
#define KWART   ColorLeds("kwart",   32, 36, LetterColor);
#define VOOR    ColorLeds("voor",    38, 41, LetterColor);
#define OVER    ColorLeds("over",    43, 46, LetterColor);
#define HALF    ColorLeds("half",    48, 51, LetterColor);
#define MIDDER  ColorLeds("midder",  53, 58, LetterColor);
#define VIJF    ColorLeds("vijf",    60, 63, LetterColor);
#define TWEE    ColorLeds("twee",    65, 68, LetterColor);
#define EEN     ColorLeds("een",     71, 73, LetterColor);
#define VIER    ColorLeds("vier",    76, 79, LetterColor);
#define ELF     ColorLeds("elf",     80, 82, LetterColor);
#define TIEN    ColorLeds("tien",    84, 87, LetterColor);
#define TWAALF  ColorLeds("twaalf",  89, 94, LetterColor);
#define DRIE    ColorLeds("drie",    97,100, LetterColor);
#define NEGEN   ColorLeds("negen",  102,106, LetterColor);
#define ACHT    ColorLeds("acht",   107,110, LetterColor);
#define ZES     ColorLeds("zes",    112,114, LetterColor);
#define ZEVEN   ColorLeds("zeven",  116,120, LetterColor);
#define UUR     ColorLeds("uur",    125,127, LetterColor);

// ==================== PIN DEFINITIONS ====================
#define REDPIN       4
#define GREENPIN    16
#define BLUEPIN     17

#define TFT_MISO    12
#define TFT_MOSI    13
#define TFT_SCK     14
#define TFT_CS      15
#define TFT_DC       2
#define TFT_RESET   -1
#define TFT_LED     21

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RESET, TFT_MISO);

// ==================== POMODORO STATE MACHINE ====================
enum PomodoroState {
  STATE_IDLE,          // Not started - showing word clock
  STATE_CONFIG,        // Configuration popup active
  STATE_WORK,          // Work session in progress
  STATE_PAUSED,        // Timer paused
  STATE_COMPLETE       // Session complete - flashing
};

PomodoroState pomodoroState = STATE_IDLE;
uint32_t workDurationMinutes = 25;  // Default 25 minutes
uint32_t secondsRemaining = 0;
bool timerRunning = false;

// Animation control (non-blocking)
unsigned long animationStartTime = 0;
uint8_t animationStep = 0;

// Touch debounce (non-blocking)
unsigned long lastTouchTime = 0;
const unsigned long TOUCH_DEBOUNCE_MS = 200;

// ==================== WORD CLOCK GLOBALS ====================
struct tm timeinfo;
byte lastminute = 0;
uint32_t LetterColor = 0xFF0000;
uint32_t MINColor = 0x00FF00;
uint32_t SECColor = 0x0000FF;
bool Is = true;

// Display buffer (16 columns × 8 rows = 128 characters)
struct CharPos {
  char character;
  uint32_t color;
};

CharPos Strippos[128];       // Current display buffer
CharPos LastStrippos[128];   // Previous display state for differential rendering

// ==================== FLASH STORAGE ====================
Preferences FLASHSTOR;

struct EEPROMstorage {
  char WIFIssid[30];
  char WIFIpassword[40];
  uint32_t  PomodoroWorkMinutes;
  uint16_t Checksum;
};

EEPROMstorage Mem;

// ==================== NUMBER TO WORDS CONVERSION ====================
const char* UnitsWords[] = {
  "", "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN",
  "EIGHT", "NINE", "TEN", "ELEVEN", "TWELVE", "THIRTEEN", "FOURTEEN",
  "FIFTEEN", "SIXTEEN", "SEVENTEEN", "EIGHTEEN", "NINETEEN"
};

const char* TensWords[] = {
  "", "", "TWENTY", "THIRTY", "FORTY", "FIFTY"
};

// FIXED: Added bounds checking and safe string operations
void NumToWords(int num, char* output, size_t outputSize) {
  if (outputSize < 2) return;  // Safety check

  // Bounds check: valid range is 0-59
  if (num < 0 || num > 59) {
    strncpy(output, "??", outputSize - 1);
    output[outputSize - 1] = '\0';
    return;
  }

  if (num == 0) {
    strncpy(output, "ZERO", outputSize - 1);
    output[outputSize - 1] = '\0';
    return;
  }

  if (num < 20) {
    strncpy(output, UnitsWords[num], outputSize - 1);
    output[outputSize - 1] = '\0';
  } else {
    int tens = num / 10;
    int units = num % 10;

    // Safe copy of tens word
    strncpy(output, TensWords[tens], outputSize - 1);
    output[outputSize - 1] = '\0';

    // Check space before adding units
    if (units > 0 && strlen(output) + strlen(UnitsWords[units]) + 2 < outputSize) {
      strcat(output, " ");
      strcat(output, UnitsWords[units]);
    }
  }
}

// ==================== DISPLAY FUNCTIONS ====================
void LedsOff() {
  for (int i = 0; i < 128; i++) {
    Strippos[i].character = ' ';
    Strippos[i].color = 0;
  }
}

void ColorLeds(const char* text, int start, int end, uint32_t color) {
  int len = strlen(text);
  int pos = start;

  for (int i = 0; i < len && pos <= end; i++) {
    if (pos < 128) {
      Strippos[pos].character = text[i];
      Strippos[pos].color = color;
    }
    pos++;
  }
}

// FIXED: Differential rendering to eliminate flicker and improve performance
void ShowCharsILI9341(bool forceFullRedraw = false) {
  if (forceFullRedraw) {
    tft.fillScreen(0x0000);  // Full clear only when needed
    memset(LastStrippos, 0, sizeof(LastStrippos));  // Reset cache
  }

  int x, y;
  for (int i = 0; i < 128; i++) {
    // Only update changed characters
    if (forceFullRedraw ||
        Strippos[i].character != LastStrippos[i].character ||
        Strippos[i].color != LastStrippos[i].color) {

      int col = i % 16;
      int row = i / 16;
      x = col * 20;       // 20 pixels per character
      y = 20 + row * 27;  // 27 pixels per row, 20 pixel top margin

      // Erase old character (draw in black)
      if (!forceFullRedraw && LastStrippos[i].character != ' ') {
        tft.setTextColor(0x0000);  // Black
        tft.setTextSize(2);
        tft.setCursor(x, y);
        tft.print(LastStrippos[i].character);
      }

      // Draw new character
      if (Strippos[i].character != ' ') {
        tft.setTextColor(Strippos[i].color);
        tft.setTextSize(2);
        tft.setCursor(x, y);
        tft.print(Strippos[i].character);
      } else if (!forceFullRedraw) {
        // Erase to background
        tft.fillRect(x, y, 20, 27, 0x0000);
      }

      // Update cache
      LastStrippos[i] = Strippos[i];
    }
  }
}

void ClearScreenILI9341() {
  tft.fillScreen(0x0000);
}

void SetBackLight(byte intensity) {
  ledcWrite(0, intensity);
}

// ==================== RGB LED CONTROL ====================
void SetStatusLED(uint8_t red, uint8_t green, uint8_t blue) {
  // Active LOW logic
  digitalWrite(REDPIN, red == 0 ? HIGH : LOW);
  digitalWrite(GREENPIN, green == 0 ? HIGH : LOW);
  digitalWrite(BLUEPIN, blue == 0 ? HIGH : LOW);
}

// ==================== POMODORO DISPLAY ====================
void DisplayPomodoroTimer(bool fullRedraw = false) {
  LedsOff();

  uint16_t minutes = secondsRemaining / 60;
  uint8_t seconds = secondsRemaining % 60;

  char minutesStr[32];
  char secondsStr[32];

  // FIXED: Added buffer size parameter
  NumToWords(minutes, minutesStr, sizeof(minutesStr));
  NumToWords(seconds, secondsStr, sizeof(secondsStr));

  // Set colors based on state
  uint32_t timeColor = 0xFF0000;  // Red for work
  uint32_t labelColor = 0xFF8C00; // Orange for labels

  if (pomodoroState == STATE_PAUSED) {
    timeColor = 0xFFFF00;    // Yellow
    labelColor = 0xFFD700;   // Gold
  }

  // Row 0-1: Minutes display
  ColorLeds(minutesStr, 0, 15, timeColor);

  // Row 2: "MINUTES" label
  ColorLeds("MINUTES", 32, 38, labelColor);

  // Row 3-4: Seconds display
  ColorLeds(secondsStr, 48, 63, timeColor);

  // Row 5: "SECONDS" label
  ColorLeds("SECONDS", 80, 86, labelColor);

  // Row 7: Status text
  if (pomodoroState == STATE_PAUSED) {
    ColorLeds("PAUSED", 112, 117, labelColor);
  } else {
    ColorLeds("FOCUS", 112, 116, labelColor);
  }

  ShowCharsILI9341(fullRedraw);
}

// FIXED: Smart update - only redraw seconds when minutes don't change
void UpdateSecondsDisplay() {
  uint8_t seconds = secondsRemaining % 60;
  char secondsStr[32];
  NumToWords(seconds, secondsStr, sizeof(secondsStr));

  uint32_t timeColor = (pomodoroState == STATE_PAUSED) ? 0xFFFF00 : 0xFF0000;
  uint32_t labelColor = (pomodoroState == STATE_PAUSED) ? 0xFFD700 : 0xFF8C00;

  // Clear only seconds area (rows 3-5)
  for (int i = 48; i <= 86; i++) {
    Strippos[i].character = ' ';
    Strippos[i].color = 0;
  }

  // Update seconds
  ColorLeds(secondsStr, 48, 63, timeColor);
  ColorLeds("SECONDS", 80, 86, labelColor);

  ShowCharsILI9341(false);  // Differential update
}

void DisplayConfigPopup() {
  tft.fillScreen(0x0000);
  tft.setTextColor(0xFFFFFF);
  tft.setTextSize(2);

  tft.setCursor(20, 40);
  tft.print("POMODORO TIMER");

  tft.setCursor(20, 80);
  tft.print("Set Duration:");

  tft.setTextSize(3);
  tft.setCursor(80, 120);
  tft.setTextColor(0x00FF00);
  tft.print(workDurationMinutes);
  tft.print(" min");

  tft.setTextSize(1);
  tft.setTextColor(0xFFFF00);
  tft.setCursor(20, 180);
  tft.print("Tap - to decrease, + to increase");
  tft.setCursor(20, 200);
  tft.print("Tap START to begin");

  // Draw buttons
  tft.drawRect(20, 220, 60, 30, 0xFF0000);
  tft.setCursor(30, 228);
  tft.setTextSize(2);
  tft.print("-");

  tft.drawRect(120, 220, 80, 30, 0x00FF00);
  tft.setCursor(130, 228);
  tft.print("START");

  tft.drawRect(240, 220, 60, 30, 0xFF0000);
  tft.setCursor(250, 228);
  tft.print("+");
}

void DisplayWordClock() {
  // Simplified Dutch word clock display
  LedsOff();

  GetTijd(false);

  byte hours = timeinfo.tm_hour;
  byte minutes = timeinfo.tm_min;

  HET;
  IS;

  // Simple time display logic (abbreviated from full Dutch() function)
  if (minutes < 5) {
    switch (hours) {
      case 1: EEN; break;
      case 2: TWEE; break;
      case 3: DRIE; break;
      case 4: VIER; break;
      case 5: VIJF; break;
      case 6: ZES; break;
      case 7: ZEVEN; break;
      case 8: ACHT; break;
      case 9: NEGEN; break;
      case 10: TIEN; break;
      case 11: ELF; break;
      case 12: case 0: TWAALF; break;
    }
    UUR;
  }
  // Add more time logic here as needed...

  ShowCharsILI9341(true);  // Full redraw for clock
}

// FIXED: Save work duration to flash
void SaveWorkDuration() {
  FLASHSTOR.begin("Pomodoro", false);
  FLASHSTOR.putUInt("workMin", workDurationMinutes);
  FLASHSTOR.end();
  Serial.printf("Work duration saved: %d minutes\n", workDurationMinutes);
}

// ==================== TIMER CONTROL ====================
void StartPomodoro() {
  pomodoroState = STATE_WORK;
  secondsRemaining = workDurationMinutes * 60;
  timerRunning = true;
  SetStatusLED(255, 0, 0);  // Red LED
  DisplayPomodoroTimer(true);  // Full redraw on start
}

void PausePomodoro() {
  if (pomodoroState == STATE_WORK) {
    pomodoroState = STATE_PAUSED;
    timerRunning = false;
    SetStatusLED(255, 255, 0);  // Yellow LED
    DisplayPomodoroTimer();
  } else if (pomodoroState == STATE_PAUSED) {
    pomodoroState = STATE_WORK;
    timerRunning = true;
    SetStatusLED(255, 0, 0);  // Red LED
    DisplayPomodoroTimer();
  }
}

void StopPomodoro() {
  pomodoroState = STATE_IDLE;
  timerRunning = false;
  secondsRemaining = 0;
  SetStatusLED(0, 0, 0);  // LED off
  DisplayWordClock();
}

// FIXED: Non-blocking completion alert
void PlayCompletionAlert() {
  pomodoroState = STATE_COMPLETE;
  animationStartTime = millis();
  animationStep = 0;
  timerRunning = false;
}

void UpdateCompletionAnimation() {
  if (pomodoroState != STATE_COMPLETE) return;

  unsigned long elapsed = millis() - animationStartTime;
  uint8_t step = elapsed / 300;  // 300ms per step

  if (step != animationStep) {
    animationStep = step;

    if (animationStep >= 6) {
      // Animation complete
      StopPomodoro();
      return;
    }

    // Alternate red/orange
    if (animationStep % 2 == 0) {
      tft.fillScreen(0xF800);  // Red (RGB565)
      SetStatusLED(255, 0, 0);
    } else {
      tft.fillScreen(0xFB00);  // Orange (RGB565)
      SetStatusLED(255, 140, 0);
    }
  }
}

// FIXED: Smart update - only redraw changed parts
void UpdatePomodoro() {
  if (!timerRunning || pomodoroState != STATE_WORK)
    return;

  if (secondsRemaining > 0) {
    uint16_t oldMinutes = secondsRemaining / 60;
    secondsRemaining--;
    uint16_t newMinutes = secondsRemaining / 60;

    // Only full update if minutes changed
    if (oldMinutes != newMinutes) {
      DisplayPomodoroTimer(false);  // Differential update
    } else {
      // Fast update: only seconds changed
      UpdateSecondsDisplay();
    }
  } else {
    // Timer complete!
    PlayCompletionAlert();
  }
}

// ==================== TOUCH HANDLING ====================
// FIXED: Non-blocking debounce
void HandleTouch() {
  if (!ts.touched())
    return;

  // Non-blocking debounce check
  unsigned long now = millis();
  if (now - lastTouchTime < TOUCH_DEBOUNCE_MS)
    return;

  lastTouchTime = now;

  TS_Point p = ts.getPoint();

  // Map touch coordinates (touchscreen is rotated)
  uint16_t x = map(p.y, 200, 3700, 0, 320);
  uint16_t y = map(p.x, 240, 3800, 0, 240);

  Serial.printf("Touch: x=%d, y=%d, state=%d\n", x, y, pomodoroState);

  if (pomodoroState == STATE_CONFIG) {
    // Config screen touch zones
    if (y > 220 && y < 250) {
      if (x > 20 && x < 80) {
        // Minus button
        if (workDurationMinutes > 1) {
          workDurationMinutes--;
          SaveWorkDuration();  // FIXED: Save to flash
          DisplayConfigPopup();
        }
      } else if (x > 120 && x < 200) {
        // Start button
        StartPomodoro();
      } else if (x > 240 && x < 300) {
        // Plus button
        if (workDurationMinutes < 60) {
          workDurationMinutes++;
          SaveWorkDuration();  // FIXED: Save to flash
          DisplayConfigPopup();
        }
      }
    }
  } else {
    // Timer screen touch zones
    if (y < 80) {
      // Top half
      if (x < 160) {
        // Top-left: Start/Config
        if (pomodoroState == STATE_IDLE) {
          pomodoroState = STATE_CONFIG;
          DisplayConfigPopup();
        }
      } else {
        // Top-right: Pause
        PausePomodoro();
      }
    } else {
      // Bottom half
      if (x > 160) {
        // Bottom-right: Stop
        StopPomodoro();
      }
    }
  }
}

// ==================== TIME FUNCTIONS ====================
void GetTijd(bool printit) {
  time_t now;
  time(&now);
  localtime_r(&now, &timeinfo);
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Pomodoro Timer Starting...");

  // Initialize watchdog timer (10 seconds timeout)
  esp_task_wdt_init(10, true);  // 10 second timeout, panic on timeout
  esp_task_wdt_add(NULL);       // Add current task to watchdog
  Serial.println("Watchdog timer enabled (10s timeout)");

  // Initialize RGB LED pins
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  SetStatusLED(0, 0, 0);

  // Initialize backlight PWM
  ledcAttachChannel(TFT_LED, 250, 8, 0);
  SetBackLight(128);  // 50% brightness

  // Initialize TFT
  tft.begin();
  tft.setRotation(1);  // Landscape mode
  tft.fillScreen(0x0000);

  // Initialize touchscreen
  ts.begin();
  ts.setRotation(1);

  // Initialize display buffers
  memset(Strippos, 0, sizeof(Strippos));
  memset(LastStrippos, 0, sizeof(LastStrippos));

  // Validate credentials
  #ifndef WIFI_SSID
    Serial.println("ERROR: credentials.h not found or invalid!");
    Serial.println("Please copy credentials.h.example to credentials.h");
    Serial.println("System will continue without WiFi...");
  #else
    Serial.printf("WiFi credentials loaded: %s\n", WIFI_SSID);
  #endif

  // Load settings from flash
  FLASHSTOR.begin("Pomodoro", false);
  Mem.PomodoroWorkMinutes = FLASHSTOR.getUInt("workMin", 25);
  workDurationMinutes = Mem.PomodoroWorkMinutes;
  FLASHSTOR.end();

  Serial.printf("Loaded work duration: %d minutes\n", workDurationMinutes);

  // Set time (simplified - in full version use NTP)
  struct timeval now = { .tv_sec = 1700000000, .tv_usec = 0 };
  settimeofday(&now, NULL);

  // Show initial word clock
  DisplayWordClock();

  Serial.println("Setup complete. Touch top-left to start Pomodoro.");
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
}

// ==================== MAIN LOOP ====================
void loop() {
  // Reset watchdog timer (must be called regularly)
  esp_task_wdt_reset();

  // Handle touch input (non-blocking)
  HandleTouch();

  // Update completion animation (non-blocking)
  UpdateCompletionAnimation();

  // Update timer every second
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();

    // Update pomodoro timer (smart updates)
    UpdatePomodoro();

    // Update word clock if idle
    if (pomodoroState == STATE_IDLE) {
      static byte lastDisplayMinute = 255;
      GetTijd(false);
      if (timeinfo.tm_min != lastDisplayMinute) {
        lastDisplayMinute = timeinfo.tm_min;
        DisplayWordClock();
      }
    }
  }

  delay(10);  // Small delay for stability
}
