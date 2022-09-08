#include <SPI.h>
#include <Wire.h>
#include <LovyanGFX.hpp>
#include "makerfabs_pin.h"

#define S_IDLE 0
#define S_MENU 1
#define MENU_ABS 11
#define MENU_CONC 12
#define MENU_LIB 13
#define MENU_SETTINGS 14

#define S_ABS_BLANK 21
#define S_ABS_SAMPLE 22
#define S_ABS_RES 23
#define S_ABS_STORE 24

#define S_CONC_BLANK 31
#define S_CONC_CURV 32
#define S_CONC_INPUT 33
#define S_CONC_REG 34
#define S_CONC_CURV_STORE 35
#define S_CONC_SAMPLE 36
#define S_CONC_RES 37
#define S_CONC_STORE 38

#define S_LIB 4
#define S_SET 5

#define TAB_WIDTH 80
#define BEGIN_LEN 10
#define BUTTON_LEN 60
#define BUTTON_HIG 360

int DOWN_BT = 0;
int UP_BT = 0;
int BACK_BT = 0;
int OK_BT = 0;

int state = S_IDLE;
int prev_state;
int menu_state = MENU_ABS;

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

int reading;
int input = 0;

int a[4][4];

void setup()
{
  Serial.begin(115200);

  xTaskCreatePinnedToCore(vTaskButtonDOWN, "Input DOWN", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(vTaskButtonUP, "Input UP", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(vTaskButtonBACK, "Input BACK", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(vTaskButtonOK, "Input OK", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(vTaskDisplay, "Input DOWN", 2048, NULL, 2, NULL, 1);
}

void loop()
{
  //update_state(&state);
  Serial.println(state);
  //delay(3000);
}

void vTaskButtonDOWN(void *pvParameters)
{
   while(1){
    if(Serial.available() > 0){
      reading = Serial.read();
    }
    reading = reading - 48;
    if (reading == 1) DOWN_BT = 1;
    if (DOWN_BT == 1){
      update_main_fsm(DOWN_BT, 0, 0, 0, &state, &menu_state);
      DOWN_BT = 0;
    }
    delay(1);
  }
}

void vTaskButtonUP(void *pvParameters)
{
   while(1){
    if(Serial.available() > 0){
      reading = Serial.read();
    }
    reading = reading - 48;
    if (reading == 2) UP_BT = 1;
    if (UP_BT == 1){
      update_main_fsm(UP_BT, 0, 0, 0, &state, &menu_state);
      UP_BT = 0;
    }
    delay(1);
  }
}

void vTaskButtonBACK(void *pvParameters)
{
  while(1){
    if(Serial.available() > 0){
      reading = Serial.read();
    }
    reading = reading - 48;
    if (reading == 3) BACK_BT = 1;
    if (BACK_BT == 1){
      update_main_fsm(DOWN_BT, 0, 0, 0, &state, &menu_state);
      BACK_BT = 0;
    }
    delay(1);
  }
}

void vTaskButtonOK(void *pvParameters)
{
   while(1){
    if(Serial.available() > 0){
      reading = Serial.read();
    }
    reading = reading - 48;
    if (reading == 4) OK_BT = 1;
    if (OK_BT == 1){
      update_main_fsm(DOWN_BT, 0, 0, 0, &state, &menu_state);
      OK_BT = 0;
    }
    delay(1);
  }
}

void vTaskDisplay(void *pvParameters)
{
    //TickType_t xLastWakeTime;
    //const TickType_t xFrequency = 100;
    //LCD_home();
    //LCD_clearScreen();
    set_tft();
    TFT.begin();
    TFT.setFont(&fonts::FreeSansBold9pt7b);
    
    while (1)
    {
        // menunggu sampai waktunya untuk jalan
        //vTaskDelayUntil(&xLastWakeTime, (xFrequency / portTICK_PERIOD_MS));
        // memanggil fungsi untuk update display (updateDisplay)
        update_display(state, menu_state);
        vTaskDelay(1000);
        // kick watchdog timer
        //CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);
    }
}

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

void update_main_fsm(int DOWN_BT, int UP_BT, int BACK_BT, int OK_BT, int *state, int *menu_state)
{
//  if(Serial.available() > 0){
//    reading = Serial.read();
//  }
//  reading = reading - 48;
//  if ((reading >= 1) && (reading <= 4)){
//      input = reading;
//  }
//  Serial.println(input);
  switch(*state){
    case S_IDLE:{
     if((DOWN_BT == 1) || (UP_BT == 1) || (BACK_BT == 1) || (OK_BT == 1)){
      *state = S_MENU;
     }
     break;
    }
    case S_MENU:{
      //if(input == 0) break;
      //menu();
      switch(*menu_state){
        case MENU_ABS:{
          if(DOWN_BT == 1) *menu_state = MENU_CONC;
          if(UP_BT == 1) *menu_state = MENU_SETTINGS;
          if(OK_BT == 1){
            *state = S_ABS_BLANK;
            *menu_state = MENU_ABS;
          }
          break;
        }
        case MENU_CONC:{
          if(DOWN_BT == 1) *menu_state = MENU_LIB;
          if(UP_BT == 1) *menu_state = MENU_ABS;
          if(OK_BT == 1){
            *state = S_CONC_BLANK;
            *menu_state = MENU_ABS;
          }
          break;
        }
        case MENU_LIB:{
          if(DOWN_BT == 1) *menu_state = MENU_SETTINGS;
          if(UP_BT == 1) *menu_state = MENU_CONC;
          if(OK_BT == 1){
            *state = S_LIB;
            *menu_state = MENU_ABS;
          }
          break;
        }
        case MENU_SETTINGS:{
          if(DOWN_BT == 1) *menu_state = MENU_ABS;
          if(UP_BT == 1) *menu_state = MENU_LIB;
          if(OK_BT == 1){
            *state = S_SET;
            *menu_state = MENU_ABS;
          }
          break;
        }
      }
      break;
    }
    case S_ABS_BLANK:{
      input_blank();
/*       if (OK_BT == 1){
        TFT.setTextColor(TFT_RED);
        TFT.setCursor(45, 225);
        TFT.println("Please wait, Measuring Absorbance Reference...");
        //call abs measuring function
        delay(3000);
        *state = S_ABS_SAMPLE; 
      }*/
      break;
    }
    case S_ABS_SAMPLE:{
      abs_input();
/*       if (input == 4){
        TFT.setTextColor(TFT_RED);
        TFT.setCursor(45, 225);
        TFT.println("Please wait, Measuring Sample Absorbance...");
        *state = S_ABS_RES; 
      }*/
      break;
    }
    case S_ABS_RES:{
      abs_graph();
      break;
    }
    case S_ABS_STORE:{
      break;
    }
    case S_CONC_INPUT:{
      conc_input();
      break;
    }
    default:{
      *menu_state = MENU_ABS;
      break;
    }
  }
}

void update_display( int state,
                      int menu_state)
{
    //  if(Serial.available() > 0){
//    reading = Serial.read();
//  }
//  reading = reading - 48;
//  if ((reading >= 1) && (reading <= 4)){
//      input = reading;
//  }
//  Serial.println(input);
  //TFT.setPanel(&panel);
  switch(state){
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
/*       if (OK_BT == 1){
        TFT.setTextColor(TFT_RED);
        TFT.setCursor(45, 225);
        TFT.println("Please wait, Measuring Absorbance Reference...");
        //call abs measuring function
        delay(3000);
        *state = S_ABS_SAMPLE; 
      }*/
      break;
    }
    case S_ABS_SAMPLE:{
      abs_input();
/*       if (input == 4){
        TFT.setTextColor(TFT_RED);
        TFT.setCursor(45, 225);
        TFT.println("Please wait, Measuring Sample Absorbance...");
        *state = S_ABS_RES; 
      }*/
      break;
    }
    case S_ABS_RES:{
      abs_graph();
      break;
    }
    case S_ABS_STORE:{
      break;
    }
    case S_CONC_INPUT:{
      conc_input();
      break;
    }
    default:{
      menu_state = MENU_ABS;
      break;
    }
  }
}

void start()
{
  TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLUE);
  TFT.setTextSize(2.5);
  // half width - num int * int width in pixels
  TFT.setCursor(185, 75);
  TFT.println("SCTP");
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1.5);
  TFT.setCursor(75, 125);
  TFT.println("Visible Spectrophotometer");
  TFT.setTextSize(1);
  TFT.setCursor(140, 225);
  TFT.println("Press any button to start");
}

void menu(int menu_state)
{
  TFT.fillScreen(TFT_WHITE);
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
  TFT.fillScreen(TFT_WHITE);
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
  TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1);
  TFT.setCursor(120, 125);
  TFT.println("Put the sample");
  TFT.fillRoundRect(120, 160, 245, 40, 10, TFT_LIGHTGREY);
  TFT.setCursor(215, 175);
  TFT.println("NEXT");
  TFT.drawRoundRect(120, 160, 245, 40, 10, TFT_BLACK);
}

void abs_graph()
{
  TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLUE);
  TFT.setTextSize(1);
  TFT.setCursor(10, 10);
  TFT.println("ABSORBANCE");
  TFT.drawRect(35, 35, 300, 225, TFT_BLACK);
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1);
  TFT.setCursor(35, 290);
  TFT.println("PEAK: XXX nm, ABS: X.XX");

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
  TFT.fillRoundRect(365, 235, 100, 35, 5, TFT_LIGHTGREY);
  TFT.drawRoundRect(365, 235, 100, 35, 5, TFT_BLACK);
  TFT.setCursor(377, 248);
  TFT.println("MAIN MENU");
}

void conc_input()
{
  TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1.25);
  TFT.setCursor(60, 70);
  TFT.println("Set wavelength for measurement");
  TFT.setCursor(60, 120);
  TFT.println("WAVELENGTH:");
  TFT.setCursor(240, 120);
  TFT.println("XXX nm");

  TFT.setTextColor(TFT_BLUE);
  TFT.setTextSize(1);
  TFT.setCursor(60, 260);
  TFT.println("Press the arrow keys to set concentrations");
  TFT.setCursor(150, 285);
  TFT.println("Press OK to continue");

  TFT.fillTriangle(400, 135, 410, 120, 420, 135, TFT_BLACK);
  TFT.fillTriangle(430, 120, 440, 135, 450, 120, TFT_BLACK);
  delay(1500);

  TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLUE);
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
  
  TFT.setTextColor(TFT_BLUE);
  TFT.setTextSize(1);
  TFT.setCursor(60, 220);
  TFT.println("Press the arrow keys to set concentrations");
  TFT.setCursor(60, 250);
  TFT.println("Press OK to measure STD and display curve");

  TFT.fillTriangle(400, 75, 410, 60, 420, 75, TFT_BLACK);
  TFT.fillTriangle(430, 60, 440, 75, 450, 60, TFT_BLACK);

  delay(1500);
  TFT.setTextColor(TFT_RED);
  TFT.setCursor(90, 280);
  TFT.println("Please wait, Measuring Standards");
}

void conc_curve()
{
  TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLUE);
  TFT.setTextSize(1);
  TFT.setCursor(75, 10);
  TFT.println("CONC VS. ABS");
  TFT.drawRect(75, 35, 330, 220, TFT_BLACK);
  
  TFT.setTextColor(TFT_BLUE);
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
  TFT.fillScreen(TFT_WHITE);
  TFT.setTextColor(TFT_BLACK);
  TFT.setTextSize(1.25);
  TFT.setCursor(95, 75);
  TFT.println("CONCENTRATION:");
  TFT.setTextColor(TFT_RED);
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
  
  TFT.setTextColor(TFT_BLUE);
  TFT.setTextSize(1);
  TFT.setCursor(30, 290);
  TFT.println("BACK");
  TFT.setTextColor(TFT_BLACK);
  TFT.setCursor(90, 290);
  TFT.println("Main Menu");
  
  TFT.fillTriangle(220, 285, 230, 305, 240, 285, TFT_BLACK);
  TFT.setTextColor(TFT_BLACK);
  TFT.setCursor(250, 290);
  TFT.println("Resample");
  TFT.fillTriangle(370, 305, 380, 285, 390, 305, TFT_BLACK);
  TFT.setCursor(400, 290);
  TFT.println("Save");
}

void draw()
{
  int x, y;

  //TFT.fillRect(0, 0, 320, 320, TFT_BLACK);

  for (y = 0; y < 4; y++)
  {
    for (x = 0; x < 4; x++)
    {
      draw_one(y, x);
    }
  }
}

void draw_one(int x, int y)
{
  int i, posx, posy;
  i = a[y][x];
  int color = 0;

  switch (i)
  {
    case 0:
      color = TFT_DARKGREY;
      break;
    case 2:
      color = TFT_WHITE;
      break;
    case 4:
      color = TFT_CYAN;
      break;
    case 8:
      color = TFT_YELLOW;
      break;
    case 16:
      color = TFT_LIGHTGREY;
      break;
    case 32:
      color = TFT_BLUE;
      break;
    case 64:
      color = TFT_BROWN;
      break;
    case 128:
      color = TFT_MAROON;
      break;
    case 256:
      color = TFT_GREEN;
      break;
    case 512:
      color = TFT_RED;
      break;
    case 1024:
      color = TFT_GOLD;
      break;
  }

  posx = TAB_WIDTH * x;
  posy = TAB_WIDTH * y;

  TFT.setTextColor(0xff - color);
  TFT.setTextSize(3);

  TFT.fillRect(BEGIN_LEN + posx - 10, BEGIN_LEN + posy - 10, TAB_WIDTH - 10, TAB_WIDTH - 10, color);
  TFT.setCursor(BEGIN_LEN + posx, BEGIN_LEN + posy + 10);
  if (i != 0)
    TFT.println(i);
}
