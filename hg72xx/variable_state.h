#ifndef _VARIABLE_STATE_H
#define _VARIABLE_STATE_H


//HG7220.c
extern  union  U_220VFlag   gu_220V_Flag;

//rate.c 
extern  uint8_t    g_Output_Delay;
extern  uint8_t    g_Emu_Reg[5];
extern  struct    S_EmuParameter    gs_Emu_Para;   //计量芯片调校参数
extern struct  S_MeterVariable   gs_Meter_Variable; 

#endif
