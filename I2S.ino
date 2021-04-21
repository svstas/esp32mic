#include <driver/i2s.h>
const i2s_port_t I2S_PORT = I2S_NUM_0;
const int BLOCK_SIZE = 128;
const int headerSize = 44;
const char * udpAddress = "192.168.88.234";
bool mstarted = false;

#define I2S_WS 12
#define I2S_SD 23
#define I2S_SCK 13
#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE   (22050)
//#define I2S_SAMPLE_RATE   (44100)
#define I2S_SAMPLE_BITS   (16)
#define I2S_READ_LEN      (5 * 1024)
#define I2S_CHANNEL_NUM   (1)
#define FLASH_RECORD_SIZE (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8 * RECORD_TIME)

int i2s_read_len = I2S_READ_LEN;

bool i2sinited = false;

void i2sInit(){
  if (i2sinited) return; else i2sinited = true;
  
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = srate,
    .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
//    .dma_buf_count = 64,
//    .dma_buf_len = 1024,
    .dma_buf_count = 32,
    .dma_buf_len = 64,
    .use_apll = 1
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
  trace(F("I2S MIC started"));
}



static void i2s_adc_task(void *arg)
{
if (!isadc) isadc = true; else return;

   if (!i2sinited) {
    i2sInit();
    vTaskDelay(100);  
    }

  size_t bytes_read;
  char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
    Serial.println("ADC STARTED");
    while (1) {
        i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);

    if (clid) {ws.binary(clid, (uint8_t*)i2s_read_buff, xscale((uint8_t*)i2s_read_buff, i2s_read_len, sdelay));
    } else {
//      udp.beginPacket(udpAddress, 9000);
//      udp.write((uint8_t*)i2s_read_buff,i2s_read_len);
//      udp.write((uint8_t*)i2s_read_buff,200);
//      udp.endPacket();
    example_disp_buf((uint8_t*) i2s_read_buff, 48);
    }

        
//        i2s_adc_data_scale(flash_write_buff, i2s_read_buff, i2s_read_len);
//        ws.binary(clidx, (uint8_t*)i2s_read_buff, i2s_read_len);
//        ws.binary(clidx, (uint8_t*)flash_write_buff, i2s_read_len);
//        ets_printf("Never Used Stack Size: %u\n", uxTaskGetStackHighWaterMark(NULL));
//      vTaskDelay(20);
    }
    
    free(i2s_read_buff);
    i2s_read_buff = NULL;
 
}

time_t oldxxx = 0;


uint8_t buffer[50] = "hello world";


uint16_t xscale(uint8_t* s_buff, uint32_t len, uint16_t sdelay) {

    if (sdelay<2) return sdelay;
    
    uint16_t dac_value = 0;
    uint8_t xdrop = len/sdelay;
    uint32_t wpoint = 0;
    uint8_t xstep = 0;
    for (int i = 0; i < len; i += 2) {
     xstep++;
      if (xstep == xdrop) {xstep = 0; continue;}
    dac_value = (uint16_t)((s_buff[i]<<8)+s_buff[i+1]);
      if (dac_value>12) dac_value = dac_value*volume;  
     s_buff[wpoint+0] =  (dac_value & 0xff00)>>8;
     s_buff[wpoint+1] = (dac_value & 0xff);
     wpoint +=2;
    }
    return wpoint-2;
//    Serial.println()
}

void readmic() {

  ///                    Сделать событие на обрыв сети вырубить трансляцию
    if (!i2sinited) {
    i2sInit();
    vTaskDelay(100);  
    }
    
//    int flash_wr_size = 0;
    size_t bytes_read;
  
  char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
//  char* out_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
  
if (!mstarted) { mstarted = true;
	for (uint8_t i=0;i<4;i++) i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
}
    i2s_read(I2S_PORT, (uint8_t*)i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
    
    if (clid) {ws.binary(clid, (uint8_t*)i2s_read_buff, xscale((uint8_t*)i2s_read_buff, i2s_read_len, sdelay));
    } else {
//      udp.beginPacket(udpAddress, 9000);
//      udp.write((uint8_t*)i2s_read_buff,i2s_read_len);
//      udp.write((uint8_t*)i2s_read_buff,200);
//      udp.endPacket();
    example_disp_buf((uint8_t*) i2s_read_buff, 48);

    }

    

    free(i2s_read_buff);
    i2s_read_buff = NULL;

   if (micstart) tasks.push_back("rm");
}

void example_disp_buf(uint8_t* buf, int length) {
  String out = "";
    for (int i = 0; i < length; i++) out+=String(buf[i],HEX)+" ";
    sendEvent(out);
}
