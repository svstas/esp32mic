String doshell(String msg) {
  msg.toLowerCase();
  if (msg.substring(0,1)=="!") {
    tasks.push_back(msg.substring(1));
    return "";
  }

        if (msg == "reset") {
          trace("Restarting...");
            sendEvent("Restarting...","ota");
              delay(1000);
              ESP.restart();
//              ESP.reset();
//          return "Restarting...";
          }
          
         if (msg == "free") return "Free heap memory : "+String(ESP.getFreeHeap())+" of "+String(ESP.getHeapSize());
         //+" "+String(ESP.getPsramSize())+" "+String(ESP.getFreePsram());

         if (msg == "psram") return "Free PSRAM memory : "+String(ESP.getFreePsram());//+" of "+String(ESP.getPsramSize());

         if (msg == "clid") return String(clid);
         if (msg == "mic") {

              micstart = !micstart;
         if (msg == "clid") return String(micstart);

         }

         if (msg == "time") {
            if (xdisplay) disp(String(printDateTime()));
          return String(printDateTime()).c_str();}
          
         if (msg == "ws") {clid = clidx; ws.text(clid,"test"); return "Starting... ";}

return "";
}
