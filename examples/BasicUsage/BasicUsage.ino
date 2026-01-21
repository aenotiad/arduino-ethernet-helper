/*
 * EthernetHelper - Basic Usage Example
 *
 * Demonstrates robust Ethernet initialization with DHCP and static IP fallback.
 *
 * Hardware:
 * - Arduino Uno/Mega with Ethernet Shield
 *
 * Features demonstrated:
 * - DHCP with static IP fallback
 * - Hardware and cable detection
 * - Automatic DHCP lease renewal
 * - Link status monitoring
 */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetHelper.h>

// Network configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress fallbackIP(192, 168, 1, 100);  // Used if DHCP fails

void setup() {
  Serial.begin(115200);
  Serial.println("EthernetHelper Basic Example");
  Serial.println("============================");

  // Initialize Ethernet with robust error handling
  Serial.println("Initializing Ethernet...");

  if (!EthernetHelper::begin(mac, fallbackIP)) {
    // Fatal error - Ethernet hardware not found
    Serial.println();
    Serial.println("FATAL ERROR: Ethernet shield not detected!");
    Serial.println("Please check:");
    Serial.println("  1. Shield is properly seated");
    Serial.println("  2. SPI connections (pins 10-13)");
    Serial.println("  3. Power supply is adequate");
    Serial.println();
    Serial.println("System halted.");

    // Halt - cannot continue without hardware
    while (true) {
      delay(1000);
    }
  }

  Serial.println();
  Serial.println("Ethernet initialized successfully!");
  Serial.println();

  // Print final network configuration
  EthernetHelper::printConfig();

  Serial.println();
  Serial.println("System ready.");
}

void loop() {
  // CRITICAL: Call maintain() regularly to handle:
  // - DHCP lease renewal
  // - Link status monitoring
  EthernetHelper::maintain();

  // Check link status
  static bool wasConnected = false;
  bool isConnected = EthernetHelper::isLinkUp();

  if (isConnected && !wasConnected) {
    Serial.println("*** Network cable RECONNECTED ***");
  }
  else if (!isConnected && wasConnected) {
    Serial.println("*** WARNING: Network cable DISCONNECTED ***");
  }

  wasConnected = isConnected;

  // Your application code here
  delay(100);
}
