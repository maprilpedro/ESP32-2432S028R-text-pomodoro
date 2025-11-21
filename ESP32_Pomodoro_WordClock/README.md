# ESP32 Pomodoro Timer with Word Clock

An interactive Pomodoro timer for the ESP32-2432S028R (Cheap Yellow Display) that displays time in words and integrates a classic word clock when idle.

## Features

✓ **Word Clock Display** - Shows current time in Dutch word format when timer is idle
✓ **Pomodoro Timer** - Countdown displayed in English words (e.g., "TWENTY FOUR MINUTES")
✓ **Touch Controls** - Intuitive touch zones for start, pause, and stop
✓ **Configuration Popup** - Set timer duration (1-60 minutes) via touchscreen
✓ **Visual Alerts** - Screen flashes red then orange 3 times when timer completes
✓ **RGB LED Status** - Red during work session, yellow when paused
✓ **Persistent Settings** - Timer duration saved to flash memory

## Hardware Requirements

- **Board**: ESP32-2432S028R (Cheap Yellow Display)
- **Display**: 2.8" ILI9341 TFT (320×240 pixels, built-in)
- **Touch**: Resistive touchscreen XPT2046 (built-in)
- **RGB LED**: Built-in status indicator
- **Power**: 5V USB (micro-USB)

## Touch Controls

### Timer Idle Mode (Word Clock Showing)
- **Tap top-left** → Open configuration popup

### Configuration Popup
- **Tap "-" button** → Decrease duration
- **Tap "+" button** → Increase duration
- **Tap "START" button** → Begin Pomodoro timer

### Timer Running Mode
- **Tap top-left** → (reserved for future use)
- **Tap top-right** → Pause/Resume timer
- **Tap bottom-right** → Stop timer (returns to word clock)

## Touch Zone Map

```
┌─────────────────────────────┐
│ START/   │     PAUSE        │  Top half (y < 80)
│ CONFIG   │                  │
│          │                  │
├──────────┴──────────────────┤
│          │                  │  Bottom half
│          │      STOP        │
│          │   (bottom-right) │
└──────────┴──────────────────┘
 x<160      x>160
```

## Installation

### 1. Install Required Libraries

Open Arduino IDE Library Manager (Sketch → Include Library → Manage Libraries) and install:

```
- Adafruit GFX Library (1.11.3+)
- Adafruit ILI9341 (1.6.0+)
- XPT2046_Touchscreen (1.4.0+)
- NimBLE-Arduino (1.4.0+) [for future BLE support]
- ESPNtpClient (0.2.5+) [for future WiFi/NTP support]
- AsyncTCP (1.1.1+) [for future web interface]
- ESPAsyncWebServer (1.2.3+) [for future web interface]
- AsyncElegantOTA (2.2.7+) [for future OTA updates]
```

### 2. Install ESP32 Board Support

1. Go to **File → Preferences**
2. Add to "Additional Board Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to **Tools → Board → Boards Manager**
4. Search for "esp32" and install **ESP32 by Espressif Systems** (version 3.0.0 or higher)

### 3. Setup WiFi Credentials (IMPORTANT!)

**For security, WiFi credentials are NOT stored in the main code.**

1. **Copy the credentials template**:
   ```
   Copy: credentials.h.example
   To:   credentials.h
   ```

2. **Edit credentials.h** with your actual WiFi information:
   ```cpp
   #define WIFI_SSID "Your_Network_Name"
   #define WIFI_PASSWORD "Your_Password"
   #define TIMEZONE "EST5EDT,M3.2.0,M11.1.0"  // Optional
   ```

3. **The file `credentials.h` is in .gitignore** and will NOT be pushed to GitHub
4. **On your other laptop**: Copy `credentials.h.example` to `credentials.h` and add credentials again

**Note**: The sketch won't compile without `credentials.h` file!

### 4. Configure Board Settings

Set the following in **Tools** menu:

```
Board: "ESP32 Dev Module"
Flash Size: 4MB (32Mb)
Partition Scheme: "Default 4MB with spiffs"
Upload Speed: 921600
CPU Frequency: 240MHz (WiFi/BT)
Flash Frequency: 80MHz
Flash Mode: QIO
Core Debug Level: None
```

### 5. Upload the Sketch

1. Open `ESP32_Pomodoro_WordClock.ino` in Arduino IDE
2. Connect your ESP32-2432S028R via USB
3. Select the correct COM port in **Tools → Port**
4. Click **Upload** (→)
5. Wait for "Done uploading" message

## First Time Setup

1. **Power on the device** - Word clock should appear
2. **Tap top-left corner** - Configuration popup appears
3. **Adjust duration** using - and + buttons (default: 25 minutes)
4. **Tap START** - Pomodoro timer begins
5. **Watch the countdown** in words (e.g., "TWENTY FOUR MINUTES THIRTY SECONDS")
6. **When complete** - Screen flashes red/orange 3 times, returns to word clock

## How It Works

### Display System

The timer uses a **16×8 character grid** (128 total positions) to display words:

- **Rows 0-1**: Minute number in words
- **Row 2**: "MINUTES" label
- **Rows 3-4**: Second number in words
- **Row 5**: "SECONDS" label
- **Row 7**: Status ("FOCUS" or "PAUSED")

### State Machine

```
STATE_IDLE → Shows word clock (Dutch time)
    ↓ (tap top-left)
STATE_CONFIG → Configuration popup
    ↓ (tap START)
STATE_WORK → Countdown timer running (RED LED)
    ↓ (tap top-right)
STATE_PAUSED → Timer paused (YELLOW LED)
    ↓ (tap bottom-right)
STATE_IDLE → Returns to word clock

STATE_WORK → (timer reaches 0:00)
STATE_COMPLETE → Flash animation
    ↓ (automatic)
STATE_IDLE → Returns to word clock
```

### RGB LED Indicators

- **OFF** (all LEDs off) - Idle, showing word clock
- **RED** - Work session in progress
- **YELLOW** - Timer paused
- **RED/ORANGE flashing** - Timer completed

## Customization

### Change Default Duration

Edit in the code (line ~90):

```cpp
uint32_t workDurationMinutes = 25;  // Change to desired minutes
```

### Change Display Colors

Edit in `DisplayPomodoroTimer()` function (around line 245):

```cpp
uint32_t timeColor = 0xFF0000;   // Work session color (red)
uint32_t labelColor = 0xFF8C00;  // Label color (orange)
```

Available colors defined in `Colors.h`:
- `0xFF0000` - Red
- `0x00FF00` - Green
- `0x0000FF` - Blue
- `0xFFFF00` - Yellow
- `0xFF8C00` - Dark orange
- `0xFFD700` - Gold
- See `Colors.h` for 140+ more colors

### Add Sound/Buzzer

Connect a passive buzzer to GPIO 27 and add to `PlayCompletionAlert()`:

```cpp
void PlayCompletionAlert() {
  // Existing flash code...

  // Add buzzer tones
  tone(27, 1000, 200);  // 1kHz for 200ms
  delay(300);
  tone(27, 1500, 200);  // 1.5kHz for 200ms
  delay(300);
  tone(27, 2000, 400);  // 2kHz for 400ms
}
```

## Troubleshooting

### Touch Not Responding
- Check touchscreen library is installed: `XPT2046_Touchscreen`
- Verify pin definitions: `TOUCH_CS = 33`, `TOUCH_IRQ = 36`
- Try adjusting touch mapping values in `HandleTouch()` function

### Display Freezes
- Ensure GPIO 2 is only used for TFT_DC (don't use LDR on GPIO 34)
- Reset device and re-upload sketch
- Check power supply provides stable 5V

### Colors Look Wrong
- RGB LED is **active LOW** (LOW = ON, HIGH = OFF)
- Verify pin connections: Red=GPIO4, Green=GPIO16, Blue=GPIO17
- Check `SetStatusLED()` function logic

### Timer Not Counting
- Open Serial Monitor (115200 baud) to check for errors
- Verify time is being updated in `loop()` function
- Check `UpdatePomodoro()` is being called every second

### Words Don't Display Correctly
- Verify `Strippos[]` array has 128 elements
- Check `ColorLeds()` function isn't exceeding array bounds
- Ensure `ShowCharsILI9341()` is being called after updates

## Future Enhancements

Planned features (not yet implemented):

- [ ] WiFi configuration via web interface
- [ ] NTP time synchronization
- [ ] Bluetooth BLE control from mobile app
- [ ] Session statistics (daily/lifetime counts)
- [ ] Auto-start/auto-stop scheduling
- [ ] Multiple timer presets (short break, long break)
- [ ] Full Dutch/English/German/French word clock
- [ ] Sound alerts via buzzer
- [ ] Brightness control based on ambient light

## File Structure

```
ESP32_Pomodoro_WordClock/
├── ESP32_Pomodoro_WordClock.ino    # Main sketch
├── Colors.h                         # RGB565 color definitions (140+ colors)
├── Webpage.h                        # Web interface (for future use)
└── README.md                        # This file
```

## Credits

**Based on**:
- ESP322432S028_WordClockV016 by Ed Nieuwenhuys
- GitHub: https://github.com/ednieuw/ESP32-2432S028R-display
- GitHub: https://github.com/ednieuw/Arduino-ESP32-Nano-Wordclock

**Modified by**: Pedro Fernandes (2025)

**Hardware Platform**: ESP32-2432S028R (Cheap Yellow Display)

## License

Open source - inherits licensing from Ed Nieuwenhuys' original work.
Respect original author credits when modifying or redistributing.

## Support

For issues or questions:
1. Check this README's troubleshooting section
2. Review original project documentation: `/claude.md`
3. Consult hardware specifications: `/ESP32-2432S028 Specifications-EN.pdf`
4. Open an issue on GitHub

---

**Enjoy your productive Pomodoro sessions!** 🍅
