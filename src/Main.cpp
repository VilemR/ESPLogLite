#include <Arduino.h>
#include "SerialConsoleControl.h"
#include <WebGui.h>
#include <WebServer.h>
#include "Config.h"
#include "Persistent.h"

// TODO: Vyladit nahozeni serial console - v deep sleep to nepotrebuji

extern "C"
{
  #include "gpio.h"
  #include "user_interface.h"
}

SerialConsoleControl *controll;
WebServer *webServer;
WebGui *webGui;
Persistent *config;

void setup()
{
  config = new Persistent();
  controll = new SerialConsoleControl(config);
  controll->logger->i("Main.setup() : Logger Device starting !", SERIAL_PRINT);
  controll->periodicWiFiConnectionMaintenance();
  controll->logger->i("WiFi Mode : " + controll->wifi->getWiFiModeString(), SERIAL_PRINT);
  webServer = new WebServer();
  webGui = new WebGui(webServer, controll);
  controll->logger->i("Main.setup() : finished...", SERIAL_PRINT);
  delay(10);
}

void loop()
{
  controll->cron();
  controll->decodeSerialCommand();
  webGui->handleWebRequests();
}
