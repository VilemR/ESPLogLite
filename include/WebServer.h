#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

class WebServer
{
public:
    ESP8266WebServer *serverViR;

public:
    WebServer();
    void init();
    void handleRequests();
    String getMainPageTemplate();
    String getControlPageTemplate();
    String getConfigPageTemplate();
    String getLightIntensityPageTemplate();
    String getFilesPageTemplate();
    void handle_OnConnect();
    void handle_NotFound();
};

#endif // WEB_SERVER_H
