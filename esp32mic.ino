#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <TimeLib.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
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
extern "C" {
#include "lwip/err.h"
#include "lwip/dns.h"
}
#include "config.h"

//#include <ArduinoWebsockets.h>

DNSServer dns;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");


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

    const char* hostName = "EcoBooster";
    const char* password = "freestyle";
    const char* http_username = "admin";
    const char* http_password = "admin";
    bool wifi = true;
    bool ota  = true;
    bool tcp  = true;
    bool display = false;
    bool lora = false;
    

} conf;
File xxxf; 

uint8_t clid = 0;
uint8_t clidx = 0;
bool xdisplay = false;
time_t mls,omls;
void loop(){
  mls = millis();
  ArduinoOTA.handle();
  dns.processNextRequest();
  if (tasks.size()) dotask(true); //else dotask(false);
//  ets_printf("> %lu\n",mls-omls);
  omls = mls;
}
