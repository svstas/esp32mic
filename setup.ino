void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    clidx = client->id();
    client->ping(); 
  } else if(type == WS_EVT_DISCONNECT){
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
//      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());
//      int smpls = msg.toInt();
//      int sm = 22050; if (smpls>35000) sm = 44100; 
//      sdelay = (sm - smpls)/2;
//      if (!(sdelay % 2)) sdelay++;
//      if (sdelay>1000) sdelay = 1000;
    
      Serial.print("DELAY: ");
      Serial.println(sdelay);
      Serial.print("msg: ");
      Serial.println(msg.toInt());

      if (sdelay<=0) 
      sdelay=300;                                       // Синхронизация звука

      if(info->opcode == WS_TEXT) {
//        client->text("I got your text message");
      }
      else
        client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if((info->index + len) == info->len){
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}

void setup(){

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  I2Cone.begin(21, 22);
  
#ifdef BUTTON  
  semaphore = xSemaphoreCreateBinary();
  gpio_pad_select_gpio((gpio_num_t)0);
  gpio_set_direction((gpio_num_t)0, GPIO_MODE_INPUT);
  gpio_set_intr_type((gpio_num_t)0, GPIO_INTR_NEGEDGE);
  xTaskCreate(button_task, "button_task", 4096, NULL, 10, NULL);
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  gpio_isr_handler_add((gpio_num_t)BUTTON_PIN, handler, NULL);
#endif
 
 pinMode(LED, OUTPUT);

  trace(F("System starting..."));
  LITTLEFS.begin();
  trace(F("FS init"));
  if (loadConfig()) trace(F("Config loaded"));

#ifdef DISP
 pinMode(OLED_RST, OUTPUT);
 digitalWrite(LED,HIGH);
 digitalWrite(OLED_RST, LOW);delay(50); digitalWrite(OLED_RST, HIGH);
 display.setTextAlignment(TEXT_ALIGN_CENTER);
 display.init(); delay(200);display.clear();
  trace(F("Display started"));
#endif

#ifdef LORA
  SPI.begin(SX1278_SCK, SX1278_MISO, SX1278_MOSI, SX1278_CS);
  LoRa.setPins(SX1278_CS, SX1278_RST, SX1278_IRQ);
  trace(F("LoRa started"));
#endif

    
if (conf.wifi ==  1) {  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(conf.hostName,conf.password);
  delay(2000);
  WiFi.softAPConfig(ip, ip, netmask);
  trace(F("WiFi Started"));
} else if (conf.wifi ==  2) {
  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.setBreakAfterConfig(true);
  wifiManager.resetSettings();

  if (!wifiManager.autoConnect("PonikaMic", "11111111")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.restart();
    ESP.restart();
    delay(5000);
  }
}

if (conf.ota) {
  ArduinoOTA.onStart([]() { events.send("Update Start", "ota"); });
  ArduinoOTA.onEnd([]() { events.send("Update End", "ota"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    char p[32];
    sprintf(p, "Progress: %u%%\n", (progress/(total/100)));
    events.send(p, "ota");
  });
  ArduinoOTA.onError([](ota_error_t error) {
    if(error == OTA_AUTH_ERROR) events.send("Auth Failed", "ota");
    else if(error == OTA_BEGIN_ERROR) events.send("Begin Failed", "ota");
    else if(error == OTA_CONNECT_ERROR) events.send("Connect Failed", "ota");
    else if(error == OTA_RECEIVE_ERROR) events.send("Recieve Failed", "ota");
    else if(error == OTA_END_ERROR) events.send("End Failed", "ota");
  });
  ArduinoOTA.setHostname(conf.hostName);
  ArduinoOTA.begin();
}

  MDNS.addService("http","tcp",80);

    WiFi.onEvent(WiFiEvent);


if (conf.tcp) {
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  events.onConnect([](AsyncEventSourceClient *client){
    client->send("hello!",NULL,millis(),1000);
    xxclient = client;
    digitalWrite(2,LOW);
  });
  server.addHandler(&events);

  server.addHandler(new SPIFFSEditor(LITTLEFS, conf.http_username,conf.http_password));

  server.on("/generate_204", handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request) {request->send(200, "text/plain", String(now()));});

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){request->send(200, "text/plain", String(ESP.getFreeHeap()));});

  server.on("/_cmd", HTTP_GET, [](AsyncWebServerRequest *request){if(request->method() == HTTP_GET && request->hasParam("cmd")){
    String cres = doshell(request->getParam("cmd")->value());
    if (!cres.length()) request->send(304); else request->send(200,"text/html",cres);
    cres = String();}});

  server.serveStatic("/", LITTLEFS, "/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *request){
    handleRoot(request);
//    request->send(404);
  });
  server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
//    if(!index)
//      trace("UploadStart: %s\n", filename.c_str());
//      trace("%s", (const char*)data);
//    if(final)  trace("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index)
      Serial.printf("BodyStart: %u\n", total);
      Serial.printf("%s", (const char*)data);
    if(index + len == total)
      Serial.printf("BodyEnd: %u\n", total);
  });
  server.begin();
 }
  dns.setErrorReplyCode(DNSReplyCode::NoError);
  dns.start(53, "*", IPAddress(192,168,4,1));
  Serial.println("DNS Started");
//  i2sInit();

}
