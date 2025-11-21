# WiFi Credentials Setup Guide

## Overview

This project uses a **separate credentials file** to store sensitive WiFi information. This keeps your passwords secure and prevents them from being accidentally committed to GitHub.

## How It Works

```
credentials.h.example  →  Template file (committed to GitHub)
credentials.h          →  Your actual credentials (NOT committed)
.gitignore            →  Blocks credentials.h from Git
```

## Setup Instructions

### Option 1: First Time Setup (This Computer)

1. **Navigate to project folder**:
   ```bash
   cd ESP32_Pomodoro_WordClock/
   ```

2. **Copy the template**:
   ```bash
   cp credentials.h.example credentials.h
   ```

3. **Edit credentials.h** with your text editor:
   ```cpp
   #define WIFI_SSID "YourNetworkName"
   #define WIFI_PASSWORD "YourPassword123"
   #define TIMEZONE "EST5EDT,M3.2.0,M11.1.0"
   ```

4. **Save the file** - It's now ready to use!

### Option 2: Setup on Another Laptop

When you clone this repo on another computer:

1. **Clone the repository**:
   ```bash
   git clone <your-repo-url>
   cd ESP32-2432S028R-text-pomodoro/ESP32_Pomodoro_WordClock/
   ```

2. **Copy the template**:
   ```bash
   cp credentials.h.example credentials.h
   ```

3. **Edit credentials.h** with your WiFi info

4. **Done!** The .gitignore will prevent accidental commits

## Timezone Configuration

Common timezone strings (POSIX format):

### United States
```cpp
#define TIMEZONE "EST5EDT,M3.2.0,M11.1.0"     // Eastern
#define TIMEZONE "CST6CDT,M3.2.0,M11.1.0"     // Central
#define TIMEZONE "MST7MDT,M3.2.0,M11.1.0"     // Mountain
#define TIMEZONE "PST8PDT,M3.2.0,M11.1.0"     // Pacific
```

### Europe
```cpp
#define TIMEZONE "GMT0BST,M3.5.0/1,M10.5.0"   // UK
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3" // Central Europe
#define TIMEZONE "EET-2EEST,M3.5.0,M10.5.0/3" // Eastern Europe
```

### Other Regions
```cpp
#define TIMEZONE "<-03>3"                     // Brazil (São Paulo)
#define TIMEZONE "JST-9"                      // Japan
#define TIMEZONE "AEST-10AEDT,M10.1.0,M4.1.0" // Australia (Sydney)
#define TIMEZONE "NZST-12NZDT,M9.5.0,M4.1.0"  // New Zealand
```

More timezones: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

## Security Best Practices

### ✅ DO:
- Keep `credentials.h` on your local machine only
- Use different passwords for each WiFi network
- Update credentials.h when you change your WiFi password
- Check .gitignore includes `credentials.h` before committing

### ❌ DON'T:
- Never commit `credentials.h` to GitHub
- Don't share credentials.h in screenshots or logs
- Don't hardcode passwords in the main .ino file
- Don't remove `credentials.h` from .gitignore

## Troubleshooting

### Error: "credentials.h: No such file or directory"

**Problem**: The credentials file doesn't exist.

**Solution**:
```bash
cd ESP32_Pomodoro_WordClock/
cp credentials.h.example credentials.h
# Edit credentials.h with your WiFi info
```

### Error: "WIFI_SSID was not declared in this scope"

**Problem**: credentials.h exists but is empty or malformed.

**Solution**: Make sure credentials.h contains:
```cpp
#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#define WIFI_SSID "YourSSID"
#define WIFI_PASSWORD "YourPassword"

#endif
```

### Accidental Commit of credentials.h

**If you accidentally committed credentials.h to Git**:

1. **Remove from Git tracking**:
   ```bash
   git rm --cached ESP32_Pomodoro_WordClock/credentials.h
   git commit -m "Remove credentials file from tracking"
   git push
   ```

2. **Change your WiFi password** (credentials are now public!)

3. **Update your credentials.h** with new password

### Verifying .gitignore Works

**Check if credentials.h will be ignored**:
```bash
git status
# credentials.h should NOT appear in the list
```

**Test explicitly**:
```bash
git check-ignore ESP32_Pomodoro_WordClock/credentials.h
# Should output: ESP32_Pomodoro_WordClock/credentials.h
```

## Alternative: Use Environment Variables (Advanced)

For CI/CD or advanced users, you can use environment variables:

1. **Edit credentials.h**:
   ```cpp
   #ifndef CREDENTIALS_H
   #define CREDENTIALS_H

   #ifndef WIFI_SSID
     #define WIFI_SSID "DefaultSSID"
   #endif

   #ifndef WIFI_PASSWORD
     #define WIFI_PASSWORD "DefaultPassword"
   #endif

   #endif
   ```

2. **Set environment variables before compiling**:
   ```bash
   export WIFI_SSID="MyNetwork"
   export WIFI_PASSWORD="MyPassword123"
   arduino-cli compile --fqbn esp32:esp32:esp32
   ```

## File Structure

```
ESP32_Pomodoro_WordClock/
├── ESP32_Pomodoro_WordClock.ino  ← Main code (includes credentials.h)
├── credentials.h.example          ← Template (committed to Git)
├── credentials.h                  ← Your credentials (NOT in Git)
├── Colors.h
├── Webpage.h
└── README.md

../.gitignore                      ← Blocks credentials.h from Git
```

## What Gets Committed to GitHub

✅ **Committed** (safe to share):
- `credentials.h.example` - Template with dummy values
- `.gitignore` - Protects your credentials
- All .ino, .h, .md files (except credentials.h)

❌ **NOT Committed** (stays on your computer):
- `credentials.h` - Your actual WiFi password

## Quick Reference

| Task | Command |
|------|---------|
| Create credentials.h | `cp credentials.h.example credentials.h` |
| Edit credentials | `nano credentials.h` or open in text editor |
| Check if ignored | `git status` (should not list credentials.h) |
| Verify .gitignore works | `git check-ignore */credentials.h` |
| Remove from Git | `git rm --cached */credentials.h` |

## Support

If you have issues with WiFi credentials:
1. Verify credentials.h exists in ESP32_Pomodoro_WordClock/ folder
2. Check file contains valid C/C++ defines
3. Ensure WiFi SSID and password are correct
4. Open Serial Monitor (115200 baud) to see connection status

---

**Remember**: Your WiFi password is sensitive! Keep credentials.h private and secure.
