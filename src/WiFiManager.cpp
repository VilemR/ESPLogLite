#include <ESP8266WiFi.h>
#include "base64.hpp"
#include "Enums.h"
#include "WiFiManager.h"
#include "Config.h"
#include <string>
#include "EspDns.h"

WiFiManager::WiFiManager(WIFI_MODE mode)
{
    setWiFiMode(mode);
}

WiFiManager::WiFiManager(String mode)
{
    setWiFiMode(mode);
}

void WiFiManager::setWiFiMode(String mode)
{
    if (mode == "AP")
    {
        setWiFiMode(AP);
    }
    else if (mode == "STA")
    {
        setWiFiMode(STA);
    }
    else if (mode == "MIXED")
    {
        setWiFiMode(MIXED);
    }
    else if (mode == "OFF")
    {
        setWiFiMode(OFF);
    }
    else
    {
        setWiFiMode(AP);
    }
}

void WiFiManager::setWiFiMode(WIFI_MODE mode)
{
    wifi_mode = mode;
    mdns = nullptr;
    switch (mode)
    {
    case AP:
        WiFi.mode(WIFI_AP);
        startAP();
        break;
    case STA:
        WiFi.mode(WIFI_STA);
        startStation();
        break;
    case MIXED:
        WiFi.mode(WIFI_AP_STA);
        startAP_and_Station();
        break;
    case OFF:
        WiFi.mode(WIFI_OFF);
        break;
    default:
        break;
    }
}

boolean WiFiManager::maintainDnsDiscovery()
{
    if (MDNS_ENABLED == true)
    {
        if ((getWiFiMode() == MIXED || getWiFiMode() == STA) && getStationConnectionStatus() == "WL_CONNECTED")
        {
            if (mdns == nullptr)
            {
                mdns = new EspDns(getStationdIP(), HOSTNAME);
            }
            else
            {
                mdns->update();
            }
        }
        else
        {
            if (mdns != nullptr)
            {
                delete mdns;
                mdns = nullptr;
            }
            else
            {
                mdns = nullptr;
            }
            return false;
        }
        return false;
    }
    else
    {
        return false;
    }
}

boolean WiFiManager::maintainNetworkConnection()
{
    if (!((getWiFiMode() == MIXED || getWiFiMode() == STA)))
    {
        return false;
    }
    if ((getWiFiMode() == MIXED || getWiFiMode() == STA) && getStationConnectionStatus() != "WL_CONNECTED")
    {
        unsigned long currentTime = millis();
        unsigned long absoluteDifference = (currentTime > lastStationReconnectAttemptTime) ? (currentTime - lastStationReconnectAttemptTime) : (lastStationReconnectAttemptTime - currentTime);
        if (absoluteDifference > (reconnectStationPeriodSeconds * 1000))
        {
            startStation();
            lastStationReconnectAttemptTime = millis();
        }
    }

    if (getStationConnectionStatus() == "WL_CONNECTED")
    {
        lastStationReconnectAttemptTime = millis();
        maintainDnsDiscovery();
    }
    return true;
}

WIFI_MODE WiFiManager::getWiFiMode()
{
    return wifi_mode;
}

String WiFiManager::getWiFiModeString()
{
    String ssid = "";
    switch (getWiFiMode())
    {
    case AP:
        ssid = getWiFiApSSID();
        return "AP mode only (" + ssid + ")";
        break;
    case STA:
        return "STATION mode only";
        break;
    case MIXED:
        return "MIXED (AP and STATION)";
        break;
    default:
        return "!UNKNOWN!";
        break;
    }
}

String WiFiManager::getWiFiStationSSID()
{
    return current_sta_ssid;
}

String WiFiManager::getWiFiApSSID()
{
    return current_ap_ssid;
}
String WiFiManager::getWiFiApPassword()
{
    return current_ap_pass;
}

String WiFiManager::getWiFiStatus()
{
    String ret_str = "";
    switch (wifi_mode)
    {
    case AP:
        ret_str = ret_str + "WIFI MODE    : " + "AP" + "\n";
        ret_str = ret_str + "AP SSID      : " + getWiFiApSSID() + "\n";
        ret_str = ret_str + "AP IP Address: " + String(getAPIP()) + " ( http://" + String(getAPIP()) + "/ )\n";
        ret_str = ret_str + "MAC Address  : " + String(getMac()) + "\n";
        ret_str = ret_str + "AP Password  : " + current_ap_pass + "\n";
        ret_str = ret_str + "Connections  : " + String(getCountAPConnected()) + "\n";
        break;
    case STA:
        ret_str = ret_str + "WIFI MODE    : " + "STATION" + "\n";
        ret_str = ret_str + "SSID         : " + current_sta_ssid + "\n";
        ret_str = ret_str + "IP Address   : " + String(getStationdIPString()) + " ( http://" + String(getStationdIPString()) + "/ )\n";
        ret_str = ret_str + "MAC Address  : " + String(getMac()) + "\n";
        ret_str = ret_str + "Connection   : " + getStationConnectionStatus() + "\n";
        break;
    case MIXED:
        ret_str = ret_str + "WIFI MODE    : " + "MIXED" + "\n";
        ret_str = ret_str + "AP SSID      : " + current_ap_ssid + "\n";
        ret_str = ret_str + "AP IP Address: " + String(getAPIP()) + "\n";
        ret_str = ret_str + "MAC Address  : " + String(getMac()) + "\n";
        ret_str = ret_str + "AP Password  : " + current_ap_pass + "\n";
        ret_str = ret_str + "Connections  : " + String(getCountAPConnected()) + "\n\n";
        ret_str = ret_str + "STATION      : " + getStationConnectionStatus() + "\n";
        ret_str = ret_str + "STA SSID     : " + getWiFiStaSSID() + "\n";
        ret_str = ret_str + "IP Address   : " + String(getStationdIPString()) + " ( http://" + String(getStationdIPString()) + "/ )\n";
        break;
    default:
        ret_str = ret_str + "Not implemented yet\n";
        break;
    }
    return ret_str;
}

void WiFiManager::startAP()
{
    if (WIFI_AP_ENABLED == true)
        return;
    IPAddress local_ip(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    if (HOSTNAME != "")
    {
        WiFi.softAP(HOSTNAME,
                    decodePassword(reinterpret_cast<const char *>(WIFI_PASS_AP_ENCODED)),
                    channel, hide_SSID, max_connection);
        current_ap_ssid = HOSTNAME;
    }
    else
    {
        WiFi.softAP(reinterpret_cast<const char *>(WIFI_SSID_AP),
                    decodePassword(reinterpret_cast<const char *>(WIFI_PASS_AP_ENCODED)),
                    channel, hide_SSID, max_connection);
        current_ap_ssid = String(reinterpret_cast<const char *>(WIFI_SSID_AP));
    }
    current_ap_pass = String(decodePassword(reinterpret_cast<const char *>(WIFI_PASS_AP_ENCODED)));
    WiFi.softAPConfig(local_ip, gateway, subnet);
    IPAddress IP = WiFi.softAPIP();
    WIFI_AP_ENABLED = true;
}

void WiFiManager::trackWiFiStatusChange(boolean currentStatus)
{ 
    if (WIFI_STA_CONNECTED == false && currentStatus == true)
    {
        WIFI_STA_CONNECTED = true;
        current_sta_ssid = getWiFiStaSSID();
    }
    else if (WIFI_STA_CONNECTED == true && currentStatus == false)
    {
        WIFI_STA_CONNECTED = false;
        current_sta_ssid = "?";
    }
}

void WiFiManager::startStation()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        current_sta_ssid = "?";
        WIFI_STA_CONNECTED = false;
        trackWiFiStatusChange(false);
        WiFi.begin(reinterpret_cast<const char *>(WIFI_SSID_STA),
                   decodePassword(reinterpret_cast<const char *>(WIFI_PASS_STA_ENCODED)));
        lastStationReconnectAttemptTime = millis();
    }
    else
    {
        trackWiFiStatusChange(true);
    }
}

void WiFiManager::startAP_and_Station()
{
    startAP();
    startStation();
}

void WiFiManager::disableWiFi()
{
    WiFi.disconnect(true);
    delay(100);
    WiFi.mode(WIFI_OFF);
}

int WiFiManager::getCountAPConnected()
{
    return WiFi.softAPgetStationNum();
}

String WiFiManager::getAPIP()
{
    return WiFi.softAPIP().toString();
}

String WiFiManager::getStationdIPString()
{
    return WiFi.localIP().toString();
}

IPAddress WiFiManager::getStationdIP()
{
    return WiFi.localIP();
}

String WiFiManager::getMac()
{
    return WiFi.macAddress();
}

IPAddress WiFiManager::getIP()
{
    return WiFi.localIP();
}

String WiFiManager::getWiFiStaSSID()
{
    return WiFi.SSID();
}

String WiFiManager::getStationConnectionStatus()
{
    int status = WiFi.status();
    switch (status)
    {
    case WL_CONNECTED:
        return "WL_CONNECTED";
    case WL_NO_SSID_AVAIL:
        return "WL_NO_SSID_AVAIL";
    case WL_CONNECT_FAILED:
        return "WL_CONNECT_FAILED";
    case WL_WRONG_PASSWORD:
        return "WL_WRONG_PASSWORD";
    case WL_IDLE_STATUS:
        return "WL_IDLE_STATUS";
    default:
        return "WL_DISCONNECTED" + String(status);
    }
}

String WiFiManager::convertToString(unsigned char *unsignedCharString)
{
    return "wifiSSIDString";
}

String WiFiManager::decodePassword(const String &encodedPassword)
{
    unsigned char decodedPassword[40];
    const char *encodedPasswordChar = encodedPassword.c_str();
    decode_base64(reinterpret_cast<const unsigned char *>(encodedPasswordChar), strlen(encodedPasswordChar), (unsigned char *)decodedPassword);
    String decoded_string = reinterpret_cast<const char *>(decodedPassword);
    int start_index = decoded_string.indexOf("|") + 1;
    int end_index = decoded_string.indexOf("|", start_index + 1);
    String result_string = decoded_string.substring(start_index, end_index);
    return result_string;
}

String WiFiManager::encodePassword(const String &plainTextPassword)
{
    unsigned char encodedPassword[64]; // Adjust the size as needed
    encode_base64(reinterpret_cast<const unsigned char *>(plainTextPassword.c_str()), plainTextPassword.length(), encodedPassword);
    return reinterpret_cast<const char *>(encodedPassword);
}

String WiFiManager::getWiFiNetworks()
{
    String buffer = "=========================\n";
    int n = WiFi.scanNetworks();
    if (n == 0)
        buffer = "No WiFi network found!\n";
    else
    {
        buffer = buffer + "WiFi network(s) : " + String(n) + " found.\n";
        for (int i = 0; i < n; ++i)
        {
            buffer = buffer + String(i + 1) + ". " + String(WiFi.SSID(i)) + " : " + String(WiFi.RSSI(i)) + "dB\n";
            delay(10);
        }
    }
    return buffer;
}
