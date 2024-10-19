#ifndef WEB_GUI_H
#define WEB_GUI_H

#include <Arduino.h>
#include "WebServer.h"
#include "SerialConsoleControl.h"
#include <ESP8266WiFi.h>


class WebGui
{

struct WebPage {
  const char* path;
  HTTPMethod method;
  ESP8266WebServer::THandlerFunction handler;
  const char* contentType = "text/plain";
};

private:
    WebServer *serverIn;
    SerialConsoleControl *controlIn;

public:


    WebGui();
    WebGui(WebServer *serverViR, SerialConsoleControl *control);
    
    void init();
    void renderRebootPage();
    
    [[deprecated]]
    String renderHomePage();
    
    String renderLightPage();
    String renderFilesPage();
    void renderControlPage();
    void renderSubmitPage();
    void handleWebRequests();
    void renderConfigPage();
    void renderFilePage();
    void renderDeletePage();
    void renderDeepSleepPage();
    void renderSetClockEntry();

    void registerPage(const WebPage& page);

};

#endif // WEB_GUI_H