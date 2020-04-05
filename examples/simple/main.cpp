#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WebService.h>

// configure server on port 80
WebService w{80};

// setup hooks
void setup() { w.setup(); }

// serve requests
// for more details try http://<deviceip>/ 
void loop() { w.process(); }
