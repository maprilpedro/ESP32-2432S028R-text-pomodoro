# Pomodoro Timer Documentation Index

Complete reference for understanding and implementing the word-based Pomodoro timer on the ESP32-2432S028R.

---

## Quick Navigation

### For Different Audiences

**If you want to...**

1. **Understand the architecture**
   → Read: `WORDCLOCK_ANALYSIS.md` (25 KB)
   - Deep dive into every function
   - Data structure explanations
   - Complete loop flow
   - All helper functions documented

2. **Start coding immediately**
   → Read: `QUICK_START_GUIDE.md` (8 KB) + `POMODORO_IMPLEMENTATION_EXAMPLES.h` (13 KB)
   - 7-step integration walkthrough
   - Copy-paste ready code
   - Testing checklist
   - Common customizations

3. **Plan your display layout**
   → Read: `DISPLAY_GRID_REFERENCE.md` (11 KB)
   - 16×8 character grid mapping
   - Pixel position calculations
   - Layout templates
   - Touch zone definitions

4. **See visual examples**
   → Read: `EXAMPLE_DISPLAYS.md` (9 KB)
   - Real display outputs
   - Color schemes
   - State transitions
   - Serial output samples

5. **Get the big picture**
   → Read: `CLAUDE.md` (project overview)
   - Hardware specifications
   - System architecture
   - Feature list
   - Development roadmap

---

## Document Descriptions

### WORDCLOCK_ANALYSIS.md (25 KB)
**Complete technical reference of Word Clock V016 architecture**

**Contains:**
- EEPROMstorage struct (lines 361-394)
- LEDPrintBuffer struct and 16×8 grid (lines 314-328)
- ColorLeds() deep dive (lines 1079-1094)
- Displaytime() orchestration (lines 1190-1222)
- Dutch() state machine (lines 1348-1457)
- Display rendering (ClearScreenILI9341, ShowCharsILI9341)
- Loop architecture (loop, EverySecondCheck, EveryMinuteUpdate)
- Helper functions (LedsOff, SetStatusLED, GetTijd)
- Color system and conversion
- LED strip management

**Use when:**
- You want deep understanding of the design
- You need to debug something specific
- You want to see complete code examples
- You're learning from the Word Clock project

**Key sections:**
- Section 1: Structure definitions
- Section 2: ColorLeds() function (core mechanism)
- Section 3: Dutch() state machine (model for your function)
- Section 4: Display rendering pipeline
- Section 5: Main loop architecture
- Section 9: Architecture diagram

---

### POMODORO_IMPLEMENTATION_EXAMPLES.h (13 KB)
**Ready-to-use code templates and snippets**

**Contains:**
- NumToWords() function (converts 24 → "TWENTY FOUR")
- Display macros and position constants
- DisplayCountdownInWords() main function
- PomodoroState enum and globals
- Timer control functions (start, pause, resume, reset)
- Integration into existing loop
- Serial command handlers
- Touch interface example
- Color theme templates
- Completion alert function

**Use when:**
- You're ready to write code
- You want copy-paste templates
- You need example implementations
- You're adding new features

**Examples include:**
- Example 1: Number-to-word conversion
- Example 2: Display macros
- Example 3: Core display function
- Example 4: Display orchestrator
- Example 5: Loop integration
- Example 6: Timer control functions
- Example 7: Serial commands
- Example 8: Touch interface
- Example 9: Complete second-by-second update
- Example 10: Color themes

---

### DISPLAY_GRID_REFERENCE.md (11 KB)
**Visual reference for the 16×8 character display grid**

**Contains:**
- Physical display layout (320×240 pixels)
- Character-to-pixel mapping formulas
- 16×8 grid visualization
- Position calculation examples
- 3 recommended layout templates
- Strippos array index reference
- Position assignment examples
- Color mapping for states
- Touch zone definitions
- Code templates for custom layouts
- Pixel-perfect verification method
- Common mistakes list

**Use when:**
- Designing your display layout
- Calculating character positions
- Debugging rendering issues
- Planning touch interface zones
- Verifying grid alignment

**Key formulas:**
```
linearPosition = (row * 16) + column
X pixel = column * 20
Y pixel = 20 + row * 27
```

---

### EXAMPLE_DISPLAYS.md (9 KB)
**Visual examples of actual display outputs**

**Contains:**
- 8 complete display examples with ASCII art
- Example 1: Work session 24:35 (RED)
- Example 2: Short break 4:12 (GREEN)
- Example 3: Long break 14:47 (BLUE)
- Example 4: Paused timer (YELLOW)
- Example 5: Idle/ready state
- Example 6: Word clock mode
- Example 7: Session completion animation
- Example 8: Progress bar (optional)
- Number-to-word conversion table
- Color reference table with hex codes
- Touchscreen interaction examples
- Serial terminal output samples
- State transition diagram
- Display state matrix

**Use when:**
- You want to visualize the final result
- You need color hex codes
- You want example serial output
- You're testing state transitions
- You need to verify layout looks right

**Includes:**
- ASCII grid representations
- Code that produces each display
- Visual properties (colors, LEDs)
- Complete state examples
- Debug output samples

---

### QUICK_START_GUIDE.md (8 KB)
**Step-by-step integration instructions**

**Contains:**
- 7-step integration walkthrough
- Position constant definitions
- Function copy-paste checklist
- Display function template
- Loop integration example
- Testing checklist (16 items)
- Key code snippets summary
- Common customizations
- Troubleshooting guide
- Performance notes
- Next steps

**Use when:**
- You're ready to integrate into your sketch
- You need step-by-step instructions
- You want a testing checklist
- You're debugging integration issues

**Sections:**
1. Choose display layout
2. Define position constants
3. Copy core functions
4. Create display function
5. Integrate into loop
6. Add timer update
7. Add control commands

---

### CLAUDE.md (Project Overview)
**High-level project documentation and specifications**

**Contains:**
- Project overview
- Hardware platform (ESP32-2432S028R specs)
- Pin configuration
- Software architecture
- Display modes (4 types)
- Command menu system
- Statistics tracking
- File structure
- Development roadmap
- Known issues

**Use when:**
- You need hardware specifications
- You want the big picture
- You're setting up the board
- You need GPIO pin assignments

---

## File Locations

All documentation is in the project root:

```
/Users/pedrofer/GitHub/ESP32-2432S028R-text-pomodoro/
├── DOCUMENTATION_INDEX.md              ← You are here
├── WORDCLOCK_ANALYSIS.md               ← Deep technical reference
├── QUICK_START_GUIDE.md                ← Step-by-step integration
├── POMODORO_IMPLEMENTATION_EXAMPLES.h  ← Code templates
├── DISPLAY_GRID_REFERENCE.md           ← Layout planning
├── EXAMPLE_DISPLAYS.md                 ← Visual examples
├── CLAUDE.md                           ← Project specifications
├── ESP322432S028_WordClockV016/
│   ├── ESP322432S028_WordClockV016.ino ← Word Clock source (3500 lines)
│   ├── Colors.h                        ← Color definitions
│   └── Webpage.h                       ← Web interface
└── ESP32_Pomodoro_Timer/
    └── [Your Pomodoro sketch will go here]
```

---

## Reading Path by Goal

### Goal: Understand How Word Clock Works
1. WORDCLOCK_ANALYSIS.md - Section 2 (ColorLeds)
2. WORDCLOCK_ANALYSIS.md - Section 3 (Dutch function)
3. WORDCLOCK_ANALYSIS.md - Section 6 (Main loop)
4. EXAMPLE_DISPLAYS.md - Example 6 (Word clock output)

**Time: 45 minutes**

---

### Goal: Implement Pomodoro Timer ASAP
1. QUICK_START_GUIDE.md - Steps 1-7
2. POMODORO_IMPLEMENTATION_EXAMPLES.h - Example 3
3. DISPLAY_GRID_REFERENCE.md - Layout section
4. Test using QUICK_START_GUIDE.md checklist

**Time: 2-3 hours**

---

### Goal: Create Custom Display Layout
1. DISPLAY_GRID_REFERENCE.md - Grid section
2. DISPLAY_GRID_REFERENCE.md - Layout templates
3. EXAMPLE_DISPLAYS.md - Examples 1-4
4. POMODORO_IMPLEMENTATION_EXAMPLES.h - Example 2
5. Use DebugDisplayPositions() to verify

**Time: 1 hour**

---

### Goal: Deep Understand All Components
1. WORDCLOCK_ANALYSIS.md - All sections
2. QUICK_START_GUIDE.md - Architecture summary
3. POMODORO_IMPLEMENTATION_EXAMPLES.h - All examples
4. DISPLAY_GRID_REFERENCE.md - Complete reference
5. EXAMPLE_DISPLAYS.md - All examples

**Time: 3-4 hours**

---

## Key Concepts Summary

### The Three-Part Architecture

```
1. STATE MACHINE (What to display)
   └─ Displaytime() → Dutch()
      └─ Calls ColorLeds() to populate Strippos[]

2. DATA STRUCTURE (How to store it)
   └─ Strippos[] array (128 characters + colors)

3. RENDERER (How to draw it)
   └─ ShowCharsILI9341()
      └─ Reads Strippos[], converts colors, draws characters
```

### Core Functions You'll Use

**From Word Clock (Already exist):**
- `ColorLeds(text, start, end, color)` - Add text to display
- `LedsOff()` - Clear display
- `ShowCharsILI9341()` - Render to screen
- `GetTijd(0)` - Get current time
- `SetStatusLED()` - Control RGB LED

**You'll create:**
- `NumToWords(num)` - Convert 24 to "TWENTY FOUR"
- `DisplayPomodoroTimer()` - Main orchestration
- `UpdatePomodoroTimer()` - Countdown logic
- Timer control functions (start, pause, etc.)

### Memory Footprint

```
Strippos[] array:        640 bytes (fixed, pre-allocated)
Global variables:        ~100 bytes
Stack during operation:  <100 bytes
Total:                   <1 KB (out of 520 KB available)
```

### Timing Hierarchy

```
Main loop() runs continuously
    ├─ EverySecondCheck() runs once per second
    │   └─ UpdatePomodoroTimer() [decrement timer]
    │   └─ EveryMinuteUpdate() runs when minute changes
    │       └─ DisplayPomodoroTimer() [populate Strippos[]]
    │
    └─ ShowCharsILI9341() runs every second [render to screen]
```

---

## Common Tasks

### Task: Change Timer Duration
See: QUICK_START_GUIDE.md - "Change Timer Durations"
Code location: POMODORO_IMPLEMENTATION_EXAMPLES.h - Example 1

### Task: Change Colors
See: QUICK_START_GUIDE.md - "Change Colors"
Code location: POMODORO_IMPLEMENTATION_EXAMPLES.h - Example 10

### Task: Add Sound Alert
See: POMODORO_IMPLEMENTATION_EXAMPLES.h - Example 6

### Task: Implement Touch Control
See: POMODORO_IMPLEMENTATION_EXAMPLES.h - Example 8

### Task: Add Progress Bar
See: QUICK_START_GUIDE.md - "Add Progress Bar"

### Task: Test Display Layout
See: DISPLAY_GRID_REFERENCE.md - "Testing Your Layout"

### Task: Debug Display Issues
See: QUICK_START_GUIDE.md - "Troubleshooting"

---

## Code Examples Cross-Reference

| Feature | Where to Find | File |
|---------|---------------|------|
| ColorLeds() usage | Section 2 | WORDCLOCK_ANALYSIS.md |
| Dutch() model | Section 3 | WORDCLOCK_ANALYSIS.md |
| Number conversion | Example 1 | POMODORO_IMPLEMENTATION_EXAMPLES.h |
| Display layout | Layout templates | DISPLAY_GRID_REFERENCE.md |
| Loop integration | Example 5 | POMODORO_IMPLEMENTATION_EXAMPLES.h |
| Timer functions | Example 6 | POMODORO_IMPLEMENTATION_EXAMPLES.h |
| Serial commands | Example 7 | POMODORO_IMPLEMENTATION_EXAMPLES.h |
| Touch interface | Example 8 | POMODORO_IMPLEMENTATION_EXAMPLES.h |
| Color themes | Example 10 | POMODORO_IMPLEMENTATION_EXAMPLES.h |
| Position calculation | Display Grid Reference | DISPLAY_GRID_REFERENCE.md |
| State examples | Examples 1-5 | EXAMPLE_DISPLAYS.md |

---

## Quick Reference: Essential Functions

### ColorLeds()
```cpp
ColorLeds("TWENTY", 0, 5, 0xFF0000);  // Add red "TWENTY" at positions 0-5
```
**In**: Text, start pos, end pos, RGB color (0xRRGGBB)
**Out**: Populates Strippos[] array

### LedsOff()
```cpp
LedsOff();  // Clear all characters and colors
```
**In**: None
**Out**: Zeroes Strippos[] array

### ShowCharsILI9341()
```cpp
ShowCharsILI9341();  // Render Strippos[] to screen
```
**In**: Reads global Strippos[] array
**Out**: Draws to ILI9341 display

### SetStatusLED()
```cpp
SetStatusLED(0, 0, 255, 0, 0);  // Red LED on
SetStatusLED(0, 0, 0, 0, 0);    // All LEDs off
```
**In**: WW, CW, Red, Green, Blue (0=OFF, 255=ON)
**Out**: Controls ESP2432S028 RGB LED

### GetTijd()
```cpp
GetTijd(0);  // Update global timeinfo from NTP/RTC
```
**In**: printit (0=silent, 1=print to serial)
**Out**: Updates struct tm timeinfo

---

## Glossary

| Term | Definition |
|------|-----------|
| Strippos[] | Global array of 128 characters + colors (the display "buffer") |
| ColorLeds() | Function that fills Strippos[] with text and colors |
| ShowCharsILI9341() | Function that renders Strippos[] to the physical display |
| DisplayChoice | User's selected display mode (word clock vs. digital vs. analog) |
| LanguageChoice | User's selected language (Dutch, English, German, French) |
| State Machine | Function that determines what words to display based on time |
| EverySecondCheck | Main timing function called once per second |
| EveryMinuteUpdate | Timing function called when minute changes |
| Pomodoro State | Current timer state (WORK, SHORT_BREAK, LONG_BREAK, PAUSED) |
| RGB565 | 16-bit color format used by ILI9341 display |
| RGB32 (0xRRGGBB) | 32-bit color format used in code |
| NTP | Network Time Protocol (WiFi time sync) |
| RTC | Real-Time Clock (built-in ESP32 time) |
| LEDs | Individual "LED" positions (actually characters in word clock) |
| Preferences | ESP32's non-volatile flash storage (like EEPROM) |

---

## Troubleshooting Index

| Problem | Solution | Location |
|---------|----------|----------|
| Display shows garbage | Check ColorLeds() positions don't exceed 127 | WORDCLOCK_ANALYSIS.md |
| Colors are wrong | Verify 0xRRGGBB format and use ConvertRGB32toRGB16() | WORDCLOCK_ANALYSIS.md |
| Timer doesn't count | Verify UpdatePomodoroTimer() called in loop | QUICK_START_GUIDE.md |
| Text overlaps | Recalculate positions using grid formula | DISPLAY_GRID_REFERENCE.md |
| Memory errors | Reduce ColorLeds() calls or simplify strings | QUICK_START_GUIDE.md |
| LED doesn't light | Remember GPIO2432S028 uses digital pins (not PWM) | WORDCLOCK_ANALYSIS.md |
| Touch doesn't work | Verify ProcessTouchInput() called in loop | POMODORO_IMPLEMENTATION_EXAMPLES.h |
| Sketch won't compile | Check includes and function declarations | QUICK_START_GUIDE.md |

---

## Next Steps

1. **Read** QUICK_START_GUIDE.md for your chosen reading path
2. **Choose** a display layout from DISPLAY_GRID_REFERENCE.md
3. **Copy** code templates from POMODORO_IMPLEMENTATION_EXAMPLES.h
4. **Integrate** following QUICK_START_GUIDE.md steps 1-7
5. **Test** using the provided checklist
6. **Debug** using EXAMPLE_DISPLAYS.md as reference
7. **Customize** colors, durations, and features

---

## Support Files in Repository

```
Source Code:
├── ESP322432S028_WordClockV016/
│   ├── ESP322432S028_WordClockV016.ino (3500 lines)
│   ├── Colors.h (140+ colors)
│   └── Webpage.h (web interface)
├── ESP32_Pomodoro_Timer/ (your new sketch)

Documentation:
├── WORDCLOCK_ANALYSIS.md (25 KB)
├── POMODORO_IMPLEMENTATION_EXAMPLES.h (13 KB)
├── DISPLAY_GRID_REFERENCE.md (11 KB)
├── EXAMPLE_DISPLAYS.md (9 KB)
├── QUICK_START_GUIDE.md (8 KB)
├── CLAUDE.md (project specs)
└── DOCUMENTATION_INDEX.md (this file)

Total Documentation: ~75 KB (comprehensive reference)
```

---

## Document Version Info

- **Created**: November 21, 2025
- **Based on**: Word Clock V016 (Ed Nieuw)
- **For**: ESP32-2432S028R Pomodoro Timer
- **Coverage**: Complete architecture understanding + implementation guide
- **Code Examples**: 50+ ready-to-use snippets
- **Diagrams**: 10+ ASCII architecture diagrams

---

## Quick Links Within Documents

- ColorLeds deep dive → WORDCLOCK_ANALYSIS.md Section 2
- Dutch state machine → WORDCLOCK_ANALYSIS.md Section 3
- Display pipeline → WORDCLOCK_ANALYSIS.md Section 5
- Main loop diagram → WORDCLOCK_ANALYSIS.md Section 9
- Integration steps → QUICK_START_GUIDE.md
- Code templates → POMODORO_IMPLEMENTATION_EXAMPLES.h Examples 1-10
- Grid formulas → DISPLAY_GRID_REFERENCE.md
- Visual examples → EXAMPLE_DISPLAYS.md Examples 1-8

---

**Start reading based on your goal above!**

For questions about implementation, refer to the specific document listed. All documents cross-reference each other for easy navigation.
