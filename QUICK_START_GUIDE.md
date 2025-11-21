# Pomodoro Timer - Quick Start Integration Guide

## Overview

You now have a complete understanding of the Word Clock V016 architecture and how to build a word-based Pomodoro timer. This guide provides the fastest path from concept to working code.

---

## Files Created for Reference

### 1. **WORDCLOCK_ANALYSIS.md** (25 KB)
Complete technical breakdown of the Word Clock V016 architecture:
- EEPROMstorage struct definition
- ColorLeds() function deep dive
- Dutch() state machine implementation
- Display rendering pipeline (ShowCharsILI9341)
- Main loop and timing architecture
- All helper functions documented
- **Use this for**: Understanding how existing code works

### 2. **POMODORO_IMPLEMENTATION_EXAMPLES.h** (13 KB)
Ready-to-use code snippets:
- Number-to-word conversion functions
- Display macros for your custom layout
- Core Pomodoro display function
- Timer control functions (start, pause, resume, reset)
- Serial command handlers
- Touch interface example
- Color themes
- **Use this for**: Copy-paste template code

### 3. **DISPLAY_GRID_REFERENCE.md** (11 KB)
Visual reference for the 16×8 character grid:
- Physical pixel-to-character mapping
- Recommended layout templates
- Strippos array index reference
- Position calculation formulas
- Touch zone definitions
- Common mistakes to avoid
- **Use this for**: Planning your display layout

---

## Step-by-Step Integration

### STEP 1: Choose Your Display Layout

Open **DISPLAY_GRID_REFERENCE.md** and pick a layout:

**Recommended: Option 3 (Dual Stack)**
```
Row 0: M I N U T E S         S E C O N D S
Row 1: [minutes number]      [seconds number]
Row 2: [empty]
Row 3: WORK / BREAK SESSION
Row 4: Session 2/4  Today: 8
```

**Or use the simpler centered layout if you prefer.**

### STEP 2: Define Your Position Constants

Add this to your main sketch or a new header file:

```cpp
// Display positions for pomodoro timer
#define POS_MIN_LABEL_START     0
#define POS_MIN_LABEL_END       7
#define POS_MIN_NUM_START       9
#define POS_MIN_NUM_END        14

#define POS_SEC_LABEL_START    16
#define POS_SEC_LABEL_END      23
#define POS_SEC_NUM_START      25
#define POS_SEC_NUM_END        30

#define POS_SESSION_START      48
#define POS_SESSION_END        63
```

**How to calculate**: Use DISPLAY_GRID_REFERENCE.md formula:
```
linearPosition = (row * 16) + column
```

### STEP 3: Copy Core Functions

From **POMODORO_IMPLEMENTATION_EXAMPLES.h**, copy these functions into your sketch:

1. **NumToWords()** - Converts numbers to English words
2. **DisplayCountdownInWords()** - Main display function
3. **PomodoroState enum** and global variables
4. **Timer control functions** (StartWorkSession, PauseTimer, etc.)
5. **UpdatePomodoroTimer()** - Called every second

### STEP 4: Create Your Display Function

Adapt DisplayCountdownInWords() to your chosen layout:

```cpp
void DisplayPomodoroTimer(void)
{
  char minutesStr[32];
  char secondsStr[32];

  uint16_t minutes = secondsRemaining / 60;
  uint8_t seconds = secondsRemaining % 60;

  NumToWords(minutes, minutesStr);
  NumToWords(seconds, secondsStr);

  LedsOff();

  // Set colors based on session type
  uint32_t numColor = (currentState == STATE_WORK) ? C_RED : C_GREEN;
  uint32_t labelColor = C_YELLOW;

  // Display using YOUR position constants
  ColorLeds("MINUTES", POS_MIN_LABEL_START, POS_MIN_LABEL_END, labelColor);
  ColorLeds(minutesStr, POS_MIN_NUM_START, POS_MIN_NUM_END, numColor);

  ColorLeds("SECONDS", POS_SEC_LABEL_START, POS_SEC_LABEL_END, labelColor);
  ColorLeds(secondsStr, POS_SEC_NUM_START, POS_SEC_NUM_END, numColor);

  ColorLeds(sessionStr, POS_SESSION_START, POS_SESSION_END, labelColor);
}
```

### STEP 5: Integrate into Existing Loop

**BEFORE** (in EveryMinuteUpdate()):
```cpp
void EveryMinuteUpdate(void)
{
  lastminute = timeinfo.tm_min;
  SetStatusLED(0,0,0,0,0);

  if(!LEDsAreOff)
    Displaytime();  // ← Original word clock

  if(timeinfo.tm_hour != lasthour)
    EveryHourUpdate();
}
```

**AFTER**:
```cpp
void EveryMinuteUpdate(void)
{
  lastminute = timeinfo.tm_min;
  SetStatusLED(0,0,0,0,0);

  if(!LEDsAreOff)
  {
    if (pomodoroMode)
      DisplayPomodoroTimer();  // ← NEW: Your function
    else
      Displaytime();           // ← Keep original for word clock mode
  }

  if(timeinfo.tm_hour != lasthour)
    EveryHourUpdate();
}
```

### STEP 6: Add Timer Update to EverySecondCheck()

Add this line to your EverySecondCheck() function:

```cpp
void EverySecondCheck(void)
{
  static bool Toggle = 1;
  uint32_t msLeap;

  msLeap = millis() - msTick;
  if(msLeap > 999)
  {
    msTick = millis();
    GetTijd(0);

    // ... existing code ...

    UpdatePomodoroTimer();  // ← ADD THIS LINE

    if (timeinfo.tm_min != lastminute)
      EveryMinuteUpdate();

    Loopcounter=0;
  }
}
```

### STEP 7: Add Control Commands

Add to your serial/BLE command handler:

```cpp
void ProcessCommand(String cmd)
{
  // ... existing commands ...

  if (cmd == "PS")      // Pomodoro Start
  {
    pomodoroMode = true;
    StartWorkSession();
  }
  else if (cmd == "PP")  // Pomodoro Pause
    PauseTimer();
  else if (cmd == "PR")  // Pomodoro Resume
    ResumeTimer();
  else if (cmd == "PX")  // Pomodoro Stop
  {
    ResetTimer();
    pomodoroMode = false;
  }
}
```

---

## Testing Checklist

Use this to verify everything works:

- [ ] Sketch compiles without errors
- [ ] Board uploads successfully
- [ ] Serial monitor shows startup messages
- [ ] Send command "PS" via serial → timer starts
- [ ] Display shows time in words (e.g., "TWENTY FOUR MINUTES")
- [ ] Color is red (work session)
- [ ] Status LED is red (work session)
- [ ] Timer counts down every second
- [ ] After 25 seconds, display updates
- [ ] Send "PP" → timer pauses
- [ ] Display still shows time, color changes to yellow
- [ ] Send "PR" → timer resumes
- [ ] Send "PX" → timer stops, returns to word clock
- [ ] Touch to pause/resume (if implemented)
- [ ] Upon completion: RGB LED pulses, sound plays (if enabled)

---

## Key Code Snippets You'll Reuse

### From Word Clock (Already Exists)

```cpp
// CORE - YOU MUST USE THESE
void ColorLeds(char const *Text, int FirstLed, int LastLed, uint32_t RGBColor)
  → Adds words to display

void LedsOff(void)
  → Clears display

void ShowCharsILI9341(void)
  → Renders Strippos[] to screen

void GetTijd(byte printit)
  → Updates timeinfo struct

void SetBackLight(int intensity)
  → Controls brightness

void SetStatusLED(int WW, int CW, int Re, int Gr, int Bl)
  → Controls RGB LED

// LOOP STRUCTURE
void loop()
  → Already calls EverySecondCheck()

void EverySecondCheck(void)
  → Already handles 1-second intervals

void EveryMinuteUpdate(void)
  → Where you call DisplayPomodoroTimer()
```

### You Need to Create

```cpp
enum PomodoroState { STATE_IDLE, STATE_WORK, ... }
  → Define your states

void NumToWords(int num, char* output)
  → Convert 24 to "TWENTY FOUR"

void DisplayPomodoroTimer(void)
  → Main display function

void UpdatePomodoroTimer(void)
  → Decrement timer each second

void StartWorkSession(void)
  → Initialize work period

void StartBreakSession(bool isLong)
  → Initialize break period

void PauseTimer(void)
  → Pause current session

void ResumeTimer(void)
  → Resume current session
```

---

## Common Customizations

### Change Colors
```cpp
// Work session colors
#define WORK_COLOR    0xFF0000  // Red
#define WORK_LABEL    0xFF4500  // Orange-red

// Break session colors
#define BREAK_COLOR   0x00FF00  // Green
#define BREAK_LABEL   0x00CED1  // Turquoise

// In DisplayPomodoroTimer():
uint32_t numColor = (currentState == STATE_WORK) ? WORK_COLOR : BREAK_COLOR;
```

### Change Timer Durations
```cpp
// Default values
uint32_t workDuration = 25 * 60;        // 25 minutes
uint32_t shortBreakDuration = 5 * 60;   // 5 minutes
uint32_t longBreakDuration = 15 * 60;   // 15 minutes

// Change via serial command:
// "W30" → 30 minute work session
// "B10" → 10 minute break
// "L20" → 20 minute long break
```

### Change Display Text
```cpp
// Instead of "MINUTES" use "MINUTES REMAINING"
ColorLeds("MINUTES REMAINING", POS_MIN_LABEL_START, POS_MIN_LABEL_END, labelColor);

// Or use shorter text to save space
ColorLeds("MIN", POS_MIN_LABEL_START, POS_MIN_LABEL_END, labelColor);
```

### Add Progress Bar
```cpp
// After ColorLeds calls in DisplayPomodoroTimer():
uint16_t progressPercent = (secondsRemaining * 100) / workDuration;
uint8_t filledChars = (progressPercent * 16) / 100;

// Display filled positions in a progress row
for (int i = 0; i < filledChars; i++)
{
  Strippos[96 + i].Character = '█';  // Full block
  Strippos[96 + i].RGBColor = numColor;
}
for (int i = filledChars; i < 16; i++)
{
  Strippos[96 + i].Character = '░';  // Light block
  Strippos[96 + i].RGBColor = 0x404040;  // Dark gray
}
```

### Play Sound on Completion
```cpp
void PlayCompletionAlert(void)
{
  // Flash LED
  for (int i = 0; i < 3; i++)
  {
    SetStatusLED(0, 0, 255, 255, 0);  // Yellow
    delay(200);
    SetStatusLED(0, 0, 0, 0, 0);
    delay(200);
  }

  // Play tone (if SPEAKER_PIN defined)
  #ifdef SPEAKER_PIN
    tone(SPEAKER_PIN, 1000, 200);
    delay(300);
    tone(SPEAKER_PIN, 1500, 200);
    delay(300);
    tone(SPEAKER_PIN, 2000, 400);
  #endif
}
```

---

## Architecture Summary

Your Pomodoro timer follows the exact same pattern as the Word Clock:

```
Timer State (in UpdatePomodoroTimer)
    ↓
DisplayPomodoroTimer() [called once per minute]
    ├→ LedsOff()              [Clear display]
    ├→ NumToWords()           [Convert 24 → "TWENTY FOUR"]
    ├→ ColorLeds() calls      [Populate Strippos[]]
    └→ [Strippos[] now contains "TWENTY FOUR MINUTES", etc]
    ↓
ShowCharsILI9341() [called every second in main loop]
    ├→ Loop through Strippos[]
    ├→ ConvertRGB32toRGB16()  [Color conversion]
    └→ tft.drawChar() [Render to screen]
```

**Key Insight**: DisplayPomodoroTimer() only runs when the MINUTE changes. ShowCharsILI9341() runs every SECOND. This separation of "what to display" from "when to draw" is the elegant architecture of the Word Clock.

---

## Troubleshooting

### Display Shows Garbage Characters
- Check ColorLeds() position ranges don't exceed 127
- Verify NumToWords() produces valid strings
- Add Serial debug output to see what's being added

### Colors Wrong
- Verify 32-bit RGB format: 0xRRGGBB
- Remember ConvertRGB32toRGB16() is called in ShowCharsILI9341()
- Check Mem.BackGround is set correctly

### Timer Doesn't Count Down
- Verify UpdatePomodoroTimer() is called in EverySecondCheck()
- Check pomodoroMode is set to true
- Add Serial output: `sprintf(sptext, "Time: %lu", secondsRemaining); Tekstprintln(sptext);`

### Text Overlaps
- Recalculate position ranges using DISPLAY_GRID_REFERENCE.md formula
- Make sure end position doesn't exceed text length
- Leave gaps between words

### Memory Issues
- Strippos[] is 640 bytes (fixed)
- NumToWords() uses stack temporarily (harmless)
- If out of memory, reduce number of ColorLeds() calls per update

---

## Performance Notes

**Display Update Frequency**:
- ShowCharsILI9341() called: ~60 times per minute (at each second)
- DisplayPomodoroTimer() called: 1 time per minute
- ColorLeds() calls per update: 4-6 typical
- Total per minute: < 100ms processing time

**Memory Usage**:
- Strippos[] array: 640 bytes
- Global variables: < 100 bytes
- Stack (during NumToWords): < 100 bytes
- **Total: < 1 KB** (out of 520 KB available)

**Power Consumption**:
- No impact compared to Word Clock
- Same display refresh rate
- Same LED/backlight control
- Same WiFi/BLE if enabled

---

## Next Steps

1. **Test the template code** from POMODORO_IMPLEMENTATION_EXAMPLES.h
2. **Plan your layout** using DISPLAY_GRID_REFERENCE.md
3. **Integrate step-by-step** following the 7-step guide above
4. **Test each feature** using the checklist
5. **Customize colors/durations** to your preference
6. **Add touch zones** (optional) for start/pause/skip
7. **Add sound/vibration** (optional) on session completion

---

## Resources Available

| Document | Purpose | Size | Read If... |
|----------|---------|------|-----------|
| WORDCLOCK_ANALYSIS.md | Complete architecture explanation | 25 KB | You want to understand the design |
| POMODORO_IMPLEMENTATION_EXAMPLES.h | Ready-to-use code | 13 KB | You're ready to start coding |
| DISPLAY_GRID_REFERENCE.md | Visual grid and positioning | 11 KB | You're designing your layout |
| QUICK_START_GUIDE.md | This file | 8 KB | You want step-by-step instructions |

---

## Final Notes

- **You don't need to reinvent anything.** The Word Clock already handles display, timing, colors, and connectivity.
- **Your job is simple:** Convert time to words, call ColorLeds(), integrate into the existing loop.
- **The hardest part is done.** Ed Nieuw's Word Clock architecture is proven and stable.
- **Stay modular.** Keep your Pomodoro code separate from Word Clock code so either can work independently.
- **Test early.** Get DisplayPomodoroTimer() working before adding fancy features.

Good luck with your Pomodoro timer!
