#ifndef SPIFF_LOG_H
#define SPIFF_LOG_H

#define FORMAT_SPIFFS_IF_FAILED false
#define FILE_READ "r"
#define FILE_WRITE "w"
#define FILE_APPEND "a"
#define data_file_name "data_logger.txt"

#include <Arduino.h>
#include "FS.h"
#include "Config.h"


class SpiffLog
{
public:
    boolean FS_MOUNTED = false;
    String outscan = "";

public:
    SpiffLog();
    boolean mountSpiffs(boolean printinfo);
    void initFileSystem();
    void i(String inString);
    void i(String inString, boolean serialPrint);
    void l(String inString);
    void l(String inString, boolean serialPrint);
    void e(String inString);
    void e(String inString, boolean serialPrint);
    void formatFs();
    String getHtmlLogString();
    FSInfo getFsInfo();
    void listRoot();
    long getUsedBytes();
    long getTotalBytes();
    String getFsStatusInfo();
    String getStorageIndicator();
    void readData(String filename = data_file_name);
    void writeData(String data, String filename = data_file_name);
    void appendData(String data, String filename);
    void appendData(String data);
    void deleteData();
    void deleteData(String filename);
    void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
    void readFile(fs::FS &fs, const char *path);
    void writeFile(fs::FS &fs, const char *path, const char *message);
    void appendFile(fs::FS &fs, const char *path, const char *message);
    void renameFile(fs::FS &fs, const char *path1, const char *path2);
    void deleteFile(fs::FS &fs, const char *path);
    boolean deletePhoto_oneOldest();
    float getSPIFFSUsedCapacityRatio();
    void createDddefaultConfig(String CFG_FILENAME);
    void spiffsFormatInitRestart();
};

#endif // SPIFF_LOG_H
