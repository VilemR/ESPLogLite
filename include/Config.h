// ============================================================================================================
#pragma once
#ifndef CONFIG_H
#define CONFIG_H
#include "Enums.h"
#include <Arduino.h>

inline String data_file        = "data_logger.txt";
inline boolean SERIAL_PRINT     = false;
inline int SERIAL_PORT_SPEED   = 115200; 
inline unsigned int logRetentionLength = 2000;

inline boolean WIFI_STA_CONNECTED = false;
inline boolean WIFI_AP_ENABLED    = false;

// Wifi Config
inline const int channel = 10;       
inline const bool hide_SSID = false; 
inline const int max_connection = 5;

/* To get your custom password hashed by BASE64 use ChatGPT
   use prompt: prosim zaheshuj heslo |987456321| pomoci base64  (Czech)
               please encode password |987456321| using base64  (English)

   Wrap the password into | (pines) e.g if your pasword is myPassword, 
   used wrapped string |myPassword| and asl GatGPT:
            please encode password |myPassword| using base64 
            and you will obtain :  fG15UGFzc3dvcmR8       
*/

inline unsigned char *WIFI_SSID_STA = (unsigned char *)"home";
inline unsigned char *WIFI_PASS_STA_ENCODED = (unsigned char *)"="; 

inline unsigned char *WIFI_SSID_AP = (unsigned char *)"ESPLOGLITE";
inline unsigned char *WIFI_PASS_AP_ENCODED = (unsigned char *)"fDk4NzQ1NjMyMXw="; // |987456321|

inline WIFI_MODE DEFAULT_WIFI_MODE = AP;

inline unsigned long run_time_since = 0;

inline int EEPROM_SIZE = 32;
inline boolean SHOWFILE_INSTEAD_OF_DOWNLOAD = false;
inline String LOG_FILENAME_PATTERN="";
inline String LOG_FILENAME_PATTERN_DEFAULT="LOG_DATA";

inline boolean MDNS_ENABLED = false;
inline String HOSTNAME = "";

inline float STORAGE_CAPACITY_LIMIT_PCT = 0.90;
inline long int SLEEP_EXPIRATION_SEC = 20000; 

#endif // CONFIG_H
