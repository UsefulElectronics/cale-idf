#include "gdew027w3.h"
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/task.h"

/*
DO NOT USE: https://twitter.com/martinfasani/status/1265762052880175107
Could not get it to work correctly yet
*/

//Place data into DRAM. Constant data gets placed into DROM by default, which is not accessible by DMA.
//full screen update LUT
DRAM_ATTR const epd_init_44 Gdew027w3::lut_20_vcomDC={
0x20, {
  0x00, 0x00,
  0x00, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x60, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},44};

DRAM_ATTR const epd_init_42 Gdew027w3::lut_21_ww={
0x21, {
  0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
  0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},42};

DRAM_ATTR const epd_init_42 Gdew027w3::lut_22_bw={
0x22,{
  0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
  0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},42};

DRAM_ATTR const epd_init_42 Gdew027w3::lut_23_wb={
0x23,{
  0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},42};

DRAM_ATTR const epd_init_42 Gdew027w3::lut_24_bb={
0x24,{
  0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},42};

DRAM_ATTR const epd_power_4 Gdew027w3::epd_wakeup_power={
0x01,{0x03,0x00,0x2b,0x2b},4
};

DRAM_ATTR const epd_init_3 Gdew027w3::epd_soft_start={
0x06,{0x07,0x07,0x17},3
};

DRAM_ATTR const epd_init_1 Gdew027w3::epd_extra_setting={
0x16,{0x00},1
};

DRAM_ATTR const epd_init_1 Gdew027w3::epd_panel_setting={
0x00,{0xbf},1
};

DRAM_ATTR const epd_init_1 Gdew027w3::epd_pll={
0x30,{0x3a},1
};

DRAM_ATTR const epd_init_4 Gdew027w3::epd_resolution={
0x61,{0x00, 0xb0, /*176*/ 0x01, 0x08 // 264
},4};

DRAM_ATTR const epd_init_1 Gdew027w3::epd_vcom1={
0x82,{0x08},1
};

DRAM_ATTR const epd_init_1 Gdew027w3::epd_vcom2={
0x50,{0x97},1
};

//partial screen update LUT
DRAM_ATTR const epd_init_44 Gdew027w3::lut_20_vcomDC_partial={
0x20, {
  0x00, 0x00,
  0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},44};

DRAM_ATTR const epd_init_42 Gdew027w3::lut_21_ww_partial={
0x21, {
  0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},42};

DRAM_ATTR const epd_init_42 Gdew027w3::lut_22_bw_partial={
0x22, {
  0x80, 0x19, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},42};

DRAM_ATTR const epd_init_42 Gdew027w3::lut_23_wb_partial={
0x23, {
  0x40, 0x19, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},42};

DRAM_ATTR const epd_init_42 Gdew027w3::lut_24_bb_partial={
0x24, {
  0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
},42};

// Partial Update Delay, may have an influence on degradation
#define GxGDEW027W3_PU_DELAY 100


// Constructor
Gdew027w3::Gdew027w3(EpdSpi& dio): 
  Adafruit_GFX(GxGDEW027W3_WIDTH, GxGDEW027W3_HEIGHT),
  Epd(GxGDEW027W3_WIDTH, GxGDEW027W3_HEIGHT), IO(dio)
{
  printf("Gdew027w3() injects IO and extends Adafruit_GFX(%d,%d)\n",
  GxGDEW027W3_WIDTH, GxGDEW027W3_HEIGHT);  

  printf("\n\n\n____________DO NOT USE: https://twitter.com/martinfasani/status/1265762052880175107");
}

void Gdew027w3::initFullUpdate(){
    IO.cmd(0x82);  //vcom_DC setting
    IO.data(0x08);

    IO.cmd(0X50);  //VCOM AND DATA INTERVAL SETTING
    IO.data(0x97); //WBmode:VBDF 17|D7 VBDW 97 VBDB 57

    IO.cmd(lut_20_vcomDC.cmd);
    IO.data(lut_20_vcomDC.data,lut_20_vcomDC.databytes);
   
    IO.cmd(lut_21_ww.cmd);
    IO.data(lut_21_ww.data,lut_21_ww.databytes);

    IO.cmd(lut_22_bw.cmd);
    IO.data(lut_22_bw.data,lut_22_bw.databytes);

    IO.cmd(lut_23_wb.cmd);
    IO.data(lut_23_wb.data,lut_23_wb.databytes);

    IO.cmd(lut_24_bb.cmd);
    IO.data(lut_24_bb.data,lut_24_bb.databytes);
    if (debug_enabled) printf("initFullUpdate() LUT\n");
}

void Gdew027w3::initPartialUpdate(){
    IO.cmd(0x82);  //vcom_DC setting
    IO.data(0x08);

    IO.cmd(0X50);  //VCOM AND DATA INTERVAL SETTING
    IO.data(0x17);

    IO.cmd(lut_20_vcomDC_partial.cmd);
    IO.data(lut_20_vcomDC_partial.data,lut_20_vcomDC_partial.databytes);
   
    IO.cmd(lut_21_ww_partial.cmd);
    IO.data(lut_21_ww_partial.data,lut_21_ww_partial.databytes);

    IO.cmd(lut_22_bw_partial.cmd);
    IO.data(lut_22_bw_partial.data,lut_22_bw_partial.databytes);

    IO.cmd(lut_23_wb_partial.cmd);
    IO.data(lut_23_wb_partial.data,lut_23_wb_partial.databytes);

    IO.cmd(lut_24_bb_partial.cmd);
    IO.data(lut_24_bb_partial.data,lut_24_bb_partial.databytes);
    if (debug_enabled) printf("initPartialUpdate() LUT\n");
}

//Initialize the display
void Gdew027w3::init(bool debug)
{
    debug_enabled = debug;
    if (debug_enabled) printf("Gdew027w3::init(%d) and reset EPD\n", debug);
    //Initialize the Epaper and reset it
    IO.init(4, debug); // 4MHz frequency, debug
    //Reset the display
    IO.reset();
    printf("Free heap:%d\n",xPortGetFreeHeapSize());

    fillScreen(GxEPD_WHITE);
}

void Gdew027w3::fillScreen(uint16_t color)
{
  uint8_t data = (color == GxEPD_WHITE) ? 0xFF : 0x00;
  for (uint16_t x = 0; x < sizeof(_buffer); x++)
  {
    _buffer[x] = data;
  }

  if (debug_enabled) printf("fillScreen(%d) _buffer len:%d\n",data,sizeof(_buffer));
}

void Gdew027w3::_wakeUp(){
  printf("wakeup() start\n");
  IO.reset();

  IO.cmd(epd_wakeup_power.cmd);
  IO.data(epd_wakeup_power.data,epd_wakeup_power.databytes);
  
  IO.cmd(epd_soft_start.cmd);
  IO.data(epd_soft_start.data,epd_soft_start.databytes);
  IO.cmd(epd_extra_setting.cmd);
  IO.data(epd_extra_setting.data,epd_extra_setting.databytes);

  IO.cmd(0x04);
  _waitBusy("epd_wakeup_power");

  IO.cmd(epd_panel_setting.cmd);
  IO.data(epd_panel_setting.data,epd_panel_setting.databytes);

  // 3a 100HZ   29 150Hz 39 200HZ 31 171HZ
  IO.cmd(epd_pll.cmd);
  IO.data(epd_pll.data,epd_pll.databytes);   

  //resolution setting
  IO.cmd(epd_resolution.cmd);
  IO.data(epd_resolution.data,epd_resolution.databytes);
  IO.cmd(epd_vcom1.cmd);
  IO.data(epd_vcom1.data,1);

  IO.cmd(epd_vcom2.cmd);
  IO.data(epd_vcom2.data,1);

  initFullUpdate();
}

void Gdew027w3::update()
{
  _wakeUp();
  
  uint8_t *_wbuffer = new uint8_t[GxGDEW027W3_BUFFER_SIZE];
    for (uint16_t x = 0; x < GxGDEW027W3_BUFFER_SIZE; x++){
    _wbuffer[x] = 0xFF;
  }
  if (_initial) {       // init clean old data
    IO.cmd(0x10);
    IO.data(_wbuffer,GxGDEW027W3_BUFFER_SIZE);
    _initial=false;
  } 

  IO.cmd(0x13);        // update current data
  IO.data(_buffer,sizeof(_buffer));

  IO.cmd(0x12);        // display refresh
  _waitBusy("update"); // update old data

  IO.cmd(0x10);
  IO.data(_buffer,sizeof(_buffer));

  _sleep();
}

uint16_t Gdew027w3::_setPartialRamArea(uint16_t x, uint16_t y, uint16_t xe, uint16_t ye)
{
  x &= 0xFFF8; // byte boundary
  xe = (xe - 1) | 0x0007; // byte boundary - 1
  IO.cmd(0x90); // partial window
  IO.data(x % 256);
  IO.data(xe % 256);
  IO.data(y / 256);
  IO.data(y % 256);
  IO.data(ye / 256);
  IO.data(ye % 256);
  IO.data(0x01);
  IO.data(0x00);
  return (7 + xe - x) / 8; // number of bytes to transfer per line
}

void Gdew027w3::updateWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool using_rotation)
{
  printf("deprecated: updateWindow does not work\n");
  if (using_rotation) _rotate(x, y, w, h);
  if (x >= GxGDEW027W3_WIDTH) return;
  if (y >= GxGDEW027W3_HEIGHT) return;
  uint16_t xe = gx_uint16_min(GxGDEW027W3_WIDTH, x + w) - 1;
  uint16_t ye = gx_uint16_min(GxGDEW027W3_HEIGHT, y + h) - 1;
  // x &= 0xFFF8; // byte boundary, not needed here
  uint16_t xs_bx = x / 8;
  uint16_t xe_bx = (xe + 7) / 8;
  if (!_using_partial_mode) _wakeUp();
  _using_partial_mode = true;
  initPartialUpdate();
  for (uint16_t twice = 0; twice < 2; twice++)
  { // leave both controller buffers equal
    IO.cmd(0x91); // partial in
    _setPartialRamArea(x, y, xe, ye);
    IO.cmd(0x13);

    uint16_t counter = 0;
    uint8_t data[GxGDEW027W3_WIDTH*GxGDEW027W3_HEIGHT];
    for (int16_t y1 = y; y1 <= ye; y1++)
    {
      for (int16_t x1 = xs_bx; x1 < xe_bx; x1++)
      {
        uint16_t idx = y1 * (GxGDEW027W3_WIDTH / 8) + x1;
        data[counter] = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00;
        //uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00; // white is 0x00 in buffer
        //IO.data(~data); // white is 0xFF on device
        ++counter;
      }
    }
    IO.data(data, counter);
    IO.cmd(0x12);      // display refresh
    _waitBusy("updateWindow");
    IO.cmd(0x92);      // partial out
  } // leave both controller buffers equal
  vTaskDelay(GxGDEW027W3_PU_DELAY);
}

void Gdew027w3::updateToWindow(uint16_t xs, uint16_t ys, uint16_t xd, uint16_t yd, uint16_t w, uint16_t h, bool using_rotation)
{
  printf("deprecated: updateToWindow does not work\n");
  if (using_rotation)
  {
    switch (getRotation())
    {
      case 1:
        swap(xs, ys);
        swap(xd, yd);
        swap(w, h);
        xs = GxGDEW027W3_WIDTH - xs - w - 1;
        xd = GxGDEW027W3_WIDTH - xd - w - 1;
        break;
      case 2:
        xs = GxGDEW027W3_WIDTH - xs - w - 1;
        ys = GxGDEW027W3_HEIGHT - ys - h - 1;
        xd = GxGDEW027W3_WIDTH - xd - w - 1;
        yd = GxGDEW027W3_HEIGHT - yd - h - 1;
        break;
      case 3:
        swap(xs, ys);
        swap(xd, yd);
        swap(w, h);
        ys = GxGDEW027W3_HEIGHT - ys  - h - 1;
        yd = GxGDEW027W3_HEIGHT - yd  - h - 1;
        break;
    }
  }
  if (xs >= GxGDEW027W3_WIDTH) return;
  if (ys >= GxGDEW027W3_HEIGHT) return;
  if (xd >= GxGDEW027W3_WIDTH) return;
  if (yd >= GxGDEW027W3_HEIGHT) return;
  // the screen limits are the hard limits
  uint16_t xde = gx_uint16_min(GxGDEW027W3_WIDTH, xd + w) - 1;
  uint16_t yde = gx_uint16_min(GxGDEW027W3_HEIGHT, yd + h) - 1;
  if (!_using_partial_mode) _wakeUp();
  _using_partial_mode = true;
  initPartialUpdate();

  for (uint16_t twice = 0; twice < 2; twice++)
  { // leave both controller buffers equal
    IO.cmd(0x91); // partial in
    // soft limits, must send as many bytes as set by _SetRamArea
    uint16_t yse = ys + yde - yd;
    uint16_t xss_d8 = xs / 8;
    uint16_t xse_d8 = xss_d8 + _setPartialRamArea(xd, yd, xde, yde);
    IO.cmd(0x13);
    uint16_t counter = 0;
    //uint8_t data[GxGDEW027W3_WIDTH*GxGDEW027W3_HEIGHT];
    for (int16_t y1 = ys; y1 <= yse; y1++)
    {
      for (int16_t x1 = xss_d8; x1 < xse_d8; x1++)
      {
        uint16_t idx = y1 * (GxGDEW027W3_WIDTH / 8) + x1;
        uint8_t data = (idx < sizeof(_buffer)) ? _buffer[idx] : 0x00; // white is 0x00 in buffer
        IO.data(data);
        counter++;
      }
    }
    //IO.data(data, counter); // white is 0xFF on device

    IO.cmd(0x12);      //display refresh
    _waitBusy("updateToWindow");
    IO.cmd(0x92); // partial out
  } // leave both controller buffers equal
  vTaskDelay(GxGDEW027W3_PU_DELAY); 
}

void Gdew027w3::_waitBusy(const char* message){
  if (debug_enabled) {
    ESP_LOGI(TAG, "_waitBusy for %s", message);
  }
  int64_t time_since_boot = esp_timer_get_time();

  while (1){
    if (gpio_get_level((gpio_num_t)CONFIG_EINK_BUSY) == 1) break;
    vTaskDelay(1);
    if (esp_timer_get_time()-time_since_boot>2800000)
    {
      if (debug_enabled) ESP_LOGI(TAG, "Busy Timeout");
      break;
    }
  }
}

void Gdew027w3::_sleep(){
  IO.cmd(0x02); // power off display
  _waitBusy("power_off");
  IO.cmd(0x07); // deep sleep
  IO.data(0xa5);
}

void Gdew027w3::_rotate(uint16_t& x, uint16_t& y, uint16_t& w, uint16_t& h)
{
  switch (getRotation())
  {
    case 1:
      swap(x, y);
      swap(w, h);
      x = GxGDEW027W3_WIDTH - x - w - 1;
      break;
    case 2:
      x = GxGDEW027W3_WIDTH - x - w - 1;
      y = GxGDEW027W3_HEIGHT - y - h - 1;
      break;
    case 3:
      swap(x, y);
      swap(w, h);
      y = GxGDEW027W3_HEIGHT - y - h - 1;
      break;
  }
}


void Gdew027w3::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

  // check rotation, move pixel around if necessary
  switch (getRotation())
  {
    case 1:
      swap(x, y);
      x = GxGDEW027W3_WIDTH - x - 1;
      break;
    case 2:
      x = GxGDEW027W3_WIDTH - x - 1;
      y = GxGDEW027W3_HEIGHT - y - 1;
      break;
    case 3:
      swap(x, y);
      y = GxGDEW027W3_HEIGHT - y - 1;
      break;
  }
  uint16_t i = x / 8 + y * GxGDEW027W3_WIDTH / 8;

  if (color)
    _buffer[i] = (_buffer[i] | (1 << (7 - x % 8)));
  else
    _buffer[i] = (_buffer[i] & (0xFF ^ (1 << (7 - x % 8))));
}