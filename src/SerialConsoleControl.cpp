#include <Arduino.h>
// #include <string>
#include "SpiffLog.h"
#include "WebServer.h"
#include "WiFiManager.h"
#include "SerialConsoleControl.h"
#include "Persistent.h"
#include "Config.h"
#include <EEPROM.h> // read and write from flash memory

const char* HELP_FILE PROGMEM = R"=====(
REBOOT - reboot ESP device
FORMAT - format SPIFF/FS disk (and init default config)
CFG - Print runtime CONFIG VARIABLES
CFG (R,W,D,I) - Config Read(CFGRNAME)/CFGWNAME=VALUE/Init
LS - list SPIFFS files
WFSCAN - Set WIFI scan
WM (A,S,M) - Set WIFI mode
WS - Show WiFi status
SLEEP - Enter DeppSleep() routine (Sleep forever)
)=====";


void SerialConsoleControl::periodicWiFiConnectionMaintenance()
{
    if (wifi == nullptr)
    { // WiFi not active yet
        String wifiMode = cfg->getParameter("WIFIMODE");
        if (wifiMode != "")
        {
            wifi = new WiFiManager(wifiMode);
        }
        else
        {
            wifi = new WiFiManager(DEFAULT_WIFI_MODE);
        }
    }
    else
    {
        wifi->maintainNetworkConnection();
    }
}

void SerialConsoleControl::overrideVariablesFromConfig()
{

    String parameter = "";

    parameter = cfg->getParameter("SERIAL_PORT_SPEED");
    if (parameter != nullptr && parameter != "")
    {
        SERIAL_PORT_SPEED = (int)parameter.toInt();
    }

    parameter = cfg->getParameter("LOG_FILENAME_PATTERN");
    if (parameter != nullptr && parameter != "")
    {
        LOG_FILENAME_PATTERN = parameter;
    }
    else
    {
        LOG_FILENAME_PATTERN = LOG_FILENAME_PATTERN_DEFAULT;
    }

    parameter = cfg->getParameter("HOSTNAME");
    if (parameter != nullptr && parameter != "")
    {
        HOSTNAME = String(parameter);
        const unsigned char *parmSsid = reinterpret_cast<const unsigned char *>(HOSTNAME.c_str());
        *WIFI_SSID_AP = *parmSsid;
    }
    else
    {
        parameter = cfg->getParameter("WIFI_SSID_AP");
        if (parameter != nullptr && parameter != "")
        {
            const unsigned char *parmSsid = reinterpret_cast<const unsigned char *>(parameter.c_str());
            *WIFI_SSID_AP = *parmSsid;
        }
    }

    parameter = cfg->getParameter("SHOWFILE_INSTEAD_OF_DOWNLOAD");
    if (parameter != nullptr && parameter != "")
    {
        parameter.toUpperCase();
        SHOWFILE_INSTEAD_OF_DOWNLOAD = (parameter == "TRUE");
    }

    parameter = cfg->getParameter("SERIAL_PRINT");
    if (parameter != nullptr && parameter != "")
    {
        parameter.toUpperCase();
        SERIAL_PRINT = (parameter == "TRUE");
    }

    parameter = cfg->getParameter("MDNS_ENABLED");
    if (parameter != nullptr && parameter != "")
    {
        parameter.toUpperCase();
        MDNS_ENABLED = (parameter == "TRUE");
    }

    parameter = cfg->getParameter("WIFI_PASS_AP_ENCODED");
    if (parameter != nullptr && parameter != "")
    {
        const unsigned char *parmPass = reinterpret_cast<const unsigned char *>(parameter.c_str());
        *WIFI_PASS_AP_ENCODED = *parmPass;
    }

    parameter = cfg->getParameter("WIFI_SSID_STA");
    if (parameter != nullptr && parameter != "")
    {
        const unsigned char *parmSsidSta = reinterpret_cast<const unsigned char *>(parameter.c_str());
        *WIFI_SSID_STA = *parmSsidSta;
    }

    parameter = cfg->getParameter("WIFI_PASS_STA_ENCODED");
    if (parameter != nullptr && parameter != "")
    {
        const unsigned char *parmPassSta = reinterpret_cast<const unsigned char *>(parameter.c_str());
        *WIFI_PASS_STA_ENCODED = *parmPassSta;
    }
}

void SerialConsoleControl::logConfigInfo()
{
    logger->i("===============================================", SERIAL_PRINT);
    logger->i("SERIAL_PORT_SPEED: " + String(SERIAL_PORT_SPEED) + " ", SERIAL_PRINT);
    logger->i("WIFI_PASS_AP     : " + wifi->decodePassword(reinterpret_cast<const char *>(WIFI_PASS_AP_ENCODED)) + " ", SERIAL_PRINT);
    logger->i("WIFI_SSID_AP     : " + wifi->getWiFiApSSID(), SERIAL_PRINT);
    logger->i("WiFi_SSID_STA    : " + wifi->getWiFiStationSSID() + "" + " ", SERIAL_PRINT);
    logger->i("DEFAULT_WIFI_MODE: " + wifi->getWiFiModeString() + " ", SERIAL_PRINT);
    logger->i("MDNS_ENABLED     : " + String(MDNS_ENABLED) + " ", SERIAL_PRINT);
    logger->i("HOSTNAME         : " + String(HOSTNAME) + " ", SERIAL_PRINT);
    logger->i("============ RUNTIME CONFIG VALUES ============", SERIAL_PRINT);
}

SerialConsoleControl::SerialConsoleControl(Persistent *cfgIn)
{
    Serial.begin(SERIAL_PORT_SPEED);
    Serial.setRxBufferSize(1024);
    cfg = cfgIn;
    overrideVariablesFromConfig();
    Serial.begin(SERIAL_PORT_SPEED);
    logger = new SpiffLog();
    wifi = nullptr;
    saveIndex = cfg->getParameter("SAVE_INDEX").toInt(); 
    targetLogFileName = getNewIndexFile(saveIndex);
    serialLogBuffer = "";
    logger->i("targetLogFileName : " + targetLogFileName, SERIAL_PRINT);
    run_time_since = 0;
}

String SerialConsoleControl::getNewIndexFile(int current)
{
    int saveIndex = current;
    cfg->saveParameter("SAVE_INDEX", String(saveIndex));
    return cfg->getNumberIndexedFilename(LOG_FILENAME_PATTERN, saveIndex, ".txt");
}

String SerialConsoleControl::getTargetLogFilename(int saveIndex)
{
    return LOG_FILENAME_PATTERN + "_" + cfg->getLeadingNumber(7, saveIndex) + ".txt";
}

void SerialConsoleControl::printHelp()
{   //HELP_FILE
    logger->i("===========================================");
    logger->i(HELP_FILE);
    logger->i("===========================================");
}

void SerialConsoleControl::cron()
{
    if (wifi != nullptr)
    {
        wifi->maintainNetworkConnection();
    }
}

void SerialConsoleControl::goToSleep()
{
    Serial.println("Going to sleep...");
    wifi = nullptr;

    attachInterrupt(digitalPinToInterrupt(1), wakeUpFromUart, CHANGE);
    interrupts();
    wifi_station_disconnect();
    wifi_set_opmode_current(NULL_MODE);
    wifi_fpm_set_sleep_type(LIGHT_SLEEP_T); 
    wifi_fpm_open();                        
    wifi_fpm_do_sleep(0xFFFFFFF); // Sleep for longest possible time
    delay(20);
}

void SerialConsoleControl::decodeWebCommand(String command)
{
    decodeCommand(command);
}

void SerialConsoleControl::decodeSerialCommand()
{
    if (Serial.available())
    {
        String data = Serial.readString();
        if (data == NULL || data.length() < 1)
        {
            return;
        }
        if (decodeCommand(data) != true) // Not applicable for logging *only for debugging
        {
            for (int i = 0; i < data.length(); i++)
            {
                if (data[i] == 0x1B or data[i] == 0x00)
                {
                    data[i] = ' ';
                }
            }
            serialLogBuffer = serialLogBuffer + data;
            lastReceivedTime = millis();
        }
        else
        {
            return;
        }
    }
    else
    {
        if (serialLogBuffer.length() > 0 or
            (serialLogBuffer.length() > 0 and (millis() - lastReceivedTime) > 2000))
        {
            serialLogBuffer = saveBuffer(serialLogBuffer);
            lastReceivedTime = millis();
        }
    }
}

String SerialConsoleControl::saveBuffer(String buffer)
{
    if (buffer == nullptr or buffer.length() <= 0)
    {
        return "";
    }

    int beginRemaindingBufferForFurtherProcessing = 0;
    for (int i = 0; i < buffer.length(); i++)
    {
        if (i > 1 and buffer[i] == 0x0A and buffer[i - 1] == 0x0D)
        {
            String bufferLine = buffer.substring(beginRemaindingBufferForFurtherProcessing, i);
            saveBufferLine(bufferLine);
            beginRemaindingBufferForFurtherProcessing = i + 1;
        }
    }
    if (beginRemaindingBufferForFurtherProcessing < buffer.length() - 1)
    {
        return buffer.substring(beginRemaindingBufferForFurtherProcessing);
    }
    return "";
}

void SerialConsoleControl::saveBufferLine(String bufferLineX)
{
    String bufferLine = String(bufferLineX);
    if (bufferLine == nullptr or bufferLine.length() <= 0)
    {
        return;
    }
    if (bufferLine.endsWith("\n"))
    {
        bufferLine.remove(bufferLine.length() - 1);
    }
    if (bufferLine.endsWith("\r"))
    {
        bufferLine.remove(bufferLine.length() - 1);
    }
    if (bufferLine.endsWith("\n"))
    {
        bufferLine.remove(bufferLine.length() - 1);
    }
    String cutUpTo = "[0m|";
    while (bufferLine.indexOf(cutUpTo) != -1)
    {
        unsigned int pos = (unsigned int)bufferLine.indexOf(cutUpTo);
        bufferLine = bufferLine.substring(pos + cutUpTo.length());
    }
    bufferLine.replace("[31m ", "");
    bufferLine.replace("[32m ", "");
    bufferLine.replace("[33m ", "");
    bufferLine.replace("[34m ", "");
    bufferLine.trim();
    logger->l(bufferLine);
    logger->appendData("" + bufferLine + "", targetLogFileName);
    savedLines = savedLines + 1;
    if (savedLines > 300)
    {
        saveIndex = saveIndex + 1;
        targetLogFileName = getNewIndexFile(saveIndex);
        logger->i("targetLogFileName : " + targetLogFileName, SERIAL_PRINT);
        savedLines = 0;
    }

    while (logger->getSPIFFSUsedCapacityRatio() > STORAGE_CAPACITY_LIMIT_PCT)
    {
        logger->i("Deleting obsolete files...", SERIAL_PRINT);
        logger->deletePhoto_oneOldest();
    }
}

boolean SerialConsoleControl::decodeCommand(String data)
{
    String cmd = data;
    if (cmd.endsWith("\n"))
    {
        cmd = cmd.substring(0, cmd.length() - 1);
    }
    if (cmd.length() < 1)
    {
        return false;
    }
    else if (cmd.length() == 1)
    {
        cmd.toUpperCase();
    }
    else
    {
        String command = cmd.substring(0, 1);
        String content = cmd.substring(1, cmd.length());
        command.toUpperCase();
        cmd = command + content;
    }
    if (cmd == "H" || cmd == "h" || cmd == "HELP" || cmd == "Help")
    {
        printHelp();
        return true;
    }
    else if (cmd == "WS" || cmd == "Ws")
    {
        logger->i("======= WIFI STATUS =======\n" + wifi->getWiFiStatus() + "\n");
        return true;
    }
    else if (cmd == "WMA" || cmd == "Wma")
    {
        wifi->setWiFiMode(AP);
        wifi->startAP();
        return true;
    }
    else if (cmd == "WMS" || cmd == "Wms")
    {
        wifi->setWiFiMode(STA);
        wifi->startStation();
        return true;
    }
    else if (cmd == "WMM" || cmd == "Wmm")
    {
        wifi->setWiFiMode(MIXED);
        wifi->startAP_and_Station();
        return true;
    }
    else if (cmd == "LS" || cmd == "Ls")
    {
        logger->listRoot();
        return true;
    }
    else if (cmd == "FORMAT" || cmd == "Format")
    {
        logger->spiffsFormatInitRestart();
        return true;
    }
    else if (cmd == "SLEEP" || cmd == "Sleep")
    {
        ESP.deepSleep(ESP.deepSleepMax() - 5000000);
        return true;
    }
    else if (cmd == "REBOOT" || cmd == "Reboot" || cmd == "reboot")
    {
        logger->i("Going to reboot...");
        ESP.restart();
        return true;
    }
    else if ((data.indexOf("CFGR") == 0) || (data.indexOf("Cfgr") == 0)) //
    {
        data = data.substring(4, data.length());
        logger->i("CFGR: " + data);
        String parameter = data;
        String value = cfg->getParameter(parameter);
        logger->i(parameter + ": " + value);
        return true;
    }
    else if ((data.indexOf("CFGI") == 0) || (data.indexOf("Cfgi") == 0)) //
    {
        cfg->initConfig();
        return true;
    }

    else if ((data.indexOf("WFSCAN") == 0) || (data.indexOf("Wfscan") == 0)) //
    {
        logger->i(wifi->getWiFiNetworks());
        return true;
    }

    else if ((data.indexOf("CFGD") == 0) || (data.indexOf("Cfgd") == 0)) //
    {
        data = data.substring(4, data.length());
        data.toUpperCase();
        logger->i("CFGD: " + data);
        String parameter = data;
        cfg->deleteParameter(parameter);
        logger->i(parameter + " deleted.");
        return true;
    }

    else if ((data.indexOf("CFGW") == 0) || (data.indexOf("Cfgw") == 0)) //
    {
        data = data.substring(4, data.length());
        logger->i("CFGW: " + data);
        int equalsIndex = data.indexOf('=');
        int spaceIndex = data.indexOf(' ');
        if (spaceIndex >= 1)
        {
            logger->e("CFGW: Empty space ' ' found in the string (command ignored)! ");
            return true;
        }
        if (equalsIndex != -1)
        {
            String parameter = data.substring(0, equalsIndex);
            String value = data.substring(equalsIndex + 1);
            cfg->saveParameter(parameter, value);
            logger->i(parameter + " saved.");
        }
        else
        {
            logger->e("CFGW: No '=' found in the string.");
        }
        return true;
    }
    else if ((data.indexOf("CFG") == 0) || (data.indexOf("Cfg") == 0)) //
    {
        logConfigInfo();
        return true;
    }
    return false;
}

void SerialConsoleControl::ledEnable()
{
    ledIndicator->enable();
    logger->i("SerialConsoleControl::ledEnabled()");
}
void SerialConsoleControl::ledDisable()
{
    ledIndicator->disable();
    logger->i("SerialConsoleControl::ledDisabled()");
}
