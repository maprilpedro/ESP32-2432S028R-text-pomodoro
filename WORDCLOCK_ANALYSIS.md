# Word Clock V016 Architecture Analysis
## Reference Guide for Pomodoro Timer Implementation

---

## 1. COMPLETE STRUCTURE DEFINITIONS

### 1.1 EEPROMStorage Struct
Location: `/Users/pedrofer/GitHub/ESP32-2432S028R-text-pomodoro/ESP322432S028_WordClockV016/ESP322432S028_WordClockV016.ino` (lines 361-394)

```cpp
struct EEPROMstorage {                   // Data storage in EEPROM to maintain them after power loss
  byte DisplayChoice    = 0;              // 0=Yellow, 1=Hourly, 2=White, 3=All Own, 4=Own, 5=Wheel, 6=Digital, 7=Analog
  byte TurnOffLEDsAtHH  = 0;
  byte TurnOnLEDsAtHH   = 0;
  byte LanguageChoice   = 0;              // 0=NL, 1=UK, 2=DE, 3=FR, 4=Random
  byte LightReducer     = 0;
  int  LowerBrightness  = 0;
  int  UpperBrightness  = 0;
  int  NVRAMmem[24];                      // LDR readings
  byte BLEOnOff         = 1;
  byte NTPOnOff         = 1;
  byte WIFIOnOff        = 1;
  byte StatusLEDOnOff   = 1;
  int  ReconnectWIFI    = 0;              // No of times WIFI reconnected
  byte UseSDcard        = 0;
  byte WIFINoOfRestarts = 0;              // If 1 than restart MCU once
  byte ByteFuture1      = 0;              // For future use
  byte ByteFuture2      = 0;
  byte ByteFuture3      = 0;
  byte ByteFuture4      = 0;
  int  IntFuture1       = 0;              // For future use
  int  IntFuture2       = 0;
  int  IntFuture3       = 0;
  int  IntFuture4       = 0;
  byte UseBLELongString = 0;              // Send strings longer than 20 bytes per message
  uint32_t OwnColour    = 0;              // Self defined colour for clock display
  uint32_t DimmedLetter = 0;
  uint32_t BackGround   = 0;
  char Ssid[30];                          // WiFi SSID
  char Password[40];                      // WiFi password
  char BLEbroadcastName[30];              // Name of the BLE beacon
  char Timezone[50];                      // POSIX timezone format
  int  Checksum        = 0;               // Validation checksum
}  Mem;  // Global instance
```

### 1.2 LEDPrintBuffer Struct
Location: Lines 314-318

```cpp
const byte MATRIX_WIDTH     = 16;         // 16 characters wide
const byte MATRIX_HEIGHT    = 8;          // 8 rows tall
const byte NUM_LEDS         = MATRIX_WIDTH * MATRIX_HEIGHT;  // 128 total "LEDs" (chars)

struct LEDPrintbuffer {
  char Character;                         // The character to display
  uint32_t RGBColor;                      // 32-bit RGB color (0xRRGGBB)
} Strippos[NUM_LEDS+1];                   // Array of characters with their colors
```

### 1.3 Global Color Variables
Location: Lines 322-328

```cpp
uint32_t MINColor              = C_YELLOW;    // Color for "IS" and "WAS"
uint32_t SECColor              = C_YELLOW;    // Color for minutes/seconds indicator
uint32_t LetterColor           = C_YELLOW;    // Default letter color for time words
uint32_t HourColor[24] = {                    // Hourly color scheme (one per hour)
  C_WHITE, C_RED, C_ORANGE, C_YELLOW,
  C_YELLOW_GREEN, C_GREEN,
  C_CYAN,  C_DODGER_BLUE, C_PURPLE, C_MAGENTA, C_GOLD, C_SPRING_GREEN,
  C_WHITE, C_CHOCOLATE, C_ORANGE, C_KHAKI, C_YELLOW_GREEN, C_BEIGE,
  C_AQUA_MARINE,  C_SKY_BLUE, C_HOT_PINK, C_DEEP_PINK, C_CORAL, C_LAWN_GREEN
};
```

---

## 2. COLORLEDS() FUNCTION - CORE TEXT DISPLAY MECHANISM

Location: `/Users/pedrofer/GitHub/ESP32-2432S028R-text-pomodoro/ESP322432S028_WordClockV016/ESP322432S028_WordClockV016.ino` (lines 1079-1094)

### How ColorLeds Works
This function fills the `Strippos[]` array with characters and colors. It's the key to displaying text.

```cpp
void ColorLeds(char const *Texkst, int FirstLed, int LastLed, uint32_t RGBColor)
{
  int n, i=0;
  char Tekst[strlen(Texkst)+5];

  // Print debug text (optional)
  if (!NoTextInColorLeds && (strlen(Texkst) > 0 && strlen(Texkst) <10) )
  {
    sprintf(sptext,"%s ",Texkst);
    Tekstprint(sptext);  // Serial output
  }

  // Convert input string to uppercase
  strcpy(Tekst, Texkst);
  to_upper(Tekst);

  // Fill the Strippos array with characters and color
  for (n=FirstLed; n<=FirstLed + (LastLed-FirstLed); n++)
  {
    Strippos[n].Character = Tekst[i++];
    Strippos[n].RGBColor = RGBColor;  // Every character gets the same color
  }
}
```

### Key Points
- **Input**: Text string, starting position (FirstLed), ending position (LastLed), and RGB color
- **Output**: Updates global `Strippos[]` array with characters and colors
- **Character Range**: Maps positions 0-127 on the 16x8 character grid
- **Color**: 32-bit RGB format (0xRRGGBB) - stored at every character position
- **Uppercase**: All text is auto-converted to uppercase
- **Serial Debug**: Optional printing of which words are being added

### Usage Pattern in Dutch Clock
The Dutch clock uses macro definitions to call ColorLeds with predefined positions:

```cpp
#define HET     ColorLeds("Het",     0,   2, MINColor);
#define IS      ColorLeds("is",      4,   5, SECColor);    Is = true;
#define MVIJF   ColorLeds("vijf",    11, 14, LetterColor);
#define PRECIES ColorLeds("precies", 16, 22, LetterColor);
#define MTIEN   ColorLeds("tien",    25, 28, LetterColor);
#define KWART   ColorLeds("kwart",   32, 36, LetterColor);
#define VOOR    ColorLeds("voor",    38, 41, LetterColor);
#define OVER    ColorLeds("over",    43, 46, LetterColor);
#define HALF    ColorLeds("half",    48, 51, LetterColor);
```

### For Your Pomodoro Timer
You would create similar macros for numbers:

```cpp
// Example for countdown display
#define TWENTY  ColorLeds("twenty",  0,   5, LetterColor);
#define FOUR    ColorLeds("four",    8,  11, LetterColor);
#define MINUTES ColorLeds("minutes", 14, 20, LetterColor);
#define THIRTY  ColorLeds("thirty", 24, 29, LetterColor);
#define SECONDS ColorLeds("seconds", 32, 38, LetterColor);
```

---

## 3. DUTCH() FUNCTION - STATE MACHINE FOR TEXT DISPLAY

Location: Lines 1348-1457

### Overview
The Dutch clock function determines which words to display based on current time. It's a pure state machine with NO display output - it only calls ColorLeds() macros.

```cpp
void Dutch(void)
{
  HET;  // "HET" is always displayed (like "IT IS" in English)

  // Special case for noon
  if (timeinfo.tm_hour == 12 && timeinfo.tm_min == 0 && random(2)==0)
  {
    IS;
    NOEN;  // "NOON"
    return;
  }

  // Special case for midnight
  if (timeinfo.tm_hour == 00 && timeinfo.tm_min == 0 && random(2)==0)
  {
    IS;
    MIDDER;  // "MIDDLE"
    NACHT;   // "NIGHT"
    return;
  }

  // Main time expression based on minutes
  switch (timeinfo.tm_min)
  {
    case  0: IS;  PRECIES; break;           // "IT IS EXACTLY"
    case  1: IS;  break;                     // "IT IS"
    case  2:
    case  3: WAS; break;                     // "IT WAS"
    case  4:
    case  5:
    case  6: IS;  MVIJF; OVER; break;       // "IT IS FIVE PAST"
    case  7:
    case  8: WAS; MVIJF; OVER; break;       // "IT WAS FIVE PAST"
    case  9:
    case 10:
    case 11: IS;  MTIEN; OVER; break;       // "IT IS TEN PAST"
    // ... continues for all 60 minutes
    case 29: IS;  HALF; break;              // "IT IS HALF"
    case 30: IS;  PRECIES; HALF; break;    // "IT IS EXACTLY HALF"
    case 59: IS;  break;                     // "IT IS" (one minute to hour)
  }

  // Handle hour display
  sayhour = timeinfo.tm_hour;
  if (timeinfo.tm_min > 18 )  sayhour = timeinfo.tm_hour+1;  // Adjust hour after 18 minutes past
  if (sayhour == 24) sayhour = 0;

  // Display the hour name
  switch (sayhour)
  {
    case 13:
    case 1: EEN; break;      // "ONE"
    case 14:
    case 2: TWEE; break;     // "TWO"
    // ... continues for all hours
    case 0:
    case 12: TWAALF; break;  // "TWELVE"
  }

  // Display "O'CLOCK" only at certain minutes
  switch (timeinfo.tm_min)
  {
    case 59:
    case  0:
    case  1:
    case  2:
    case  3: UUR;  break;    // "O'CLOCK"
  }
}
```

### Logic Breakdown
1. **Always On**: "HET" (it) is always displayed
2. **Minutes**: Switch statement for each minute (0-59)
   - Determines if "IS" (present) or "WAS" (past) should show
   - Determines time expression (5 past, 10 past, quarter, half, etc.)
3. **Hour**: Adjusted based on minutes past
   - After 18 minutes, the NEXT hour is displayed
   - Example: 3:20 displays "TWENTY PAST FOUR" (not THREE)
4. **O'Clock**: Only shown when minutes are 0-3

### Key Concepts for Your Pomodoro
- **No Display Output**: Completely separated from rendering
- **Pure State Machine**: Only calls ColorLeds() macros based on current time
- **Flexible Text Composition**: Multiple words can be displayed together
- **Language Independent**: Same pattern works for English, German, French, etc.

### Implementation Strategy for Pomodoro
You need TWO functions:
1. **TimeToWords()** - Converts a time duration to English words (e.g., 24m 35s → "TWENTY FOUR MINUTES THIRTY FIVE SECONDS")
2. **UpdateDisplay()** - Calls ColorLeds() macros to show the words

---

## 4. DISPLAYTIME() FUNCTION - ORCHESTRATION

Location: Lines 1190-1222

### Purpose
Orchestrates the entire display update. It's the PUBLIC API for showing time.

```cpp
void Displaytime(void)
{
  // Skip if not in word clock mode (digital/analog modes handled elsewhere)
  if (Mem.DisplayChoice == DIGITAL) return;
  if (Mem.DisplayChoice == ANALOOG) return;

  byte Language;
  SetSecondColour();  // Update colors based on display choice

  // Select language
  if(Mem.LanguageChoice == 4)
    Language = random(4);  // Random language each minute
  else
    Language = Mem.LanguageChoice;

  // Clear all LEDs first
  LedsOff();  // Zeroes out Strippos array

  // Call appropriate language function
  switch(Language)
  {
    case 0: Dutch();    break;
    case 1: English();  break;
    case 2: German();   break;
    case 3: French();   break;
  }
}
```

### Execution Flow
1. **Check Display Mode**: Returns early if not word clock mode
2. **Set Colors**: Calls `SetSecondColour()` to determine colors based on user preference
3. **Select Language**: Picks which language function to call
4. **Clear Display**: Calls `LedsOff()` to empty Strippos array
5. **Populate Display**: Calls language function (Dutch/English/German/French) which fills Strippos via ColorLeds()
6. **Rendering**: ShowCharsILI9341() is called separately (from main loop)

### For Your Pomodoro
Structure it similarly:

```cpp
void DisplayPomodoro(void)
{
  // Only show in word mode (not digital/progress bar)
  if (timerMode != MODE_WORD_DISPLAY) return;

  SetTimerColors();      // Set colors based on session type
  LedsOff();             // Clear display

  if (timerRunning)
  {
    // Show countdown in words
    SecondsToWords(secondsRemaining);  // Calls ColorLeds() macros
  }
  else
  {
    // Show word clock when not running
    Dutch();             // Reuse the existing function!
  }

  // Rendering happens separately in main loop via ShowCharsILI9341()
}
```

---

## 5. DISPLAY RENDERING FUNCTIONS

### 5.1 ClearScreenILI9341()
Location: Lines 2010-2013

```cpp
void ClearScreenILI9341(void)
{
  tft.fillScreen(ConvertRGB32toRGB16(Mem.BackGround));
}
```

**Purpose**: Clears the entire display to the background color
**Use**: Called once during setup, then ShowCharsILI9341() handles updates

---

### 5.2 ShowCharsILI9341()
Location: Lines 2019-2029

**This is the core rendering function. It reads the Strippos[] array and draws it on screen.**

```cpp
void ShowCharsILI9341(void)
{
  int LEDnr = 0;

  // Iterate through 16x8 character grid
  for(int y = 0; y < MATRIX_HEIGHT; y++)
    for(int x = 0; x < MATRIX_WIDTH; x++, LEDnr++)
    {
      // Draw one character at position (20*x, 20+27*y)
      tft.drawChar(
        20 * x,                                    // X position (0, 20, 40, 60... 300)
        20 + 27 * y,                               // Y position (20, 47, 74, 101...)
        Strippos[LEDnr].Character,                 // The character from our array
        ConvertRGB32toRGB16(Strippos[LEDnr].RGBColor),    // Foreground color
        ConvertRGB32toRGB16(Mem.BackGround), 2);   // Background color, text size=2
    }
}
```

### Key Details
- **Grid Layout**: 16 columns × 8 rows = 128 positions
- **Character Position Formula**:
  - X = 20 × column (0, 20, 40, 60, ... 300)
  - Y = 20 + 27 × row (20, 47, 74, 101, ...)
- **Text Size**: 2 (medium, approximately 20 pixels wide per character)
- **Color Conversion**: RGB32→RGB16 (32-bit to 16-bit color)
- **Called Every Second**: From EverySecondCheck() → EveryMinuteUpdate()

### Color Conversion Function
Location: Lines 1999-2005

```cpp
uint16_t ConvertRGB32toRGB16(uint32_t sourceColor)
{
  uint32_t red   = (sourceColor & 0x00FF0000) >> 16;
  uint32_t green = (sourceColor & 0x0000FF00) >> 8;
  uint32_t blue  =  sourceColor & 0x000000FF;

  // Convert from 8-bit (256 values) to RGB565 format
  // Red: 5 bits,   Green: 6 bits,   Blue: 5 bits
  return (red >> 3 << 11) + (green >> 2 << 5) + (blue >> 3);
}
```

**Important**: Always use this function when passing custom 32-bit colors to Adafruit_ILI9341 functions.

---

### 5.3 InitScreenILI9341()
Location: Lines 1985-1995

```cpp
void InitScreenILI9341(void)
{
  tft.begin();              // Initialize Adafruit display
  SetBackLight(0XA0);       // Turn on backlight (0-255, 0xA0 = ~160)
  tft.setRotation(1);       // Rotate display 90 degrees
  tft.setTextColor(C_BLACK, ConvertRGB32toRGB16(C_ORANGE));
  tft.setCursor(0, tft.height()/3);
  tft.setTextSize(2);
  tft.println("Starting");
}
```

**Setup**: Call once in Arduino setup()

---

### 5.4 SetBackLight()
Location: Lines 1913-1919

```cpp
void SetBackLight(int intensity)
{
  analogWrite(TFT_LED, intensity);
  ledcWrite(0, intensity);    // PWM on channel 0
}
```

**Purpose**: Control display brightness (0-255)
**Note**: Uses LEDC PWM (compatible with ESP32 3.0.0+ board package)

---

## 6. MAIN LOOP AND TIMING ARCHITECTURE

### 6.1 Main Loop()
Location: Lines 480-485

```cpp
void loop()
{
  Loopcounter++;
  EverySecondCheck();        // All timing logic happens here
  CheckDevices();            // BLE, Serial, Touch input
}
```

**Simple structure**: Calls `EverySecondCheck()` continuously until 1 second has passed

---

### 6.2 EverySecondCheck()
Location: Lines 497-514

**This is where all timing and display updates happen.**

```cpp
void EverySecondCheck(void)
{
  static bool Toggle = 1;
  uint32_t msLeap;

  // Only enter this block once per second
  msLeap = millis() - msTick;
  if(msLeap >999)                    // Wait ~1 second
  {
    msTick = millis();
    GetTijd(0);                      // Update timeinfo struct from NTP/RTC
    Toggle = !Toggle;

    // Update LED status indicator
    if (Mem.StatusLEDOnOff)
      SetStatusLED(0,0,Toggle,Toggle*WIFIConnected,Toggle*deviceConnected);

    // Update digital/analog display (if applicable)
    if(!LEDsAreOff) ScreenSecondProcess();

    // Adjust brightness from LDR sensor
    DimLeds(TestLDR);

    // Check if minute has changed
    if (timeinfo.tm_min != lastminute)
      EveryMinuteUpdate();           // Enter once per minute

    Loopcounter=0;
  }
}
```

### Execution Frequency
- **Main loop**: Runs many times per second
- **EverySecondCheck**: Enters once per second (at 1000ms boundary)
- **EveryMinuteUpdate**: Enters when minute changes
- **EveryHourUpdate**: Enters when hour changes

---

### 6.3 EveryMinuteUpdate()
Location: Lines 518-526

```cpp
void EveryMinuteUpdate(void)
{
  lastminute = timeinfo.tm_min;
  SetStatusLED(0,0,0,0,0);           // Turn off status LED

  if(!LEDsAreOff)
    Displaytime();                   // Regenerate word display

  if(timeinfo.tm_hour != lasthour)
    EveryHourUpdate();               // Check for hour change
}
```

**Key Point**: `Displaytime()` is called HERE, which populates Strippos array.
Then `ShowCharsILI9341()` renders it on screen.

---

### 6.4 GetTijd()
Location: Lines 1226-1236

```cpp
void GetTijd(byte printit)
{
  if(Mem.NTPOnOff)
    getLocalTime(&timeinfo);         // Get time from NTP (if enabled)
  else
  {
    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);    // Get time from RTC
  }

  if (printit)
    Print_RTC_tijd();
}
```

**Updates the global `struct tm timeinfo`** with current time values:
- `timeinfo.tm_hour` (0-23)
- `timeinfo.tm_min` (0-59)
- `timeinfo.tm_sec` (0-59)
- `timeinfo.tm_mday` (1-31)
- `timeinfo.tm_mon` (0-11)
- `timeinfo.tm_year` (years since 1900)

---

## 7. COLOR DEFINITIONS

File: `/Users/pedrofer/GitHub/ESP32-2432S028R-text-pomodoro/ESP322432S028_WordClockV016/Colors.h`

### Color Format
All colors are 32-bit RGB: `0xRRGGBB`

### Essential Colors for Pomodoro
```cpp
#define C_RED             0xFF0000   // Work session (red tomato)
#define C_GREEN           0x00FF00   // Short break
#define C_BLUE            0x0000FF   // Long break
#define C_YELLOW          0xFFFF00   // Word clock display
#define C_ORANGE          0xFFA500   // Alternative
#define C_CYAN            0x00FFFF   // Light break color
#define C_WHITE           0xFFFFFF   // Paused/neutral
#define C_BLACK           0x000000   // Background
#define C_GRAY            0x808080   // Disabled/off
#define C_ORANGE_RED      0xFF4500   // Alert color
#define C_SPRING_GREEN    0x00FF7F   // Completion
#define C_GOLD            0xFFD700   // Highlight
```

---

## 8. HELPER FUNCTIONS

### LedsOff()
Location: Lines 1118-1122

```cpp
void LedsOff(void)
{
  for (int n=0; n<NUM_LEDS; n++)
    Strippos[n].Character = Strippos[n].RGBColor = 0;
}
```

**Purpose**: Clear all characters and colors from display (prepare for new content)

---

### SetSecondColour()
Location: Lines 1128-1148

```cpp
void SetSecondColour(void)
{
  switch (Mem.DisplayChoice)
  {
    case DEFAULTCOLOUR:
      LetterColor = C_YELLOW;
      MINColor = C_YELLOW;
      SECColor = C_YELLOW;
      break;

    case HOURLYCOLOUR:
      LetterColor = MINColor = SECColor = HourColor[timeinfo.tm_hour];
      break;

    case WHITECOLOR:
      LetterColor = MINColor = SECColor = C_WHITE;
      break;

    case OWNCOLOUR:
      LetterColor = MINColor = SECColor = Mem.OwnColour;
      break;

    case WHEELCOLOR:
      LetterColor = MINColor = SECColor = (17*(timeinfo.tm_min*60));
      break;
  }
}
```

**Purpose**: Updates global color variables based on user's color choice preference

---

### SetStatusLED()
Location: Lines 1025-1047

```cpp
void SetStatusLED(int WW, int CW, int Re, int Gr, int Bl)
{
  #ifdef ESP2432S028
    // For CYD: RGB LED is active LOW (0=ON, 255=OFF)
    if(Re >0 ) digitalWrite(REDPIN,   LOW);  else digitalWrite(REDPIN,   HIGH);
    if(Gr >0 ) digitalWrite(GREENPIN, LOW);  else digitalWrite(GREENPIN, HIGH);
    if(Bl >0 ) digitalWrite(BLUEPIN,  LOW);  else digitalWrite(BLUEPIN,  HIGH);
  #endif
}
```

**Purpose**: Control RGB status LED
**Important**: Uses binary GPIO control (NOT PWM) - LED is either ON (LOW) or OFF (HIGH)
**Use**: Call with 1 for ON, 0 for OFF: `SetStatusLED(0, 0, 255, 0, 0)` = RED ON

---

## 9. ARCHITECTURE DIAGRAM

```
Main Loop (runs continuously)
    │
    ├─→ EverySecondCheck() [once per second via millis()]
    │    │
    │    ├─→ GetTijd()              [Update timeinfo from NTP/RTC]
    │    │
    │    ├─→ SetStatusLED()         [Toggle status LED]
    │    │
    │    └─→ EveryMinuteUpdate() [when minute changes]
    │         │
    │         ├─→ Displaytime()     [Populate Strippos[] with text]
    │         │    │
    │         │    ├─→ LedsOff()    [Clear Strippos array]
    │         │    │
    │         │    ├─→ SetSecondColour()  [Set global color vars]
    │         │    │
    │         │    └─→ Dutch()      [Determine which words to show]
    │         │         │
    │         │         └─→ ColorLeds("word", start, end, color)
    │         │              [Fill Strippos with characters]
    │         │
    │         └─→ EveryHourUpdate() [when hour changes]
    │
    └─→ CheckDevices()              [BLE, Serial, Touch input]

[Separate rendering happens in EveryMinuteUpdate]
    └─→ ShowCharsILI9341()          [Read Strippos[], draw to screen]
         │
         ├─→ ConvertRGB32toRGB16()  [Color conversion]
         │
         └─→ tft.drawChar()         [Adafruit graphics call]
```

---

## 10. IMPLEMENTATION GUIDE FOR POMODORO TIMER

### Step 1: Create Your Data Structure
```cpp
struct PomodoroSettings {
  uint16_t workMinutes;
  uint16_t shortBreakMinutes;
  uint16_t longBreakMinutes;
  uint32_t workColor;
  uint32_t breakColor;
  int checksum;
} pomodoroSettings;
```

### Step 2: Create Word Conversion Function
```cpp
// Convert seconds to words: 24m35s → "TWENTY FOUR MINUTES THIRTY FIVE SECONDS"
void SecondsToWords(uint32_t totalSeconds)
{
  uint16_t minutes = totalSeconds / 60;
  uint8_t seconds = totalSeconds % 60;

  // Use ColorLeds() to populate display with word representation
  // Example: MinutesToWords(minutes) + SecondsToWords(seconds)
}
```

### Step 3: Create Display Orchestration Function
```cpp
void DisplayPomodoroTimer(void)
{
  LedsOff();  // Clear display

  if (timerRunning)
  {
    // Show countdown in words
    SetTimerColors();  // Red for work, green for break
    SecondsToWords(secondsRemaining);
  }
  else
  {
    // Show word clock when idle
    SetSecondColour();
    Dutch();  // Reuse existing function!
  }
}
```

### Step 4: Integrate into Main Loop
```cpp
void EveryMinuteUpdate(void)
{
  lastminute = timeinfo.tm_min;

  if(!LEDsAreOff)
  {
    if (pomodoroEnabled)
      DisplayPomodoroTimer();  // NEW: Your function
    else
      Displaytime();           // ORIGINAL: Word clock
  }

  if(timeinfo.tm_hour != lasthour)
    EveryHourUpdate();
}
```

### Step 5: Reusable Code You Can Copy
- **ColorLeds()** - For displaying text with colors
- **ShowCharsILI9341()** - For rendering Strippos[] to screen
- **LedsOff()** - For clearing the display
- **GetTijd()** - For getting current time
- **ConvertRGB32toRGB16()** - For color conversion
- **SetBackLight()** - For brightness control
- **Dutch()** - Model for your own state machine

---

## 11. PIN CONFIGURATION REFERENCE

From CLAUDE.md:
```cpp
// ILI9341 Display (SPI Interface)
TFT_MISO    = GPIO 12
TFT_MOSI    = GPIO 13
TFT_SCK     = GPIO 14
TFT_CS      = GPIO 15
TFT_DC      = GPIO 2
TFT_RESET   = -1         // Not used
TFT_LED     = GPIO 21    // Backlight PWM control

// RGB Status LED
REDPIN      = GPIO 4     // Active LOW
GREENPIN    = GPIO 16    // Active LOW
BLUEPIN     = GPIO 17    // Active LOW

// Future enhancements
SPEAKER_PIN = GPIO 27    // PWM buzzer
```

---

## 12. CRITICAL IMPLEMENTATION NOTES

### For Word-Based Countdown
1. **Create Number-to-Word Mappings**:
   ```cpp
   char* NumberWords[] = {
     "", "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT",
     "NINE", "TEN", "ELEVEN", "TWELVE", "THIRTEEN", ...
   };
   ```

2. **Handle Special Cases**:
   - 1 minute = "ONE MINUTE" (singular)
   - 2+ minutes = "X MINUTES" (plural)
   - Same for seconds

3. **Position Management**:
   - Reserve positions 0-7 for minutes word
   - Reserve positions 11-15 for number
   - Reserve positions 20-25 for "MINUTES"
   - Similar pattern for seconds

4. **Color Management**:
   - Work sessions: Red (0xFF0000)
   - Short breaks: Green (0x00FF00)
   - Long breaks: Blue (0x0000FF)
   - Paused: Yellow (0xFFFF00)

### Performance Considerations
- **Strippos Update**: Only happens once per minute in `EveryMinuteUpdate()`
- **Screen Redraw**: `ShowCharsILI9341()` is called ~60 times per minute (when seconds change)
- **ColorLeds Calls**: Minimal overhead - just array assignments

### Memory Usage
- `Strippos[] array`: 128 × (1 byte char + 4 bytes color) = 640 bytes
- `EEPROMstorage struct`: ~400 bytes
- Total: < 2KB (easily fits in ESP32 RAM)

---

## 13. TESTING CHECKLIST

- [ ] Display initializes correctly
- [ ] ColorLeds() populates Strippos array correctly
- [ ] ShowCharsILI9341() renders characters to screen
- [ ] Colors display correctly (use ConvertRGB32toRGB16)
- [ ] LedsOff() clears display
- [ ] Text updates every minute
- [ ] RGB LED responds to SetStatusLED()
- [ ] Backlight brightness controlled by SetBackLight()
- [ ] Word clock displays when timer not running
- [ ] Countdown displays in words when timer running
- [ ] Color changes based on session type (work/break)

---

## SUMMARY

The Word Clock architecture separates concerns beautifully:

1. **Data Structure** (Strippos[]): Holds characters and colors for a 16×8 grid
2. **State Machine** (Dutch()): Determines which words to display
3. **Rendering** (ShowCharsILI9341()): Draws the populated grid to screen
4. **Timing** (EverySecondCheck/EveryMinuteUpdate): Controls when updates happen

Your Pomodoro timer should follow the exact same pattern:
- Create `PomodoroToWords()` function (like Dutch())
- Call `ColorLeds()` macros to populate Strippos[]
- Reuse `ShowCharsILI9341()` to render
- Integrate into existing timing loop

This ensures your code is modular, testable, and maintainable.
