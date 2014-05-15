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

#define DUTY_START  5
#define DUTY_MAX    100
#define INCREMENT   1

UINT16 Flag;
//UINT16 UpDown[10] = {0,0,0,0,0,0,0,0,0,0};
//UINT16 StartCycle[10] = {DUTY_START,0,0,0,0,0,0,0,0,0};
//UINT16 DutyCycle[10] = {DUTY_START,0,0,0,0,0,0,0,0,0};
//UINT16 OnOff[10] = {0,0,0,0,0,0,0,0,0,0};
UINT16 UpDown = 0;
UINT16 StartCycle = DUTY_START;
UINT16 DutyCycle = DUTY_START;
UINT16 OnOff = 0;

_FBS( BWRP_WRPROTECT_OFF )
_FSS( SWRP_WRPROTECT_OFF )
_FGS(GWRP_OFF & GCP_OFF )
_FOSCSEL( FNOSC_FRCPLL  & IESO_OFF )
_FOSC( POSCMD_HS & OSCIOFNC_OFF & IOL1WAY_OFF & FCKSM_CSDCMD )
_FWDT( WDTPOST_PS8192 & WDTPRE_PR32 & WINDIS_OFF & FWDTEN_OFF)
_FPOR( FPWRT_PWR128 & ALTI2C_ON )
_FICD( ICS_PGD1 & JTAGEN_OFF )

int main(int argc, char** argv) {

    //UINT16 DutyCycle = DUTY_START;
    //DutyCycleHold = DutyCycle;
    UINT16 UpCycle = 1;
    UINT16 i = 0;

    Chip_Go_Fast();     //max out chipspeed
    StartUp();      //run a setup of chosen modules and debug states (see "StartUp.c")

    TRISA = 0;
    TRISB = 0;
    AD1PCFGL = 0xFFFF;
    LATA = 0xFFFF;
    LATB = 0xFFFF;

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
            T1_INT_PRIOR_2
            );

    OpenTimer2(T2_ON &
            T2_IDLE_CON &
            T2_GATE_OFF &
            T2_PS_1_256 &
            T2_32BIT_MODE_OFF &
            T2_SOURCE_INT
            ,
            3000
            );

    ConfigIntTimer2(T2_INT_ON &
            T2_INT_PRIOR_1
            );

    while(1)        //loop forever
    {
//        for(i = 0; i < 10; i++)
//        {
            if(Flag == 1)   //if timer 1 has proced
            {
                Flag = 0;   //set the flag back to 0
                if(UpCycle == 1)      //if the cycle is 1
                {
                    if(DutyCycle > 0)   //and if the dutycycle is greater than 0
                    {
                        LATBbits.LATB6 = 1;     //output high on pin 15
                        DutyCycle--;            //decrease the duty cycle by 1
                    }
                    else            //however if the dutycycle is equal to zero (predicting boundary problems)
                    {
                        UpCycle = 0;      //set the cycle to 0
                        DutyCycle = DUTY_MAX - StartCycle;  //load the amount of 0 time
                    }
                }
                else        //if the cycle is 0
                {
                    if(DutyCycle > 0)   //and the DutyCycle is greater than 0
                    {
                        LATBbits.LATB6 = 0;     //drive pin 15 low
                        DutyCycle--;            //decrease the duty cycle
                    }
                    else                    //if the duty cycle is 0
                    {
                        UpCycle = 1;              //set the cycle back to 1
                        DutyCycle = StartCycle;     //set the cycle back to a predetermined duty cycle
                    }
                }
            }
//        }
    }



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

    //static UINT16 i;
    //for(i = 0; i < 1; i++)
    //{
        if(UpDown == 1)
        {
            if(StartCycle >= (DUTY_MAX - INCREMENT))
            {
                StartCycle = DUTY_MAX;
                UpDown = 0;
            }
            else
            {
                StartCycle = StartCycle + INCREMENT;
            }
        }
        else
        {
            if(StartCycle <= INCREMENT)
            {
                StartCycle = 0;
                UpDown = 1;
            }
            else
            {
                StartCycle = StartCycle - INCREMENT;
            }
        }
    //}

    return;
}


void __attribute__ ((auto_psv))     _ISR    _T1Interrupt(void)
{
    _T1IF = 0;          //clear interrupt flag

    Flag = 1;

    return;
}


