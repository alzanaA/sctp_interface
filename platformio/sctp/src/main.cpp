#include <string.h>
#include "init_sctp.h"
#include "init_display.h"
#include "LovyanGFX.hpp"

#include <esp_log.h>
#include <driver/spi_master.h>
#include <driver/periph_ctrl.h>
#include <driver/rtc_io.h>
#include <driver/spi_common.h>
#include <esp_heap_caps.h>
#include <soc/rtc.h>
#include <soc/spi_reg.h>
#include <soc/spi_struct.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <esp_task_wdt.h>
#include <driver/gpio.h>

#define DOWN_PIN GPIO_NUM_39
#define UP_PIN GPIO_NUM_40
#define L_PIN GPIO_NUM_35
#define R_PIN GPIO_NUM_41
#define OK_PIN GPIO_NUM_42

char TAG[] = "main";

volatile int DOWN_BT = 0;
volatile int UP_BT = 0;
volatile int L_BT = 0;
volatile int R_BT = 0;
volatile int OK_BT = 0;
// count button debounce
int count_DOWN = 0;
int count_UP = 0;
int count_L = 0;
int count_R = 0;
int count_OK = 0;
// initialize button state
static button_state state_DOWN = waiting;
static button_state state_UP = waiting;
static button_state state_L = waiting;
static button_state state_R = waiting;
static button_state state_OK = waiting;

volatile int state = S_IDLE;
volatile int prev_state;
volatile int menu_state = MENU_ABS;
int prev_menu_state = MENU_ABS;
int abs_res_state = ABS_RELOAD;
int prev_abs_res_state = ABS_RELOAD;
int set_state = SET_EN;
int prev_set_state = SET_EN;
int list_curve = 0;
int processing = NONE;

int reading;
int input = 0;

void vTaskButtonDOWN(void *pvParameters);
void vTaskButtonUP(void *pvParameters);
void vTaskButtonLEFT(void *pvParameters);
void vTaskButtonRIGHT(void *pvParameters);
void vTaskButtonOK(void *pvParameters);
void vUpdateDisplay(void *pvParameters);

void set_tft();

extern "C" void app_main() {
  ESP_LOGI(TAG, "Hello world");

  gpio_set_direction(DOWN_PIN, GPIO_MODE_INPUT);
  gpio_set_direction(UP_PIN, GPIO_MODE_INPUT);
  gpio_set_direction(L_PIN, GPIO_MODE_INPUT);
  gpio_set_direction(R_PIN, GPIO_MODE_INPUT);
  gpio_set_direction(OK_PIN, GPIO_MODE_INPUT);

  xTaskCreatePinnedToCore(vTaskButtonDOWN, "Input DOWN", 2048, NULL, 1, NULL, 1);
//xTaskCreatePinnedToCore(vTaskButtonUP, "Input UP", 2048, NULL, 1, NULL, 1);
//xTaskCreatePinnedToCore(vTaskButtonLEFT, "Input LEFT", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(vTaskButtonRIGHT, "Input RIGHT", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(vTaskButtonOK, "Input OK", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(vUpdateDisplay, "Display", 2048, NULL, 2, NULL, 1);

}

void vTaskButtonDOWN(void *pvParameters)
{
  int input;
  for(;;){
    input = gpio_get_level(DOWN_PIN);
    button_debounce(input, &state_DOWN, &DOWN_BT, &count_DOWN);
    if (DOWN_BT == 1){
      update_main_fsm(DOWN_BT, 0, 0, 0, 0, &processing, &state, &menu_state, &abs_res_state, &set_state);
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}

void vTaskButtonUP(void *pvParameters)
{
  int input;
  for(;;){
    input = gpio_get_level(UP_PIN);
    button_debounce(input, &state_UP, &UP_BT, &count_UP);
    if (UP_BT == 1){
      update_main_fsm(0, UP_BT, 0, 0, 0, &processing, &state, &menu_state, &abs_res_state, &set_state);
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}

void vTaskButtonLEFT(void *pvParameters)
{
  int input;
  for(;;){
    input = gpio_get_level(L_PIN);
    button_debounce(input, &state_L, &L_BT, &count_L);
    if (L_BT == 1){
      update_main_fsm(0, 0, L_BT, 0, 0, &processing, &state, &menu_state, &abs_res_state, &set_state);
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}

void vTaskButtonRIGHT(void *pvParameters)
{
  int input;
  for(;;){
    input = gpio_get_level(R_PIN);
    button_debounce(input, &state_R, &R_BT, &count_R);
    if (R_BT == 1){
      update_main_fsm(0, 0, R_BT, 0, 0, &processing, &state, &menu_state, &abs_res_state, &set_state);
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}

void vTaskButtonOK(void *pvParameters)
{
  int input;
  for(;;){
    input = gpio_get_level(OK_PIN);
    button_debounce(input, &state_OK, &OK_BT, &count_OK);
    if (OK_BT == 1){
      update_main_fsm(0, 0, 0, 0, OK_BT, &processing, &state, &menu_state, &abs_res_state, &set_state);
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}

void vUpdateDisplay(void *pvParameters)
{
    //TickType_t xLastWakeTime;
    //const TickType_t xFrequency = 100; 
    while (1)
    {
        // menunggu sampai waktunya untuk jalan
        //vTaskDelayUntil(&xLastWakeTime, xFrequency);
        // memanggil fungsi untuk update display (updateDisplay)
        update_display(&state, menu_state, abs_res_state, set_state, &prev_state, &prev_menu_state, &prev_abs_res_state, &prev_set_state, &processing);
        vTaskDelay(500/portTICK_PERIOD_MS);
        // kick watchdog timer
        //CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);
    }
}