unsigned char Code_tay_cam = 0x53;
#include <mega8.h>
#include <delay.h>
#include "rf.c"

#define     blue_led    PORTD.0
#define     green_led   PORTD.1
#define     red_led     PORTD.2
#include    <stdbool.h>

#define     ALERT       1
#define     NORMAL      0

#define     ON          1
#define     OFF         0

#define     ALERT_DURATION 20

unsigned char status_code=0;
unsigned int time_ms = 0;
unsigned int alert_left = 0;
// bool is_alert = false;


void blink_alert();
void blink_normal();

interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    TCNT0=0x83; //  1ms  
    time_ms += 1;

    // each 0,5s
    if (time_ms == 500) {
        // blink green if not alerted
        if (alert_left > 0) 
        {
            // blink alert for ALERT_DURATION times
            alert_left--;
            blink_alert();
        } 
        else 
        { //else blink red & blue in sequence
            blink_normal();
        }
        time_ms = 0;
    }
}

void main(void)
{

DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

DDRC=(0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (1<<DDC3) | (1<<DDC2) | (1<<DDC1) | (1<<DDC0);
PORTC=(0<<PORTC6) | (1<<PORTC5) | (10<<PORTC4) | (1<<PORTC3) | (1<<PORTC2) | (1<<PORTC1) | (1<<PORTC0);

DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (1<<DDD2) | (1<<DDD1) | (1<<DDD0);
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

TCCR0=(0<<CS02) | (1<<CS01) | (1<<CS00);
TCNT0=0x83;

TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (1<<TOIE0);

#asm("sei")
RF_Config();
RF_Init();

while (1)
      {
        RX_Mode();
            if(IRQ==0) 
            {
                status_code = RF_RX_Read();
                if (status_code == ALERT) {
                    alert_left  = ALERT_DURATION;
                    red_led     = OFF;
                    blue_led    = ON;
                    green_led   = OFF;
                }

                // if (status_code == NORMAL) {
                //     green_led   = ON;
                //     red_led     = OFF;
                //     blue_led    = OFF;
                // }
            }
      }   
}

void blink_alert()
{
    green_led =   OFF;
    red_led   =   !red_led;  //toggle state
    blue_led  =   !blue_led;
    delay_ms(250);
}
void blink_normal()
{
    green_led   =   !green_led; 
    red_led     =   OFF;
    blue_led    =   OFF;
    delay_ms(250);
}