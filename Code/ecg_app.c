/********************************************************************
* Filename: ecg_app.c
* Description: This file implements the ecg medical application
* Developed by: Marco Carvalho
* Created on: 30/03/05
/*********************************************************************/

#include "wifidev.h"
#include "ecg_app.h"

/****************************Global Variables *****************************/
unsigned int xdata tick_count;
unsigned int xdata samples = 0;

#define ECG_POST_COMMAND_LEN 130
u8_t xdata *ecg_apphdr = &uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN + ECG_POST_COMMAND_LEN];
u8_t xdata *ecg_appdata = &uip_buf[UIP_TCPIP_HLEN + UIP_LLH_LEN + ECG_POST_COMMAND_LEN + 4];

unsigned char xdata ecg_state;

unsigned int xdata ecg_index = 0;

unsigned long xdata lba_address = 0;

// read adc data buffer. 
// This buffer simulate 3 samples from ecg adc. In the future it will be change 
// by a real ecg adc sample 
unsigned char xdata read_adc[]={0, 34, 18};

static char xdata i_adc=0;

/*********************************************************************
/* ecg_read(void)
/* read an ecg sample 
*********************************************************************/
char ecg_read(void)
{
	char xdata data_read;

	/* Implement circular buffer */
	if (cis_device == CISTPL_FUNCID_NETWORK)
	{
		if(ecg_index >= (UIP_BUFSIZE - UIP_TCPIP_HLEN + UIP_LLH_LEN + ECG_POST_COMMAND_LEN + 4))
    	{
			ecg_index = 0;
		}
	}

	/* reset simulated ecg sample data buffer */
	if(i_adc == 3)
		i_adc = 0;

	/* read a simulated ecg sample */
	data_read = read_adc[i_adc++];

	return data_read;
}

/*********************************************************************
*                         ecg_timer0_isr()                     
*                                                                   
* This function deal with timer 0 irq. It manages two modes of operation
* according to cis_device variable. 
*
* If the CF card is a network card, the timer 0 overflow is setup to 
* 24Hz frequency and it calls ecg_read function to collect ecg data. 
* In network mode also the machine state goes to busy or power down 
* state depending on some variables states.
*
* If the CF is a memory card, the sample frequency is increased to 
* 1000Hz and it continues to store ecg data.
*********************************************************************/
static void ecg_timer0_isr(void) interrupt 1 using 2
{

	/*Reload timer/ counter 0 according to CF mode */   
	if (cis_device == CISTPL_FUNCID_NETWORK)
	{
    	TH0 = ECG_T0_RELOAD_NETWORK >> 8; // 24Hz
    	TL0 = ECG_T0_RELOAD_NETWORK;
    
		// Increment 24ths of a second counter.
    	tick_count++;

		/* Read ecg data to get the analog value from adc */
		//ecg_appdata[ecg_index++] = ecg_read();

		/* Increment the number of samples */
		//samples++;

		/* if collect time actived so change to Power down state */
		if(tick_count == 600) 
		{
			/* if reset by Watch dog try to send data again*/
			if(WDS)
			{
				ecg_state = ECG_STATE_DPM_BUSY;
				CLEAR_TIMER_INTERVAL_ENABLE_BIT(); // TIEN = TIMECON.1 = 0
				ecg_send_button = 1;
				DISABLE_TMR0_IRQ(); // stop acquiring new data
				tick_count = 0;
		
				/* set WDE bit to clear WDT*/			
				DISABLE_GLOBAL_IRQ();
				ENABLE_WDT_WRITE();		
				WDS = 0;
				ENABLE_GLOBAL_IRQ();
			}
			/* enter in power down mode */
			else
			{
				ecg_state = ECG_STATE_DPM_PWRDWN;
			}
		}
	}
	else // if(cis_device == CISTPL_FUNCID_FIXED) 
	{
    	TH0 = ECG_T0_RELOAD_MEMORY >> 8;  //1000Hz
    	TL0 = ECG_T0_RELOAD_MEMORY;

		uip_buf[ecg_index++] = ecg_read();
	}
	
	return;
}

/*********************************************************************
*                         ecg_tick_isr()                     
* this interrupt service routine deals with the power down wake-up.
* It clear the tic bit, change the ecg state machine to the idle state
* and reset the tick_count variable                                                                  
*********************************************************************/
static void ecg_tick_isr(void) interrupt 10 using 3
{
	CLEAR_TIC_BIT(); //TII = TIMECON.2 = 0
	CLEAR_TIMER_INTERVAL_ENABLE_BIT(); // TIEN = TIMECON.1 = 0
	ecg_state = ECG_STATE_DPM_IDLE;
	tick_count = 0; //reset tick counter
	return;
}

/*********************************************************************
*                         ecg_int0_isr()                     
* This interrupt service routine controls the int0 irq associated to
* the ecg emergency switch. it change the ecg state machine to busy, set 
* a variable associate with the irq, stop acquiring new data disabling 
* time 0 irq and clear the tick count variable                                                                   
*********************************************************************/
static void ecg_int0_isr(void) interrupt 0 using 1
{
	/* Wait PLL to lock */
	while(!(PLLCON && 0x40));

	ecg_state = ECG_STATE_DPM_BUSY;
	CLEAR_TIMER_INTERVAL_ENABLE_BIT(); // TIEN = TIMECON.1 = 0
	ecg_send_button = 1;
	DISABLE_TMR0_IRQ(); // stop acquiring new data
	tick_count = 0;

	return;
}

/*********************************************************************
* ecg_prepare_data()
* prepare data to send. After sign the uIP TCP/IP Stack with newdata 
* to send                                                                  
*********************************************************************/
void ecg_prepare_data(void)
{
	/* prepare app header */
	/* get the timestamp */
	ecg_apphdr[0] = HOUR; 
	ecg_apphdr[1] = MIN;
	ecg_apphdr[2] = SEC;
	/* set the sample frequency */
	ecg_apphdr[3] = 24;

	/* setup the app len */
	uip_len = samples+4;

	/* reset samples number */
	samples = 0;

	return;
}


