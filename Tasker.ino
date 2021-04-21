//#ifdef GEST
TaskHandle_t GestHandler = NULL;
//#endif

TaskHandle_t adcHandler = NULL;

void dotask(bool flag) {
String xt;
if (flag) {xt = tasks.back();tasks.pop_back();}
//if (xt=="rm") readmic();

//#ifdef GEST
if (xt=="adc") {
xTaskCreatePinnedToCore(i2s_adc_task, "adc", 8192, NULL, 1, &adcHandler,1);
}

if (xt=="off") if (adcHandler != NULL) {vTaskDelete(adcHandler);}
if (xt=="goff") if (GestHandler != NULL) {vTaskDelete(GestHandler);isgest=false;sendEvent("GESTURE: OFF");}

if (xt=="gest") {
if (!isgest) {sendEvent("GESTURE: ON");xTaskCreatePinnedToCore(gesture, "gesture", 8192, NULL, 2, &GestHandler,1);isgest=true;}
//else {vTaskDelete(&GestHandler);isgest = false;sendEvent("GESTURE: OFF");}
}
//#endif

if (xt=="i2c") { scan_i2c();return;} 


if (xt.substring(0,4)=="dsp") {
uint8_t stat = xt.substring(4).toInt();
//digitalWrite(OLED_RST, LOW);delay(50); digitalWrite(OLED_RST, HIGH);
  if (stat) { 
    digitalWrite(OLED_RST, LOW);vTaskDelay(50);digitalWrite(OLED_RST, HIGH);
    display.displayOn();
  } else {
    digitalWrite(OLED_RST, LOW);
    display.displayOff();
  }
  vTaskDelay(20);
  sendEvent("OK");
}

if (xt=="i2s") i2sInit();
if (xt=="lora") LoraInit();

}
