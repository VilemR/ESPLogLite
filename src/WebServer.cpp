#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "html_index.h"
#include "html_page_template.h"
#include "html_control.h"

#include "html_index_default.h"
#include "html_files.h"
#include "html_command.h"
#include "html_config.h"
#include <WebServer.h>
#include "Config.h"

WebServer::WebServer()
{
  serverViR = new ESP8266WebServer(80);
  init();
}

void WebServer::init()
{
  serverViR->begin(80);
  serverViR->on("/", HTTP_GET, std::bind(&WebServer::handle_OnConnect, this));
  serverViR->onNotFound(std::bind(&WebServer::handle_NotFound, this));
}

String WebServer::getMainPageTemplate()
{
  String page = FPSTR(index_html);

  return page;
}

String WebServer::getConfigPageTemplate()
{
  String page = FPSTR(page_template_html);
  String body = FPSTR(config_html);
  page.replace("__body__", body);
  return page;
}

String WebServer::getControlPageTemplate()
{
  String page = FPSTR(page_template_html);
  String body = FPSTR(control_html);
  page.replace("__body__", body);
  return page;
}

String WebServer::getFilesPageTemplate()
{
  String page = FPSTR(page_template_html);
  String body = FPSTR(index_files);
  page.replace("__body__", body);
  return page;
}

void WebServer::handle_OnConnect()
{
  serverViR->sendHeader("Location", "/files", true);
  serverViR->send(302, "text/plain", "");
}

void WebServer::handle_NotFound()
{
  serverViR->send(404, "text/plain", "Not found");
}

void WebServer::handleRequests()
{
  serverViR->handleClient();
}
