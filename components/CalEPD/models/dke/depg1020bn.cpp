#include "dke/depg1020bn.h"

#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
//Place data into DRAM. Constant data gets placed into DROM by default, which is not accessible by DMA.

const epd_lut_159 Depg1020bn::lut_4_grays={
0x32, {
  0x40,	0x48,	0x80,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
0x8,	0x48,	0x10,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
0x2,	0x48,	0x4,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
0x20,	0x48,	0x1,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
0xA,	0x19,	0x0,	0x3,	0x8,	0x0,	0x0,					
0x14,	0x1,	0x0,	0x14,	0x1,	0x0,	0x3,					
0xA,	0x3,	0x0,	0x8,	0x19,	0x0,	0x0,					
0x1,	0x0,	0x0,	0x0,	0x0,	0x0,	0x1,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
0x22,	0x22,	0x22,	0x22,	0x22,	0x22,	0x0,	0x0,	0x0,			
0x22,	0x17,	0x41,	0x0,	0x32,	0x1C
},159};

DRAM_ATTR const epd_init_3 Depg1020bn::GDOControl={
0x01,{(DEPG1020BN_HEIGHT - 1) % 256, (DEPG1020BN_HEIGHT - 1) / 256, 0x00},3
};

// Constructor
Depg1020bn::Depg1020bn(EpdSpi& dio): 
  Adafruit_GFX(DEPG1020BN_WIDTH, DEPG1020BN_HEIGHT),
  Epd(DEPG1020BN_WIDTH, DEPG1020BN_HEIGHT), IO(dio)
{
  printf("Depg1020bn() %d*%d\n",
  DEPG1020BN_WIDTH, DEPG1020BN_HEIGHT);  
}

void Depg1020bn::initFullUpdate(){
    _wakeUp(0x01);
    _PowerOn();
    if (debug_enabled) printf("initFullUpdate() LUT\n");
}

//Initialize the display
void Depg1020bn::init(bool debug)
{
    debug_enabled = debug;
    if (debug_enabled) printf("Depg1020bn::init(%d)\n", debug);
    IO.init(4, debug); // 4MHz frequency

    printf("Free heap:%d\n", (int)xPortGetFreeHeapSize());
    fillScreen(EPD_WHITE);
    _mono_mode = 1;
    fillScreen(EPD_WHITE);
}

void Depg1020bn::fillScreen(uint16_t color)
{
  if (_mono_mode) {
    // 0xFF = 8 pixels black, 0x00 = 8 pix. white
    uint8_t data = (color == EPD_BLACK) ? DEPG1020BN_8PIX_BLACK : DEPG1020BN_8PIX_WHITE;
    for (uint32_t x = 0; x < sizeof(_mono_buffer); x++)
    {
      _mono_buffer[x] = data;
    }
  } else {
    // 4 Grays mode
    // This is to make faster black & white
    if (color == 255 || color == 0) {
      for(uint32_t i=0;i<DEPG1020BN_BUFFER_SIZE;i++)
      {
        _buffer1[i] = (color == 0xFF) ? 0xFF : 0x00;
        _buffer2[i] = (color == 0xFF) ? 0xFF : 0x00;
      }
    return;
     }
   
    for (uint32_t y = 0; y < DEPG1020BN_HEIGHT; y++)
    {
      for (uint32_t x = 0; x < DEPG1020BN_WIDTH; x++)
      {
        drawPixel(x, y, color);
        if (x % 8 == 0)
          {
            vTaskDelay(pdMS_TO_TICKS(2));
          }
      }
    }
  }
 
  if (debug_enabled) printf("fillScreen(%d) _mono_buffer len:%d\n",color,sizeof(_mono_buffer));
}

// Now redefined as 4 gray mode
void Depg1020bn::_wakeUp(){
  printf("_wakeUp 4 Gray\n");
  IO.reset(10);
  IO.cmd(0x12);  //SWRESET
  _waitBusy("SWRESET");
  
  IO.cmd(0x74); //set analog block control       
	IO.data(0x54);
	IO.cmd(0x7E); //set digital block control          
	IO.data(0x3B);

	IO.cmd(0x01); //Driver output control      
	IO.data(0x07);
	IO.data(0x01);
	IO.data(0x00);

	IO.cmd(0x11); //data entry mode       
	IO.data(0x01);

  IO.cmd(0x44);
  IO.data(0x00); // RAM x address start at 0
  IO.data(0x00);
  IO.data(0xBF); // RAM x address end at 36Fh -> 879
  IO.data(0x03);
  IO.cmd(0x45);
  IO.data(0x7F); // RAM y address start at 20Fh;
  IO.data(0x02);
  IO.data(0x00); // RAM y address end at 00h;
  IO.data(0x00);

	IO.cmd(0x3C); //BorderWavefrom
	IO.data(0x00);	

	IO.cmd(0x2C);     //VCOM Voltage
	IO.data(lut_4_grays.data[158]);    //0x1C

	IO.cmd(0x3F); //EOPQ    
	IO.data(lut_4_grays.data[153]);
	
	IO.cmd(0x03); //VGH      
	IO.data(lut_4_grays.data[154]);

	IO.cmd(0x04); //      
	IO.data(lut_4_grays.data[155]); //VSH1   
	IO.data(lut_4_grays.data[156]); //VSH2   
	IO.data(lut_4_grays.data[157]); //VSL

  // LUT init table for 4 gray. Check if it's needed!
  IO.cmd(lut_4_grays.cmd);     // boost
  for (int i=0; i<lut_4_grays.databytes; ++i) {
      IO.data(lut_4_grays.data[i]);
  }
}

void Depg1020bn::_wakeUp(uint8_t em) {
  IO.reset(10);
  IO.cmd(0x12); // SWRESET
  // Theoretically this display could be driven without RST pin connected
  _waitBusy("SWRESET");

  IO.cmd(0x0C);  // Soft start setting
  IO.data(0xAE);
  IO.data(0xC7);
  IO.data(0xC3);
  IO.data(0xC0);
  IO.data(0xFF);   

  IO.cmd(0x01);  // Set MUX as 527
  IO.data(0x7F);
  IO.data(0x02);
  IO.data(0x00);

  IO.cmd(0x11);  // Data entry mode
  IO.data(0x01);
  
  IO.cmd(0x44);
  IO.data(0x00); // RAM x address start at 0
  IO.data(0x00);
  IO.data(0xBF); // RAM x address end at 36Fh -> 879
  IO.data(0x03);
  IO.cmd(0x45);
  IO.data(0x7F); // RAM y address start at 20Fh;
  IO.data(0x02);
  IO.data(0x00); // RAM y address end at 00h;
  IO.data(0x00);

  IO.cmd(0x3C); // VBD
  IO.data(0x01); // LUT1, for white

  IO.cmd(0x18);
  IO.data(0X80);

  IO.cmd(0x4E); 
  IO.data(0x00);
  IO.data(0x00);
  IO.cmd(0x4F); 
  IO.data(0x7F);
  IO.data(0x02);
}

void Depg1020bn::update()
{
  uint64_t startTime = esp_timer_get_time();
  uint8_t xLineBytes = DEPG1020BN_WIDTH / 8;
  uint8_t x1buf[xLineBytes];
  uint32_t idx = 0;
  if (_mono_mode) {
    _wakeUp(0x01);
    _PowerOn();
    IO.cmd(0x24);        // send framebuffer to RAM1
    
    if (spi_optimized) {
      // v2 SPI optimizing. Check: https://github.com/martinberlin/cale-idf/wiki/About-SPI-optimization
      printf("SPI optimized buffer_len:%d", sizeof(_mono_buffer));

      for (uint16_t y = DEPG1020BN_HEIGHT; y > 0; y--)
      {
        for (uint16_t x = 0; x < xLineBytes; x++)
        {
          idx = y * xLineBytes + x;  
          x1buf[x] = ~_mono_buffer[idx];
        }
        // Flush the X line buffer to SPI
        IO.data(x1buf, sizeof(x1buf));
      }
      // Needed?
      /* IO.cmd(0x26); // RAM2
      for (uint16_t y = DEPG1020BN_HEIGHT; y > 0; y--)
      {
        for (uint16_t x = 0; x < xLineBytes; x++)
        {
          idx = y * xLineBytes + x;  
          x1buf[x] = 0xFF;
        }
        IO.data(x1buf, sizeof(x1buf));
      } */

    } else  {
      // NOT optimized: is minimal the time difference for small buffers like this one
      for (uint16_t y = DEPG1020BN_HEIGHT; y > 0; y--)
      {
        for (uint16_t x = 0; x < DEPG1020BN_WIDTH / 8; x++)
        {
          idx = y * (DEPG1020BN_WIDTH / 8) + x;
          IO.data(~_mono_buffer[idx]);
        }
      }
    }

  } else {
    // 4 gray mode!
    _wakeUp();
    printf("buffer size: %d", sizeof(_buffer1));

    IO.cmd(0x24); // RAM1
    for (uint16_t y = DEPG1020BN_HEIGHT; y > 0; y--)
      {
        for (uint16_t x = 0; x < xLineBytes; x++)
        {
          idx = y * xLineBytes + x;  
          x1buf[x] = (idx < sizeof(_buffer1)) ? ~ _buffer1[idx] : 0xFF;
        }
        // Flush the X line buffer to SPI
        IO.data(x1buf, sizeof(x1buf));
      }
    IO.cmd(0x26); // RAM2
    for (uint16_t y = DEPG1020BN_HEIGHT; y > 0; y--)
      {
        for (uint16_t x = 0; x < xLineBytes; x++)
        {
          idx = y * xLineBytes + x;  
          x1buf[x] = (idx < sizeof(_buffer2)) ? ~ _buffer2[idx] : 0xFF;
        }
        // Flush the X line buffer to SPI
        IO.data(x1buf, sizeof(x1buf));
      }
  }
  uint64_t endTime = esp_timer_get_time();
  IO.cmd(0x22);
  IO.data(0xF7);
  // NOTE: Using F7 as in the GD example the display turns black into gray at the end. With C4 is fine
  IO.cmd(0x20);
  _waitBusy("_Update_Full", 1200);
  uint64_t powerOnTime = esp_timer_get_time();

  printf("\n\nSTATS (ms)\n%llu _wakeUp settings+send Buffer\n%llu _powerOn\n%llu total time in millis\n",
  (endTime-startTime)/1000, (powerOnTime-endTime)/1000, (powerOnTime-startTime)/1000);

  _sleep();
}

void Depg1020bn::_setRamDataEntryMode(uint8_t em)
{
  const uint16_t xPixelsPar = DEPG1020BN_WIDTH - 1;
  const uint16_t yPixelsPar = DEPG1020BN_HEIGHT - 1;
  em = gx_uint16_min(em, 0x03);
  IO.cmd(0x11);
  IO.data(em);
  switch (em)
  {
    case 0x00: // x decrease, y decrease
      _SetRamArea(xPixelsPar / 8, 0x00, yPixelsPar % 256, yPixelsPar / 256, 0x00, 0x00);  // X-source area,Y-gate area
      _SetRamPointer(xPixelsPar / 8, yPixelsPar % 256, yPixelsPar / 256); // set ram
      break;
    case 0x01: // x increase, y decrease : as in demo code
      _SetRamArea(0x00, xPixelsPar / 8, yPixelsPar % 256, yPixelsPar / 256, 0x00, 0x00);  // X-source area,Y-gate area
      _SetRamPointer(0x00, yPixelsPar % 256, yPixelsPar / 256); // set ram
      break;
    case 0x02: // x decrease, y increase
      _SetRamArea(xPixelsPar / 8, 0x00, 0x00, 0x00, yPixelsPar % 256, yPixelsPar / 256);  // X-source area,Y-gate area
      _SetRamPointer(xPixelsPar / 8, 0x00, 0x00); // set ram
      break;
    case 0x03: // x increase, y increase : normal mode
      _SetRamArea(0x00, xPixelsPar / 8, 0x00, 0x00, yPixelsPar % 256, yPixelsPar / 256);  // X-source area,Y-gate area
      _SetRamPointer(0x00, 0x00, 0x00); // set ram
      break;
  }
}

void Depg1020bn::_SetRamArea(uint8_t Xstart, uint8_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1)
{
  IO.cmd(0x44);
  IO.data(Xstart);
  IO.data(Xend);
  IO.cmd(0x45);
  IO.data(Ystart);
  IO.data(Ystart1);
  IO.data(Yend);
  IO.data(Yend1);
}

void Depg1020bn::_SetRamPointer(uint8_t addrX, uint8_t addrY, uint8_t addrY1)
{
  IO.cmd(0x4e);
  IO.data(addrX);
  IO.cmd(0x4f);
  IO.data(addrY);
  IO.data(addrY1);
}

void Depg1020bn::_PowerOn(void)
{
  IO.cmd(0x22);
  IO.data(0xc0);
  IO.cmd(0x20);
  _waitBusy("_PowerOn");
}

void Depg1020bn::updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation)
{
  ESP_LOGE("PARTIAL", "update NOT tested in this epaper x:%d y:%d\n", (int)x, (int)y);

  if (!_using_partial_mode) {
    _using_partial_mode = true;
    _wakeUp(0x03);
    _PowerOn();
    // Fix gray partial update
    IO.cmd(0x26);
    for (int16_t i = 0; i <= DEPG1020BN_BUFFER_SIZE; i++)
    {
      IO.data(0xFF);
    }
  }
  if (using_rotation) _rotate(x, y, w, h);
  if (x >= DEPG1020BN_WIDTH) return;
  if (y >= DEPG1020BN_HEIGHT) return;
  uint16_t xe = gx_uint16_min(DEPG1020BN_WIDTH, x + w) - 1;
  uint16_t ye = gx_uint16_min(DEPG1020BN_HEIGHT, y + h) - 1;
  uint16_t xs_d8 = x / 8;
  uint16_t xe_d8 = xe / 8;

  IO.cmd(0x12); //SWRESET
  _waitBusy("SWRESET");
  _setRamDataEntryMode(0x03);
  _SetRamArea(xs_d8, xe_d8, y % 256, y / 256, ye % 256, ye / 256); // X-source area,Y-gate area
  _SetRamPointer(xs_d8, y % 256, y / 256); // set ram
  //_waitBusy("partialUpdate1", 100); // needed ?

  IO.cmd(0x24);
  for (int16_t y1 = y; y1 <= ye; y1++)
  {
    for (int16_t x1 = xs_d8; x1 <= xe_d8; x1++)
    {
      uint32_t idx = y1 * (DEPG1020BN_WIDTH / 8) + x1;
      uint8_t data = _mono_buffer[idx];
      IO.data(~data);
    }
  }
  

  IO.cmd(0x22);
  IO.data(0xFF); //0x04
  IO.cmd(0x20);
  _waitBusy("updateWindow");
}

void Depg1020bn::_waitBusy(const char* message, uint16_t busy_time){
  if (debug_enabled) {
    ESP_LOGI(TAG, "_waitBusy for %s", message);
  }
  int64_t time_since_boot = esp_timer_get_time();
  // On high is busy
  if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 1) {
  while (1){
    if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 0) break;
    vTaskDelay(1);
    if (esp_timer_get_time()-time_since_boot>7000000)
    {
      if (debug_enabled) ESP_LOGI(TAG, "Busy Timeout");
      break;
    }
  }
  } else {
    vTaskDelay(busy_time/portTICK_RATE_MS); 
  }
}

void Depg1020bn::_waitBusy(const char* message){
  if (debug_enabled) {
    ESP_LOGI(TAG, "_waitBusy for %s", message);
  }
  int64_t time_since_boot = esp_timer_get_time();

  while (1){
    // On low is not busy anymore
    if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 0) break;
    vTaskDelay(1);
    if (esp_timer_get_time()-time_since_boot>7000000)
    {
      if (debug_enabled) ESP_LOGI(TAG, "Busy Timeout");
      break;
    }
  }
}

void Depg1020bn::_sleep(){
  IO.cmd(0x10); // power off display
  IO.data(0x01);
  _waitBusy("power_off");
}

void Depg1020bn::_rotate(uint16_t& x, uint16_t& y, uint16_t& w, uint16_t& h)
{
  switch (getRotation())
  {
    case 1:
      swap(x, y);
      swap(w, h);
      x = DEPG1020BN_WIDTH - x - w - 1;
      break;
    case 2:
      x = DEPG1020BN_WIDTH - x - w - 1;
      y = DEPG1020BN_HEIGHT - y - h - 1;
      break;
    case 3:
      swap(x, y);
      swap(w, h);
      y = DEPG1020BN_HEIGHT - y - h - 1;
      break;
  }
}


void Depg1020bn::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

  // check rotation, move pixel around if necessary
  switch (getRotation())
  {
    case 1:
      swap(x, y);
      x = DEPG1020BN_WIDTH - x - 1;
      break;
    case 2:
      x = DEPG1020BN_WIDTH - x - 1;
      y = DEPG1020BN_HEIGHT - y - 1;
      break;
    case 3:
      swap(x, y);
      y = DEPG1020BN_HEIGHT - y - 1;
      break;
  }
  uint32_t i = x / 8 + y * DEPG1020BN_WIDTH / 8;

 if (_mono_mode) {
    // This is the trick to draw colors right. Genious Jean-Marc
    if (color) {
      _mono_buffer[i] = (_mono_buffer[i] & (0xFF ^ (1 << (7 - x % 8))));
      } else {
      _mono_buffer[i] = (_mono_buffer[i] | (1 << (7 - x % 8)));
      }
 } else {
  // 4 gray mode
  uint8_t mask = 0x80 >> (x & 7);

  color >>= 6; // Color is from 0 (black) to 255 (white)
      
      switch (color)
      {
      case 1:
        // Dark gray: Correct
        _buffer1[i] = _buffer1[i] | mask;
        _buffer2[i] = _buffer2[i] & (0xFF ^ mask);
        break;
      case 2:
        // Light gray: Correct
        _buffer1[i] = _buffer1[i] & (0xFF ^ mask);
        _buffer2[i] = _buffer2[i] | mask;
        break;
      case 3:
        // WHITE
        _buffer1[i] = _buffer1[i] | mask;
        _buffer2[i] = _buffer2[i] | mask;
        break;
      default:
        // Black
        _buffer1[i] = _buffer1[i] & (0xFF ^ mask);
        _buffer2[i] = _buffer2[i] & (0xFF ^ mask);
        break;
      }
 }
}

void Depg1020bn::setMonoMode(bool mode) {
  ESP_LOGE("WARNING", "4 gray mode NOT implemented/NOT working in this epaper");
  _mono_mode = mode;
}