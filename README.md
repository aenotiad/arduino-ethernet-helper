# EthernetHelper Library

So I've been using the standard Arduino Ethernet Library for years to manage Ethernet connected Arduinos in my home automation system. As this was always an incremental project I made small changes and wrappers to the basic library in order to simplify attaching devices to a real, operating environment with power failures, crashes etc. As my codebase started getting out of control in terms of different workarounds and implementations I used Claude Code to rationalise all the different options into a well structured and documented helper Library. 
The result is this EthernetHelper library, a robust Ethernet initialization library for Arduino with hardware detection, DHCP fallback, and connection monitoring. I hope it is useful to more than just me.

Andreas (with the help of Claude Code)

## Features

- ✅ **Hardware Detection** - Detects missing/faulty Ethernet shields
- ✅ **Cable Detection** - Detects unplugged network cables
- ✅ **DHCP with Fallback** - Tries DHCP first, falls back to static IP automatically
- ✅ **DHCP Lease Maintenance** - Automatically renews DHCP leases
- ✅ **Link Monitoring** - Tracks cable connection status
- ✅ **Detailed Diagnostics** - Comprehensive logging of network status
- ✅ **Non-blocking** - Never hangs on network failures

## Problem Solved

### Before (Standard Arduino Ethernet)

```cpp
void setup() {
  if (Ethernet.begin(mac) == 0) {
    Serial.println("DHCP failed");
    for(;;);  // ⚠️ INFINITE HANG - Device becomes unresponsive!
  }
}
```

**Issues:**
- Infinite hang if DHCP fails
- No hardware detection
- No cable detection
- No fallback option
- Device requires physical reset

### After (With EthernetHelper)

```cpp
void setup() {
  if (!EthernetHelper::begin(mac, fallbackIP)) {
    Serial.println("Ethernet hardware not found");
    while(true) { delay(1000); }  // Only halts on hardware error
  }
  // Device is now online (either DHCP or static IP)
}

void loop() {
  EthernetHelper::maintain();  // Handle DHCP renewal and monitoring
}
```

**Benefits:**
- Falls back to static IP if DHCP fails
- Detects hardware problems
- Detects cable issues
- Automatic DHCP renewal
- Detailed diagnostics

## Installation

1. Copy the `EthernetHelper` folder to your Arduino `libraries` directory
2. Include in your sketch: `#include <EthernetHelper.h>`

## Quick Start

```cpp
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetHelper.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress fallbackIP(192, 168, 1, 100);

void setup() {
  Serial.begin(115200);

  // Initialize Ethernet with DHCP and static IP fallback
  if (!EthernetHelper::begin(mac, fallbackIP)) {
    Serial.println("FATAL: Ethernet hardware not found!");
    while(true) { delay(1000); }
  }

  // Ethernet is ready - either via DHCP or static IP
}

void loop() {
  // Maintain DHCP lease and monitor connection
  EthernetHelper::maintain();

  // Your code here
}
```

## API Reference

### Static Methods

#### `begin()`

Initialize Ethernet with DHCP and automatic fallback.

```cpp
bool begin(
  byte* mac,                                      // MAC address (6 bytes)
  IPAddress fallbackIP,                           // Static IP if DHCP fails
  IPAddress gateway = IPAddress(0, 0, 0, 0),     // Gateway (auto: fallbackIP with .1)
  IPAddress subnet = IPAddress(255, 255, 255, 0), // Subnet mask
  IPAddress dns = IPAddress(0, 0, 0, 0),         // DNS server (auto: same as gateway)
  unsigned long dhcpTimeout = 60000               // DHCP timeout in ms (default: 60s)
)
```

**Returns:** `true` if Ethernet initialized successfully (either DHCP or static IP), `false` only if hardware not found (fatal)

**Auto-detection behavior:**
- Gateway `0.0.0.0` → Uses `fallbackIP` with last octet set to `.1` (e.g., `192.168.1.100` → `192.168.1.1`)
- DNS `0.0.0.0` → Uses same IP as gateway

**Important:** After falling back to static IP, there is a 1-second delay to allow Ethernet shield initialization.

**Example:**
```cpp
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress fallbackIP(192, 168, 1, 100);

if (!EthernetHelper::begin(mac, fallbackIP)) {
  // Hardware not found - cannot continue
  Serial.println("FATAL: No Ethernet shield detected");
  while(true) { delay(1000); }
}
```

**With Custom Gateway/DNS:**
```cpp
IPAddress fallbackIP(192, 168, 10, 50);
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

EthernetHelper::begin(mac, fallbackIP, gateway, subnet, dns);
```

**With Custom DHCP Timeout:**
```cpp
// Use 15 second timeout instead of default 60 seconds
// Note: Must specify all intermediate parameters to reach dhcpTimeout (6th param)
// Use IPAddress(0,0,0,0) for gateway/DNS to auto-detect from fallbackIP
EthernetHelper::begin(mac, fallbackIP,
                      IPAddress(0,0,0,0),        // Gateway (auto-detect)
                      IPAddress(255,255,255,0),  // Subnet
                      IPAddress(0,0,0,0),        // DNS (auto-detect)
                      15000);                    // DHCP timeout in milliseconds
```

---

#### `maintain()`

Maintain Ethernet connection - handles DHCP renewal and link monitoring.

```cpp
void maintain(unsigned long linkCheckInterval = 10000)
```

**Parameters:**
- `linkCheckInterval` - How often to check link status in milliseconds (default: 10000 = 10 seconds)

**Example:**
```cpp
void loop() {
  EthernetHelper::maintain();  // Call regularly in loop()

  // Check every 5 seconds instead of default 10
  // EthernetHelper::maintain(5000);
}
```

**What it does:**
- Automatically renews DHCP leases when needed (only if currently using DHCP)
- Monitors cable connection status periodically
- Logs connection changes to Serial

**Notes:**
- DHCP renewal only runs when the library obtained IP via DHCP (not when using static IP fallback)
- Link status check runs every `linkCheckInterval` milliseconds
- First link check happens `linkCheckInterval` ms after calling `begin()`

---

#### `isLinkUp()`

Check if Ethernet cable is connected.

```cpp
bool isLinkUp()
```

**Returns:** `true` if cable connected, `false` if disconnected

**Example:**
```cpp
if (!EthernetHelper::isLinkUp()) {
  Serial.println("WARNING: Network cable unplugged!");
}
```

---

#### `printConfig()`

Print current network configuration to Serial.

```cpp
void printConfig()
```

**Example Output:**
```
=== Network Configuration ===
IP: 192.168.1.100
Gateway: 192.168.1.1
Subnet: 255.255.255.0
DNS: 8.8.8.8
Mode: DHCP
Link: Connected
============================
```

**Example:**
```cpp
void setup() {
  Serial.begin(115200);

  EthernetHelper::begin(mac, fallbackIP);
  EthernetHelper::printConfig();  // Show final configuration
}
```

## Diagnostic Output

### Successful DHCP
```
=== Ethernet Initialization ===
Attempting DHCP configuration...
  DHCP IP: 192.168.1.150
  Gateway: 192.168.1.1
  Subnet: 255.255.255.0
  DNS: 192.168.1.1
  Link: Connected
=== Ethernet Ready ===
```

### DHCP Failed - Cable Unplugged
```
=== Ethernet Initialization ===
Attempting DHCP configuration...
DHCP failed!
WARNING: Ethernet cable not connected
Falling back to static IP configuration...
  Static IP: 192.168.1.100
  Gateway: 192.168.1.1
  Subnet: 255.255.255.0
  DNS: 192.168.1.1
  Link: Disconnected
=== Ethernet Ready ===
```

### Hardware Not Found
```
=== Ethernet Initialization ===
Attempting DHCP configuration...
DHCP failed!
ERROR: Ethernet shield not found!
Cannot continue without hardware.
```

### DHCP Renewal (during operation)
```
DHCP: Renewed - IP: 192.168.1.150
```

### Cable Status Changes
```
Link status changed: Cable DISCONNECTED
Link status changed: Cable CONNECTED
```

## Complete Example

```cpp
/*
 * Complete Ethernet Example with EthernetHelper
 */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetHelper.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress fallbackIP(192, 168, 1, 100);

EthernetServer server(80);

void setup() {
  Serial.begin(115200);

  Serial.println("Starting device...");

  // Initialize Ethernet with robust error handling
  if (!EthernetHelper::begin(mac, fallbackIP)) {
    Serial.println("FATAL: Cannot initialize Ethernet");
    Serial.println("Check hardware connection!");
    while(true) {
      delay(1000);  // Halt - hardware problem
    }
  }

  // Print final network configuration
  EthernetHelper::printConfig();

  // Start web server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // Maintain Ethernet connection (CRITICAL - call regularly!)
  EthernetHelper::maintain();

  // Check if cable is connected
  if (!EthernetHelper::isLinkUp()) {
    // Cable unplugged - maybe skip network operations
    return;
  }

  // Handle web server requests
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Client connected");

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // Process request...
      }
    }

    client.stop();
    Serial.println("Client disconnected");
  }
}
```

## Advanced Usage

### Custom DHCP Timeout

The `dhcpTimeout` parameter (6th parameter) controls how long to wait for DHCP before falling back to static IP.

**Default:** 60000ms (60 seconds)

```cpp
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress fallbackIP(192, 168, 1, 100);

// Custom timeout examples:
// Fast boot (15 seconds)
EthernetHelper::begin(mac, fallbackIP,
                      IPAddress(0,0,0,0),        // Gateway (auto-detect)
                      IPAddress(255,255,255,0),  // Subnet
                      IPAddress(0,0,0,0),        // DNS (auto-detect)
                      15000);                    // 15 second timeout

// Slow network (120 seconds)
EthernetHelper::begin(mac, fallbackIP,
                      IPAddress(0,0,0,0),
                      IPAddress(255,255,255,0),
                      IPAddress(0,0,0,0),
                      120000);                   // 120 second timeout
```

**Note:** You must specify all intermediate parameters (gateway, subnet, DNS) to reach the timeout parameter. Use `IPAddress(0,0,0,0)` for auto-detection.

### Static IP Only (No DHCP)

If you want static IP only without DHCP attempt, use standard Ethernet library directly:

```cpp
IPAddress ip(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

Ethernet.begin(mac, ip, gateway, subnet);
```

### Periodic DHCP Retry

If DHCP fails initially, you can periodically retry to obtain a DHCP address while operating on static IP:

```cpp
const unsigned long DHCP_RETRY_INTERVAL = 60000;  // Retry every 60 seconds
const unsigned long DHCP_TIMEOUT = 15000;          // 15 second timeout
unsigned long lastDHCPRetry = 0;
bool usingStaticIP = false;

void setup() {
  // Initial attempt with shorter timeout
  if (!EthernetHelper::begin(mac, fallbackIP, IPAddress(0,0,0,0),
                              IPAddress(255,255,255,0), IPAddress(0,0,0,0),
                              DHCP_TIMEOUT)) {
    while (true) { delay(1000); }  // Hardware error
  }

  // Check if we fell back to static IP
  usingStaticIP = (Ethernet.localIP() == fallbackIP);
  lastDHCPRetry = millis();
}

void loop() {
  EthernetHelper::maintain();

  // Retry DHCP periodically if using static IP
  if (usingStaticIP && millis() - lastDHCPRetry >= DHCP_RETRY_INTERVAL) {
    if (EthernetHelper::isLinkUp() && Ethernet.begin(mac, DHCP_TIMEOUT) != 0) {
      Serial.println("DHCP retry successful!");
      usingStaticIP = false;
    }
    lastDHCPRetry = millis();
  }
}
```

**See also:** `examples/PeriodicDHCPRetry/` for complete implementation.

### Monitor Connection Changes

```cpp
bool wasConnected = false;

void loop() {
  EthernetHelper::maintain();

  bool isConnected = EthernetHelper::isLinkUp();

  if (isConnected && !wasConnected) {
    Serial.println("Network cable reconnected!");
    // Reinitialize services, reconnect to servers, etc.
  }
  else if (!isConnected && wasConnected) {
    Serial.println("Network cable disconnected!");
    // Pause network operations
  }

  wasConnected = isConnected;
}
```

## Troubleshooting

### "Ethernet shield was not found"
- Check that Ethernet shield is properly seated
- Verify SPI connections (pins 10, 11, 12, 13)
- Try different Ethernet shield
- Check power supply (shields can draw significant current)

### "Ethernet cable is not connected"
- Check cable is plugged in firmly
- Try different cable
- Verify router/switch is powered on
- Check cable crimping/damage

### "Failed to configure Ethernet using DHCP"
- Check DHCP server is running on network
- Verify network settings allow DHCP
- Try increasing DHCP timeout parameter
- Check router DHCP pool not exhausted
- Device will fall back to static IP automatically

### DHCP Works Initially but Fails to Renew
- Ensure `maintain()` is called regularly in `loop()`
- Check `maintain()` is not blocked by delays
- Verify DHCP lease time is reasonable (typically 24 hours)

## Technical Details

### DHCP Lease Maintenance

The library automatically calls `Ethernet.maintain()` **only when using DHCP** (not when using static IP fallback).

DHCP maintenance handles:
- **Renew:** Halfway through lease period
- **Rebind:** If renew fails, tries rebind at 7/8 of lease period
- **Result Codes:**
  - 0: Nothing happened
  - 1: Renew failed
  - 2: Renew success
  - 3: Rebind failed
  - 4: Rebind success

### Link Status Detection

Uses `Ethernet.linkStatus()` which returns:
- `LinkON`: Cable connected and link established
- `LinkOFF`: Cable disconnected or no link
- `Unknown`: Status cannot be determined (older shields)

**Initial state:** Link status starts as `Unknown` until first check in `maintain()`

### Hardware Detection

Uses `Ethernet.hardwareStatus()` which returns:
- `EthernetNoHardware`: No shield detected (SPI not responding)
- `EthernetW5100`: WIZnet W5100 chip detected
- `EthernetW5200`: WIZnet W5200 chip detected
- `EthernetW5500`: WIZnet W5500 chip detected

### Implementation Notes

**State tracking:**
- `usingDHCP` (static bool): Tracks whether DHCP was successful or static IP fallback was used
- `lastLinkStatus` (static EthernetLinkStatus): Stores previous link state to detect changes
- `lastLinkCheck` (static unsigned long): Timestamp of last link status check

**Ethernet.begin() parameter order:**
When falling back to static IP, the library calls:
```cpp
Ethernet.begin(mac, fallbackIP, dns, gateway, subnet);
```
Note the Arduino Ethernet library's parameter order: `(mac, ip, dns, gateway, subnet)`

**Timing behaviors:**
- DHCP attempt waits up to `dhcpTimeout` milliseconds (default 60 seconds)
- Static IP fallback includes 1-second `delay()` for shield initialization
- Link checks happen every `linkCheckInterval` milliseconds (default 10 seconds)
- First link check occurs `linkCheckInterval` ms after `begin()` completes

## Compatibility

- **Arduino Uno/Mega:** Full support
- **Ethernet Shield:** W5100, W5200, W5500
- **Arduino Ethernet Library:** Version 2.0.0 or higher recommended
- **Memory:** ~200 bytes SRAM overhead

## Best Practices

1. **Always call `maintain()`** in `loop()` - critical for DHCP renewal
2. **Check return value** of `begin()` - only `false` indicates hardware failure
3. **Use serial output** during development for diagnostics
4. **Test cable disconnect** scenarios in production code
5. **Reserve static IP** in DHCP server to avoid conflicts
6. **Document fallback IP** used by each device

## License

This library is released under the MIT License.

## Author

Andreas Enotiadis with the help of Claude Code, 2026.
