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

String udpAddress = "192.168.88.234";


extern "C" {
#include "lwip/err.h"
#include "lwip/dns.h"
}
#include "config.h"

//#include <ArduinoWebsockets.h>

#ifdef DISP
#include <Wire.h>
#include "SSD1306.h"
#define OLED_ADDR 0x3C
#define OLED_SDA  4
#define OLED_SCL  15
#define OLED_RST  16

SSD1306 display(OLED_ADDR, OLED_SDA, OLED_SCL);
#endif

WiFiUDP udp;
DNSServer dns;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

int volume = 1;
TwoWire I2Cone = TwoWire(1);

#ifdef GEST
#include "RevEng_PAJ7620.h"
RevEng_PAJ7620 gest = RevEng_PAJ7620();
#endif 

#ifdef VLX
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
bool odisplay = false;
uint16_t sdelay = 300;
uint16_t srate = 22050;


time_t mls,omls;

 
bool isgest = false;
bool isadc  = false;
void loop(){
  mls = millis();
  ArduinoOTA.handle();
  dns.processNextRequest();
  if (tasks.size()) dotask(true); //else dotask(false);
//  ets_printf("> %lu\n",mls-omls);
  omls = mls;

  if (xdisplay!=odisplay) {
    sendEvent("DISP: "+String(xdisplay));
    if (xdisplay) display.displayOn(); else display.displayOff(); 
  }
  odisplay=xdisplay;
}
