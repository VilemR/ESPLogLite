#include <FS.h>
#include "Persistent.h"

const char* DEFAULT_CONFIG_FILE PROGMEM = R"=====(
------- DEFAULT CONFIG -------
HOSTNAME=ML-Z004
MDNS_ENABLED=TRUE
SERIAL_PRINT=FALSE
WIFIMODE=MIXED
SHOWFILE_INSTEAD_OF_DOWNLOAD=TRUE
WIFI_SSID_STA=home
WIFI_PASS_STA_ENCODED=fDk4NzQ1NjMyYWF8
SAVE_INDEX=999

------- QUICK RE-CONFIG -------
-SERIAL_PRINT=TRUE
-WIFIMODE=AP
-SHOWFILE_INSTEAD_OF_DOWNLOAD=FALSE
)=====";

Persistent::Persistent()
{
    SPIFFS.begin();
    File file = SPIFFS.open(CFG_FILENAME, "r");

    if (!SPIFFS.exists(CFG_FILENAME))
    {
        Serial.println("File does not exist. Creating...");
        File file = SPIFFS.open(CFG_FILENAME, "w");
        if (file)
        {
            file.println(DEFAULT_CONFIG_FILE);  // Write default config content
            file.close();
        }
        else
        {
            Serial.println("Error creating file.");
        }
    }
}

String Persistent::getCfgFilename(){
    return CFG_FILENAME;
}

String Persistent::getParameter(String parameterName)
{
    File file = SPIFFS.open(CFG_FILENAME, "r");
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return "";
    }

    String buffer = "";
    while (file.available())
    {
        String line = file.readStringUntil('\n');
        line.trim();
        int separatorIndex = line.indexOf('=');
        if (separatorIndex != -1)
        {
            String key = line.substring(0, separatorIndex);
            String value = line.substring(separatorIndex + 1);

            int commentStart = value.indexOf("/*");
            if (commentStart != -1)
            {
                value = value.substring(0, commentStart);
            }
            value.trim();
            if (parameterName == nullptr || parameterName == "")
            {
                buffer = buffer + "\n" + "[" + key + "] = " + value;
            }
            else if (key == parameterName)
            {
                file.close();
                return value;
            }
        }
    }
    file.close();
    return buffer;
}

void Persistent::deleteParameter(String parameterName)
{
    File file = SPIFFS.open(CFG_FILENAME, "r");
    File tempFile = SPIFFS.open("temp.cfg", "w");
    if (!file || !tempFile)
    {
        Serial.println("Failed to open files for reading/writing");
        return;
    }
    boolean parameterFound = false;
    while (file.available())
    {
        String line = file.readStringUntil('\n');
        line.trim();
        int separatorIndex = line.indexOf('=');
        if (separatorIndex != -1)
        {
            String key = line.substring(0, separatorIndex);
            if (key == parameterName)
            {
                parameterFound = true;
            }
            else
            {
                tempFile.println(line);
            }
        }
        else
        {
            tempFile.println(line);
        }
    }
    file.close();
    tempFile.close();
    SPIFFS.remove(CFG_FILENAME);
    SPIFFS.rename("temp.cfg", CFG_FILENAME);
}

void Persistent::saveParameter(String parameterName, String parameterValue)
{
    File file = SPIFFS.open(CFG_FILENAME, "r");
    File tempFile = SPIFFS.open("temp.cfg", "w");
    if (!file || !tempFile)
    {
        Serial.println("Failed to open files for reading/writing");
        return;
    }
    boolean parameterFound = false;
    while (file.available())
    {
        String line = file.readStringUntil('\n');
        line.trim();
        int separatorIndex = line.indexOf('=');
        if (separatorIndex != -1)
        {
            String key = line.substring(0, separatorIndex);
            if (key == parameterName)
            {
                tempFile.println(parameterName + "=" + parameterValue);
                parameterFound = true;
            }
            else
            {
                tempFile.println(line);
            }
        }
        else
        {
            tempFile.println(line);
        }
    }
    if (!parameterFound)
    {
        tempFile.println(parameterName + "=" + parameterValue);
    }
    file.close();
    tempFile.close();
    SPIFFS.remove(CFG_FILENAME);
    SPIFFS.rename("temp.cfg", CFG_FILENAME);
}

void Persistent::loadConfig()
{
}

void Persistent::initConfig()
{
    SPIFFS.remove(CFG_FILENAME);
    ESP.restart();
}

String Persistent::getNumberIndexedFilename(String prefix, int number, String suffix){
    return prefix + "_" + getLeadingNumber(7, number) + suffix;
}

String Persistent::getLeadingNumber(int digits, int number)
{
    String res = String(number);
  while (res.length() <= digits)
  {
    res = "0" + res;
  }
  return res;

}
