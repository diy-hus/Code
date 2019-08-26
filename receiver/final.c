unsigned char Code_tay_cam = 0x53; //
#include <mega8.h>
#include "init.c"
#include "rf.c"

void main(void){
Init_System();
RF_Config();
RF_Init();
glcd_clear();
while (1)
    {
      RX_Mode();
            if(IRQ==0)
            {
                LED=!LED;
                glcd_moveto(20,0);
                xx=RF_RX_Read();
                sprintf(glcd_buff,"%u",xx);
                glcd_outtext(glcd_buff);
            }
    }
}