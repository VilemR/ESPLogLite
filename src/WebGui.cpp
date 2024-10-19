#include <Arduino.h>
#include "WebGui.h"
#include "WebServer.h"
#include "SerialConsoleControl.h"
#include "Config.h"

WebGui::WebGui() {};

using RenderHandlerPtr = void (WebGui::*)();

WebGui::WebGui(WebServer *serverViR, SerialConsoleControl *control)
{
  serverIn = serverViR;
  controlIn = control;
  init();
}


void WebGui::init()
{
  serverIn->serverViR->on("/files", HTTP_GET, std::bind(&WebGui::renderFilesPage, this));
  serverIn->serverViR->on("/file", HTTP_GET, std::bind(&WebGui::renderFilePage, this));
  serverIn->serverViR->on("/control", HTTP_GET, std::bind(&WebGui::renderControlPage, this));
  serverIn->serverViR->on("/submit", HTTP_POST, std::bind(&WebGui::renderSubmitPage, this));
  serverIn->serverViR->on("/config", HTTP_GET, std::bind(&WebGui::renderConfigPage, this));
  serverIn->serverViR->on("/reboot", HTTP_GET, std::bind(&WebGui::renderRebootPage, this));
  serverIn->serverViR->on("/delete", HTTP_GET, std::bind(&WebGui::renderDeletePage, this));
}

void WebGui::renderRebootPage()
{
  serverIn->serverViR->sendHeader("Location", "/control", true);
  serverIn->serverViR->send(302, "text/plain", "Reboot in progress..");
  ESP.restart();
}

void WebGui::renderDeletePage()
{
  String paramValue = serverIn->serverViR->arg("name");
  controlIn->logger->i("Delete command " + paramValue + " received.");

  if (SPIFFS.exists(paramValue))
  {
    SPIFFS.remove(paramValue);
    serverIn->serverViR->sendHeader("Location", "/files", true);
    serverIn->serverViR->send(302, "text/plain", paramValue);
  }
  else
  {
    serverIn->serverViR->send(404, "text/plain", "File not found (not deleted)!");
  }
}

String CFG_FILENAME = "appconfig.cfg";
void WebGui::renderConfigPage()
{
  String newConfig = serverIn->serverViR->arg("configText");
  if (newConfig != nullptr && newConfig != "")
  {
    File file = SPIFFS.open(CFG_FILENAME, "w");
    file.println(newConfig);
    file.close();
  }
  String page = serverIn->getConfigPageTemplate();
  page.replace("__PAGE_TITLE__", HOSTNAME + " : Config");
  if (SPIFFS.exists(CFG_FILENAME))
  {
    File photoFile = SPIFFS.open(CFG_FILENAME, "r");
    if (photoFile)
    {
      String fileContent = photoFile.readString();
      photoFile.close();
      page.replace("__config__", fileContent);
      serverIn->serverViR->sendHeader("Location", "/config", true);
      serverIn->serverViR->sendHeader("Cache-Control", "no-store, must-revalidate, max-age=0");
      serverIn->serverViR->send(200, "text/html", page);
      return;
    }
    else
    {
      Serial.println("Failed to CFG open file!");
    }
  }
  else
  {
    serverIn->serverViR->send(404, "text/plain", "File not found");
  }
  return;
}

void WebGui::renderSubmitPage()
{
  String command = serverIn->serverViR->arg("commandInput");
  controlIn->logger->i("Web command " + command + " received.");
  serverIn->serverViR->sendHeader("Location", "/control", true);
  serverIn->serverViR->send(302, "text/plain", command);
  controlIn->decodeWebCommand(command);
}

void WebGui::renderControlPage()
{
  String page = serverIn->getControlPageTemplate();
  String br = controlIn->logger->outscan;
  br.replace("\n", "<br>");
  page.replace("__log__", br);
  page.replace("__PAGE_TITLE__", HOSTNAME + " : Control");
  serverIn->serverViR->send(200, "text/html", page);
  return;
}

void WebGui::renderFilePage()
{
  String photoFileName = serverIn->serverViR->arg("name");
  String contentType = "text/plain";
  if (SPIFFS.exists(photoFileName))
  {
    File photoFile = SPIFFS.open(photoFileName, "r");
    if (SHOWFILE_INSTEAD_OF_DOWNLOAD == false)
    {
      serverIn->serverViR->sendHeader("Content-Disposition", "attachment; filename=" + HOSTNAME + photoFileName, true);
      serverIn->serverViR->sendHeader("X-Content-Type-Options", "nosniff", true);
    }
    serverIn->serverViR->streamFile(photoFile, contentType);
    photoFile.close();
  }
  else
  {
    serverIn->serverViR->send(404, "text/plain", "File not found");
  }
}

String WebGui::renderFilesPage()
{
  String page = serverIn->getFilesPageTemplate();
  String html = "";
  Dir dir = SPIFFS.openDir("/");
  int spiffFiles = 0;
  while (dir.next())
  {
    String fileName = dir.fileName();
    html += "<li><small><a href='/file?name=" + fileName + "'>" + fileName + "</a></small> ";
    html += "<a href='/delete?name=" + fileName + "'>D</a>";
    html += "<small><small> (" + String(dir.fileSize()) + ") </small></small>";
    html += "</li>";
    spiffFiles = spiffFiles + 1;
  }
  html += "</ul>";
  page.replace("__files__", html);
  page.replace("__footer__", controlIn->logger->getStorageIndicator());
  page.replace("__PAGE_TITLE__", HOSTNAME + " : Files");
  page.replace("__count__", String(spiffFiles));
  page.replace("__logfile__", String(controlIn->targetLogFileName));
  page.replace("__runtime__", String(((millis() - run_time_since)/1000)));
  serverIn->serverViR->sendHeader("Cache-Control", "no-store, must-revalidate, max-age=0");
  serverIn->serverViR->send(200, "text/html", page);
  return page;
}

void WebGui::handleWebRequests()
{
  serverIn->handleRequests();
}
