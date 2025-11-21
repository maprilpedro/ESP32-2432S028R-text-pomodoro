# Display Grid Reference Guide

## Physical Display Layout

The 16×8 character grid maps to the 320×240 pixel ILI9341 display:

```
Character Position to Screen Position Mapping:
- Column (x): position * 20 pixels (0, 20, 40, 60, ... 300)
- Row (y):    20 + row * 27 pixels (20, 47, 74, 101, ... 236)
- Text Size:  2 (approximately 20 pixels wide per character)
```

### Visual Grid (16 columns × 8 rows)

```
    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
0 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  Y=20px
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
1 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  Y=47px
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
2 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  Y=74px
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
3 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  Y=101px
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
4 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  Y=128px
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
5 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  Y=155px
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
6 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  Y=182px
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
7 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  Y=209px
  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
   X=0 X=20 X=40 X=60 X=80...               X=300
```

---

## Suggested Layout for Pomodoro Timer

### Layout Option 1: Centered Time with Session Type

```
Row 0: [empty] TWENTY FOUR MINUTES [empty]
Row 1: [empty] THIRTY FIVE SECONDS [empty]
Row 2: [empty] [empty] [empty] [empty]
Row 3: [empty] FOCUS TIMER [empty]
Row 4: [empty] Session 2 of 4 [empty]
Row 5: [empty] [empty] [empty] [empty]
Row 6: [empty] TAP TO PAUSE [empty]
Row 7: [empty] [empty] [empty] [empty]
```

**Character positions**:
- Row 0: "TWENTY FOUR MINUTES"
  - "TWENTY" → positions 1-6 (cols 1-6)
  - "FOUR" → positions 8-11 (cols 8-11)
  - "MINUTES" → positions 13-15 (cols 13-15, next row starts)

---

### Layout Option 2: Compact (Top-to-Bottom)

```
Row 0: TWENTY FOUR MINUTES THIRTY
Row 1: FIVE SECONDS FOCUS SESSION
Row 2: [empty]
Row 3: [progress indicator or next break info]
Row 4: [empty]
Row 5: [empty]
Row 6: TAP CENTER TO PAUSE
Row 7: [empty]
```

---

### Layout Option 3: Dual Stack (Recommended for Clarity)

```
Row 0: M I N U T E S         S E C O N D S
Row 1: ╔═════════════════╗   ╔═════════════╗
Row 2: ║  TWENTY FOUR    ║   ║  THIRTY FIVE║
Row 3: ║    (24)         ║   ║    (35)     ║
Row 4: ╚═════════════════╝   ╚═════════════╝
Row 5: WORK SESSION (RED)
Row 6: Session 2/4  Today: 8
Row 7: [TAP CENTER TO PAUSE] [SKIP RIGHT]
```

---

## Strippos Array Index Reference

The `Strippos[]` array is populated row-by-row, left-to-right:

```
Position 0-15:   Row 0 (characters 0-15)
Position 16-31:  Row 1 (characters 16-31)
Position 32-47:  Row 2 (characters 32-47)
Position 48-63:  Row 3 (characters 48-63)
Position 64-79:  Row 4 (characters 64-79)
Position 80-95:  Row 5 (characters 80-95)
Position 96-111: Row 6 (characters 96-111)
Position 112-127: Row 7 (characters 112-127)
```

### Position Calculation
```
linearPosition = (row * 16) + column

Example: Row 2, Column 5
linearPosition = (2 * 16) + 5 = 37
```

---

## Pomodoro Grid Template

### Recommended: Centered Layout

```
      Column:  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
      ─────────────────────────────────────────────────────
Row 0 │  │     TWENTY FOUR MINUTES OR THIRTY FIVE SEC     │  │
Row 1 │  │                                                │  │
Row 2 │  │              WORK SESSION                      │  │
Row 3 │  │              (or BREAK TIME)                   │  │
Row 4 │  │         Session 2 of 4 Today: 8               │  │
Row 5 │  │                                                │  │
Row 6 │  │    ◄ PAUSE ▼   ●●●   ► SKIP                  │  │
Row 7 │  │    (Left)   (Center) (Right)                  │  │
      ─────────────────────────────────────────────────────
```

### Position Assignments

```
MINUTES (Row 0):
  "TWENTY" → positions 4-9 (cols 4-9)
  "FOUR" → positions 11-14 (cols 11-14)
  "MINUTES" → positions 16-22 (spans into row 1, cols 0-6)

SECONDS (Row 0-1):
  "THIRTY" → positions 25-30 (cols 9-14)
  "FIVE" → positions 31-34 (cols 15, spans into row 2)
  "SECONDS" → positions 34-40 (cols 2-8, row 1)

SESSION INFO (Row 2):
  "WORK" or "BREAK" → positions 48-51 or 48-52
  "SESSION" → positions 53-59

COUNTER (Row 3):
  "Session 2 of 4" → positions 64-77
```

---

## Color Mapping

```
Work Session (Red):
  - Time text: 0xFF0000 (bright red)
  - Label: 0xFF4500 (orange-red)

Short Break (Green):
  - Time text: 0x00FF00 (bright green)
  - Label: 0x00CED1 (turquoise)

Long Break (Blue):
  - Time text: 0x0000FF (bright blue)
  - Label: 0x00FFFF (cyan)

Paused (Yellow):
  - Time text: 0xFFFF00 (bright yellow)
  - Label: 0xFFD700 (gold)

Labels/Punctuation:
  - Always: 0xFFFFFF (white) or theme accent
```

---

## Touch Zone Reference (If Implemented)

The 320×240 display can be divided into zones:

```
┌─────────────────┬─────────────────┬─────────────────┐
│      LEFT       │     CENTER      │     RIGHT       │
│   (x: 0-106)    │   (x: 107-213)  │   (x: 214-320)  │
│                 │                 │                 │
│   PREVIOUS      │   START/PAUSE   │     SKIP        │
│   or            │                 │                 │
│   SETTINGS      │     RESUME      │     RESET       │
│                 │                 │                 │
└─────────────────┴─────────────────┴─────────────────┘
        Y: 0-240 (full height)
```

---

## Code Template for Custom Layout

```cpp
// Define your positions
#define POS_MINUTES_START    4
#define POS_MINUTES_END     14
#define POS_MINUTES_LABEL   20   // "MINUTES" on next line

#define POS_SECONDS_START   32
#define POS_SECONDS_END     42
#define POS_SECONDS_LABEL   52

#define POS_SESSION_START   64
#define POS_SESSION_END     79

// Use in your function
void DisplayPomodoroCustom(uint32_t secondsRemaining)
{
  char minStr[16], secStr[16];

  // Convert to words
  uint16_t minutes = secondsRemaining / 60;
  uint8_t seconds = secondsRemaining % 60;
  NumToWords(minutes, minStr);
  NumToWords(seconds, secStr);

  LedsOff();

  // Display with your custom positions
  ColorLeds(minStr, POS_MINUTES_START, POS_MINUTES_END, workColor);
  ColorLeds("MINUTES", POS_MINUTES_LABEL, POS_MINUTES_LABEL+6, labelColor);

  ColorLeds(secStr, POS_SECONDS_START, POS_SECONDS_END, workColor);
  ColorLeds("SECONDS", POS_SECONDS_LABEL, POS_SECONDS_LABEL+6, labelColor);

  ColorLeds(sessionStr, POS_SESSION_START, POS_SESSION_END, labelColor);
}
```

---

## Quick Visual Check

Use this to verify your layout:

```cpp
void DebugDisplayPositions()
{
  LedsOff();

  // Show positions 0-9 in first row
  for (int i = 0; i < 10; i++)
  {
    Strippos[i].Character = '0' + i;
    Strippos[i].RGBColor = C_YELLOW;
  }

  // Show positions 10-15 in first row
  for (int i = 10; i < 16; i++)
  {
    Strippos[i].Character = 'A' + (i - 10);  // A, B, C, D, E, F
    Strippos[i].RGBColor = C_YELLOW;
  }

  // Show positions 16-31 in second row
  for (int i = 16; i < 32; i++)
  {
    Strippos[i].Character = '0' + ((i - 16) % 10);
    Strippos[i].RGBColor = C_GREEN;
  }

  ShowCharsILI9341();

  // Will display:
  // 0123456789ABCDEF
  // 0123456789012345
}
```

This helps you see exactly where each position is on your display.

---

## Practical Example: 24:35 Layout

```cpp
void DisplayExample()
{
  LedsOff();

  // Show "TWENTY FOUR MINUTES THIRTY FIVE SECONDS"
  ColorLeds("TWENTY",     0,  5,  C_RED);    // Row 0, cols 0-5
  ColorLeds("FOUR",       7, 10,  C_RED);    // Row 0, cols 7-10
  ColorLeds("MINUTES",   12, 18,  C_YELLOW);// Row 0-1, cols 12-15, 0-2

  ColorLeds("THIRTY",    20, 25,  C_RED);    // Row 1, cols 4-9
  ColorLeds("FIVE",      27, 30,  C_RED);    // Row 1, cols 11-14
  ColorLeds("SECONDS",   32, 38,  C_YELLOW);// Row 2, cols 0-6

  ShowCharsILI9341();
}
```

---

## Memory Considerations

Each ColorLeds() call:
- **Time complexity**: O(n) where n = number of characters in text
- **Space complexity**: O(1) - just updates Strippos array (pre-allocated)

Typical calls per display update:
- ~8-10 ColorLeds() calls per minute
- Total text: ~80-100 characters per display

**No memory issues expected** - ESP32 has 520KB SRAM available.

---

## Testing Your Layout

1. **Use numbers**: Replace words with position numbers (0-127)
2. **Use dots**: Fill entire area with dots to see grid alignment
3. **Alternating colors**: Use different colors for each section to verify boundaries
4. **Serial debugging**: Print positions of each word added

```cpp
// Debug: Print what you're adding
void DisplayWithDebug()
{
  LedsOff();

  sprintf(sptext, "Adding 'TWENTY' at pos 0-5");
  Tekstprintln(sptext);
  ColorLeds("TWENTY", 0, 5, C_RED);

  sprintf(sptext, "Adding 'FOUR' at pos 7-10");
  Tekstprintln(sptext);
  ColorLeds("FOUR", 7, 10, C_RED);

  ShowCharsILI9341();
}
```

---

## Common Mistakes to Avoid

1. **Position Range Too Large**
   - ❌ ColorLeds("MINUTES", 20, 50) → will exceed row 2
   - ✓ ColorLeds("MINUTES", 20, 26) → stays in one row

2. **Overlapping Positions**
   - ❌ ColorLeds("TWENTY", 0, 5) + ColorLeds("FOUR", 4, 7) → overlap at 4-5
   - ✓ ColorLeds("TWENTY", 0, 5) + ColorLeds("FOUR", 7, 10)

3. **Incorrect Row Calculation**
   - ❌ Position 20 is in row 1 (20 / 16 = 1.25)
   - ✓ Position 20 = row 1, col 4 (20 = 1*16 + 4)

4. **Color Not Converting**
   - ❌ Passing C_RED (0xFF0000) directly to tft.drawChar()
   - ✓ Use ConvertRGB32toRGB16() in ShowCharsILI9341() (already done)

---

## Final Checklist

- [ ] Verified all position ranges stay within 0-127
- [ ] No overlapping character positions
- [ ] Colors are defined (0xRRGGBB format)
- [ ] ShowCharsILI9341() called after all ColorLeds()
- [ ] LedsOff() called before building display
- [ ] Serial debug output shows correct positions
- [ ] Display renders at correct screen locations
- [ ] Text size and spacing look good
- [ ] Touch zones marked (if using touchscreen)
- [ ] Word conversion tested for edge cases (0, 1, 59 seconds, etc.)
