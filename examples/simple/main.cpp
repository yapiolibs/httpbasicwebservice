#include <WebService.h>

// configure ser on port 80
WebService w{80};

// setup hooks
void setup() { w.setup(); }

// serve requests
// for more details try http://<yourip>/ 
void loop() { w.process(); }
