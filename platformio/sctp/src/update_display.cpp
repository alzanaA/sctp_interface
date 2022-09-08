#include <stdio.h>
#include "init_sctp.h"
#include "init_display.h"
#include "LovyanGFX.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

struct LGFX_Config
{
  static constexpr spi_host_device_t spi_host = ESP32_TSC_9488_LCD_SPI_HOST;
  static constexpr int dma_channel = 1;
  static constexpr int spi_sclk = ESP32_TSC_9488_LCD_SCK;
  static constexpr int spi_mosi = ESP32_TSC_9488_LCD_MOSI;
  static constexpr int spi_miso = ESP32_TSC_9488_LCD_MISO;
};

static lgfx::LGFX_SPI<LGFX_Config> TFT;
static LGFX_Sprite sprite(&TFT);
static lgfx::Panel_ILI9488 panel;

int TFT_TOSCA = 0x5D35;
int TFT_MUSTARD = 0xD461;

int a[4][4];

void set_tft()
{
  panel.freq_write = 60000000;

  panel.freq_fill = 60000000;

  panel.freq_read = 16000000;

  panel.spi_mode = 0;

  panel.spi_mode_read = 0;

  panel.len_dummy_read_pixel = 8;


  panel.spi_read = true;

  panel.spi_3wire = false;

  panel.spi_cs = ESP32_TSC_9488_LCD_CS;

  panel.spi_dc = ESP32_TSC_9488_LCD_DC;

  panel.gpio_rst = ESP32_TSC_9488_LCD_RST;

  panel.gpio_bl = ESP32_TSC_9488_LCD_BL;

  panel.pwm_ch_bl = -1;

  panel.backlight_level = true;

  panel.invert = false;

  panel.rgb_order = false;

  panel.memory_width = ESP32_TSC_9488_LCD_WIDTH;
  panel.memory_height = ESP32_TSC_9488_LCD_HEIGHT;

  panel.panel_width = ESP32_TSC_9488_LCD_WIDTH;
  panel.panel_height = ESP32_TSC_9488_LCD_HEIGHT;

  panel.offset_x = 0;
  panel.offset_y = 0;

  panel.rotation = 1;

  panel.offset_rotation = 0;

  TFT.setPanel(&panel);
}

void start()
{
  //TFT.fillScreen(TFT_WHITE);
  TFT.setFont(&fonts::FreeSansBold24pt7b);
  TFT.setTextColor(TFT_TOSCA);
  TFT.setTextSize(1);
  // half width - num int * int width in pixels
  TFT.setCursor(185, 75);
  TFT.println("SCTP");
  TFT.setFont(&fonts::FreeSansBold12pt7b);
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1);
  TFT.setCursor(85, 125);
  TFT.println("Visible Spectrophotometer");
  TFT.setFont(&fonts::FreeSansBold9pt7b);
  TFT.setTextColor(TFT_MUSTARD);
  TFT.setTextSize(1);
  TFT.setCursor(140, 225);
  TFT.println("Press any button to start");
}

void menu(volatile int menu_state)
{
  //TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1);
  switch(menu_state){
    case MENU_ABS:{
      TFT.fillRoundRect(120, 60, 245, 40, 10, TFT_LIGHTGREY);
      break;
    }
    case MENU_CONC:{
      TFT.fillRoundRect(120, 110, 245, 40, 10, TFT_LIGHTGREY);
      break;
    }
    case MENU_LIB:{
      TFT.fillRoundRect(120, 160, 245, 40, 10, TFT_LIGHTGREY);
      break;
    }
    case MENU_SETTINGS:{
      TFT.fillRoundRect(120, 210, 245, 40, 10, TFT_LIGHTGREY);
      break;
    }
    default:{
      TFT.fillRoundRect(120, 60, 245, 40, 10, TFT_LIGHTGREY);
      break;
    }
  }
  TFT.setCursor(165, 75);
  TFT.println("SPECTRUM MODE");
  TFT.drawRoundRect(120, 60, 245, 40, 10, TFT_BLACK);
  TFT.setCursor(145, 125);
  TFT.println("QUANTIZATION MODE");
  TFT.drawRoundRect(120, 110, 245, 40, 10, TFT_BLACK);
  TFT.setCursor(205, 175);
  TFT.println("LIBRARY");
  TFT.drawRoundRect(120, 160, 245, 40, 10, TFT_BLACK);
  TFT.setCursor(198, 225);
  TFT.println("SETTINGS");
  TFT.drawRoundRect(120, 210, 245, 40, 10, TFT_BLACK);
}

void input_blank()
{
  //TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1);
  TFT.setCursor(120, 125);
  TFT.println("Put the blank sample");
  TFT.fillRoundRect(120, 160, 245, 40, 10, TFT_LIGHTGREY);
  TFT.setCursor(215, 175);
  TFT.println("NEXT");
  TFT.drawRoundRect(120, 160, 245, 40, 10, TFT_BLACK);
}

void abs_input()
{
  //TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1);
  TFT.setCursor(120, 125);
  TFT.println("Put the sample");
  TFT.fillRoundRect(120, 160, 245, 40, 10, TFT_LIGHTGREY);
  TFT.setCursor(215, 175);
  TFT.println("NEXT");
  TFT.drawRoundRect(120, 160, 245, 40, 10, TFT_BLACK);
}

void abs_graph(int abs_res_state)
{
  //TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_TOSCA);
  TFT.setTextSize(1);
  TFT.setCursor(10, 10);
  TFT.println("ABSORBANCE");
  TFT.drawRect(33, 35, 305, 225, TFT_BLACK);
  TFT.setCursor(10, 30);
  TFT.println("1");
  TFT.setCursor(10, 250);
  TFT.println("0");
  TFT.setCursor(23, 265);
  TFT.println("400");
  TFT.setCursor(325, 265);
  TFT.println("700");

  int y_max = 1;
  int y_min = 0;
  int x_max = 700;
  int x_min = 400;
  
  int x[] = {400,400,401,402,402,403,404,404,405,406,407,407,408,409,409,410,411,411,412,413,413,414,415,416,416,417,418,418,419,420,420,421,422,422,423,424,425,425,426,427,427,428,429,429,430,431,431,432,433,433,434,435,436,436,437,438,438,439,440,440,441,442,442,443,444,445,445,446,447,447,448,449,449,450,451,451,452,453,454,454,455,456,456,457,458,458,459,460,460,461,462,462,463,464,465,465,466,467,467,468,469,470,470,471,472,473,473,474,475,476,476,477,478,479,479,480,481,482,482,483,484,485,485,486,487,488,488,489,490,491,491,492,493,494,494,495,496,497,497,498,499,500,500,501,502,503,503,504,505,506,506,507,508,509,509,510,511,512,512,513,514,515,515,516,517,518,518,519,520,521,521,522,523,524,524,525,526,527,527,528,529,530,530,531,532,533,533,534,535,535,536,537,537,538,539,539,540,541,542,542,543,544,544,545,546,546,547,548,548,549,550,550,551,552,553,553,554,555,555,556,557,557,558,559,559,560,561,561,562,563,563,564,565,566,566,567,568,568,569,570,570,571,572,572,573,574,574,575,576,577,577,578,579,579,580,581,581,582,583,583,584,585,585,586,587,588,588,589,590,590,591,592,592,593,594,594,595,596,596,597,598,599,599,600,601,601,602,603,603,604,605,605,606,607,607,608,609,609,610,611,612,612,613,614,614,615,616,616,617,618,618,619,620,620,621,622,623,623,624,625,625,626,627,627,628,629,629,630,631,631,632,633,634,634,635,636,636,637,638,638,639,640,640,641,642,642,643,644,645,645,646,647,647,648,649,649,650,651,651,652,653,653,654,655,655,656,657,658,658,659,660,660,661,662,662,663,664,664,665,666,666,667,668,669,669,670,671,671,672,673,673,674,675,675,676,677,677,678,679,680,680,681,682,682,683,684,684,685,686,686,687,688,688,689,690,691,691,692,693,693,694,695,695,696,697,697,698,699,699,700};
  float y[] = {0.1,0.08,0.04,0.04,0.02,0.01,0.07,0.07,0.06,0.04,0.03,0.04,0.06,0.06,0.04,0.06,0.05,0.09,0.09,0.08,0.09,0.09,0.13,0.08,0.12,0.15,0.14,0.16,0.18,0.17,0.16,0.17,0.17,0.17,0.21,0.23,0.2,0.2,0.21,0.22,0.25,0.23,0.27,0.25,0.24,0.27,0.28,0.28,0.29,0.28,0.31,0.3,0.3,0.29,0.28,0.3,0.29,0.29,0.29,0.3,0.31,0.3,0.32,0.35,0.35,0.36,0.33,0.33,0.35,0.31,0.31,0.31,0.3,0.33,0.33,0.35,0.34,0.34,0.35,0.33,0.32,0.3,0.29,0.28,0.28,0.27,0.3,0.29,0.3,0.29,0.29,0.29,0.28,0.26,0.25,0.25,0.25,0.25,0.27,0.27,0.3,0.31,0.32,0.31,0.3,0.28,0.26,0.24,0.24,0.25,0.28,0.32,0.34,0.32,0.29,0.28,0.27,0.27,0.28,0.31,0.34,0.36,0.35,0.33,0.31,0.28,0.28,0.26,0.26,0.27,0.28,0.29,0.31,0.3,0.28,0.27,0.27,0.27,0.26,0.25,0.24,0.25,0.26,0.28,0.28,0.28,0.25,0.23,0.21,0.2,0.24,0.27,0.32,0.31,0.29,0.25,0.23,0.22,0.25,0.28,0.31,0.35,0.35,0.31,0.28,0.23,0.25,0.26,0.31,0.36,0.34,0.33,0.31,0.29,0.28,0.28,0.28,0.31,0.34,0.34,0.32,0.31,0.29,0.28,0.26,0.28,0.3,0.34,0.36,0.34,0.32,0.29,0.29,0.31,0.33,0.35,0.36,0.37,0.36,0.36,0.35,0.34,0.34,0.35,0.37,0.37,0.39,0.4,0.38,0.36,0.33,0.32,0.33,0.39,0.43,0.4,0.39,0.35,0.35,0.36,0.37,0.39,0.4,0.41,0.41,0.39,0.36,0.35,0.37,0.4,0.43,0.43,0.42,0.4,0.38,0.38,0.39,0.4,0.42,0.44,0.43,0.42,0.39,0.39,0.41,0.44,0.49,0.48,0.48,0.46,0.46,0.45,0.45,0.47,0.5,0.53,0.53,0.52,0.5,0.47,0.5,0.54,0.57,0.58,0.58,0.54,0.54,0.55,0.57,0.59,0.62,0.63,0.64,0.63,0.63,0.63,0.65,0.67,0.7,0.72,0.73,0.74,0.73,0.7,0.69,0.71,0.73,0.78,0.8,0.77,0.74,0.72,0.75,0.77,0.8,0.81,0.79,0.78,0.76,0.76,0.76,0.8,0.81,0.84,0.83,0.8,0.78,0.77,0.8,0.84,0.86,0.86,0.83,0.81,0.8,0.82,0.83,0.85,0.87,0.86,0.85,0.82,0.78,0.8,0.83,0.86,0.88,0.86,0.81,0.8,0.81,0.84,0.88,0.93,0.93,0.93,0.91,0.88,0.87,0.89,0.92,0.97,0.96,0.93,0.86,0.83,0.86,0.93,0.93,0.94,0.9,0.83,0.79,0.79,0.8,0.8,0.8,0.78,0.75,0.71,0.69,0.67,0.69,0.69,0.67,0.68,0.67,0.64,0.63,0.62,0.58,0.57,0.53,0.51,0.52,0.5,0.44,0.44,0.41,0.42,0.4,0.4,0.4,0.37,0.36,0.34,0.34,0.3,0.28,0.26,0.28,0.27,0.24,0.24,0.24,0.24,0.23,0.22,0.22,0.21,0.18,0.18,0.17,0.17,0.18,0.16,0.17,0.16,0.14,0.12,0.14,0.16,0.16,0.15,0.14,0.13,0.13,0.12,0.12,0.12,0.12,0.12,0.11,0.11,0.11,0.12,0.11,0.12,0.13,0.13};

  int i = 0;
  int x_px;
  int y_px;
  float peak_abs = y[0];
  int peak_wl= x[0];
  
  while(i < 430){
    if(x[i] == x[i+1]){
      x_px = x[i] - x_min + 35;
      y_px = 260 - ((y[i] + y[i+1]) / 2) * 225;
      i = i+2;
    }
    else{
      x_px = x[i] - x_min + 35;
      y_px = 260 - y[i] * 225;
      i = i+1;
    }
    
    if(i==1){
      if(y[i] > peak_abs){
        peak_abs = y[i];
        peak_wl = x[i];
      }
      TFT.drawPixel(x_px, y_px-1, TFT_MUSTARD);
      TFT.drawPixel(x_px, y_px, TFT_MUSTARD);
      TFT.drawPixel(x_px, y_px+1, TFT_MUSTARD);
      TFT.drawPixel(x_px+1, y_px-1, TFT_MUSTARD);
      TFT.drawPixel(x_px+1, y_px, TFT_MUSTARD);
      TFT.drawPixel(x_px+1, y_px+1, TFT_MUSTARD);
    }
    else if(i==sizeof(x)){
      TFT.drawPixel(x_px, y_px-1, TFT_MUSTARD);
      TFT.drawPixel(x_px, y_px, TFT_MUSTARD);
      TFT.drawPixel(x_px, y_px+1, TFT_MUSTARD);
      TFT.drawPixel(x_px-1, y_px-1, TFT_MUSTARD);
      TFT.drawPixel(x_px-1, y_px, TFT_MUSTARD);
      TFT.drawPixel(x_px-1, y_px+1, TFT_MUSTARD);
    }
    else{
      if(y[i] > peak_abs){
        peak_abs = y[i];
        peak_wl = x[i];
      }
      TFT.drawPixel(x_px, y_px-1, TFT_MUSTARD);
      TFT.drawPixel(x_px, y_px, TFT_MUSTARD);
      TFT.drawPixel(x_px, y_px+1, TFT_MUSTARD);
      TFT.drawPixel(x_px-1, y_px-1, TFT_MUSTARD);
      TFT.drawPixel(x_px-1, y_px, TFT_MUSTARD);
      TFT.drawPixel(x_px-1, y_px+1, TFT_MUSTARD);
      TFT.drawPixel(x_px+1, y_px-1, TFT_MUSTARD);
      TFT.drawPixel(x_px+1, y_px, TFT_MUSTARD);
      TFT.drawPixel(x_px+1, y_px+1, TFT_MUSTARD);
    }
  }
  
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1);
  TFT.setCursor(35, 290);
  TFT.println("PEAK: ");
  TFT.setCursor(105, 290);
  TFT.println(peak_wl);
  TFT.setCursor(140, 290);
  TFT.println("nm,    ABS:");
  TFT.setCursor(245, 290);
  TFT.println(peak_abs);
  
//  if(y_px[1] >= y_px[0])
//    TFT.drawFastVLine(x_px[0], y_px[0] , y_px[1] - y_px[0], TFT_RED);
//  else
//    TFT.drawFastVLine(x_px[0], y_px[1] , y_px[0] - y_px[1], TFT_RED);
//  for(j = 1; j < 300; j++){
//    if(y_px[j+1] >= y_px[j-1])
//      TFT.drawFastVLine(x_px[j], y_px[j-1], y_px[j+1] - y_px[j-1], TFT_RED);
//    else
//      TFT.drawFastVLine(x_px[j], y_px[j+1], y_px[j-1] - y_px[j+1], TFT_RED);
//  }
//  if (y_px[300] >= y_px[299])
//    TFT.drawFastVLine(x_px[300], y_px[299] , y_px[300] - y_px[299], TFT_RED);
//  else
//    TFT.drawFastVLine(x_px[300], y_px[300] , y_px[299] - y_px[300], TFT_RED);

  switch(abs_res_state){
    case ABS_RELOAD:{
      TFT.fillRoundRect(365, 35, 100, 35, 5, TFT_LIGHTGREY);
      break;
    }
    case ABS_CHANGE_REF:{
      TFT.fillRoundRect(365, 85, 100, 35, 5, TFT_LIGHTGREY);
      break;
    }
    case ABS_RESAMPLE:{
      TFT.fillRoundRect(365, 135, 100, 35, 5, TFT_LIGHTGREY);
      break;
    }
    case ABS_STORE:{
      TFT.fillRoundRect(365, 185, 100, 35, 5, TFT_LIGHTGREY);
      break;
    }
    case ABS_MENU:{
      TFT.fillRoundRect(365, 235, 100, 35, 5, TFT_LIGHTGREY);
      break;
    }
    default:{
      TFT.fillRoundRect(365, 35, 100, 35, 5, TFT_LIGHTGREY);
      break;
    }
  }
  TFT.setTextSize(0.75);
  TFT.drawRoundRect(365, 35, 100, 35, 5, TFT_BLACK);
  TFT.setCursor(390, 48);
  TFT.println("RELOAD");
  TFT.drawRoundRect(365, 85, 100, 35, 5, TFT_BLACK);
  TFT.setCursor(372, 98);
  TFT.println("CHANGE REF");
  TFT.drawRoundRect(365, 135, 100, 35, 5, TFT_BLACK);
  TFT.setCursor(380, 148);
  TFT.println("RESAMPLE");
  TFT.drawRoundRect(365, 185, 100, 35, 5, TFT_BLACK);
  TFT.setCursor(400, 198);
  TFT.println("SAVE");
  TFT.drawRoundRect(365, 235, 100, 35, 5, TFT_BLACK);
  TFT.setCursor(377, 248);
  TFT.println("MAIN MENU");
}

void store()
{
  TFT.setTextColor(TFT_MUSTARD);
  TFT.setTextSize(1);
  TFT.setCursor(215, 125);
  TFT.println("Saving...");

  TFT.setTextColor(TFT_TOSCA);
  TFT.setTextSize(1);
  TFT.setCursor(120, 290);
  TFT.println("OK");
  TFT.setTextColor(TFT_BLACK);
  TFT.setCursor(150, 290);
  TFT.println("Main Menu");
  
  TFT.fillTriangle(295, 285, 305, 305, 315, 285, TFT_BLACK);
  TFT.setTextColor(TFT_BLACK);
  TFT.setCursor(325, 290);
  TFT.println("Back");
}

void conc_curve(){
  //TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_TOSCA);
  TFT.setTextSize(1.25);
  TFT.setCursor(60, 20);
  TFT.println("Filename");

  TFT.setTextSize(1);
  TFT.setTextColor(TFT_MUSTARD);
  TFT.setCursor(60, 60);
  TFT.println("Curve1.csv");
  TFT.setTextColor(TFT_BLACK);
  TFT.setCursor(60, 90);
  TFT.println("Curve2.csv");
  TFT.setCursor(60, 120);
  TFT.println("Curve3.csv");
  TFT.setCursor(60, 150);
  TFT.println("Curve4.csv");
  TFT.setCursor(60, 180);
  TFT.println("Curve5.csv");
  
  TFT.setTextColor(TFT_TOSCA);
  TFT.setTextSize(1);
  TFT.setCursor(30, 290);
  TFT.println("OK");
  TFT.setTextColor(TFT_BLACK);
  TFT.setCursor(60, 290);
  TFT.println("Load a curve");
  
  TFT.fillTriangle(275, 295, 255, 305, 255, 285, TFT_BLACK);
  TFT.setTextColor(TFT_BLACK);
  TFT.setCursor(285, 290);
  TFT.println("Make a new curve");
}

void conc_input_1(){
  //TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1.25);
  TFT.setCursor(60, 70);
  TFT.println("Set wavelength for measurement");
  TFT.setCursor(60, 120);
  TFT.println("WAVELENGTH:");
  TFT.setCursor(240, 120);
  TFT.println("XXX nm");

  TFT.setTextColor(TFT_TOSCA);
  TFT.setTextSize(1);
  TFT.setCursor(70, 260);
  TFT.println("Press the arrow keys to set wavelength");
  TFT.setCursor(150, 285);
  TFT.println("Press OK to continue");

  TFT.fillTriangle(400, 135, 410, 120, 420, 135, TFT_BLACK);
  TFT.fillTriangle(430, 120, 440, 135, 450, 120, TFT_BLACK);
}

void conc_input_2()
{
  //TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_TOSCA);
  TFT.setTextSize(1.25);
  TFT.setCursor(180, 20);
  TFT.println("CONC");
  TFT.setCursor(310, 20);
  TFT.println("ABS");
  
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1.25);
  TFT.setCursor(60, 60);
  TFT.println("STD 1");
  TFT.setCursor(180, 60);
  TFT.println("X.XX");
  TFT.setCursor(300, 60);
  TFT.println("X.XX");

  TFT.setCursor(60, 100);
  TFT.println("STD 2");
  TFT.setCursor(180, 100);
  TFT.println("X.XX");
  TFT.setCursor(300, 100);
  TFT.println("X.XX");

  TFT.setCursor(60, 140);
  TFT.println("STD 3");
  TFT.setCursor(180, 140);
  TFT.println("X.XX");
  TFT.setCursor(300, 140);
  TFT.println("X.XX");

  TFT.setCursor(60, 180);
  TFT.println("STD 4");
  TFT.setCursor(180, 180);
  TFT.println("X.XX");
  TFT.setCursor(300, 180);
  TFT.println("X.XX");
  
  TFT.setTextColor(TFT_TOSCA);
  TFT.setTextSize(1);
  TFT.setCursor(60, 220);
  TFT.println("Press the arrow keys to set concentrations");
  TFT.setCursor(60, 250);
  TFT.println("Press OK to measure STD and display curve");

  TFT.fillTriangle(400, 75, 410, 60, 420, 75, TFT_BLACK);
  TFT.fillTriangle(430, 60, 440, 75, 450, 60, TFT_BLACK);
}

void conc_reg()
{
  //TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_TOSCA);
  TFT.setTextSize(1);
  TFT.setCursor(75, 10);
  TFT.println("CONC VS. ABS");
  TFT.drawRect(75, 35, 330, 220, TFT_BLACK);

  int x_max = 1.1;
  int x_min = 0;
  int y_max = 0.8;
  int y_min = 0;

  float conc[] = {0.16, 0.4, 0.86, 1.04}; 
  float ab[] = {0.08, 0.2, 0.42, 0.52};
  
  int i = 0;
  int x_px;
  int y_px;

  while(i < 4){
    x_px = conc[i] - x_min + 75;
    y_px = 250 - ab[i] * 220;
    i = i+1;

    TFT.drawPixel(x_px-1, y_px-1, TFT_MUSTARD);
    TFT.drawPixel(x_px-1, y_px, TFT_MUSTARD);
    TFT.drawPixel(x_px-1, y_px+1, TFT_MUSTARD);
    TFT.drawPixel(x_px, y_px-1, TFT_MUSTARD);
    TFT.drawPixel(x_px, y_px, TFT_MUSTARD);
    TFT.drawPixel(x_px, y_px+1, TFT_MUSTARD);
    TFT.drawPixel(x_px+1, y_px-1, TFT_MUSTARD);
    TFT.drawPixel(x_px+1, y_px, TFT_MUSTARD);
    TFT.drawPixel(x_px+1, y_px+1, TFT_MUSTARD);
  }
  
  TFT.setTextColor(TFT_TOSCA);
  TFT.setTextSize(1);
  TFT.setCursor(30, 290);
  TFT.println("OK");
  TFT.setTextColor(TFT_BLACK);
  TFT.setCursor(60, 290);
  TFT.println("Measure Sample");
  
  TFT.fillTriangle(255, 285, 265, 305, 275, 285, TFT_BLACK);
  TFT.setTextColor(TFT_BLACK);
  TFT.setCursor(285, 290);
  TFT.println("Edit");
  TFT.fillTriangle(370, 305, 380, 285, 390, 305, TFT_BLACK);
  TFT.setCursor(400, 290);
  TFT.println("Save");
}

void conc_result()
{
  //TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1.25);
  TFT.setCursor(95, 75);
  TFT.println("CONCENTRATION:");
  TFT.setTextColor(TFT_MUSTARD);
  TFT.setCursor(325, 75);
  TFT.println("X.XX");
  
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1);
  TFT.setCursor(120, 125);
  TFT.println("ABSORBANCE:");
  TFT.setCursor(300, 125);
  TFT.println("X.XX");
  TFT.setCursor(120, 175);
  TFT.println("WAVELENGTH:");
  TFT.setCursor(300, 175);
  TFT.println("XXX nm");
  
  TFT.setTextColor(TFT_TOSCA);
  TFT.setTextSize(1);
  TFT.setCursor(30, 290);
  TFT.println("OK");
  TFT.setTextColor(TFT_BLACK);
  TFT.setCursor(60, 290);
  TFT.println("Main Menu");
  
  TFT.fillTriangle(205, 285, 215, 305, 225, 285, TFT_BLACK);
  TFT.setTextColor(TFT_BLACK);
  TFT.setCursor(235, 290);
  TFT.println("Resample");
  TFT.fillTriangle(370, 305, 380, 285, 390, 305, TFT_BLACK);
  TFT.setCursor(400, 290);
  TFT.println("Save");
}

void settings(int set_state)
{
  //TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1);
  switch(set_state){
    case SET_EN:{
      TFT.fillRoundRect(120, 110, 245, 40, 10, TFT_LIGHTGREY);
      break;
    }
    case SET_ID:{
      TFT.fillRoundRect(120, 160, 245, 40, 10, TFT_LIGHTGREY);
      break;
    }
    case SET_MENU:{
      TFT.fillRoundRect(120, 210, 245, 40, 10, TFT_LIGHTGREY);
      break;
    }
    default:{
      TFT.fillRoundRect(120, 110, 245, 40, 10, TFT_LIGHTGREY);
      break;
    }
  }
  TFT.setCursor(165, 75);
  TFT.println("Change language:");
  TFT.setCursor(200, 125);
  TFT.println("ENGLISH");
  TFT.drawRoundRect(120, 110, 245, 40, 10, TFT_BLACK);
  TFT.setCursor(205, 175);
  TFT.println("BAHASA");
  TFT.drawRoundRect(120, 160, 245, 40, 10, TFT_BLACK);
  TFT.setCursor(190, 225);
  TFT.println("MAIN MENU");
  TFT.drawRoundRect(120, 210, 245, 40, 10, TFT_BLACK);
}

void update_display(  volatile int *state,
                      int menu_state,
                      int abs_res_state,
                      int set_state,
                      volatile int *prev_state,
                      int *prev_menu_state,
                      int *prev_abs_res_state,
                      int *prev_set_state,
                      int *processing)
{
  if((state != prev_state) || (menu_state != prev_menu_state) || (abs_res_state != prev_abs_res_state) || (set_state != prev_set_state))
    TFT.fillScreen(TFT_WHITE);
  *prev_state = *state;
  *prev_menu_state = menu_state;
  *prev_abs_res_state = abs_res_state;
  *prev_set_state = set_state;
  switch(*state){
    case S_IDLE:{
     start();
     break;
    }
    case S_MENU:{
      menu(menu_state);
      break;
    }
    case S_ABS_BLANK:{
      input_blank();
      if (*processing == ONGOING){
        TFT.setTextColor(TFT_MUSTARD);
        TFT.setCursor(45, 225);
        TFT.println("Please wait, Measuring Absorbance Reference...");
        //call abs measuring function
        vTaskDelay(250/portTICK_PERIOD_MS);
        *processing = DONE;
        if (*processing == DONE){
          *state = S_ABS_SAMPLE; 
          *processing = NONE;
        }
      }
      break;
    }
    case S_ABS_SAMPLE:{
      abs_input();
      if (*processing == ONGOING){
        TFT.setTextColor(TFT_MUSTARD);
        TFT.setCursor(45, 225);
        TFT.println("Please wait, Measuring Sample Absorbance...");
        vTaskDelay(250/portTICK_PERIOD_MS);
        *processing = DONE;
        if (*processing == DONE){
          *state = S_ABS_RES; 
          *processing = NONE;
        }
      }
      break;
    }
    case S_ABS_RES:{
      abs_graph(abs_res_state);
      break;
    }
    case S_ABS_STORE:{
      store();
      //call store function
      vTaskDelay(250/portTICK_PERIOD_MS);
      *processing = DONE;
      if (*processing == DONE){
        TFT.setCursor(165, 175);
        TFT.println("SAVED SUCCESSFULLY");
      }
      break;
    }
    case S_CONC_BLANK:{
      input_blank();
      if (*processing == ONGOING){
        TFT.setTextColor(TFT_MUSTARD);
        TFT.setCursor(45, 225);
        TFT.println("Please wait, Measuring Absorbance Reference...");
        //call abs measuring function
        delay(250);
        *processing = DONE;
        if (*processing == DONE){
          *state = S_CONC_CURV; 
          *processing = NONE;
        } 
      }
      break;
    }
    case S_CONC_CURV:{
      conc_curve();
      break;
    }
    case S_CONC_INPUT_1:{
      conc_input_1();
      break;
    }
    case S_CONC_INPUT_2:{
      conc_input_2();
      if (*processing == ONGOING){
        TFT.setTextColor(TFT_MUSTARD);
        TFT.setCursor(90, 280);
        TFT.println("Please wait, Measuring Standards");
        //call regression function
        vTaskDelay(250/portTICK_PERIOD_MS);
        *processing = DONE;
        if (*processing == DONE){
          *state = S_CONC_REG; 
          *processing = NONE;
        } 
      }
      break;
    }
    case S_CONC_REG:{
      conc_reg();
      break;
    }
    case S_CONC_SAMPLE:{
      abs_input();
      if (*processing == ONGOING){
        TFT.setTextColor(TFT_MUSTARD);
        TFT.setCursor(45, 225);
        TFT.println("Please wait, Measuring Sample Concentration...");
        vTaskDelay(250/portTICK_PERIOD_MS);
        *processing = DONE;
        if (*processing == DONE){
          *state = S_CONC_RES; 
          *processing = NONE;
        }
      }
      break;
    }
    case S_CONC_CURV_STORE:{
      store();
      //call store function
      vTaskDelay(250/portTICK_PERIOD_MS);
      *processing = DONE;
      if (*processing == DONE){
        TFT.setCursor(165, 175);
        TFT.println("SAVED SUCCESSFULLY");
      }
      break;
    }
    case S_CONC_RES:{
      conc_result();
      break;
    }
    case S_CONC_STORE:{
      store();
      //call store function
      vTaskDelay(250/portTICK_PERIOD_MS);
      *processing = DONE;
      if (*processing == DONE){
        TFT.setCursor(165, 175);
        TFT.println("SAVED SUCCESSFULLY");
      }
      break;
    }
    case S_SET:{
      settings(set_state);
      break;
    }
    default:{
      menu_state = MENU_ABS;
      break;
    }
  }
}