#ifndef INIT_DISPLAY_H
#define INIT_DISPLAY_H

#ifdef __cplusplus
extern "C"{
#endif

#include "LovyanGFX.hpp"
#include "pin_config.h"

#define TAB_WIDTH 80
#define BEGIN_LEN 10
#define BUTTON_LEN 60
#define BUTTON_HIG 360

void set_tft();
void start();
void menu(volatile int menu_state);
void input_blank();
void abs_input();
void abs_graph(int abs_res_state);
void store();
void conc_curve();
void conc_input_1();
void conc_input_2();
void conc_reg();
void conc_result();
void settings(int set_state);

void update_display(  volatile int *state,
                      int menu_state,
                      int abs_res_state,
                      int set_state,
                      volatile int *prev_state,
                      int *prev_menu_state,
                      int *prev_abs_res_state,
                      int *prev_set_state,
                      int *processing);

#ifdef __cplusplus
}
#endif

#endif