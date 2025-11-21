# ESP32-2432S028R Interactive Pomodoro Timer

## Project Overview

An interactive Pomodoro timer built for the ESP32-2432S028R (Cheap Yellow Display) development board, leveraging proven components from Ed Nieuw's ESP32 Word Clock projects. This timer combines professional productivity features with modern connectivity (WiFi, BLE) and a vibrant touchscreen display.

## Hardware Platform

**ESP32-2432S028R (CYD - Cheap Yellow Display)**
- **MCU**: ESP32-WROOM-32 (dual-core, 240MHz)
- **RAM**: 520 KB SRAM
- **Flash**: 4 MB
- **Display**: 2.8" ILI9341 TFT (320x240 pixels, 16-bit color)
- **Touch**: Resistive touchscreen controller
- **Connectivity**: WiFi 2.4GHz, Bluetooth 5.0 (BLE)
- **Power**: 5V USB (micro-USB port)
- **RGB LED**: Built-in status indicator
- **GPIO**: 34 available pins

### Pin Configuration

```cpp
// ILI9341 Display (SPI Interface)
TFT_MISO    = GPIO 12   // Master In Slave Out
TFT_MOSI    = GPIO 13   // Master Out Slave In
TFT_SCK     = GPIO 14   // Serial Clock
TFT_CS      = GPIO 15   // Chip Select
TFT_DC      = GPIO 2    // Data/Command
TFT_RESET   = -1        // Not used (tied to ESP32 reset)
TFT_LED     = GPIO 21   // Backlight PWM control

// RGB Status LED
REDPIN      = GPIO 4
GREENPIN    = GPIO 16
BLUEPIN     = GPIO 17

// Sensors (Optional)
PhotoCellPin = GPIO 34  // LDR for ambient light (Note: GPIO2 conflict - disabled by default)

// Audio Output (Future Enhancement)
SPEAKER_PIN = GPIO 27   // PWM buzzer/speaker output
```

### Critical Hardware Notes

1. **GPIO 2 Conflict**: The TFT_DC pin on GPIO 2 conflicts with the LDR on GPIO 34. LDR functionality is disabled by default to prevent display freezing.
2. **LED Logic**: RGB LED is **active LOW** (LOW = ON, HIGH = OFF)
3. **Backlight PWM**: Uses LEDC channel 0, 250Hz, 8-bit resolution (0-255)

## Software Architecture

### Core Components (Inherited from Word Clock Project)

#### 1. Display System
- **Library**: Adafruit_ILI9341 + Adafruit_GFX
- **Resolution**: 320x240 pixels
- **Color Depth**: RGB565 (16-bit)
- **Features**: Hardware-accelerated primitives, custom fonts, sprite support
- **Backlight Control**: PWM-based brightness (0-255)

#### 2. Connectivity Stack

**WiFi:**
```cpp
// AsyncTCP + ESPAsyncWebServer
- Non-blocking web server for configuration
- OTA (Over-The-Air) firmware updates via AsyncElegantOTA
- mDNS support (pomodoro.local)
- Captive portal for initial setup
```

**Bluetooth Low Energy (BLE):**
```cpp
// NimBLE UART Service
Service UUID:     6E400001-B5A3-F393-E0A9-E50E24DCCA9E
TX Characteristic: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E (Notify)
RX Characteristic: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E (Write)

// Compatible mobile apps:
- iOS: "BLE Serial Pro" or "BLE serial nRF"
- Android: "Serial Bluetooth terminal"
```

**NTP Time Synchronization:**
```cpp
// ESPNtpClient
- Timezone-aware (POSIX tz format)
- Daylight saving time support
- Fallback to RTC if offline
- Sub-second accuracy when connected
```

#### 3. Persistent Storage

```cpp
// ESP32 Preferences (NVS Flash)
struct PomodoroSettings {
    // Timer Configuration
    uint16_t workDuration;        // Minutes (default: 25)
    uint16_t shortBreak;          // Minutes (default: 5)
    uint16_t longBreak;           // Minutes (default: 15)
    uint8_t  sessionsUntilLong;   // Count (default: 4)

    // Display Settings
    uint8_t  displayChoice;       // 0=Digital, 1=Analog, 2=Progress bar, 3=Minimalist
    uint32_t workColor;           // RGB565 color for work sessions
    uint32_t breakColor;          // RGB565 color for break sessions
    uint32_t backgroundColor;     // RGB565 background color
    uint8_t  brightness;          // 0-255 (default: 128)

    // Audio/Visual Alerts
    bool     soundEnabled;        // Buzzer on/off
    uint8_t  alertPattern;        // 0=Beep, 1=Melody, 2=Vibration pattern
    bool     ledEnabled;          // RGB LED feedback

    // Connectivity
    char     wifiSSID[32];
    char     wifiPassword[64];
    char     bleDeviceName[32];   // Default: "PomodoroTimer"
    char     timezone[64];         // POSIX format: "EST5EDT,M3.2.0,M11.1.0"
    bool     wifiEnabled;
    bool     bleEnabled;
    bool     ntpEnabled;

    // Scheduling
    uint8_t  autoStartHour;       // 0-23 (255 = disabled)
    uint8_t  autoStopHour;        // 0-23 (255 = disabled)
    bool     autoStartEnabled;

    // Statistics
    uint32_t totalSessions;       // Lifetime completed pomodoros
    uint32_t todaySessions;       // Today's completed count
    uint32_t lastResetDate;       // Unix timestamp of last daily reset

    uint16_t checksum;            // Validation (25065)
};
```

#### 4. Event-Driven Loop Architecture

```cpp
void loop() {
    // Continuous monitoring
    CheckDevices();           // BLE, Serial, Touch input

    // Periodic updates (every second)
    if (SecondChanged) {
        EverySecondCheck();   // Update countdown, display, LED status

        if (MinuteChanged) {
            EveryMinuteUpdate();  // Save statistics, check completion
        }

        if (HourChanged) {
            EveryHourUpdate();    // Daily reset check, WiFi reconnect
        }
    }
}
```

#### 5. Command Menu System

**Serial/BLE Commands:**
```
Timer Control:
  S               Start/Resume timer
  P               Pause timer
  R               Reset current session
  K               Skip to next session (work/break)

Configuration:
  W <minutes>     Set work duration (1-60)
  B <minutes>     Set short break duration (1-30)
  L <minutes>     Set long break duration (1-60)
  N <count>       Set sessions until long break (1-10)

Display:
  D <0-3>         Display mode (0=Digital, 1=Analog, 2=Progress, 3=Minimal)
  C <RRGGBB>      Set work color (hex)
  c <RRGGBB>      Set break color (hex)
  H <RRGGBB>      Set background color (hex)
  U <0-255>       Set brightness

WiFi/Network:
  A <SSID>        Set WiFi SSID
  a <password>    Set WiFi password
  E <timezone>    Set timezone (POSIX format: "EST5EDT,M3.2.0,M11.1.0")
  T <HHMMSS>      Set time manually

System:
  I               Print information menu
  M               Show statistics
  Z               Reset statistics
  F               Factory reset (all settings)
  @               Restart device

Options:
  O               Toggle display on/off
  o               Toggle sound on/off
  V               Toggle LED feedback on/off
  X               Toggle WiFi on/off
  Y               Toggle NTP on/off
  z               Toggle BLE on/off
```

### Pomodoro Timer State Machine

```cpp
enum PomodoroState {
    STATE_IDLE,           // Not started, showing start screen
    STATE_WORK,           // Work session in progress
    STATE_SHORT_BREAK,    // Short break in progress
    STATE_LONG_BREAK,     // Long break in progress
    STATE_PAUSED,         // Timer paused (preserves state)
    STATE_COMPLETE        // Session complete, awaiting acknowledgment
};

enum TimerEvent {
    EVENT_START,          // User starts timer
    EVENT_PAUSE,          // User pauses
    EVENT_RESUME,         // User resumes
    EVENT_RESET,          // User resets current session
    EVENT_SKIP,           // User skips to next session
    EVENT_TIMEOUT,        // Timer reaches zero
    EVENT_ACKNOWLEDGE     // User acknowledges completion
};
```

### Display Modes

#### Mode 0: Digital Countdown
```
┌────────────────────────────┐
│                            │
│      WORK SESSION          │
│                            │
│        24:35               │
│                            │
│    ████████████░░░░        │  ← Progress bar
│                            │
│   Session 2 of 4           │
│   Today: 8 completed       │
└────────────────────────────┘
```

#### Mode 1: Analog Clock
```
┌────────────────────────────┐
│                            │
│         ╭─────╮            │
│        │   │   │           │  ← Countdown fills circle
│        │ 24:35│           │
│         ╰─────╯            │
│                            │
│      WORK SESSION          │
│      Session 2/4           │
└────────────────────────────┘
```

#### Mode 2: Progress Bar Focus
```
┌────────────────────────────┐
│                            │
│  ██████████████████░░░░    │  ← Full-width progress
│                            │
│         24:35              │
│                            │
│    FOCUS TIME              │
│                            │
│  Next: Short Break (5m)    │
└────────────────────────────┘
```

#### Mode 3: Minimalist
```
┌────────────────────────────┐
│                            │
│                            │
│                            │
│         24:35              │  ← Large, centered time
│                            │
│                            │
│                            │
│          ●●●○              │  ← Session dots
└────────────────────────────┘
```

## Reusable Functions from Word Clock

### Display Functions
```cpp
// From ESP322432S028_WordClockV016.ino
void InitScreenILI9341();                    // Initialize display hardware
void ClearScreenILI9341();                   // Clear display to background color
void ShowCharsILI9341(uint16_t x, uint16_t y,
                      String text, uint32_t color);  // Render text
void SetBackLight(byte intensity);           // PWM backlight control (0-255)
void DrawRectangle(uint16_t x, uint16_t y,
                   uint16_t w, uint16_t h,
                   uint32_t color);          // Progress bars
void DrawCircle(uint16_t x, uint16_t y,
                uint16_t radius, uint32_t color);  // Analog timer
void FillCircle(uint16_t x, uint16_t y,
                uint16_t radius, uint32_t color);  // Filled shapes
```

### Time Management
```cpp
void GetTijd(bool printit);                  // Get current time (NTP/RTC)
void SetRTCTime();                           // Set RTC to specific time
void PrintNTP_tijd();                        // Display NTP sync status
void SyncEventTriggered(NTPEvent_t event);   // Handle NTP events
```

### Connectivity
```cpp
void StartBLEService();                      // Initialize BLE UART service
void CheckBLE();                             // Process BLE messages
void SendMessageBLE(String message);         // Send data via BLE
void StartWIFI_NTP();                        // Connect WiFi and sync time
void WiFiEvent(WiFiEvent_t event);           // WiFi state machine
void SetupAsyncWebServer();                  // Configure web server routes
```

### LED Control
```cpp
void SetStatusLED(uint8_t red, uint8_t green,
                  uint8_t blue);             // RGB LED control
void MakeRGBcolor(uint32_t color);           // Convert RGB565 to GPIO states
uint32_t Wheel(byte pos);                    // Rainbow color generator
uint32_t HourColor[24];                      // 24 pre-defined hourly colors
```

### Storage
```cpp
void StoreStructInFlashMemory();             // Save settings to NVS
void GetStructFromFlashMemory();             // Load settings from NVS
void Reset();                                // Factory reset
```

### Input Processing
```cpp
void ReworkInputString(String InputString);  // Parse menu commands
void SerialCheck();                          // Monitor serial input
void ProcessTouchInput();                    // Read touchscreen (new)
```

## Color Palette (from Colors.h)

**140+ Predefined Colors:**
```cpp
// Basic Colors
#define WHITE       0xFFFF
#define BLACK       0x0000
#define RED         0xF800
#define GREEN       0x07E0
#define BLUE        0x001F

// Productivity Colors (Suggested for Pomodoro)
#define TOMATO      0xFB08  // Work sessions (red tomato)
#define LIGHTGREEN  0x9772  // Short breaks
#define SKYBLUE     0x867D  // Long breaks
#define DIMGRAY     0x6B4D  // Paused state
#define GOLD        0xFEA0  // Completion alert

// Theme Sets
#define PROFESSIONAL_WORK    0x2945  // Navy blue
#define PROFESSIONAL_BREAK   0x4E99  // Teal
#define FOCUS_WORK           0xF800  // Bright red
#define FOCUS_BREAK          0x07E0  // Bright green
#define CALM_WORK            0x6B3F  // Muted purple
#define CALM_BREAK           0xAFF5  // Soft cyan
```

## Required Libraries

Install via Arduino IDE Library Manager:

```cpp
// Connectivity
#include <NimBLEDevice.h>         // BLE communication (h2zero/NimBLE-Arduino)
#include <ESPNtpClient.h>         // NTP time sync (gmag11/ESPNtpClient)
#include <AsyncTCP.h>             // Async TCP stack (me-no-dev/AsyncTCP)
#include <ESPAsyncWebServer.h>    // Async web server (me-no-dev/ESPAsyncWebServer)
#include <AsyncElegantOTA.h>      // OTA updates

// Display
#include <Adafruit_ILI9341.h>     // ILI9341 driver (adafruit/Adafruit_ILI9341)
#include <Adafruit_GFX.h>         // Graphics library (adafruit/Adafruit-GFX-Library)

// Storage & Utilities
#include <Preferences.h>          // NVS flash storage (ESP32 core)
#include <WiFi.h>                 // WiFi stack (ESP32 core)
#include <time.h>                 // Time functions (ESP32 core)
```

**Library Versions (Tested):**
- NimBLEDevice: 1.4.0+
- ESPNtpClient: 0.2.5+
- AsyncTCP: 1.1.1+
- ESPAsyncWebServer: 1.2.3+
- AsyncElegantOTA: 2.2.7+
- Adafruit_ILI9341: 1.6.0+
- Adafruit_GFX: 1.11.3+

## Development Setup

### Arduino IDE Configuration

**Board Settings:**
```
Board: "ESP32 Dev Module"
Flash Size: 4MB (32Mb)
Partition Scheme: "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)"
Upload Speed: 921600
CPU Frequency: 240MHz (WiFi/BT)
Flash Frequency: 80MHz
Flash Mode: QIO
Core Debug Level: None (or "Info" for debugging)
```

**ESP32 Board Package:**
- Required version: 3.0.0 or higher
- Install via: Tools → Board → Boards Manager → "esp32" by Espressif Systems

### TFT_eSPI Configuration

Copy the included `User_Setup.h` file to your TFT_eSPI library folder (if using TFT_eSPI instead of Adafruit):
```
Arduino/libraries/TFT_eSPI/User_Setup.h
```

This file contains CYD-specific pin mappings and optimizations.

### Initial Upload & Configuration

1. **Connect board** via USB
2. **Compile and upload** sketch
3. **Open Serial Monitor** (115200 baud)
4. **Send 'I'** to display menu
5. **Configure WiFi:**
   ```
   A<Your_SSID>
   a<Your_Password>
   ```
6. **Set timezone** (optional):
   ```
   E<EST5EDT,M3.2.0,M11.1.0>
   ```
7. **Restart device:**
   ```
   @
   ```

### OTA Updates (After Initial Setup)

1. Navigate to `http://<device-ip>/update` or `http://pomodoro.local/update`
2. Upload compiled `.bin` file
3. Device automatically restarts with new firmware

## Pomodoro Timer Features

### Core Functionality

**Standard Pomodoro Technique:**
- 25-minute work sessions
- 5-minute short breaks
- 15-minute long breaks after 4 sessions
- Configurable durations for all session types

**Enhanced Features:**
- Visual countdown on display (multiple modes)
- RGB LED status indicator (red=work, green=break, blue=paused)
- Optional buzzer alerts on completion
- Session counter with daily statistics
- Persistent session history
- Auto-start/auto-stop scheduling
- Pause/resume capability
- Skip to next session
- Manual time adjustment

### Interaction Methods

**1. Touchscreen (Primary)**
- Tap to start/pause
- Swipe right to skip session
- Swipe left to reset
- Long-press for menu
- Two-finger tap for statistics

**2. BLE Mobile App (Remote)**
- Start/stop timer
- View remaining time
- Configure all settings
- View session statistics
- Receive completion notifications

**3. Web Interface (Configuration)**
- Accessible at `http://pomodoro.local`
- Full settings management
- Statistics dashboard
- Firmware updates
- WiFi reconfiguration

**4. Serial Terminal (Debug/Setup)**
- USB connection required
- Full command menu access
- Real-time status output
- Initial configuration

### Alert Patterns

**Visual (RGB LED):**
```cpp
// Work Session
void WorkSessionLED() {
    // Solid red during work
    SetStatusLED(255, 0, 0);
}

// Short Break
void ShortBreakLED() {
    // Solid green during break
    SetStatusLED(0, 255, 0);
}

// Long Break
void LongBreakLED() {
    // Solid blue during long break
    SetStatusLED(0, 0, 255);
}

// Paused
void PausedLED() {
    // Breathing yellow (PWM)
    static uint8_t brightness = 0;
    SetStatusLED(brightness, brightness/2, 0);
}

// Complete
void CompleteLED() {
    // Fast rainbow pulse
    uint32_t color = Wheel(millis() / 10);
    MakeRGBcolor(color);
}
```

**Audio (Optional Buzzer on GPIO 27):**
```cpp
// Completion beep pattern
void PlayCompletionAlert() {
    tone(SPEAKER_PIN, 1000, 200);  // 1kHz, 200ms
    delay(300);
    tone(SPEAKER_PIN, 1500, 200);  // 1.5kHz, 200ms
    delay(300);
    tone(SPEAKER_PIN, 2000, 400);  // 2kHz, 400ms
}
```

## Statistics Tracking

**Daily Statistics:**
- Sessions completed today
- Total focus time (minutes)
- Total break time (minutes)
- Average session completion rate
- Longest streak (consecutive days)

**Lifetime Statistics:**
- Total sessions completed
- Total hours focused
- Total days used
- Most productive hour (histogram)

**Persistent Storage:**
All statistics saved to NVS flash, survive power loss and firmware updates.

## Web Interface Structure

**HTML Pages (Webpage.h):**

```html
<!-- Home Page -->
/                   Main timer display + quick controls

<!-- Configuration Pages -->
/settings           Timer duration settings
/display            Display mode and color configuration
/network            WiFi and timezone settings
/schedule           Auto-start/stop scheduling
/statistics         Session history and graphs
/about              Device info and firmware version

<!-- System Pages -->
/update             OTA firmware upload
/restart            Device restart
/reset              Factory reset (with confirmation)
```

**API Endpoints (JSON):**
```
GET  /api/status    → Current timer state, remaining time
POST /api/start     → Start/resume timer
POST /api/pause     → Pause timer
POST /api/reset     → Reset current session
POST /api/skip      → Skip to next session
GET  /api/config    → Get all settings
POST /api/config    → Update settings
GET  /api/stats     → Get statistics
```

## Project File Structure

```
ESP32-2432S028R-text-pomodoro/
├── ESP32_Pomodoro_Timer/
│   ├── ESP32_Pomodoro_Timer.ino    # Main sketch
│   ├── Colors.h                     # RGB565 color definitions
│   ├── Webpage.h                    # HTML/CSS/JS for web interface
│   ├── PomodoroTimer.h              # Timer state machine
│   ├── DisplayModes.h               # Display rendering functions
│   └── Statistics.h                 # Statistics tracking
├── User_Setup.h                     # TFT_eSPI configuration (backup)
├── claude.md                        # This file (project documentation)
├── README.md                        # User-facing documentation
└── libraries_Pomodoro.zip           # Library dependencies (backup)
```

## Development Roadmap

### Phase 1: Core Timer (MVP)
- [x] Project structure and documentation
- [ ] Port display functions from Word Clock
- [ ] Implement basic countdown timer
- [ ] Add start/pause/reset controls
- [ ] Persistent settings storage
- [ ] Digital countdown display mode

### Phase 2: Pomodoro Logic
- [ ] State machine implementation
- [ ] Work/break session management
- [ ] Session counter (sessions until long break)
- [ ] Completion alerts (LED + optional buzzer)
- [ ] Statistics tracking (daily/lifetime)

### Phase 3: Connectivity
- [ ] WiFi configuration menu
- [ ] BLE UART service
- [ ] NTP time synchronization
- [ ] Web interface (basic controls)
- [ ] OTA firmware updates

### Phase 4: Advanced Display
- [ ] Analog clock mode
- [ ] Progress bar mode
- [ ] Minimalist mode
- [ ] Color theme selection
- [ ] Brightness control

### Phase 5: Touchscreen
- [ ] Touch driver integration
- [ ] Tap to start/pause
- [ ] Swipe gestures
- [ ] Touch-based menu navigation
- [ ] Calibration routine

### Phase 6: Polish & Features
- [ ] Auto-start/stop scheduling
- [ ] Statistics dashboard (web)
- [ ] Mobile app integration
- [ ] Sound/vibration patterns
- [ ] Multi-language support
- [ ] Backup/restore settings

## Testing Strategy

**Unit Tests:**
- Timer countdown accuracy
- State machine transitions
- Settings save/load
- Command parsing

**Integration Tests:**
- WiFi connection reliability
- BLE connection stability
- NTP synchronization
- Display rendering performance

**User Acceptance Tests:**
- 25-minute work session completion
- Break transitions
- Multi-session workflow (4 work + 1 long break)
- Settings persistence across power cycles
- OTA update process

## Known Issues & Solutions

### Issue 1: GPIO 2 / LDR Conflict
**Problem:** Using LDR on GPIO 34 causes display freezing due to GPIO 2 clash
**Solution:** Disable LDR by default, or move to external ADC (ADS1115)

### Issue 2: WiFi Connection Delays
**Problem:** WiFi connection blocks main loop, causing timer inaccuracy
**Solution:** Implement async WiFi connection with timeout, fallback to offline mode

### Issue 3: Flash Wear from Frequent Writes
**Problem:** Saving statistics every second wears out NVS flash
**Solution:** Buffer statistics in RAM, write to flash only on minute changes or state transitions

### Issue 4: Touchscreen Noise
**Problem:** Capacitive touch interference from display SPI signals
**Solution:** Use resistive touch controller (XPT2046), add software debouncing

## References & Resources

**Original Projects:**
- Ed Nieuw's ESP32-2432S028R Display: https://github.com/ednieuw/ESP32-2432S028R-display
- Arduino ESP32 Nano Word Clock: https://github.com/ednieuw/Arduino-ESP32-Nano-Wordclock

**Hardware Documentation:**
- ESP32-2432S028R Specifications (see PDF in repo)
- ILI9341 Datasheet: https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf
- ESP32-WROOM-32 Datasheet: https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf

**Library Documentation:**
- NimBLE-Arduino: https://github.com/h2zero/NimBLE-Arduino
- ESPAsyncWebServer: https://github.com/me-no-dev/ESPAsyncWebServer
- Adafruit GFX Graphics Library: https://learn.adafruit.com/adafruit-gfx-graphics-library

**Pomodoro Technique:**
- Official Pomodoro Timer: https://francescocirillo.com/pages/pomodoro-technique
- Best Practices: https://todoist.com/productivity-methods/pomodoro-technique

## Contributing Guidelines

When modifying this project:

1. **Preserve Word Clock Infrastructure**: Keep BLE, WiFi, storage, and display abstraction layers intact
2. **Follow Naming Conventions**: Use Ed Nieuw's function naming style (PascalCase for major functions)
3. **Document Pin Changes**: Update claude.md if GPIO assignments change
4. **Test Connectivity**: Verify BLE and WiFi work after changes
5. **Maintain Compatibility**: Support ESP32 board package 3.0.0+
6. **Update Checksum**: Modify settings checksum if struct changes

## License

This project inherits components from Ed Nieuw's open-source ESP32 projects. Respect original author credits and maintain open-source licensing for derivative works.

---

**Document Version**: 1.0
**Last Updated**: 2025-01-21
**Author**: Pedro Fernandes
**Based on**: Ed Nieuw's ESP32-2432S028R-display (V016) and Arduino-ESP32-Nano-Wordclock
