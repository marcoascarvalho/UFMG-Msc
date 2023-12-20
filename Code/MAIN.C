/********************************************************************
* Filename: wifidev.c
* Description: This file implements The interface between the WIFI 
* device driver and the application trought the uIP TCP/IP Stack
* Developed by: Marco Carvalho
* Created on: 30/03/05
/*********************************************************************/

#include "main.h"

void main(void)
{ 
    u8_t xdata i, arptimer;

    /* Initialise the uIP TCP/IP stack. */
    uip_init();

    /* Initialise the app. */
    webclient_init();

    /* Config chip and Initialise the device driver. */ 
    config_chip();

    /* Initialise the ARP cache. */
    uip_arp_init();

    arptimer = 0;

	/* Find cF mode */
	find_CF_mode();
	if(cis_device == CISTPL_FUNCID_NOT_SUPPORTED)
	{
		printf("Device not Supported. Please put a valid CF card and try again.\n");
		//RED_ON();
		while(1);
		//return;
	}

    while(1)
    {
		switch(ecg_state)
		{
			case ECG_STATE_DPM_BUSY: //TCP/IP stack running
			{
				/* if the ecg button was pressed call the application to open a connection*/
				if(ecg_send_button)
				{
					/*init device driver*/
					wifidev_init();
					/*disable IRQ0 IRQ */
					DISABLE_INT0_IRQ();
					printf("Call the application\n");
					UIP_APPCALL();
				}

		    	uip_len = wifidev_read();

		        if(uip_len == 0)
        		{
					for(i = 0; i < UIP_CONNS; i++)
            		{
						uip_periodic(i);
                		/* If the above function invocation resulted in data that
                   		should be sent out on the network, the global variable
                   		uip_len is set to a value > 0. */
                		if(uip_len > 0)
                		{
                    		uip_arp_out();
                    		wifidev_send();
                		}
            		}

#if UIP_UDP
		            for(i = 0; i < UIP_UDP_CONNS; i++)
        		    {
                		uip_udp_periodic(i);
                		/* If the above function invocation resulted in data that
                   		should be sent out on the network, the global variable
                   		uip_len is set to a value > 0. */
                		if(uip_len > 0)
                		{
                    		uip_arp_out();
                    		wifidev_send();
                		}
            		}
#endif /* UIP_UDP */

		            /* Call the ARP timer function every 10 seconds. */
        		    if(++arptimer == 20)
            		{	
	            		uip_arp_timer();
	            		arptimer = 0;
            		}
        		}
        		else /* (uip_len != 0) Process incoming */
        		{
        		    if(BUF->type == htons(UIP_ETHTYPE_IP))
            		{
	            		uip_arp_ipin();
	            		uip_input();
						/* If the above function invocation resulted in data that
	               		should be sent out on the network, the global variable
	               		uip_len is set to a value > 0. */
	            		if(uip_len > 0)
                		{
	                		uip_arp_out();
	                		wifidev_send();
	            		}
            		}
            		else if(BUF->type == htons(UIP_ETHTYPE_ARP))
            		{
	            		uip_arp_arpin();
	            		/* If the above function invocation resulted in data that
	               		should be sent out on the network, the global variable
	               		uip_len is set to a value > 0. */	
	            		if(uip_len > 0)
                		{	
	                		wifidev_send();
	            		}
            		}
        		}
				break;
    		}//ECG_STATE_DPM_BUSY

			case ECG_STATE_DPM_IDLE:
			{
				ENABLE_IDLE_MODE(); 
				if(tick_count == 1)
				{
					/*if CF card = Network, reset card */
					if (cis_device == CISTPL_FUNCID_NETWORK)
						CF_RESET_ON();            
										
					printf("Idle mode activated\n");
#if PLL_DPM
					PLLCON = PLLCON | 0x07; //CPU clk = 0,09MHz
#endif

				}
				/* if memory mode write one sector if it is full */
				if (cis_device == CISTPL_FUNCID_FIXED)
				{
					/* if one sector is full, write it in CF memory */
					if (ecg_index == BYTE_PER_SEC)
					{
						printf("Write 512 ECG data bytes in the sector 0x%Lx\n", lba_address);
						if (lba_address == CFNumSectors)
						{
							lba_address =0;
						}
						else
						{
							DISABLE_TMR0_IRQ();
							if(CFWriteSector(lba_address++))
				 		    	printf("CFWriteSector command error \n");
							ENABLE_TMR0_IRQ();
						}
					}
					/* if one sector is full fill uip_buffer with one more sector content */
					else if (ecg_index > BYTE_PER_SEC)
					{
					//	DISABLE_TMR0_IRQ();
					//	printf("try to read one sector\n");
					//	CFReadSector(0x00000000);
					//	//for(j=0;j<BYTE_PER_SEC;j++) 
					//	//	printf("uip_buf[%d] = %Bx\n", j,(char)uip_buf[j]);
					//	ENABLE_TMR0_IRQ();
						ecg_index = 0;
					}
				}

				break;
			}
	
			case ECG_STATE_DPM_PWRDWN:
			{
				CF_RESET_ON();
				ENABLE_INT0_PD(); //enable Int0 power down bit
				ENABLE_OSC_PD(); //enable oscillator power down bit
				INTVAL = 0x1e; // config timer interval bit to 30 seconds
				//CONFIG_INTERVAL_COUNTER_HOURS();
				//CONFIG_INTERVAL_COUNTER_MINUTES();
				CONFIG_INTERVAL_COUNTER_SECONDS();
				ENABLE_TIMER_INTERVAL_COUNTER(); // Enable timer interval to count
				ENABLE_TIC_IRQ();                // Enable tick IRQ
				printf("Power Down mode activated\n");
#if PLL_DPM
				PLLCON = PLLCON | 0x07; //CPU clk = 0,09MHz
#endif
				ENABLE_POWER_DOWN();
				break;
			}
	
		}//switch(ecg_state)
	}//while(1)

    return;
}
