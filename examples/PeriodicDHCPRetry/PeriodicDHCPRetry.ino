/*
 * EthernetHelper - Periodic DHCP Retry Example
 *
 * Demonstrates automatic DHCP retry after initial fallback to static IP.
 * Useful when DHCP server may come online after device boots.
 *
 * Hardware:
 * - Arduino Uno/Mega with Ethernet Shield
 *
 * Features demonstrated:
 * - Initial DHCP attempt with static IP fallback
 * - Periodic retry to obtain DHCP address
 * - Configurable DHCP timeout
 * - Transition from static IP to DHCP when available
 */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetHelper.h>

// Network configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress fallbackIP(192, 168, 1, 100);

// DHCP retry configuration
const unsigned long DHCP_RETRY_INTERVAL = 60000;  // Retry every 60 seconds
const unsigned long DHCP_TIMEOUT = 15000;          // Wait max 15 seconds for DHCP
unsigned long lastDHCPRetry = 0;
bool usingStaticIP = false;

void setup() {
  Serial.begin(115200);
  Serial.println(F("EthernetHelper - Periodic DHCP Retry Example"));
  Serial.println(F("============================================="));
  Serial.println();

  // Initialize Ethernet with custom DHCP timeout (15 seconds instead of default 60)
  Serial.println(F("Initializing Ethernet..."));

  if (!EthernetHelper::begin(mac, fallbackIP, IPAddress(0,0,0,0),
                              IPAddress(255,255,255,0), IPAddress(0,0,0,0),
                              DHCP_TIMEOUT)) {
    // Fatal error - hardware not found
    Serial.println(F("FATAL: Ethernet hardware not found!"));
    while (true) { delay(1000); }
  }

  // Check if we got DHCP or fell back to static IP
  IPAddress currentIP = Ethernet.localIP();
  if (currentIP == fallbackIP) {
    Serial.println(F("Using static IP - will retry DHCP periodically"));
    usingStaticIP = true;
  } else {
    Serial.println(F("DHCP successful"));
    usingStaticIP = false;
  }

  Serial.println();
  EthernetHelper::printConfig();
  Serial.println();
  Serial.println(F("System ready."));
  Serial.println();

  lastDHCPRetry = millis();
}

void loop() {
  // Maintain existing connection
  EthernetHelper::maintain();

  // If using static IP, periodically retry DHCP
  if (usingStaticIP) {
    if (millis() - lastDHCPRetry >= DHCP_RETRY_INTERVAL) {
      attemptDHCPRetry();
      lastDHCPRetry = millis();
    }
  }

  // Your application code here
  delay(100);
}

void attemptDHCPRetry() {
  Serial.println(F("=== Attempting DHCP Retry ==="));

  // Check if cable is connected before trying
  if (!EthernetHelper::isLinkUp()) {
    Serial.println(F("Cable disconnected - skipping DHCP retry"));
    Serial.println();
    return;
  }

  // Try DHCP with shorter timeout
  unsigned long startTime = millis();
  if (Ethernet.begin(mac, DHCP_TIMEOUT) != 0) {
    // Success! Got DHCP
    Serial.println(F("DHCP retry SUCCESSFUL!"));
    Serial.print(F("New IP: "));
    Serial.print(Ethernet.localIP()[0]);
    Serial.print(F("."));
    Serial.print(Ethernet.localIP()[1]);
    Serial.print(F("."));
    Serial.print(Ethernet.localIP()[2]);
    Serial.print(F("."));
    Serial.println(Ethernet.localIP()[3]);

    usingStaticIP = false;

    Serial.println();
    EthernetHelper::printConfig();
    Serial.println();
  } else {
    // Still no DHCP - stay on static IP
    Serial.print(F("DHCP retry failed ("));
    Serial.print(millis() - startTime);
    Serial.println(F("ms)"));

    // Check if we lost our static IP and need to restore it
    IPAddress currentIP = Ethernet.localIP();
    if (currentIP != fallbackIP) {
      Serial.println(F("Restoring static IP configuration..."));
      IPAddress gateway(fallbackIP[0], fallbackIP[1], fallbackIP[2], 1);
      IPAddress subnet(255, 255, 255, 0);
      Ethernet.begin(mac, fallbackIP, gateway, gateway, subnet);
      delay(1000);
      Serial.println(F("Static IP restored"));
    }
    Serial.println();
  }
}
