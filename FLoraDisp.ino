#ifdef LORA
#include <SPI.h>
#include <LoRa.h>
#define SX1278_SCK  5
#define SX1278_MISO 19
#define SX1278_MOSI 27
#define SX1278_CS   18
#define SX1278_RST  14
#define SX1278_IRQ  26

#define BAND 868000000.00
#define spreadingFactor 9
#define SignalBandwidth 31.25E3
//Supported values are 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3, and 500E3.
#define preambleLength 8
#define codingRateDenominator 8
  int counter = 0;
bool linited = false;
void LoraInit() {
if (linited) return; else linited = true;
if(!LoRa.begin(BAND)) {
    sendEvent("Starting LoRa failed!");
    while(1);
}
  sendEvent("Starting LoRa ok");
  sendEvent("LoRa Frequency: " + String(BAND/1000000) + "MHz");
  sendEvent("LoRa Spreading Factor: " + String(spreadingFactor,DEC));
  LoRa.setSpreadingFactor(spreadingFactor);
  sendEvent("LoRa Signal Bandwidth: " + String(SignalBandwidth,DEC));
  LoRa.setSignalBandwidth(SignalBandwidth);
  LoRa.setCodingRate4(codingRateDenominator);
  LoRa.setPreambleLength(preambleLength);
  LoRa.setTxPower(20);

}
#endif

#ifdef DISP

void disp(String str) {
  display.clear();
  display.setFont(ArialMT_Plain_24);
  display.drawString(63, 10, str);
  display.display();
}
#endif

#ifdef BUTTON
SemaphoreHandle_t semaphore = nullptr;
 
void IRAM_ATTR handler(void* arg) {
  xSemaphoreGiveFromISR(semaphore, NULL);
}

#define ESP_INTR_FLAG_DEFAULT 0
#define BUTTON_PIN 0

void button_task(void* arg) {
  for(;;) {
    if(xSemaphoreTake(semaphore,portMAX_DELAY) == pdTRUE) {
      xdisplay = !xdisplay;
    }
  }
}
#endif
