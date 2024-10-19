#ifndef ESPDNS_HELPER_H
#define ESPDNS_HELPER_H

#include <Arduino.h>
#include <ESP8266mDNS.h>

class EspDns
{
private:
    IPAddress ipAddress;
    String mdnsHostname;
    unsigned long lastUpdateTime;
    int updatePeriodSeconds = 1;
    boolean mDnsError;

public:
    EspDns(IPAddress ip, const String hostname);
    void update();
};

#endif // ESPDNS_HELPER_H
