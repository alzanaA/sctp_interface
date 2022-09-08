#ifndef INIT_SCTP_H
#define INIT_SCTP_H

#ifdef __cplusplus
extern "C"{
#endif

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

#define ABS_RELOAD 231
#define ABS_CHANGE_REF 232
#define ABS_RESAMPLE 233
#define ABS_STORE 234
#define ABS_MENU 235

#define S_CONC_BLANK 31
#define S_CONC_CURV 32
#define S_CONC_INPUT_1 331
#define S_CONC_INPUT_2 332
#define S_CONC_REG 34
#define S_CONC_CURV_STORE 35
#define S_CONC_SAMPLE 36
#define S_CONC_RES 37
#define S_CONC_STORE 38

#define S_LIB 4
#define S_SET 5
#define SET_EN 51
#define SET_ID 52
#define SET_MENU 53

#define NONE 100
#define ONGOING 101
#define DONE 111

#define BUTTON_THRESHOLD 20

typedef enum {waiting, detected, unreleased, update} button_state;

void update_main_fsm(   int DOWN_BT,
                        int UP_BT,
                        int L_BT,
                        int R_BT,
                        int OK_BT,
                        int *processing,
                        volatile int *state,
                        volatile int *menu_state,
                        int *abs_res_state,
                        int *set_state);

void button_debounce(   int input,
                        button_state *state,
                        volatile int *output,
                        int *count);

#ifdef __cplusplus
}
#endif

#endif