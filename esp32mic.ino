#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <TimeLib.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>        
#include <SPIFFSEditor.h>
#include <Esp.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <pins_arduino.h>
#include <dummy.h> 
#include <HardwareSerial.h>
#include <FS.h>
#include <LITTLEFS.h>
#include <DNSServer.h>
#include <stdlib.h>
#include <pgmspace.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <Wire.h>
//#include <WiFiUdp.h>

extern "C" {
#include "lwip/err.h"
#include "lwip/dns.h"
}
#include "config.h"

//#include <ArduinoWebsockets.h>

WiFiUDP udp;
DNSServer dns;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

TwoWire I2Cone = TwoWire(1);

#ifdef GEST
#include "RevEng_PAJ7620.h"
RevEng_PAJ7620 gest = RevEng_PAJ7620();
#endif 

//    char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
//    char* flash_write_buff = (char*) calloc(i2s_read_len, sizeof(char));

//    char i2s_read_buff[10*1024];
//    char flash_write_buff[5*1024];


void sendEvent(String msg);

std::vector<String> tasks;
int flagtask = 0;
String xmessage = "-";
bool micstart = true;

AsyncEventSourceClient * xxclient;

struct conf {

    const char* hostName = "PonikaMic";
    const char* password = "josperado";
    const char* http_username = "admin";
    const char* http_password = "admin";
    int wifi = WIFI;
    bool ota  = true;
    bool tcp  = true;
    bool display = false;
    bool lora = false;
    

} conf;
File xxxf; 

uint8_t clid = 0;
uint8_t clidx = 0;
bool xdisplay = false;
uint16_t sdelay = 300;
uint16_t srate = 22050;


time_t mls,omls;


int ox,oy;
void loop(){
  mls = millis();
  ArduinoOTA.handle();
  dns.processNextRequest();
  if (tasks.size()) dotask(true); //else dotask(false);
//  ets_printf("> %lu\n",mls-omls);
  omls = mls;

#ifdef GEST
if( gest.isCursorInView() )
  {
    Serial.print("Has Cursor in view: ");
    digitalWrite(LED_BUILTIN, HIGH);
    int x = gest.getCursorX();
    int y = gest.getCursorY();
    if (x!=ox || y!=oy) sendEvent(String(x)+","+String(y)); 
//    Serial.println("(X,Y) (" + String(cursor_x) + "," + String(cursor_y) + ")");
    ox = x;oy = y;
  } else gesture();
#endif
}
