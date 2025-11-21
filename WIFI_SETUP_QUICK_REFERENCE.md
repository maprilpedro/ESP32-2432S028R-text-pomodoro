# WiFi Setup - Quick Reference Card

## 🔐 Secure Credentials System

Your WiFi password is **never committed to GitHub**!

## First Time Setup (This Computer)

```bash
cd ESP32_Pomodoro_WordClock/
cp credentials.h.example credentials.h
nano credentials.h    # or open in any text editor
```

Edit these lines:
```cpp
#define WIFI_SSID "YourNetworkName"
#define WIFI_PASSWORD "YourPassword123"
```

Save and close. **Done!**

## Setup on Another Laptop

```bash
git clone <your-repo-url>
cd ESP32-2432S028R-text-pomodoro/ESP32_Pomodoro_WordClock/
cp credentials.h.example credentials.h
nano credentials.h    # Add your WiFi info
```

**That's it!** The file won't be pushed to GitHub.

## What Gets Pushed to GitHub?

✅ **credentials.h.example** - Template file with dummy data
❌ **credentials.h** - Your actual password (blocked by .gitignore)

## Verify It's Working

```bash
# Check git status (credentials.h should NOT appear)
git status

# Verify it's ignored
git check-ignore ESP32_Pomodoro_WordClock/credentials.h
# Should output: ESP32_Pomodoro_WordClock/credentials.h
```

## Common Issues

### "credentials.h not found" error
```bash
cp credentials.h.example credentials.h
```

### Accidentally committed credentials.h?
```bash
git rm --cached ESP32_Pomodoro_WordClock/credentials.h
git commit -m "Remove credentials"
git push
# CHANGE YOUR WIFI PASSWORD (it's now public!)
```

## File Structure

```
Your Computer:
├── credentials.h.example  ← Template (shared on GitHub)
├── credentials.h          ← Your WiFi password (stays private)

GitHub Repository:
├── credentials.h.example  ← ✅ Committed
├── credentials.h          ← ❌ Blocked by .gitignore
```

## For More Details

See **CREDENTIALS_SETUP.md** for:
- Timezone configuration
- Security best practices
- Advanced setup options
- Troubleshooting guide

---

**🔒 Your WiFi password stays on your computer. Safe to push to GitHub!**
