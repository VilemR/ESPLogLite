#ifndef SERIAL_CONSOLE_CONTROL_H
#define SERIAL_CONSOLE_CONTROL_H

#include <Arduino.h>
#include "SpiffLog.h"
#include "WebServer.h"
#include "WiFiManager.h"
#include "Persistent.h"
#include <EEPROM.h> // 
#include "Led.h"

class SerialConsoleControl
{
public:
    SerialConsoleControl(Persistent *cfg);
    void overrideVariablesFromConfig();
    void periodicWiFiConnectionMaintenance();
    void printHelp();
    void cron();
    void decodeSerialCommand();
    void decodeWebCommand(String command);
    boolean decodeCommand(String command);
    void resetDeepSleepTimer();
    void enterDeepSleep();
    void enterDeepSleep(int DELTA_TIME);
    void ledEnable();
    void ledDisable();
    String saveBuffer(String buffer);
    void saveBufferLine(String bufferLine);

    boolean deepSleepTimerExpired();
    String deepSleepTimeStartTime();
    String getVoltage();
    String getTargetLogFilename(int saveIndex);
    String getNewIndexFile(int current);
    void goToSleep() ;

public:
    SpiffLog *logger;
    WiFiManager *wifi;
    Led *ledIndicator;
    String readString;
    String serialLogBuffer;
    float lastLevel;
    int maintenanceBeginExpirationTimeout_MILLIS;
    Persistent *cfg;
    void logConfigInfo();
    String targetLogFileName = LOG_FILENAME_PATTERN_DEFAULT;
    long int lastReceivedTime = 0;
    int savedLines = 0;
    int saveIndex = 1;
};

#endif // SERIAL_CONSOLE_CONTROL_H