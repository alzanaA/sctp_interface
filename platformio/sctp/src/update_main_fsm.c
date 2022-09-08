#include "init_sctp.h"

void update_main_fsm(int DOWN_BT, int UP_BT, int L_BT, int R_BT, int OK_BT, int *processing, volatile int *state, volatile int *menu_state, int *abs_res_state, int *set_state)
{
  switch(*state){
    case S_IDLE:{
     if(DOWN_BT == 1){
      *state = S_MENU;
     }
     if(UP_BT == 1){
      *state = S_MENU;
     }
     if(L_BT == 1){
      *state = S_MENU;
     }
     if(R_BT == 1){
      *state = S_MENU;
     }
     if(OK_BT == 1){
      *state = S_MENU;
     }
     break;
    }
    case S_MENU:{
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
      if (*processing == NONE && OK_BT == 1)
        *processing = ONGOING;
//      if (*processing == DONE){
//        *state = S_ABS_SAMPLE; 
//        *processing = NONE;
//      }
      break;
    }
    case S_ABS_SAMPLE:{
      if (*processing == NONE && OK_BT == 1)
        *processing = ONGOING;
//      if (*processing == DONE){
//        *state = S_ABS_RES; 
//        *processing = NONE;
//      }  
      break;
    }
    case S_ABS_RES:{
      switch(*abs_res_state){
        case ABS_RELOAD:{
          if(DOWN_BT == 1) *abs_res_state = ABS_CHANGE_REF;
          if(UP_BT == 1) *abs_res_state = ABS_MENU;
          if(OK_BT == 1){
            //call function measure abs again
            *abs_res_state = ABS_RELOAD;
          }
          break;
        }
        case ABS_CHANGE_REF:{
          if(DOWN_BT == 1) *abs_res_state = ABS_RESAMPLE;
          if(UP_BT == 1) *abs_res_state = ABS_RELOAD;
          if(OK_BT == 1){
            *state = S_ABS_BLANK;
            *abs_res_state = ABS_RELOAD;
          }
          break;
        }
        case ABS_RESAMPLE:{
          if(DOWN_BT == 1) *abs_res_state = ABS_STORE;
          if(UP_BT == 1) *abs_res_state = ABS_CHANGE_REF;
          if(OK_BT == 1){
            *state = S_ABS_SAMPLE;
            *abs_res_state = ABS_RELOAD;
          }
          break;
        }
        case ABS_STORE:{
          if(DOWN_BT == 1) *abs_res_state = ABS_MENU;
          if(UP_BT == 1) *abs_res_state = ABS_RESAMPLE;
          if(OK_BT == 1){
            *state = S_ABS_STORE;
            *abs_res_state = ABS_RELOAD;
          }
          break;
        }
        case ABS_MENU:{
          if(DOWN_BT == 1) *abs_res_state = ABS_RELOAD;
          if(UP_BT == 1) *abs_res_state = ABS_STORE;
          if(OK_BT == 1){
            *state = S_MENU;
            *abs_res_state = ABS_RELOAD;
          }
          break;
        }
      }
      break;
    }
    case S_ABS_STORE:{
      if (*processing == DONE){
        if(DOWN_BT == 1) *state = S_ABS_RES;
        if(OK_BT == 1) *state = S_MENU;
      }
      else
        *processing = ONGOING;
      break;
    }
    case S_CONC_BLANK:{
      if (*processing == NONE && OK_BT == 1)
        *processing = ONGOING;
//      if (*processing == DONE){
//        *state = S_CONC_INPUT; 
//        *processing = NONE;
//      }
      break;
    }
    case S_CONC_CURV:{
      if(R_BT == 1) *state = S_CONC_INPUT_1;
      //if(OK_BT == 1) --call loading curve function
      break;
    }
    case S_CONC_INPUT_1:{
      if(OK_BT == 1) *state = S_CONC_INPUT_2;
      break;
    }

    case S_CONC_INPUT_2:{
      if (*processing == NONE && OK_BT == 1)
        *processing = ONGOING;
      break;
    }

    case S_CONC_REG:{
      if(DOWN_BT == 1) *state = S_CONC_INPUT_1;
      if(UP_BT == 1) *state = S_CONC_CURV_STORE;
      if(OK_BT == 1) *state = S_CONC_SAMPLE;
      break;
    }
    case S_CONC_SAMPLE:{
      if (*processing == NONE && OK_BT == 1)
        *processing = ONGOING;
      break;
    }
    case S_CONC_CURV_STORE:{
      if (*processing == DONE){
        if(DOWN_BT == 1) *state = S_CONC_REG;
        if(OK_BT == 1) *state = S_MENU;
      }
      else
        *processing = ONGOING;
      break;
    }
    case S_CONC_RES:{
      if(DOWN_BT == 1) *state = S_CONC_SAMPLE;
      if(UP_BT == 1) *state = S_CONC_STORE;
      if(OK_BT == 1) *state = S_MENU;
      break;
    }
    case S_CONC_STORE:{
      if (*processing == DONE){
        if(DOWN_BT == 1) *state = S_CONC_RES;
        if(OK_BT == 1) *state = S_MENU;
      }
      else
        *processing = ONGOING;
      break;
    }
    case S_SET:{
      switch(*set_state){
        case SET_EN:{
          if(DOWN_BT == 1) *set_state = SET_ID;
          if(UP_BT == 1) *set_state = SET_MENU;
          if(OK_BT == 1){
            //call language function or change variable
            *menu_state = SET_ID;
          }
          break;
        }
        case SET_ID:{
          if(DOWN_BT == 1) *set_state = SET_MENU;
          if(UP_BT == 1) *set_state = SET_EN;
          if(OK_BT == 1){
            //call language function or change variable
            *menu_state = SET_ID;
          }
          break;
        }
        case SET_MENU:{
          if(DOWN_BT == 1) *set_state = SET_EN;
          if(UP_BT == 1) *set_state = SET_ID;
          if(OK_BT == 1){
            *state = S_MENU;
            *set_state = SET_ID;
          }
          break;
        }
      break;
      }
    }
    default:{
      *menu_state = MENU_ABS;
      break;
    }
  }
}