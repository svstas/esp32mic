TaskHandle_t i2sADCHandler = NULL;

void dotask(bool flag) {
String xt;
if (flag) {xt = tasks.back();tasks.pop_back();}
if (xt=="rm") readmic();
if (xt=="adc") xTaskCreate(i2s_adc_task, "i2s_adc_task", 16192, NULL, 1, &i2sADCHandler);

if (xt.substring(0,4)=="disp") {
sendEvent(xt.substring(4));
return;  
}
if (xt=="i2s") i2sInit();
if (xt=="lora") {
LoraInit();
return;
}
}
