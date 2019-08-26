#include <mega8.h>
#include <delay.h>
#include <stdbool.h>

#define         tc_1    PIND.0  // 1st Proximity sensor
#define         tc_2    PIND.1  // 2nd Proximity sensor 
char            P_Add               = 0x53;    
bool            triggered_sensor_1  = false; // Active sensor state 
unsigned int    time_s              = 0;
unsigned int    time_ms             = 0;
unsigned int    timestamp           = 0;
unsigned int    total_time          = 0;
unsigned char   payload             = 0;
#include "rf.c"

interrupt [TIM0_OVF] void timer0_ovf_isr(void) {
    TCNT0=0xB2; //10ms
	if (triggered_sensor_1)
    {
        time_ms += 10;
        if (time_ms % 1000 == 0) //1000 ms = 1s
            { 
                time_s++;
            }
    }
}

void main(void)
{

DDRC=(0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (1<<DDC3) | (1<<DDC2) | (1<<DDC1) | (1<<DDC0);
PORTC=(0<<PORTC6) | (1<<PORTC5) | (1<<PORTC4) | (1<<PORTC3) | (1<<PORTC2) | (1<<PORTC1) | (1<<PORTC0);

DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (1<<DDD3) | (1<<DDD2) | (0<<DDD1) | (0<<DDD0);
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (1<<PORTD1) | (1<<PORTD0);

TCCR0=(1<<CS02) | (0<<CS01) | (1<<CS00);
TCNT0=0xB2;
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (1<<TOIE0);
#asm("sei")
RF_Init();
RF_Config();
RX_Mode_Active();
while (1)
      {
        TX_Mode_Active(); 
        if (tc_1==0) 
            {                                
                if (!triggered_sensor_1) { //only trigger state in the first time 
                    triggered_sensor_1 = true;
                    timestamp = time_s;    //mark current time 
                }
            } 
        if (tc_2==0)
            {
                if (triggered_sensor_1) //both sensors are triggered
                    { 
                        total_time = time_s - timestamp; //time span between 2 sensors are triggered
                        if (total_time < 2)
                            {
                                payload=1;
                            }
                        else
                            {
                                payload=0;
                            }   
                        triggered_sensor_1 = false; //reset sensor state
                    }
                
            } 
        RF_TX_send(P_Add,payload);   // send command to receiver. 
        delay_ms(1); 
      }
}