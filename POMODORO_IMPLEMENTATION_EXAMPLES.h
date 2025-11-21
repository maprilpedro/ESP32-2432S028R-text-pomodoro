/*
   POMODORO TIMER - WORD CLOCK IMPLEMENTATION EXAMPLES

   This file contains ready-to-use code snippets for implementing
   a word-based Pomodoro timer using the Word Clock V016 architecture.

   Key Concept:
   - Reuse ColorLeds() to populate Strippos[] array with characters and colors
   - Reuse ShowCharsILI9341() to render the display
   - Create TimeToWords() to convert countdown seconds to English words
   - Integrate into existing EveryMinuteUpdate() timing loop
*/

#ifndef POMODORO_IMPLEMENTATION_EXAMPLES_H
#define POMODORO_IMPLEMENTATION_EXAMPLES_H

// ============================================================================
// EXAMPLE 1: NUMBER-TO-WORD CONVERSION
// ============================================================================

// Array of number words (0-20 for minutes, 0-59 for individual digits)
const char* UnitsWords[] = {
  "ZERO", "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN",
  "EIGHT", "NINE", "TEN", "ELEVEN", "TWELVE", "THIRTEEN", "FOURTEEN",
  "FIFTEEN", "SIXTEEN", "SEVENTEEN", "EIGHTEEN", "NINETEEN", "TWENTY"
};

const char* TensWords[] = {
  "", "", "TWENTY", "THIRTY", "FORTY", "FIFTY"
};

// Convert a number (1-59) to its word representation
// Example: 24 → "TWENTY FOUR"
void NumToWords(int num, char* output)
{
  if (num == 0)
  {
    strcpy(output, "");
    return;
  }

  if (num < 20)
  {
    strcpy(output, UnitsWords[num]);
  }
  else
  {
    int tens = num / 10;
    int units = num % 10;

    strcpy(output, TensWords[tens]);

    if (units > 0)
    {
      strcat(output, " ");
      strcat(output, UnitsWords[units]);
    }
  }
}

// ============================================================================
// EXAMPLE 2: DISPLAY MACROS (Define positions for your word grid)
// ============================================================================

// Assuming 16-column display, distribute words across rows:
// Row 0 (0-15):    "TWENTY FOUR MIN UTES"
// Row 1 (16-31):   "THIRTY TWO SECONDS"

#define DISPLAY_MINUTES_NUM1_START    0
#define DISPLAY_MINUTES_NUM1_END      15
#define DISPLAY_MINUTES_LABEL_START   20
#define DISPLAY_MINUTES_LABEL_END     26

#define DISPLAY_SECONDS_NUM_START     32
#define DISPLAY_SECONDS_NUM_END       47
#define DISPLAY_SECONDS_LABEL_START   52
#define DISPLAY_SECONDS_LABEL_END     58

// Macro to add a word to the display
#define DISPLAY_WORD(text, start, end, color) \
  ColorLeds(text, start, end, color)

// ============================================================================
// EXAMPLE 3: CORE POMODORO DISPLAY FUNCTION
// ============================================================================

enum PomodoroState {
  STATE_IDLE,
  STATE_WORK,
  STATE_SHORT_BREAK,
  STATE_LONG_BREAK,
  STATE_PAUSED
};

// Global state
PomodoroState currentState = STATE_IDLE;
uint32_t secondsRemaining = 0;
bool timerRunning = false;

void DisplayCountdownInWords(uint32_t totalSeconds)
{
  char minutesStr[32];
  char secondsStr[32];

  // Convert seconds to minutes and seconds
  uint16_t minutes = totalSeconds / 60;
  uint8_t seconds = totalSeconds % 60;

  // Convert to words
  NumToWords(minutes, minutesStr);
  NumToWords(seconds, secondsStr);

  // Clear display first
  LedsOff();

  // Set colors based on session type
  uint32_t timeColor = LetterColor;
  uint32_t labelColor = C_YELLOW;

  switch (currentState)
  {
    case STATE_WORK:
      timeColor = C_RED;        // Work = red
      labelColor = C_ORANGE;
      break;

    case STATE_SHORT_BREAK:
      timeColor = C_GREEN;      // Break = green
      labelColor = C_CYAN;
      break;

    case STATE_LONG_BREAK:
      timeColor = C_BLUE;       // Long break = blue
      labelColor = C_CYAN;
      break;

    case STATE_PAUSED:
      timeColor = C_YELLOW;     // Paused = yellow
      labelColor = C_GOLD;
      break;

    default:
      break;
  }

  // Display minutes and the word "MINUTES"
  if (strlen(minutesStr) > 0)
  {
    DISPLAY_WORD(minutesStr, DISPLAY_MINUTES_NUM1_START,
                 DISPLAY_MINUTES_NUM1_END, timeColor);
  }
  DISPLAY_WORD("MINUTES", DISPLAY_MINUTES_LABEL_START,
               DISPLAY_MINUTES_LABEL_END, labelColor);

  // Display seconds and the word "SECONDS"
  if (strlen(secondsStr) > 0)
  {
    DISPLAY_WORD(secondsStr, DISPLAY_SECONDS_NUM_START,
                 DISPLAY_SECONDS_NUM_END, timeColor);
  }
  DISPLAY_WORD("SECONDS", DISPLAY_SECONDS_LABEL_START,
               DISPLAY_SECONDS_LABEL_END, labelColor);

  // Optional: Display session info
  char sessionStr[16];
  switch (currentState)
  {
    case STATE_WORK:
      strcpy(sessionStr, "FOCUS");
      break;
    case STATE_SHORT_BREAK:
      strcpy(sessionStr, "SHORT BREAK");
      break;
    case STATE_LONG_BREAK:
      strcpy(sessionStr, "LONG BREAK");
      break;
    case STATE_PAUSED:
      strcpy(sessionStr, "PAUSED");
      break;
    default:
      strcpy(sessionStr, "IDLE");
  }
  DISPLAY_WORD(sessionStr, 64, 80, labelColor);
}

// ============================================================================
// EXAMPLE 4: INTEGRATED DISPLAY ORCHESTRATOR
// ============================================================================

bool pomodoroMode = false;  // Toggle between word clock and Pomodoro timer

void DisplayPomodoroOrClock(void)
{
  LedsOff();

  if (pomodoroMode && timerRunning)
  {
    // Show countdown in words
    DisplayCountdownInWords(secondsRemaining);
  }
  else if (pomodoroMode && !timerRunning)
  {
    // Show "READY" or idle message
    LedsOff();
    DISPLAY_WORD("READY", 32, 36, C_WHITE);
  }
  else
  {
    // Show word clock (reuse existing Dutch/English/German functions)
    SetSecondColour();

    // Determine language
    byte Language = Mem.LanguageChoice;
    if (Mem.LanguageChoice == 4)
      Language = random(4);

    // Call appropriate function
    switch (Language)
    {
      case 0:
        Dutch();
        break;
      case 1:
        English();
        break;
      case 2:
        German();
        break;
      case 3:
        French();
        break;
    }
  }
}

// ============================================================================
// EXAMPLE 5: INTEGRATION INTO EXISTING LOOP
// ============================================================================

// This replaces the original EveryMinuteUpdate() call to Displaytime()
void EveryMinuteUpdate_Modified(void)
{
  lastminute = timeinfo.tm_min;
  SetStatusLED(0, 0, 0, 0, 0);

  if (!LEDsAreOff)
  {
    // NEW: Call your function instead of Displaytime()
    DisplayPomodoroOrClock();
  }

  if (timeinfo.tm_hour != lasthour)
    EveryHourUpdate();
}

// ============================================================================
// EXAMPLE 6: TIMER CONTROL FUNCTIONS
// ============================================================================

uint32_t workDuration = 25 * 60;       // 25 minutes in seconds
uint32_t shortBreakDuration = 5 * 60;  // 5 minutes
uint32_t longBreakDuration = 15 * 60;  // 15 minutes
uint32_t sessionsUntilLongBreak = 4;
uint32_t completedSessions = 0;

// Start a new work session
void StartWorkSession(void)
{
  currentState = STATE_WORK;
  secondsRemaining = workDuration;
  timerRunning = true;

  // Set status LED to red
  SetStatusLED(0, 0, 255, 0, 0);  // RED on

  // Optional: Ring bell or play sound
  // PlayStartSound();
}

// Start a break session
void StartBreakSession(bool isLongBreak)
{
  if (isLongBreak)
  {
    currentState = STATE_LONG_BREAK;
    secondsRemaining = longBreakDuration;
    SetStatusLED(0, 0, 0, 0, 255);  // BLUE on
  }
  else
  {
    currentState = STATE_SHORT_BREAK;
    secondsRemaining = shortBreakDuration;
    SetStatusLED(0, 0, 0, 255, 0);  // GREEN on
  }

  timerRunning = true;
}

// Pause the timer
void PauseTimer(void)
{
  timerRunning = false;
  currentState = STATE_PAUSED;
  SetStatusLED(0, 0, 0, 0, 0);  // All off (or could be yellow)
}

// Resume the timer
void ResumeTimer(void)
{
  timerRunning = true;

  // Restore color based on original state
  switch (currentState)
  {
    case STATE_WORK:
      SetStatusLED(0, 0, 255, 0, 0);  // RED
      break;
    case STATE_SHORT_BREAK:
      SetStatusLED(0, 0, 0, 255, 0);  // GREEN
      break;
    case STATE_LONG_BREAK:
      SetStatusLED(0, 0, 0, 0, 255);  // BLUE
      break;
  }
}

// Reset the timer
void ResetTimer(void)
{
  timerRunning = false;
  currentState = STATE_IDLE;
  secondsRemaining = 0;
  SetStatusLED(0, 0, 0, 0, 0);
}

// Called every second to decrement timer
void UpdatePomodoroTimer(void)
{
  if (!timerRunning || pomodoroMode == false)
    return;

  if (secondsRemaining > 0)
  {
    secondsRemaining--;
  }
  else
  {
    // Timer complete!
    PlayCompletionAlert();

    // Determine next state
    if (currentState == STATE_WORK)
    {
      completedSessions++;
      if (completedSessions % sessionsUntilLongBreak == 0)
        StartBreakSession(true);   // Long break
      else
        StartBreakSession(false);  // Short break
    }
    else if (currentState == STATE_SHORT_BREAK || currentState == STATE_LONG_BREAK)
    {
      // Break is over, ready for next work session
      ResetTimer();
    }
  }
}

void PlayCompletionAlert(void)
{
  // Flash RGB LED
  for (int i = 0; i < 3; i++)
  {
    SetStatusLED(0, 0, 255, 255, 0);  // Yellow
    delay(200);
    SetStatusLED(0, 0, 0, 0, 0);      // Off
    delay(200);
  }

  // Optional: Play tone on buzzer
  // tone(SPEAKER_PIN, 1000, 200);
  // delay(300);
  // tone(SPEAKER_PIN, 1500, 200);
}

// ============================================================================
// EXAMPLE 7: SERIAL COMMAND HANDLERS
// ============================================================================

// Add these to your serial command processing:

void ProcessPomodoroCommand(String command)
{
  if (command == "PS")
  {
    // Pomodoro Start
    pomodoroMode = true;
    StartWorkSession();
    Tekstprintln("Pomodoro started");
  }
  else if (command == "PP")
  {
    // Pomodoro Pause
    PauseTimer();
    Tekstprintln("Pomodoro paused");
  }
  else if (command == "PR")
  {
    // Pomodoro Resume
    ResumeTimer();
    Tekstprintln("Pomodoro resumed");
  }
  else if (command == "PX")
  {
    // Pomodoro Stop/Reset
    ResetTimer();
    pomodoroMode = false;
    Tekstprintln("Pomodoro stopped");
  }
  else if (command == "PW")
  {
    // Pomodoro Work duration
    int minutes = extractNumberFromCommand(command);
    if (minutes > 0 && minutes <= 60)
    {
      workDuration = minutes * 60;
      sprintf(sptext, "Work duration set to %d minutes", minutes);
      Tekstprintln(sptext);
    }
  }
  else if (command == "PB")
  {
    // Pomodoro Break duration
    int minutes = extractNumberFromCommand(command);
    if (minutes > 0 && minutes <= 30)
    {
      shortBreakDuration = minutes * 60;
      sprintf(sptext, "Break duration set to %d minutes", minutes);
      Tekstprintln(sptext);
    }
  }
}

int extractNumberFromCommand(String cmd)
{
  // Extract number from command like "PW25" → 25
  if (cmd.length() > 2)
    return cmd.substring(2).toInt();
  return 0;
}

// ============================================================================
// EXAMPLE 8: TOUCH INTERFACE (IF AVAILABLE)
// ============================================================================

// Simple touch zones:
// Left third:  Previous/Pause
// Center third: Start/Resume
// Right third: Skip/Reset

void ProcessTouchInput(uint16_t x, uint16_t y)
{
  // 320x240 display
  if (x < 107)
  {
    // Left third
    PauseTimer();
  }
  else if (x < 213)
  {
    // Center third
    if (timerRunning)
      PauseTimer();
    else
      ResumeTimer();
  }
  else
  {
    // Right third
    ResetTimer();
  }
}

// ============================================================================
// EXAMPLE 9: COMPLETE SECOND-BY-SECOND UPDATE (ADD TO EverySecondCheck)
// ============================================================================

// Add this to your EverySecondCheck() function:
void UpdatePomodoroEverySecond(void)
{
  if (!pomodoroMode)
    return;

  // Update countdown
  UpdatePomodoroTimer();

  // Update display (every second, but ShowCharsILI9341 is called in main loop)
  // The Strippos array was already populated in EveryMinuteUpdate
  // Just let ShowCharsILI9341() render it each second
}

// ============================================================================
// EXAMPLE 10: COLOR THEME TEMPLATES
// ============================================================================

// Different color schemes for the timer
struct TimerTheme
{
  uint32_t workColor;
  uint32_t breakColor;
  uint32_t labelColor;
};

TimerTheme themes[] = {
  {0xFF0000, 0x00FF00, 0xFFFF00},  // Red/Green/Yellow (tomato theme)
  {0xFF4500, 0x00CED1, 0xFFD700},  // Orange-Red/Turquoise/Gold
  {0xDC143C, 0x32CD32, 0xFFFFFF},  // Crimson/Lime Green/White
  {0xFF1493, 0x00FA9A, 0x87CEEB},  // Deep Pink/Spring Green/Sky Blue
};

int currentThemeIdx = 0;

void SetTimerTheme(int themeIdx)
{
  if (themeIdx < 4)
  {
    currentThemeIdx = themeIdx;
  }
}

uint32_t GetSessionColor(void)
{
  if (currentState == STATE_WORK)
    return themes[currentThemeIdx].workColor;
  else
    return themes[currentThemeIdx].breakColor;
}

#endif // POMODORO_IMPLEMENTATION_EXAMPLES_H
