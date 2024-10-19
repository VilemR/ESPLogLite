#ifndef PERSISTEN_H
#define PERSISTEN_H

#include <Arduino.h>

class Persistent
{
private:
    String CFG_FILENAME = "appconfig.cfg";

public:
    Persistent();
    String getParameter(String parameterName);
    void saveParameter(String parameterName, String parameterValue);
    void deleteParameter(String parameterName);
    void loadConfig();
    void initConfig();
    String getLeadingNumber(int digits, int number);
    String getNumberIndexedFilename(String prefix, int number, String suffix);
    String getCfgFilename();
};

#endif // PERSISTEN_H
