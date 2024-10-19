#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESP8266WiFi.h>
#include "Enums.h"
#include "EspDns.h"

class WiFiManager
{

private:
    WIFI_MODE wifi_mode = AP;
    String current_sta_ssid = "";
    String current_ap_ssid = "";
    String current_ap_pass = "";
    EspDns *mdns;

    unsigned long lastStationReconnectAttemptTime;
    unsigned long reconnectStationPeriodSeconds = 60;

public:
    WiFiManager(WIFI_MODE mode);
    WiFiManager(String mode);
    void setWiFiMode(String mode);
    void setWiFiMode(WIFI_MODE mode);
    WIFI_MODE getWiFiMode();
    String getWiFiModeString();
    void disableWiFi();
    String getWiFiStatus();
    String getWiFiApSSID();
    String getWiFiStationSSID();
    String getWiFiApPassword();
    String getStationConnectionStatus();
    void startAP();
    void startStation();
    void startAP_and_Station();
    int getCountAPConnected();
    String getAPIP();
    String getStationdIPString();
    IPAddress getStationdIP();
    String getMac();
    String getWiFiStaSSID();
    IPAddress getIP();
    String decodePassword(const String &encodedPassword);
    String encodePassword(const String &plainTextPassword);
    String convertToString(unsigned char * unsignedCharString);
    boolean maintainNetworkConnection();
    boolean maintainDnsDiscovery();
    void trackWiFiStatusChange(boolean currentStatus);
    String getWiFiNetworks();
};

#endif // WiFiManager_h