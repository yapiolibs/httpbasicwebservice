#pragma once

#if defined(ESP8266)
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <WebServer.h>
#include <HTTPUpdate.h>
#include <ESPmDNS.h>
#endif

//class Print;

namespace _private
{
#if defined(ESP8266)
using WebServerType = ESP8266WebServer;
using HttpUpdateServerType = ESP8266HTTPUpdateServer;
#elif defined(ESP32)
using WebServerType = WebServer;
#endif
} // namespace private

class WebService : public _private::WebServerType
{
protected:
#if defined(ESP8266)
    ESP8266HTTPUpdateServer updateServer;
#endif
    MDNSResponder &mdns;
    String extra_pre_html;
    String extra_post_html;

public:
    explicit WebService(int16_t port = 80);

    virtual void setup();

    bool process();

protected:
    void indexHtml();

    void listFiles();

    static void reboot();

    void getDeviceStatus();

    static void resetSettings();

#if defined(ESP32)

    void onUpdate();

    void onUpdateUpload();

#endif
};

