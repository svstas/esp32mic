#ifdef GEST
static void gesture(void *ignore) {

   if( !gest.begin(&I2Cone))             // return value of 1 == success
  {
    Serial.print("PAJ7620 init error -- device not found -- halting");
    while(true) {}
  }
//    gest.setCursorMode(); 
    Serial.println("PAJ7620 started");
  Gesture xgesture;
  
int ox,oy;
  for (;;) {

if (gest.isCursorInView()) {

    int x = gest.getCursorX();
    int y = gest.getCursorY();
    if (x!=ox || y!=oy) sendEvent(String(x)+","+String(y)); 
    ox = x;oy = y;
  
  } else { 
    
  xgesture = gest.readGesture();   // Read back current gesture (if any) of type Gesture
  switch (xgesture)
  {
    case GES_FORWARD:
      {
        Serial.print("GES_FORWARD");
         sendEvent("FWD");

        break;
      }

    case GES_BACKWARD:
      {
        Serial.print("GES_BACKWARD");
         sendEvent("BACK");

        break;
      }

    case GES_LEFT:
      {
        Serial.print("GES_LEFT");

         xdisplay = 1;
//         display.displayOn();
//         tasks.push_back("dsp 1");          
//         sendEvent("DISPLAY: ON");
        break;
      }

    case GES_RIGHT:
      {
        Serial.print("GES_RIGHT");
         xdisplay = 0;
//         display.displayOff();
//         tasks.push_back("dsp 0");
//         sendEvent("DISPLAY: OFF");
//         vTaskDelay(20);
        break;
      }

    case GES_UP:
      {
        Serial.print("GES_UP");
        break;
      }

    case GES_DOWN:
      {
        Serial.print("GES_DOWN");
        break;
      }

    case GES_CLOCKWISE:
      {
        Serial.print("GES_CLOCKWISE");
        sendEvent("CW");

        break;
      }

    case GES_ANTICLOCKWISE:
      {
        Serial.print("GES_ANTICLOCKWISE");
        sendEvent("CCW");
        break;
      }

    case GES_WAVE:
      {
         sendEvent("WAVE");
 
        Serial.print("GES_WAVE");
        break;
      }

    case GES_NONE:
      {
        break;
      }
  }

  if( xgesture != GES_NONE )
  {
    Serial.print(", Code: ");
    Serial.println(xgesture);
  }
}
  vTaskDelay(5);
  }
  
}
#endif
