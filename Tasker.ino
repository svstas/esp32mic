TaskHandle_t i2sADCHandler = NULL;

void dotask(bool flag) {
String xt;
if (flag) {xt = tasks.back();tasks.pop_back();}
if (xt=="rm") readmic();
//if (xt=="adc") xTaskCreatePinnedToCore(gesture, "gesture", 4096, NULL, 1, &i2sADCHandler,1);

if (xt=="i2c") { scan_i2c();return;} 

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
