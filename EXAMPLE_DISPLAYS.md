# Example Display Outputs

## Real-World Display Examples

This document shows what your Pomodoro timer will look like in various states using the Word Clock character grid.

---

## Example 1: Work Session at 24:35

### Display Output
```
      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
   ┌─────────────────────────────────────────────────┐
0  │ M  I  N  U  T  E  S              S  E  C  O  N  D │
1  │ T  W  E  N  T  Y     F  O  U  R     S  E  C  O  N │
2  │ T  H  I  R  T  Y  -  F  I  V  E    S  E  C  O  N │
3  │                                                 │
4  │         W  O  R  K     S  E  S  S  I  O  N     │
5  │                                                 │
6  │  S  e  s  s  i  o  n   2   o  f   4  T  o  d  a │
7  │  ◄─  P A U S E  ─┼─  S K I P  ─►                │
   └─────────────────────────────────────────────────┘
```

### Code That Produces This
```cpp
void DisplayPomodoroTimer(void)
{
  LedsOff();

  // Colors for work session
  uint32_t timeColor = C_RED;        // 0xFF0000
  uint32_t labelColor = C_ORANGE;    // 0xFFA500

  // Minutes
  ColorLeds("MINUTES",     0,  7, labelColor);
  ColorLeds("TWENTY",      16, 20, timeColor);
  ColorLeds("FOUR",        22, 25, timeColor);

  // Seconds
  ColorLeds("SECONDS",     96, 102, labelColor);
  ColorLeds("THIRTY",      80, 85, timeColor);
  ColorLeds("FIVE",        87, 90, timeColor);

  // Session info
  ColorLeds("WORK SESSION", 48, 59, labelColor);
  ColorLeds("Session 2/4",  64, 74, C_YELLOW);
}
```

### Visual Appearance on Screen
```
Red text for numbers:     TWENTY FOUR THIRTY FIVE
Orange text for labels:   MINUTES SECONDS WORK SESSION
Yellow text for info:     Session 2/4 Today: 8
Red status LED:           Solid ON
```

---

## Example 2: Short Break at 4:12

### Display Output
```
      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
   ┌─────────────────────────────────────────────────┐
0  │ M  I  N  U  T  E  S              S  E  C  O  N  D │
1  │ F  O  U  R                       T  W  E  L  V  E │
2  │                                                 │
3  │                                                 │
4  │     S  H  O  R  T     B  R  E  A  K              │
5  │                                                 │
6  │         T  I  M  E     T  O     R  E  L  A  X   │
7  │                                                 │
   └─────────────────────────────────────────────────┘
```

### Display Code
```cpp
currentState = STATE_SHORT_BREAK;
secondsRemaining = 4*60 + 12;  // 4:12

// Colors change automatically
uint32_t timeColor = C_GREEN;          // 0x00FF00
uint32_t labelColor = C_CYAN;          // 0x00CED1

ColorLeds("MINUTES",      0,  7, labelColor);
ColorLeds("FOUR",        16, 19, timeColor);

ColorLeds("SECONDS",     96, 102, labelColor);
ColorLeds("TWELVE",      80, 85, timeColor);

ColorLeds("SHORT BREAK", 48, 58, labelColor);
```

### Visual Properties
```
Green text for numbers:      FOUR TWELVE
Cyan text for labels:         MINUTES SECONDS SHORT BREAK
Green status LED:            Solid ON
```

---

## Example 3: Long Break at 14:47

### Display Output
```
      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
   ┌─────────────────────────────────────────────────┐
0  │ M  I  N  U  T  E  S              S  E  C  O  N  D │
1  │ F  O  U  R  T  E  E  N            F  O  R  T  Y  │
2  │ S  E  V  E  N                                     │
3  │                                                 │
4  │         L  O  N  G     B  R  E  A  K             │
5  │                                                 │
6  │      G  O     R  E  L  A  X  !                   │
7  │                                                 │
   └─────────────────────────────────────────────────┘
```

### Code
```cpp
currentState = STATE_LONG_BREAK;
secondsRemaining = 14*60 + 47;

uint32_t timeColor = C_BLUE;           // 0x0000FF
uint32_t labelColor = C_CYAN;          // 0x00FFFF

ColorLeds("MINUTES",       0,  7, labelColor);
ColorLeds("FOURTEEN",     16, 23, timeColor);

ColorLeds("SECONDS",      96, 102, labelColor);
ColorLeds("FORTY SEVEN",  80, 90, timeColor);

ColorLeds("LONG BREAK",   48, 58, labelColor);
```

### Visual Properties
```
Blue text for numbers:       FOURTEEN FORTY SEVEN
Cyan text for labels:         MINUTES SECONDS LONG BREAK
Blue status LED:            Solid ON
```

---

## Example 4: Paused Timer at 17:35

### Display Output
```
      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
   ┌─────────────────────────────────────────────────┐
0  │ M  I  N  U  T  E  S              S  E  C  O  N  D │
1  │ S  E  V  E  N  T  E  E  N         T  H  I  R  T  │
2  │ Y  -  F  I  V  E                                  │
3  │                                                 │
4  │                P  A  U  S  E  D                   │
5  │                                                 │
6  │          T  A  P     C  E  N  T  E  R            │
7  │           T  O     R  E  S  U  M  E              │
   └─────────────────────────────────────────────────┘
```

### Code
```cpp
currentState = STATE_PAUSED;
secondsRemaining = 17*60 + 35;  // Stopped at this value

uint32_t timeColor = C_YELLOW;         // 0xFFFF00
uint32_t labelColor = C_GOLD;          // 0xFFD700

ColorLeds("MINUTES",       0,  7, labelColor);
ColorLeds("SEVENTEEN",    16, 24, timeColor);

ColorLeds("SECONDS",      96, 102, labelColor);
ColorLeds("THIRTY FIVE",  80, 91, timeColor);

ColorLeds("PAUSED",       48, 53, labelColor);
```

### Visual Properties
```
Yellow text for numbers:     SEVENTEEN THIRTY FIVE
Gold text for labels:         MINUTES SECONDS PAUSED
Status LED:                  Dim or breathing (slow flash)
```

---

## Example 5: Idle/Ready State

### Display Output
```
      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
   ┌─────────────────────────────────────────────────┐
0  │ P  O  M  O  D  O  R  O     T  I  M  E  R     R  E │
1  │                                                 │
2  │              R  E  A  D  Y                       │
3  │                                                 │
4  │         T  A  P     C  E  N  T  E  R             │
5  │            T  O     S  T  A  R  T                │
6  │                                                 │
7  │    2  5  m  i  n     -     5  /  1  5  /  1     │
   └─────────────────────────────────────────────────┘
```

### Code
```cpp
if (pomodoroMode && !timerRunning)
{
  LedsOff();

  uint32_t readyColor = C_WHITE;
  ColorLeds("POMODORO TIMER", 0, 13, readyColor);
  ColorLeds("READY",          32, 36, readyColor);
  ColorLeds("TAP CENTER",     48, 57, C_YELLOW);
  ColorLeds("TO START",       64, 71, C_YELLOW);
}
```

### Visual Properties
```
White text:                   POMODORO TIMER READY
Yellow text:                  TAP CENTER TO START
Status LED:                   Off or slow breathing
```

---

## Example 6: Word Clock Mode (Fallback)

### Display Output
When pomodoroMode = false, shows normal Dutch word clock:

```
      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
   ┌─────────────────────────────────────────────────┐
0  │ H  E  T                 I  S              V  I  J │
1  │ F     O  V  E  R     H  A  L  F     T  W  E  N  T │
2  │ Y     T  W  E  L  F     U  U  R              -  M │
3  │ I  N  U  T  E  S        D  O  W  N  W  A  R  D  S │
4  │                                                 │
5  │     (Current time in words)                     │
6  │                                                 │
7  │     (Clock keeps working normally)              │
   └─────────────────────────────────────────────────┘
```

---

## Example 7: Session Completion Animation

### Frame 1 (t=0ms)
```
WORK SESSION COMPLETE!
███████████████████████████████
Next: SHORT BREAK (5 minutes)
[Flash RGB LED - Yellow]
```

### Frame 2 (t=200ms)
```
WORK SESSION COMPLETE!
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
Next: SHORT BREAK (5 minutes)
[LED Off]
```

### Frame 3 (t=400ms)
```
WORK SESSION COMPLETE!
███████████████████████████████
Next: SHORT BREAK (5 minutes)
[Flash RGB LED - Yellow]
```

### Code
```cpp
void PlayCompletionAlert(void)
{
  // Flash animation
  for (int i = 0; i < 3; i++)
  {
    SetStatusLED(0, 0, 255, 255, 0);  // Yellow flash
    delay(200);
    SetStatusLED(0, 0, 0, 0, 0);      // Off
    delay(200);
  }

  // Optional sound (3-tone ascending)
  #ifdef SPEAKER_PIN
    tone(SPEAKER_PIN, 1000, 200);  // Low
    delay(300);
    tone(SPEAKER_PIN, 1500, 200);  // Mid
    delay(300);
    tone(SPEAKER_PIN, 2000, 400);  // High
  #endif

  // Proceed to next session
  StartBreakSession(false);
}
```

---

## Example 8: Progress Indication (Optional)

### Progress Bar Display
```
Row 6 (position 96-111): Progress indicator

Work session (20 minutes elapsed / 25 total):
█████████████░░░░░░░░░░░░
 80% complete

Break session (3 minutes elapsed / 5 total):
███████░░░░░░░░░░░░░░░░░░
 60% complete
```

### Code
```cpp
void DisplayProgressBar(uint32_t elapsed, uint32_t total)
{
  uint8_t filledChars = (elapsed * 16) / total;

  for (int i = 0; i < filledChars; i++)
  {
    Strippos[96 + i].Character = 0xDB;  // Full block █
    Strippos[96 + i].RGBColor = C_GREEN;
  }

  for (int i = filledChars; i < 16; i++)
  {
    Strippos[96 + i].Character = 0xB0;  // Light block ░
    Strippos[96 + i].RGBColor = C_DARK_GRAY;
  }
}
```

---

## Number-to-Word Examples

### Conversion Examples

| Seconds | Words | Display |
|---------|-------|---------|
| 0 | ZERO | "ZERO SECONDS" |
| 1 | ONE | "ONE SECOND" |
| 12 | TWELVE | "TWELVE SECONDS" |
| 24 | TWENTY FOUR | "TWENTY FOUR SECONDS" |
| 35 | THIRTY FIVE | "THIRTY FIVE SECONDS" |
| 59 | FIFTY NINE | "FIFTY NINE SECONDS" |

| Minutes | Words | Display |
|---------|-------|---------|
| 1 | ONE | "ONE MINUTE" |
| 5 | FIVE | "FIVE MINUTES" |
| 14 | FOURTEEN | "FOURTEEN MINUTES" |
| 25 | TWENTY FIVE | "TWENTY FIVE MINUTES" |

### Combined Examples
```
1:00   → ONE MINUTE ZERO SECONDS
5:30   → FIVE MINUTES THIRTY SECONDS
25:00  → TWENTY FIVE MINUTES ZERO SECONDS
```

---

## Color Reference Table

### RGB565 Display Colors

| Color | Hex Code | Appearance | Usage |
|-------|----------|-----------|-------|
| Red | 0xFF0000 | Bright red | Work sessions |
| Green | 0x00FF00 | Bright green | Short breaks |
| Blue | 0x0000FF | Bright blue | Long breaks |
| Yellow | 0xFFFF00 | Bright yellow | Paused, idle |
| Orange | 0xFFA500 | Orange | Work labels |
| Cyan | 0x00FFFF | Light blue | Break labels |
| Gold | 0xFFD700 | Golden | Paused labels |
| White | 0xFFFFFF | White | Ready, menu |
| Dark Gray | 0x404040 | Dark | Background, inactive |
| Black | 0x000000 | Black | Background |

---

## Touchscreen Interaction Examples

### Left Zone (Pause)
```
User taps left third of screen
    ↓
PauseTimer() called
    ↓
currentState = STATE_PAUSED
timerRunning = false
SetStatusLED(0,0,0,0,0)  [Turn off]
Display shows yellow "PAUSED"
```

### Center Zone (Start/Resume)
```
User taps center third of screen

If timerRunning:
  PauseTimer()
    ↓
  Display becomes yellow

If paused:
  ResumeTimer()
    ↓
  Display returns to work/break color
```

### Right Zone (Skip/Reset)
```
User taps right third of screen
    ↓
ResetTimer() called
    ↓
currentState = STATE_IDLE
secondsRemaining = 0
Return to READY display
```

---

## Serial Terminal Examples

### Session Output
```
> PS
[Serial] Pomodoro started - WORK SESSION
[Display] TWENTY FIVE MINUTES 0 SECONDS - RED
> [After 10 seconds]
[Serial] 10 seconds elapsed
[Display] TWENTY FOUR MINUTES FIFTY SECONDS - RED
> PP
[Serial] Pomodoro paused
[Display] TWENTY FOUR MINUTES FIFTY SECONDS - YELLOW
> PR
[Serial] Pomodoro resumed
[Display] TWENTY FOUR MINUTES FIFTY SECONDS - RED
> [After 25 minutes]
[Serial] WORK SESSION COMPLETE!
[Display] READY FOR SHORT BREAK
[RGB LED] Flashing yellow
> [Auto-start short break after 5 seconds]
[Serial] Short break started
[Display] FIVE MINUTES 0 SECONDS - GREEN
```

### Debug Output
```
[Startup]
Serial started
Preferences loaded
Display initialized
RGB LED initialized

[Timer Start]
Work duration set to 25 minutes
Work duration set to 25 minutes
Colors set: RED for work
Status LED: RED

[Timer Running - Every Second]
Time: 1499 remaining
Time: 1498 remaining
Time: 1497 remaining
...

[Timer End]
SESSION COMPLETE!
Completion alert playing...
Starting SHORT BREAK (300 seconds)
```

---

## Pixel-Perfect Layout Verification

### Character Position Grid
```
Text Rendering Test - Shows each position

Row 0: 0123456789ABCDEF
       [Position numbers 0-15 on screen]

Row 1: 0123456789012345
       [Position numbers 16-31 on screen]

Row 2: 0123456789012345
       [Position numbers 32-47 on screen]

Result: Helps you verify layout is pixel-perfect
```

---

## State Transition Diagram

```
                    ┌─────────────┐
                    │    IDLE     │
                    │  (READY)    │
                    └──────┬──────┘
                           │
                    User presses START
                           │
                           ↓
                    ┌─────────────────┐
        ┌──────────→│  WORK SESSION   │←─────────────┐
        │           │  (25 min, RED)  │              │
        │           └─────────┬───────┘              │
        │                     │                      │
        │            Session ends                    │
        │                     │                      │
        │           ┌─────────↓────────┐             │
        │           │ Choose break type│             │
        │           └─────────┬────────┘             │
        │                     │                      │
        │         ┌───────────┴──────────┐           │
        │         │                      │           │
        │         ↓                      ↓           │
        │   ┌──────────────┐       ┌──────────────┐  │
        │   │ SHORT BREAK  │       │  LONG BREAK  │  │
        │   │ (5 min, GRN) │       │ (15 min, BLU)│  │
        │   └──────┬───────┘       └──────┬───────┘  │
        │          │                      │          │
        │          └──────────┬───────────┘          │
        │                     │                      │
        │            Break ends                      │
        │                     │                      │
        └─────────────────────┘                      │
                                                     │
                        User presses SKIP ───────────┘
```

---

## Summary: Display State Matrix

| State | Minutes | Seconds | Color | Label Color | LED | Status |
|-------|---------|---------|-------|------------|-----|--------|
| WORK | MM (red) | SS (red) | Red | Orange | Solid Red | Running |
| SHORT_BREAK | MM (grn) | SS (grn) | Green | Cyan | Solid Green | Running |
| LONG_BREAK | MM (blu) | SS (blu) | Blue | Cyan | Solid Blue | Running |
| PAUSED | MM (yel) | SS (yel) | Yellow | Gold | Dim | Paused |
| IDLE | — | — | White | White | Off | Ready |
| COMPLETE | — | — | Cycling | Gold | Flashing | Alert |

---

## Testing Your Display

Use this HTML/CSS simulation to plan your layout:

```html
<div style="display: grid; grid-template-columns: repeat(16, 1fr); gap: 2px; width: 320px; font-family: monospace;">
  <div style="background: red; color: white; padding: 10px; text-align: center;">T</div>
  <div style="background: red; color: white; padding: 10px; text-align: center;">W</div>
  <!-- Continue for all 128 positions -->
</div>
```

Or use the included **DebugDisplayPositions()** function to see where each position renders on your actual display!

