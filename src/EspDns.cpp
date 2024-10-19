#include "EspDns.h"
#include <ESP8266mDNS.h>

EspDns::EspDns(const IPAddress ip, const String hostname)
{
    ipAddress = ip;
    mdnsHostname = hostname;
    mDnsError = false;
    if (!MDNS.begin(mdnsHostname))
    {
        mDnsError = true;
        Serial.println("mDNS() : Error setting up mDNS responder!");
        return;
    }
    MDNS.addService("http", "tcp", 80);
    lastUpdateTime = millis();
    Serial.println("mDNS() : Device is discoverable at http://" + mdnsHostname + "/");
}

void EspDns::update()
{
    if (mDnsError == true)
    {
        return;
    }
    unsigned long currentTime = millis();
    unsigned long absoluteDifference = (currentTime > lastUpdateTime) ? (currentTime - lastUpdateTime) : (lastUpdateTime - currentTime);
    if (absoluteDifference > (updatePeriodSeconds * 1000))
    {
        MDNS.update();
        lastUpdateTime = millis();
    }
}
