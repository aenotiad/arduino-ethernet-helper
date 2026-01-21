# GitHub Setup Guide for EthernetHelper

This guide walks you through publishing the EthernetHelper library to GitHub.

## Prerequisites

- GitHub account
- Git installed on your computer
- Arduino IDE (to test the library)

## Step 1: Create GitHub Repository

1. Go to https://github.com/new
2. Repository settings:
   - **Name:** `EthernetHelper` or `arduino-ethernet-helper`
   - **Description:** "Robust Ethernet initialization library for Arduino with hardware detection, DHCP fallback, and connection monitoring"
   - **Visibility:** Public
   - **DO NOT** initialize with README, .gitignore, or license (we already have these)
3. Click "Create repository"

## Step 2: Initialize Git Repository

Open terminal and navigate to the library directory:

```bash
cd /Users/aenotiad/Documents/Arduino/libraries/EthernetHelper

# Initialize git repository
git init

# Add all files
git add .

# Create first commit
git commit -m "Initial release v1.0.0

- DHCP with automatic static IP fallback
- Hardware and cable detection
- Automatic DHCP lease renewal
- Link status monitoring
- Safe, non-blocking operation
- Comprehensive diagnostics
- Two examples: BasicUsage and PeriodicDHCPRetry"
```

## Step 3: Link to GitHub and Push

Replace `YOUR_USERNAME` with your actual GitHub username:

```bash
# Add GitHub remote
git remote add origin https://github.com/YOUR_USERNAME/EthernetHelper.git

# Push to GitHub
git branch -M main
git push -u origin main
```

## Step 4: Create a Release (for Arduino Library Manager)

For the library to be installable via Arduino Library Manager, create a release:

1. Go to your repository on GitHub
2. Click "Releases" → "Create a new release"
3. Release settings:
   - **Tag version:** `v1.0.0` (must start with 'v')
   - **Release title:** `v1.0.0 - Initial Release`
   - **Description:**
     ```markdown
     Initial release of EthernetHelper library.

     ## Features
     - ✅ DHCP with automatic static IP fallback
     - ✅ Hardware detection (missing/faulty shields)
     - ✅ Cable detection (unplugged cables)
     - ✅ Automatic DHCP lease renewal
     - ✅ Link status monitoring
     - ✅ Non-blocking operation
     - ✅ Comprehensive diagnostics

     ## Examples
     - BasicUsage - Simple Ethernet initialization
     - PeriodicDHCPRetry - DHCP retry pattern for unreliable networks

     ## Installation

     **Via Arduino Library Manager:**
     1. Open Arduino IDE
     2. Go to Sketch → Include Library → Manage Libraries
     3. Search for "EthernetHelper"
     4. Click Install

     **Manual Installation:**
     1. Download the release ZIP
     2. In Arduino IDE: Sketch → Include Library → Add .ZIP Library
     3. Select the downloaded file
     ```
4. Click "Publish release"

## Step 5: Submit to Arduino Library Manager (Optional)

To make the library officially available in Arduino Library Manager:

1. Fork the Arduino Library Manager repository:
   https://github.com/arduino/library-registry

2. Add your library to the list (it will be auto-indexed if public)

3. The Arduino team will review and add it

**Note:** This can take several weeks. Your library is still usable via direct GitHub installation.

## Step 6: Add Badges to README (Optional)

Add these badges to the top of your README.md:

```markdown
[![GitHub release](https://img.shields.io/github/release/YOUR_USERNAME/EthernetHelper.svg)](https://github.com/YOUR_USERNAME/EthernetHelper/releases)
[![Arduino Library](https://www.ardu-badge.com/badge/EthernetHelper.svg)](https://www.ardu-badge.com/EthernetHelper)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
```

## Testing the Published Library

After publishing, test that others can install it:

```bash
# Clone from GitHub
cd ~/Documents/Arduino/libraries/
git clone https://github.com/YOUR_USERNAME/EthernetHelper.git

# Restart Arduino IDE
# Verify library appears in Sketch → Include Library
```

## Maintenance Workflow

When you make updates:

```bash
# Make your changes to the code

# Update version in library.properties
# Edit version line to: version=1.1.0

# Commit changes
git add .
git commit -m "Version 1.1.0 - Description of changes"

# Push to GitHub
git push

# Create new release on GitHub (v1.1.0)
```

## File Structure Verification

Your repository should have this structure:

```
EthernetHelper/
├── .gitignore
├── EthernetHelper.h
├── LICENSE
├── README.md
├── keywords.txt
├── library.properties
└── examples/
    ├── BasicUsage/
    │   └── BasicUsage.ino
    └── PeriodicDHCPRetry/
        └── PeriodicDHCPRetry.ino
```

✅ All files created and ready for GitHub!

## Troubleshooting

**Problem:** "Permission denied" when pushing
**Solution:** Set up SSH key or use personal access token (https://github.com/settings/tokens)

**Problem:** Library doesn't appear in Arduino Library Manager
**Solution:** Ensure `library.properties` is correct and create a release with version tag starting with 'v'

**Problem:** Examples don't show up in Arduino IDE
**Solution:** Ensure examples are in `examples/` directory with proper structure

## Next Steps

1. Create the GitHub repository
2. Push your code
3. Create v1.0.0 release
4. Share the repository URL with others
5. Consider writing a blog post or tutorial about the library

---

**Need help?** Open an issue on GitHub or check Arduino library specification:
https://arduino.github.io/arduino-cli/latest/library-specification/
