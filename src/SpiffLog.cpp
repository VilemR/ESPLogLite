#include <Arduino.h>
#include <SpiffLog.h>
#include "LittleFS.h"
#include "Config.h"

#define FORMAT_SPIFFS_IF_FAILED false // jen poprve povolit
#define FILE_READ "r"                 //
#define FILE_WRITE "w"                //
#define FILE_APPEND "a"               //

SpiffLog::SpiffLog()
{
  initFileSystem();
}

boolean SpiffLog::mountSpiffs(boolean printinfo = false)
{
  if (!FS_MOUNTED)
  {
    FS_MOUNTED = SPIFFS.begin();
    if (!FS_MOUNTED)
    {
      SpiffLog::e("SPIFFS mounting FAILED!", printinfo);

    }
  }
  if (printinfo == true)
  {
    SpiffLog::i("SPIFFS mounting status : " + FS_MOUNTED ? "SUCCESS" : "FAILED!", printinfo);
  }
  return FS_MOUNTED;
}

void SpiffLog::spiffsFormatInitRestart(){
  formatFs();
  ESP.restart();
}

void SpiffLog::initFileSystem()
{
  if(!mountSpiffs(false)) {
     ;
  }
}

void SpiffLog::i(String inString)
{
  i(inString, SERIAL_PRINT);
}

void SpiffLog::i(String inString, boolean serialPrint)
{
  if (serialPrint)
  {
    Serial.println(inString);
  }
  outscan = "I:" + inString + "\n" + outscan;
  if (outscan.length() > logRetentionLength)
  {
    outscan = outscan.substring(1, logRetentionLength);
  }
}

void SpiffLog::l(String inString)
{
  l(inString, SERIAL_PRINT);
}
void SpiffLog::l(String inString, boolean serialPrint)
{
  if (serialPrint)
  {
    Serial.println(inString);
  }
  outscan = "" + inString + "\n" + outscan;
  if (outscan.length() > logRetentionLength)
  {
    outscan = outscan.substring(0, logRetentionLength);
  }
}

void SpiffLog::e(String inString)
{
  e(inString, SERIAL_PRINT);
}

void SpiffLog::e(String inString, boolean serialPrint)
{
  if (serialPrint)
  {
    Serial.println("ERR:" + inString);
  }
  outscan = "E:" + inString + "\n" + outscan;
  if (outscan.length() > logRetentionLength)
  {
    outscan = outscan.substring(1, logRetentionLength);
  }
}

void SpiffLog::createDddefaultConfig(String CFG_FILENAME)
{
  File configFile = SPIFFS.open(CFG_FILENAME, FILE_WRITE);
  if (!configFile)
  {
    e("Failed to create default config file.");
    return;
  }
  configFile.close();
  i("Default config created.");
}

void SpiffLog::formatFs()
{
  if (mountSpiffs())
  {
    SPIFFS.format();
    i("SPIFFS Formatted");
  }
  else
  {
    e("SPIFFS Formatt failed");
  }
}

String SpiffLog::getHtmlLogString()
{
  String r = String(outscan);
  r.replace("\n", "<br>");
  return r;
}

FSInfo SpiffLog::getFsInfo()
{
  FSInfo fs_info;
  SPIFFS.info(fs_info);
  return fs_info;
}

void SpiffLog::listRoot()
{
  if (mountSpiffs())
  {
    i("-----------------------------------------", false);
    String str = "SPIFFS Files :\n";
    Dir dir = SPIFFS.openDir("/");
    while (dir.next())
    {
      str += dir.fileName();
      str += " (";
      str += dir.fileSize();
      str += " [B])\r\n";
    }
    i(str, false);
    i("-----------------------------------------", false);
  }
  else
  {
    e("Error listing files - SPIFFS not mounted.");
  }
}

long SpiffLog::getUsedBytes() { return getFsInfo().usedBytes; }
long SpiffLog::getTotalBytes() { return getFsInfo().totalBytes; }

String SpiffLog::getFsStatusInfo()
{
  return "SPIFF filesystem : " + String(getUsedBytes()) +
         " bytes used of total " + String(getTotalBytes()) + ".";
}

String SpiffLog::getStorageIndicator()
{
  String result = String(getUsedBytes()) + " / " + String(getTotalBytes()) + " (" + String((int)(100.1 * (0.99 * getUsedBytes() / getTotalBytes()))) + "%)";
  return result;
}

void SpiffLog::readData(String filename)
{
  if (mountSpiffs())
  {
    File file = SPIFFS.open("/" + filename, "r");
    if (!file)
    {
      e("Error reading file " + filename + ".", true);
      return;
    }
    i("========================================", true);
    i("Reading Data :", true);
    String buffer = "";
    while (file.available())
    {
      int item = file.read();
      buffer = buffer + char(item);
      if (String(char(item)) == "\n")
      {
        i(buffer.substring(0, buffer.length() - 1), true);
        buffer = "";
      }
    }
    i(buffer, true);
    i("========================================", true);
    file.close();
  }
  else
  {
    e("Error mounting SPIFFS.", true);
  }
}

void SpiffLog::writeData(String data, String filename)
{
  if (mountSpiffs())
  {
    File fileToWrite = SPIFFS.open("/" + filename, "w");
    if (!fileToWrite)
    {
      e("There was an error opening the file " + filename + " for writing", true);
      return;
    }
    else
    {
      if (fileToWrite.println(data))
      {
        delay(10);
      }
      else
      {
        e("writing " + filename + " failed", true);
      }
      delay(1);
      fileToWrite.close();
      delay(1);
      return;
    }
  }
  else
  {
    e("Error mounting SPIFFS for write", true);
  }
}

void SpiffLog::appendData(String data)
{
  appendData(data, data_file);
}

void SpiffLog::appendData(String data, String filename)
{
  if (mountSpiffs())
  {
    File fileToWrite = SPIFFS.open("/" + filename, FILE_APPEND);
    if (!fileToWrite)
    {
      e("There was an error opening the file " + filename + " for appending -> SPIFFS format...", true);
      spiffsFormatInitRestart();
      return;
    }
    else
    {
      if (!fileToWrite.println(data))
      {
        e("Appending " + filename + " failed -> SPIFFS format...", true);
        spiffsFormatInitRestart();
        return;
      }
      fileToWrite.close();
      return;
    }
  }
  else
  {
    e("Error mounting SPIFFS for append -> SPIFFS format...", true);
    spiffsFormatInitRestart();
    return;
  }
  return;
}

void SpiffLog::deleteData()
{
  deleteData(data_file_name);
}

void SpiffLog::deleteData(String filename)
{
  SPIFFS.remove("/" + filename);
  i("File " + filename + " deleted.", true);
}

void SpiffLog::listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  i("Listing directory: " + String(dirname) + " \n");
  File root = fs.open(dirname, "r");
  if (!root)
  {
    e("- failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println(" - not a directory");
    e(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      i("  DIR : ");
      i(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      i("  FILE: ");
      i(file.name());
      i("\tSIZE: ");
      i(String(file.size()));
    }
    file = root.openNextFile();
  }
}

void SpiffLog::readFile(fs::FS &fs, const char *path)
{
  i("Reading file:" + String(path) + "\r\n");
  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    e("- failed to open file for reading");
    return;
  }
  i("- read from file:");
  while (file.available())
  {
    i(String(file.read()));
  }
  file.close();
}

void SpiffLog::writeFile(fs::FS &fs, const char *path, const char *message)
{
  i("Writing file:" + String(path) + "\r\n");
  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    i("- failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    i("- file written");
  }
  else
  {
    i("- write failed");
  }
  file.close();
}

void SpiffLog::appendFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\r\n", path);
  i("Appending to file: " + String(path) + "\r\n");
  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    e("- failed to open file for appending");
    return;
  }
  if (file.print(message))
  {
    i("- message appended");
  }
  else
  {
    e("- append failed");
  }
}

void SpiffLog::renameFile(fs::FS &fs, const char *path1, const char *path2)
{
  Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  i("Renaming file " + String(path1) + " to " + String(path2) + "\r\n");
  if (fs.rename(path1, path2))
  {
    i("- file renamed");
  }
  else
  {
    e("- rename failed");
  }
}

void SpiffLog::deleteFile(fs::FS &fs, const char *path)
{
  i("Deleting file:" + String(path) + "\r\n");
  if (fs.remove(path))
  {
    i("- file deleted");
  }
  else
  {
    e("- delete failed");
  }
}

float SpiffLog::getSPIFFSUsedCapacityRatio()
{
  return (float)(0.99 * getUsedBytes() / getTotalBytes());
}

boolean SpiffLog::deletePhoto_oneOldest()
{
  String last = "zzzzzzzzzz";
  File root = SPIFFS.open("/", "r");
  if (!root)
  {
    root.close();
    return false;
  }
  if (!root.isDirectory())
  {
    root.close();
    return false;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (!file.isDirectory())
    {
      if (String(file.name()) < last)
      {
        last = file.name();
      }
    }
    file = root.openNextFile();
  }
  root.close();
  file.close();
  if (last != "" && last != "zzzzzzzzzz")
  {
    SPIFFS.remove("/" + last);
    return true;
  }
  return false;
}
