#include "init_sctp.h"

void button_debounce(int input, button_state *state, volatile int *output, int *count)
{ 
  switch(*state){
    case waiting:{
      if(input==1){
        *state = detected;
        *output = 1;
        ++(*count);
      }
      else if(input==0){
        *state = *state;
        *output = 0;
        *count = 0;
      }
      break;
    }
    case detected:{
      if(*count >= BUTTON_THRESHOLD){
        if(input==1){
          *state = detected;
          *output = 1;
          ++(*count);
        }
        else if(input==0){
          *state = waiting;
          *output = 0;
          *count = 0;
        }
      }
      else{
        *state = *state;
        *output = 0;
        ++(*count);
      }
      break;
    }
    case unreleased:{
      if(input==1){
        *state = *state;
        *output = 0;
        *count = 0;
      }
      else if(input==0){
        *state = update;
        *output = 0;
        ++(*count);
      }
      break;
    }
    case update:{
      if(*count >= BUTTON_THRESHOLD){
        if(input==1){
          *state = detected;
          *output = 0;
          *count = 0;
        }
        else if(input==0){
          *state = waiting;
          *output = 0;
          *count = 0;
        }
      }
      else{
        *state = *state;
        *output = 0;
        ++(*count);
      }
      break;
    }
  }
}