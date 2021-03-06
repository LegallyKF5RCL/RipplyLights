/* 
 * File:   Main_RipplyLights.c
 * Author: Brian McRee
 *
 * Created on March 17, 5:40
 *
 * Project: RipplyLights
 * Purpose: Attempt at emulating pwm with timers as to allow for more GPIO outputs
 */

#include <stdio.h>
#include <stdlib.h>

#include <Generic.h>
#include <p24hxxxx.h>
#include <uart.h>
#include <pps.h>
#include <outcompare.h>
#include <timer.h>

void StartUp (void);
void Chip_Go_Fast(void);


#define DUTY_MIN    1
#define DUTY_MAX    100
#define DELTA       1
#define MAX_CYCLES  100
#define LENGTH      10

#define LED1
#define LED2
#define LED3
#define LED4
#define LED5
#define LED6
#define LED7
#define LED8
#define LED9
#define LED10

enum Direction
{
    UP = 1,
    DOWN = 0
};

UINT16 UpDown[LENGTH] = {UP,UP,UP,UP,UP,UP,UP,UP,UP,UP};
UINT16 StartCycle[LENGTH] = {5,10,15,20,25,30,35,40,45,50};
UINT16 DutyCycle[LENGTH] = {0,0,0,0,0,0,0,0,0,0};
UINT16 OnOff[LENGTH] = {0,0,0,0,0,0,0,0,0,0};
UINT16 CycleCounter = 1;
UINT16 DutyFlag = 0;
UINT16 CycleFlag = 0;

_FBS( BWRP_WRPROTECT_OFF )
_FSS( SWRP_WRPROTECT_OFF )
_FGS(GWRP_OFF & GCP_OFF )
_FOSCSEL( FNOSC_FRCPLL  & IESO_OFF )
_FOSC( POSCMD_HS & OSCIOFNC_OFF & IOL1WAY_OFF & FCKSM_CSDCMD )
_FWDT( WDTPOST_PS8192 & WDTPRE_PR32 & WINDIS_OFF & FWDTEN_OFF)
_FPOR( FPWRT_PWR128 & ALTI2C_ON )
_FICD( ICS_PGD1 & JTAGEN_OFF )

int main(int argc, char** argv) {

    UINT16  i;

    for(i = 0; i < LENGTH; i++)
    {
        DutyCycle[i] = StartCycle[i];
    }

    Chip_Go_Fast();     //max out chipspeed
    StartUp();      //run a setup of chosen modules and debug states (see "StartUp.c")

    TRISA = 0;
    TRISB = 0;
    AD1PCFGL = 0xFFFF;
    LATA = 0x0000;
    LATB = 0x0000;

    //while(1);

    OpenTimer1(T1_ON &
            T1_IDLE_CON &
            T1_GATE_OFF &
            T1_PS_1_1 &
            T1_SYNC_EXT_OFF &
            T1_SOURCE_INT
            ,
            500
            );

    ConfigIntTimer1(T1_INT_ON &
            T1_INT_PRIOR_1
            );

    OpenTimer2(T2_ON &
            T2_IDLE_CON &
            T2_GATE_OFF &
            T2_PS_1_256 &
            T2_32BIT_MODE_OFF &
            T2_SOURCE_INT
            ,
            1000
            );

    ConfigIntTimer2(T2_INT_ON &
            T2_INT_PRIOR_2
            );

    while(1)        //loop forever
    {
        if (DutyFlag == 1)
        {
            for(i = 0; i < LENGTH; i++)
            {
                if(UpDown[i] == 1)
                {
                    DutyCycle[i] = DutyCycle[i] + DELTA;
                }
                else
                {
                    DutyCycle[i] = DutyCycle[i] - DELTA;
                }
            }
            for(i = 0; i < LENGTH; i++)
            {
                if(DutyCycle[i] >= DUTY_MAX)
                {
                    UpDown[i] = DOWN;
                }
                if(DutyCycle[i] <= DUTY_MIN)
                {
                    UpDown[i] = UP;
                }
            }//end of checking for changing the direction of the duty cycle
            DutyFlag = 0;
        }//end of adjusting duty cycle

        if(CycleFlag == 1)
        {
            #ifdef LED1
            if(DutyCycle[0] <= CycleCounter)
            {
                LATBbits.LATB10 = 1;
            }
            else
            {
                LATBbits.LATB10 = 0;
            }
            #endif
            #ifdef LED2
            if(DutyCycle[1] <= CycleCounter)
            {
                LATBbits.LATB9 = 1;
            }
            else
            {
                LATBbits.LATB9 = 0;
            }
            #endif
            #ifdef LED3
            if(DutyCycle[2] <= CycleCounter)
            {
                LATBbits.LATB8 = 1;
            }
            else
            {
                LATBbits.LATB8 = 0;
            }
            #endif
            #ifdef LED4
            if(DutyCycle[3] <= CycleCounter)
            {
                LATBbits.LATB7 = 1;
            }
            else
            {
                LATBbits.LATB7 = 0;
            }
            #endif
            #ifdef LED5
            if(DutyCycle[4] <= CycleCounter)
            {
                LATBbits.LATB6 = 1;
            }
            else
            {
                LATBbits.LATB6 = 0;
            }
            #endif
            #ifdef LED6
            if(DutyCycle[5] <= CycleCounter)
            {
                LATBbits.LATB2 = 1;
            }
            else
            {
                LATBbits.LATB2 = 0;
            }
            #endif
            #ifdef LED7
            if(DutyCycle[6] <= CycleCounter)
            {
                LATBbits.LATB3 = 1;
            }
            else
            {
                LATBbits.LATB3 = 0;
            }
            #endif
            #ifdef LED8
            if(DutyCycle[7] <= CycleCounter)
            {
                LATBbits.LATB4 = 1;
            }
            else
            {
                LATBbits.LATB4 = 0;
            }
            #endif
            #ifdef LED9
            if(DutyCycle[8] <= CycleCounter)
            {
                LATAbits.LATA4 = 1;
            }
            else
            {
                LATAbits.LATA4 = 0;
            }
            #endif
            #ifdef LED10
            if(DutyCycle[9] <= CycleCounter)
            {
                LATBbits.LATB5 = 1;
            }
            else
            {
                LATBbits.LATB5 = 0;
            }
            #endif
                    CycleCounter++;
                    if(CycleCounter >= MAX_CYCLES)
                    {
                        CycleCounter = 1;
                    }
                    CycleFlag = 0;
        }

    
    }//end while



    return (EXIT_SUCCESS);
}

inline void Chip_Go_Fast()      /*Maxs out the chip speed. Blocking*/
{
    // Configure PLL prescaler, PLL postscaler, PLL divisor
        PLLFBD = 41; // M = 43
        CLKDIVbits.PLLPOST = 0; // N2 = 2
        CLKDIVbits.PLLPRE = 0; // N1 = 2
    // Initiate Clock Switch to Internal FRC with PLL (NOSC = 0b001)
        __builtin_write_OSCCONH(0x01);
        __builtin_write_OSCCONL(0x01);
    // Wait for Clock switch to occur
        while (OSCCONbits.COSC != 0b001);
    // Wait for PLL to lock
        while(OSCCONbits.LOCK != 1) {};
    return;
}

//////////
//ISR/////
//////////


void __attribute__ ((auto_psv))     _ISR    _T2Interrupt(void)
{
    _T2IF = 0;

    DutyFlag = 1;

    return;
}


void __attribute__ ((auto_psv))     _ISR    _T1Interrupt(void)
{
    _T1IF = 0;          //clear interrupt flag

    CycleFlag = 1;

    return;
}


