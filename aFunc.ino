#define countof(a) (sizeof(a) / sizeof(a[0]))

const char mjan[] PROGMEM = "Jan";const char mfeb[] PROGMEM = "Feb";const char mmar[] PROGMEM = "Mar";const char mapr[] PROGMEM = "Apr";const char mmay[] PROGMEM = "May";const char mjun[] PROGMEM = "Jun";const char mjul[] PROGMEM = "Jul";const char maug[] PROGMEM = "Aug";const char msep[] PROGMEM = "Sep";const char moct[] PROGMEM = "Oct";const char mnov[] PROGMEM = "Nov";const char mdec[] PROGMEM = "Dec";
const char * const  xmonth[] PROGMEM = {mjan,mfeb,mmar,mapr,mmay,mjun,mjul,maug,msep,moct,mnov,mdec};

void trace(const char* msg) {
  Serial.println(msg);
}
void trace(const __FlashStringHelper *str) {
  Serial.println(str);
  
}

void trace(const char* xfmt, const char* str) {
  Serial.printf(xfmt,str);
}

void sendEvent(String msg, String to) {
  events.send(msg.c_str(),to.c_str());
}

void sendEvent(String msg) {
  events.send(msg.c_str(),"ota");
}
String printDateTime() {
    char datestring[30];
//  time_t t = now()+3600*conf.timeZone;
  time_t t = now()+3600*3;

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%02u %02u:%02u:%02u"),
            year(t),            
            month(t),
            day(t),
            hour(t),
            minute(t),
            second(t));
    return String(datestring);
}

uint8_t gmonth(char * fmonth) {
  for (uint8_t i=0;i<12;i++) if (strcmp_P(fmonth,xmonth[i])==0) return i+1;
  return 0;
}

boolean isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

String toStringIp(IPAddress ip) {
String res = "";
for (uint8_t i=0;i<3;i++) {
  res += String((ip >> (8*i)) & 0xFF)+".";
}
  res += String(((ip >> 8*3)) & 0xFF);
  return res;
}
void handleRoot(AsyncWebServerRequest *request) {
//  Serial.println("HD_ROOT");
 if (!isIp(request->host())) {
//  Serial.println("HD_ROOT_______");
  
AsyncWebServerResponse *response = request->beginResponse(302,"text/plain","");
response->addHeader("Location",String("http://")+toStringIp(request->client()->localIP()));
request->send(response);
  
 }
}

bool loadConfig() {
    String par;
    String val;
  File f = LITTLEFS.open("/inits/config.ini", "r");
  if (!f) {trace(F("Failed to open config.ini")); return false;}
//  while(f.available()) {

//  }
  f.close();
  return true;
}

std::vector<String> ssplit(char * str, const char *sep) {
  std::vector<String>vc;
  char *token = strtok(str, sep);
  while (token != NULL) {
    vc.push_back(token);
    token = strtok(NULL, sep);
  };
  return vc;
  std::vector<String>(vc).swap(vc);
}
