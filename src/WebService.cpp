#include "WebService.h"

#include <FS.h>
#include <WiFiManager.h>
#if defined(ESP32)
#include <Update.h>
#include <SPIFFS.h>
#endif

WebService::WebService(int16_t port) :
        _private::WebServerType(port),
#if defined(ESP8266)
        updateServer(),
#endif
        mdns(MDNS)
{}

void WebService::setup()
{
    mdns.begin("Relays Board");
#if defined(ESP8266)
    updateServer.setup(this, "/firmware");
#elif defined(ESP32)
    on("/firmware", HTTP_POST, [&](){onUpdate();}, [&](){onUpdateUpload();});
#endif
    begin();
    MDNS.addService("http", "tcp", 80);

    on("/", [&]()
    { indexHtml(); });
    on("/files", [&]()
    { listFiles(); });
    on("/reboot", [&]()
    { reboot(); });
    on("/status", [&]()
    { getDeviceStatus(); });
    on("/api/relayboardsettings/wifi/reset", [&]()
    { resetSettings(); });
}

void WebService::indexHtml()
{
    Serial.println("WebService::on: /");

    String info;
    info += "<html>";
    info += "  <h1> Firmware ";
    info += ESP.getSketchMD5();
    info += " size  ";
    info += ESP.getSketchSize();
    info += "  </h1> ";
    if (extra_pre_html.length() > 0) info += extra_pre_html;
    info += "  <a href=/status>device status</a><br/>";
    info += "  <a href=/firmware>upload firmware</a><br/>";
    info += "  <a href=/files>ls</a><br/>";
    info += "  <a href=/reboot>reboot (switch from wifi to normal mode)</a><br/>";
    info += "  <a href=/api/relayboardsettings/wifi/reset>reset WiFi relayboardsettings</a><br/>";
    if (extra_post_html.length() > 0) info += extra_post_html;
    info += "</html>";
    send(200, "text/html", info);
}

void WebService::reboot()
{
    Serial.println("WebService::on: /reboot");
    ESP.restart();
}

void WebService::getDeviceStatus()
{
    Serial.println("WebService::on: /status");

    String status;
#if defined(ESP8266)
    status += "<html>"
              "  <table>"
              "    <tr>"
              "      <th>status</th>"
              "      <th>value</th>"
              "    </tr>"
              "    <tr>"
              "      <td>shutdown reason</td>"
              "      <td>";
    status += ESP.getResetReason().c_str();
    status += "      </td>"
              "    </tr>";

    status +=
              "    <tr>"
              "      <td>VCC [mV]</td>"
              "      <td>";
    status += ESP.getVcc();
    status += "      </td>"
              "    </tr>";
#endif

    status += "    <tr>"
              "      <td>free heap [B]</td>"
              "      <td>";
    status += ESP.getFreeHeap();
    status += "      </td>"
              "    </tr>";

#if defined(ESP8266)

    status += "    <tr>"
              "      <td>max. free block size </td>"
              "      <td>";
    status += ESP.getMaxFreeBlockSize();
    status += "</td>"
              "    </tr>";

    status += "    <tr>"
              "      <td>heap fragmentation [%]</td>"
              "      <td>";
    status += ESP.getHeapFragmentation();
    status += "      </td>"
              "    </tr>";

    status += "    <tr>"
              "      <td>free continuous stack [B]</td>"
              "      <td>";
    status += ESP.getFreeContStack();
    status += "      </td>"
              "    </tr>";

#endif

    status += "    <tr>"
              "      <td>SDK version</td>"
              "      <td>";
    status += ESP.getSdkVersion();
    status += "      </td>"
              "    </tr>";

#if defined(ESP8266)

    status +="    <tr>"
              "      <td>core version</td>"
              "      <td>";
    status += ESP.getCoreVersion();
    status += "      </td>"
              "    </tr>";

    status += "    <tr>"
              "      <td>full version</td>"
              "      <td>";
    status += ESP.getFullVersion();
    status += "</td>"
              "    </tr>";

    status += "    <tr>"
              "      <td>boot loader version</td>"
              "      <td>";
    status += ESP.getBootVersion();
    status += "</td>"
              "    </tr>";
    status += "    <tr>"
              "      <td>boot mode</td>"
              "      <td>";
    status += ESP.getBootMode();
    status += "      </td>"
              "    </tr>";

#elif defined(ESP32)

    status+= "    <tr>"
              "      <td>chip revision</td>"
              "      <td>";
    status += ESP.getChipRevision();
    status += "      </td>"
              "    </tr>";
#endif
    status +="    <tr>"
              "      <td>CPU frequency [MHz]</td>"
              "      <td>";
    status += ESP.getCpuFreqMHz();
    status += "      </td>"
              "    </tr>";

#if defined(ESP8266)
    status += "    <tr>"
              "      <td>flash chip ID</td>"
              "      <td>";
    status += ESP.getFlashChipId();
    status += "</td>"
              "    </tr>";

    status += "    <tr>"
              "      <td>flash chip vendor ID</td>"
              "      <td>";
    status += ESP.getFlashChipVendorId();
    status += "      </td>"
              "    </tr>";

    status += "    <tr>"
              "      <td>flash chip real size[B]</td>"
              "      <td>";
    status += ESP.getFlashChipRealSize();

    status += "      </td>"
              "    </tr>";

#endif

    status += "    <tr>"
              "      <td>flash chip size [B]</td>"
              "      <td>";
    status += ESP.getFlashChipSize();
    status += "      </td>"
              "    </tr>";

    status += "    <tr>"
              "      <td>flash chip speed [Hz]</td>"
              "      <td>";
    status += ESP.getFlashChipSpeed();
    status += "      </td>"
              "    </tr>";

    status += "    <tr>"
              "      <td>flash chip mode</td>"
              "      <td>";
    status += ESP.getFlashChipMode();
    status += "      </td>"
              "    </tr>";

    #if defined(ESP8266)
    status += "    <tr>"
              "      <td>flash chip size by chip ID</td>"
              "      <td>";

    status += ESP.getFlashChipSizeByChipId();
    status += "      </td>"
              "    </tr>";

    #endif

    status += "    <tr>"
              "      <td>sketch size [B]</td>"
              "      <td>";
    status += ESP.getSketchSize();
    status += "      </td>"
              "    </tr>";

    status += "    <tr>"
              "      <td>sketch MD5</td>"
              "      <td>";
    status += ESP.getSketchMD5();
    status += "      </td>"
              "    </tr>";

    status += "    <tr>"
              "      <td>free sketch space [B]</td>"
              "      <td>";
    status += ESP.getFreeSketchSpace();

    status += "      </td>"
              "    </tr>";

#if defined(ESP8266)

    status += "    <tr>"
              "      <td>reset reason</td>"
              "      <td>";
    status += ESP.getResetReason();
    status += "      </td>"
              "    </tr>";

    status += "    <tr>"
              "      <td>reset info</td>"
              "      <td>";
    status += ESP.getResetInfo();
    status += "      </td>"
              "    </tr>";

#endif

    status += "  <table>"
              "</html>";

    send(200, "text/html", status);
}

void WebService::resetSettings()
{
    WiFiManager wifi_manager;
    wifi_manager.resetSettings();
    ESP.restart();
}

bool WebService::process()
{
    static HTTPClientStatus client_status = _private::WebServerType::_currentStatus;
    _private::WebServerType::handleClient();

    if (client_status != _private::WebServerType::_currentStatus)
    {
        client_status = _private::WebServerType::_currentStatus;
        return true;
    }
    return false;
}

#if defined(ESP32)
void WebService::onUpdate()
{
    sendHeader("Connection", "close");
    send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
}
void WebService::onUpdateUpload()
{
    HTTPUpload& upload = this->upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        Serial.setDebugOutput(true);
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin()) { //start with max available size
            Update.printError(Serial);
        }
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
        {
            Update.printError(Serial);
        }
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (Update.end(true)) { //true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        }
        else
        {
            Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
    } else
        {
        Serial.printf("Update Failed Unexpectedly (likely broken connection): status=%d\n", upload.status);
    }
}

#endif

#if defined(ESP8266)

void WebService::listFiles()
{
    Serial.println("WebService::on: /files");
    String file_listing;

    std::function<void(Dir &)> iterate_files = [&](Dir &dir)
    {
        while (dir.next())
        {
            if (dir.isDirectory())
            {
                iterate_files(dir);
            } else
            {
                File f = dir.openFile("r");
                file_listing.concat(dir.fileName() + " " + f.size() + "B\n");
                f.close();
            }
        }
    };

    SPIFFS.begin();
    Dir dir = SPIFFS.openDir("/");
    iterate_files(dir);
    SPIFFS.end();
    send(200, "text/plain", file_listing);
}

#elif defined(ESP32)

void WebService::listFiles()
{
    Serial.println("WebService::on: /files");
    String file_listing;

    std::function<void(const char *, uint8_t)> ls  = [&ls, &file_listing](const char * dirname, uint8_t levels) {
        fs::FS &fs = SPIFFS;

        Serial.printf("Listing directory: %s\r\n", dirname);

        File root = fs.open(dirname);
        if (!root) {
            Serial.println("- failed to open directory");
            return;
        }
        if (!root.isDirectory()) {
            Serial.println(" - not a directory");
            return;
        }

        File file = root.openNextFile();
        while (file) {
            if (file.isDirectory()) {
                Serial.print("  DIR : ");
                Serial.println(file.name());
                if (levels) {
                    ls(file.name(), levels - 1);
                }
            } else {
                Serial.print("  FILE: ");
                Serial.print(file.name());
                Serial.print("\tSIZE: ");
                Serial.println(file.size());
                file_listing.concat(String(file.name()) + " " + file.size() + "B\n");
            }
            file = root.openNextFile();
        }
    };

    const char * dirname = "/";
    uint8_t levels = 35;
    SPIFFS.begin();
    ls(dirname , levels);
    SPIFFS.end();

    send(200, "text/plain", file_listing);
}

#endif