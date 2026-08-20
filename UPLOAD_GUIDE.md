# Upload Files to GitHub Releases

## Quick Guide

### Step 1: Create GitHub Release

1. Go to: https://github.com/MouseTi/NexConnectSetup/releases
2. Click **"Draft a new release"**
3. Tag version: `v1.0.0`
4. Release title: `NexConnect v1.0.0`
5. Description: Initial release

### Step 2: Prepare Files to Upload

Collect these files from your NexConnect build:

```
F:\Nexconnect\build\Release\NexConnect.exe          (Main executable)
F:\Nexconnect\build\Release\nexus_runtime.dll       (Runtime DLL)
```

You may also need these Qt DLLs (if NexConnect.exe needs them):
```
Qt6Core.dll
Qt6Gui.dll
Qt6Widgets.dll
Qt6Network.dll
Qt6WebEngineCore.dll
Qt6WebEngineWidgets.dll
platforms\qwindows.dll
```

### Step 3: Upload Files

1. In the release draft, click **"Attach binaries"**
2. Upload all files listed above
3. Click **"Publish release"**

### Step 4: Get Download URLs

After publishing, right-click each file and copy link. URLs will be like:
```
https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0/NexConnect.exe
https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0/nexus_runtime.dll
```

### Step 5: Update Installer Script

Open `installer-downloader.nsi` and verify the `DOWNLOAD_URL`:
```nsis
!define DOWNLOAD_URL "https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0"
```

If you need to add more files, add them in the Section:
```nsis
DetailPrint "Downloading Qt6Core.dll..."
inetc::get /CAPTION "Downloading..." "${DOWNLOAD_URL}/Qt6Core.dll" "$INSTDIR\Qt6Core.dll" /END
```

### Step 6: Rebuild Installer

```batch
build_nsis.bat
```

### Step 7: Test

1. Run `NexConnect-Setup.exe`
2. Check if it downloads all files
3. Check if desktop shortcut is created
4. Check if NexConnect launches correctly

## Alternative: Upload via Command Line (git)

```batch
# Create a release using GitHub CLI (gh)
gh release create v1.0.0 ^
    F:\Nexconnect\build\Release\NexConnect.exe ^
    F:\Nexconnect\build\Release\nexus_runtime.dll ^
    --title "NexConnect v1.0.0" ^
    --notes "Initial release"
```

## Quick Upload Script

I can create a script that:
1. Copies all necessary files
2. Creates a release
3. Uploads everything

Want me to create that?
