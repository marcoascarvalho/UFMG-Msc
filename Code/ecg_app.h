/********************************************************************
* Filename: ecg_app.h
* Description: ecg_app.c header file
* Developed by: Marco Carvalho
* Created on: 30/03/05
/*********************************************************************/

#include "uip.h"
#include "uipopt.h"
#include "webclient.h"

/*********************************************************************
*                            Private defines.                        
*********************************************************************/
// Change ECG_CPU_XTAL to match hardware
#define ECG_CPU_XTAL       12580000             // 8051 crystal freq in Hz

// Crystal/ Baud rate combinations marked * are well supported with 100.0%
// timing accuracy.
/*                             Xtal Frequency (MHz)

                    3.6864     11.0592     18.4320     22.1184
                ÚÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄ¿
            300 ³     *     ³     *     ³     *     ³     *     ³
                ÃÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄ´
            600 ³     *     ³     *     ³     *     ³     *     ³
                ÃÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄ´
           1200 ³     *     ³     *     ³     *     ³     *     ³
                ÃÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄ´
  BAUD     2400 ³     *     ³     *     ³     *     ³     *     ³
  RATE          ÃÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄ´
           4800 ³     *     ³     *     ³     *     ³     *     ³
                ÃÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄ´
           9600 ³     *     ³     *     ³     *     ³     *     ³
                ÃÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄ´
          14400 ³     -     ³     *     ³     -     ³     *     ³
                ÃÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄ´
          19200 ³     -     ³     -     ³     -     ³     *     ³
                ÀÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÙ            */


// Low Baud rate (SCON = 0), X1 CPU mode timing parameters.
// Adjust XTAL and UART_BPS to suit your application and hardware.
#define ECG_XTAL             ECG_CPU_XTAL      // Crystal freq in Hz
#define ECG_UART_BAUD            4800               // Tranceiver baud rate
#define ECG_T1_CLOCK         ECG_XTAL / 12          // Timer 1 mode 2 clock rate 
#define ECG_T1_RELOAD            256 - ((ECG_T1_CLOCK / 32) / ECG_UART_BAUD)

#define ECG_CPU_CLOCK      ECG_CPU_XTAL / 12    // 8051 clock rate (X1 mode)

// Delay routine timing parameters
#define ECG_DELAY_CONST    9.114584e-5          // Delay routine constant
#define ECG_DELAY_MULTPLR  (unsigned char)(ECG_DELAY_CONST * ECG_CPU_CLOCK)

// X1 CPU mode timing parameters network mode
#define ECG_T0_CLOCK             ECG_CPU_XTAL / 12 // Timer 0 mode 1 clock rate
#define ECG_T0_INT_RATE_NETWORK  24                // Timer 0 intrupt rate (Hz)
#define ECG_T0_RELOAD_NETWORK    65536 - (ECG_T0_CLOCK / ECG_T0_INT_RATE_NETWORK)

// X1 CPU mode timing parameters memory mode
#define ECG_T0_CLOCK             ECG_CPU_XTAL / 12 // Timer 0 mode 1 clock rate
#define ECG_T0_INT_RATE_MEMORY   1000              // Timer 0 intrupt rate (Hz)
#define ECG_T0_RELOAD_MEMORY     65536 - (ECG_T0_CLOCK / ECG_T0_INT_RATE_MEMORY)

#define ENABLE_TMR0_IRQ() ET0 = 1;
#define DISABLE_TMR0_IRQ() ET0 = 0;

#define ENABLE_INT0_IRQ() EX0 = 1;
#define DISABLE_INT0_IRQ() EX0 = 0;

/* IRQ enable/disable macros */
#define	ENABLE_GLOBAL_IRQ() EA = 1;
#define	DISABLE_GLOBAL_IRQ() EA = 0;  

/* Watch dog timer */
#define ENABLE_WDT() WDE = 1;
#define ENABLE_WDT_WRITE() WDWR = 1;

/* GREEN LED */
#define GREEN_ON() LED2=0 //verde ( 0=aceso )
#define GREEN_OFF() LED2=1 //verde ( 1=apagado )
#define GREEN_LED LED2 
#define GREEN_TOGGLE() LED2=!LED2

#define ERASE_ALL() ECON = 0x06;

#define ECG_STATE_DPM_BUSY   0
#define ECG_STATE_DPM_IDLE   1
#define ECG_STATE_DPM_PWRDWN 2

#define CLEAR_TIC_BIT() TIMECON = TIMECON & 0xFB;
#define CLEAR_TIMER_INTERVAL_ENABLE_BIT() TIMECON = TIMECON & 0xFD; 

/*********************************************************************
*                       external variables                        
*********************************************************************/
extern unsigned char xdata ecg_state;
extern unsigned int xdata tick_count;
extern unsigned int xdata ecg_index;
extern unsigned long xdata lba_address;

/*********************************************************************
*                       function prototypes                        
*********************************************************************/
char ecg_read(void);
void ecg_prepare_data(void);


