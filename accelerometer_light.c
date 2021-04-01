#include "msp.h"
#include "adc.h"

#define X_CHAN 14
#define Y_CHAN 13
#define Z_CHAN 11

#define X_IN BIT1
#define Y_IN BIT0
#define Z_IN BIT2

#define RED_PIN P2
#define BLUE_PIN P5
#define GREEN_PIN P2
#define RED BIT6
#define GREEN BIT4
#define BLUE BIT6

#define PWM_PERIOD 30060

#define FULL_RANGE 16384.0
#define HALF_HALF_RANGE 4096
//#define FULL_RANGE 2.2

void main(void)
{
    volatile int X_result;
    volatile int Y_result;
    volatile int Z_result;
    volatile double x_percent;
    volatile double y_percent;
    volatile double z_percent;

    int red_duty_cycle = 0;
    int green_duty_cycle = 0;
    int blue_duty_cycle = 0;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer

    //set up p6.1 as analog input for x-axis
    P6->SEL0 |= X_IN;
    P6->SEL1 |= X_IN;

    //set up p4.0 as analog input for y-axis
    P4->SEL0 |= Y_IN;
    P4->SEL1 |= Y_IN;

    //set up p4.2 as analog input for z-axis
    P4->SEL0 |= Z_IN;
    P4->SEL1 |= Z_IN;

    //set red LED pin output to PWM output mode from TA0.3
    RED_PIN->DIR |= RED;
    RED_PIN->SEL1 &= ~RED;
    RED_PIN->SEL0 |= RED;

    //set green LED pin output to PWM output mode from TA0.1
    GREEN_PIN->DIR |= GREEN;
    GREEN_PIN->SEL1 &= ~GREEN;
    GREEN_PIN->SEL0 |= GREEN;

    //set blue LED pin output to PWM output mode from TA2.1
    BLUE_PIN->DIR |= BLUE;
    BLUE_PIN->SEL1 &= ~BLUE;
    BLUE_PIN->SEL0 |= BLUE;

    //enable timer A0 in up mode with SMCLK
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;

    //enable timer A2 in up mode with SMCLK
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP;

    //set PWM period in both TA0 and TA2
    TIMER_A0->CCR[0] = PWM_PERIOD - 1;
    TIMER_A2->CCR[0] = PWM_PERIOD - 1;

    //use TA0.3 & TA0.1 & TA2.1 in compare mode with reset/set output mode
    TIMER_A0->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;

    //start with 0% duty cycle
    TIMER_A0->CCR[3] = red_duty_cycle;
    TIMER_A0->CCR[1] = green_duty_cycle;
    TIMER_A2->CCR[1] = blue_duty_cycle;

    while (1)
    {
        //convert analog input in x-axis
        adcInit(X_CHAN);
        X_result = adcSample();

        //convert analog input in y-axis
        adcInit(Y_CHAN);
        Y_result = adcSample();

        //convert analog input in z-axis
        adcInit(Z_CHAN);
        Z_result = adcSample();

        //convert the result from x-axis into duty cycle for red LED
        x_percent = (X_result-HALF_HALF_RANGE) / (FULL_RANGE/2.0);
        red_duty_cycle = (PWM_PERIOD - 1) * x_percent;
        TIMER_A0->CCR[3] = red_duty_cycle;

        //convert the result from x-axis into duty cycle for red LED
        y_percent = (Y_result-HALF_HALF_RANGE) / (FULL_RANGE/2.0);
        green_duty_cycle = (PWM_PERIOD - 1) * y_percent;
        TIMER_A0->CCR[1] = green_duty_cycle;

        //convert the result from x-axis into duty cycle for red LED
        z_percent = (Z_result-HALF_HALF_RANGE) / (FULL_RANGE/2.0);
        blue_duty_cycle = (PWM_PERIOD - 1) * z_percent;
        TIMER_A2->CCR[1] = blue_duty_cycle;
    }

}
