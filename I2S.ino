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

/*

static void i2s_adc_task(void *arg)
{

//    i2s_driver_install(I2S_NUM_1, &i2s_config_rx, 0, NULL);
//    i2s_set_pin(I2S_NUM_1, &pin_config_rx);
      i2sInit();
      vTaskDelay(200);
//    int i2s_read_len = I2S_READ_LEN;
    size_t bytes_read;

    char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
    char* flash_write_buff = (char*) calloc(i2s_read_len, sizeof(char));

    Serial.println("ADC STARTED");
    while (1) {
        i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
//        i2s_adc_data_scale(flash_write_buff, i2s_read_buff, i2s_read_len);
//        ws.binary(clidx, (uint8_t*)i2s_read_buff, i2s_read_len);
//        ws.binary(clidx, (uint8_t*)flash_write_buff, i2s_read_len);
//        ets_printf("Never Used Stack Size: %u\n", uxTaskGetStackHighWaterMark(NULL));
      vTaskDelay(20);
    }
    
    free(i2s_read_buff);
    i2s_read_buff = NULL;
    free(flash_write_buff);
    flash_write_buff = NULL;

}

*/

time_t oldxxx = 0;


uint8_t buffer[50] = "hello world";


uint16_t xscale(uint8_t* s_buff, uint32_t len, uint16_t sdelay) {

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

void wavHeader(byte* header, int wavSize) {
  
    
  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  unsigned int fileSize = wavSize + headerSize - 8;
  header[4] = (byte)(fileSize & 0xFF);
  header[5] = (byte)((fileSize >> 8) & 0xFF);
  header[6] = (byte)((fileSize >> 16) & 0xFF);
  header[7] = (byte)((fileSize >> 24) & 0xFF);
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 0x10;
  header[17] = 0x00;
  header[18] = 0x00;
  header[19] = 0x00;
  header[20] = 0x01;
  header[21] = 0x00;
  header[22] = 0x01;
  header[23] = 0x00;
  header[24] = 0x80;
  header[25] = 0x3E;
  header[26] = 0x00;
  header[27] = 0x00;
  header[28] = 0x00;
  header[29] = 0x7D;
  header[30] = 0x00;
  header[31] = 0x00;
  header[32] = 0x02;
  header[33] = 0x00;
  header[34] = 0x10;
  header[35] = 0x00;
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(wavSize & 0xFF);
  header[41] = (byte)((wavSize >> 8) & 0xFF);
  header[42] = (byte)((wavSize >> 16) & 0xFF);
  header[43] = (byte)((wavSize >> 24) & 0xFF);

}
