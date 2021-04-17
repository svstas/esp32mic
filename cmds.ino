#ifdef DISP
#endif
bool xcursor = false;
String doshell(String msg) {
  msg.toLowerCase();
  if (msg.substring(0,1)=="!") {
    tasks.push_back(msg.substring(1));
    return "";
  }
#ifdef GEST
        if (msg == "cursor") {
            xcursor = !xcursor;
            if (xcursor) gest.setCursorMode(); else gest.setGestureMode();
            return "Cursor:"+String(xcursor);
        }
#endif  

        if (msg == "reset") {
          trace("Restarting...");
            sendEvent("Restarting...","ota");
              delay(1000);
              ESP.restart();
//              ESP.reset();
//          return "Restarting...";
          }
         if (msg == "i2c") tasks.push_back("i2c");
         if (msg == "free") return "Free heap memory : "+String(ESP.getFreeHeap())+" of "+String(ESP.getHeapSize());
         //+" "+String(ESP.getPsramSize())+" "+String(ESP.getFreePsram());

         if (msg == "psram") return "Free PSRAM memory : "+String(ESP.getFreePsram());//+" of "+String(ESP.getPsramSize());

         if (msg == "clid") return String(clid);
         if (msg == "mic")  micstart = !micstart;
         if (msg == "clid") return String(micstart);
         if (msg == "time") {
            if (xdisplay) disp(String(printDateTime()));
          return String(printDateTime()).c_str();}

         if (msg.substring(0,3)=="dls") {
            sdelay = msg.substring(4).toInt();
            return ""; 
          }
 
         if (msg.substring(0,3)=="vol") {
            volume = msg.substring(4).toInt();
//            sscanf(msg.substring(4).c_str(),"%f", &volume);
            return "Set volume: "+String(volume);  
          }

          if (msg.substring(0,3)=="srt") {
            srate = msg.substring(4).toInt();
            
          }
         
         if (msg == "ws") {clid = clidx; return "Starting... ";}

return "";
}
