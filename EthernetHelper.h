/*
 * EthernetHelper.h
 *
 * Robust Ethernet initialization with hardware detection,
 * DHCP fallback, and connection monitoring.
 *
 * Usage:
 *   #include <EthernetHelper.h>
 *
 *   void setup() {
 *     byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xC3 };
 *     IPAddress fallbackIP(192, 168, 10, 50);
 *
 *     if (!EthernetHelper::begin(mac, fallbackIP)) {
 *       // Fatal hardware error - halt
 *       while(true) { delay(1000); }
 *     }
 *   }
 *
 *   void loop() {
 *     EthernetHelper::maintain(); // Renew DHCP lease and check connection
 *   }
 */

#ifndef ETHERNET_HELPER_H
#define ETHERNET_HELPER_H

#include <Ethernet.h>

class EthernetHelper {
public:
  /*
   * Initialize Ethernet with DHCP and static IP fallback
   *
   * @param mac - MAC address (6 bytes)
   * @param fallbackIP - Static IP to use if DHCP fails
   * @param gateway - Gateway IP (default: derived from fallbackIP)
   * @param subnet - Subnet mask (default: 255.255.255.0)
   * @param dns - DNS server (default: same as gateway)
   * @param dhcpTimeout - DHCP timeout in ms (default: 60000)
   *
   * @return true if Ethernet initialized successfully, false if hardware error
   */
  static bool begin(
    byte* mac,
    IPAddress fallbackIP,
    IPAddress gateway = IPAddress(0, 0, 0, 0),
    IPAddress subnet = IPAddress(255, 255, 255, 0),
    IPAddress dns = IPAddress(0, 0, 0, 0),
    unsigned long dhcpTimeout = 60000
  ) {
    Serial.println(F("=== Ethernet Initialization ==="));

    // Try DHCP first
    Serial.println(F("Attempting DHCP configuration..."));
    unsigned long startTime = millis();

    if (Ethernet.begin(mac, dhcpTimeout) == 0) {
      Serial.println(F("DHCP failed!"));

      // Check for hardware issues
      if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println(F("ERROR: Ethernet shield not found!"));
        Serial.println(F("Cannot continue without hardware."));
        return false; // Fatal error
      }

      // Check link status
      if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println(F("WARNING: Ethernet cable not connected"));
      }

      // Fallback to static IP
      Serial.println(F("Falling back to static IP configuration..."));

      // Use intelligent defaults if not provided
      if (gateway == IPAddress(0, 0, 0, 0)) {
        gateway = IPAddress(fallbackIP[0], fallbackIP[1], fallbackIP[2], 1);
      }
      if (dns == IPAddress(0, 0, 0, 0)) {
        dns = gateway;
      }

      Ethernet.begin(mac, fallbackIP, dns, gateway, subnet);
      delay(1000); // Give Ethernet shield time to initialize

      Serial.print(F("  Static IP: "));
      usingDHCP = false;
    } else {
      Serial.print(F("  DHCP IP: "));
      usingDHCP = true;
    }

    // Print final configuration
    printIPAddress(Ethernet.localIP());
    Serial.print(F("  Gateway: "));
    printIPAddress(Ethernet.gatewayIP());
    Serial.print(F("  Subnet: "));
    printIPAddress(Ethernet.subnetMask());
    Serial.print(F("  DNS: "));
    printIPAddress(Ethernet.dnsServerIP());

    // Print link status
    printLinkStatus();

    Serial.println(F("=== Ethernet Ready ==="));
    Serial.println();

    lastLinkCheck = millis();
    return true;
  }

  /*
   * Maintain Ethernet connection
   * Call this in loop() to:
   * - Renew DHCP lease automatically
   * - Monitor link status
   * - Detect cable disconnection
   *
   * @param linkCheckInterval - How often to check link status in ms (default: 10000)
   */
  static void maintain(unsigned long linkCheckInterval = 10000) {
    // Maintain DHCP lease (only does work if using DHCP)
    if (usingDHCP) {
      byte result = Ethernet.maintain();

      if (result != 0) {
        // Something changed
        switch (result) {
          case 1:
            // Renew failed
            Serial.println(F("DHCP: Renew failed"));
            break;
          case 2:
            // Renew success
            Serial.print(F("DHCP: Renewed - IP: "));
            printIPAddress(Ethernet.localIP());
            break;
          case 3:
            // Rebind failed
            Serial.println(F("DHCP: Rebind failed"));
            break;
          case 4:
            // Rebind success
            Serial.print(F("DHCP: Rebound - IP: "));
            printIPAddress(Ethernet.localIP());
            break;
        }
      }
    }

    // Check link status periodically
    if (millis() - lastLinkCheck >= linkCheckInterval) {
      EthernetLinkStatus linkStatus = Ethernet.linkStatus();

      if (linkStatus != lastLinkStatus) {
        // Link status changed
        Serial.print(F("Link status changed: "));
        if (linkStatus == LinkON) {
          Serial.println(F("Cable CONNECTED"));
        } else if (linkStatus == LinkOFF) {
          Serial.println(F("Cable DISCONNECTED"));
        } else {
          Serial.println(F("Unknown"));
        }
        lastLinkStatus = linkStatus;
      }

      lastLinkCheck = millis();
    }
  }

  /*
   * Get current link status
   * @return true if cable is connected, false otherwise
   */
  static bool isLinkUp() {
    return Ethernet.linkStatus() == LinkON;
  }

  /*
   * Print current network configuration
   */
  static void printConfig() {
    Serial.println(F("=== Network Configuration ==="));
    Serial.print(F("IP: "));
    printIPAddress(Ethernet.localIP());
    Serial.print(F("Gateway: "));
    printIPAddress(Ethernet.gatewayIP());
    Serial.print(F("Subnet: "));
    printIPAddress(Ethernet.subnetMask());
    Serial.print(F("DNS: "));
    printIPAddress(Ethernet.dnsServerIP());
    Serial.print(F("Mode: "));
    Serial.println(usingDHCP ? F("DHCP") : F("Static"));
    printLinkStatus();
    Serial.println(F("============================"));
  }

private:
  static bool usingDHCP;
  static EthernetLinkStatus lastLinkStatus;
  static unsigned long lastLinkCheck;

  static void printIPAddress(IPAddress ip) {
    Serial.print(ip[0]);
    Serial.print(F("."));
    Serial.print(ip[1]);
    Serial.print(F("."));
    Serial.print(ip[2]);
    Serial.print(F("."));
    Serial.println(ip[3]);
  }

  static void printLinkStatus() {
    Serial.print(F("  Link: "));
    EthernetLinkStatus linkStatus = Ethernet.linkStatus();
    if (linkStatus == LinkON) {
      Serial.println(F("Connected"));
    } else if (linkStatus == LinkOFF) {
      Serial.println(F("Disconnected"));
    } else {
      Serial.println(F("Unknown"));
    }
  }
};

// Initialize static members
bool EthernetHelper::usingDHCP = false;
EthernetLinkStatus EthernetHelper::lastLinkStatus = Unknown;
unsigned long EthernetHelper::lastLinkCheck = 0;

#endif // ETHERNET_HELPER_H
