/********************************************************************
* Filename: Prism.h
* Description: This file implements the header file for the prism device 
*              driver for 8051 microcontroller 
* Developed by: Marco Carvalho
* Created on: 09/03/05
/*********************************************************************/

//#include "config.h"
#include "uip.h"
#include "uip_arp.h"
#include "Cfdev.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <intrins.h>

/* Turn to 1 if you want simulate */
#define SIMULATE 0

/*control variable to test PLL DPM */
#define PLL_DPM 0

// Change WIFI_CPU_XTAL to match hardware
#if PLL_DPM
//#define WIFI_CPU_XTAL        1572864
#define WIFI_CPU_XTAL          98304
#else
#define WIFI_CPU_XTAL       12580000             // 8051 crystal freq in Hz
#endif

#define CISTPL_FUNCID_NETWORK       0x06
#define CISTPL_FUNCID_MEMORY        0x01
#define CISTPL_FUNCID_FIXED         0x04 
#define CISTPL_FUNCID_NOT_SUPPORTED 0x00 
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
#define XTAL                 WIFI_CPU_XTAL      // Crystal freq in Hz

#if PLL_DPM
#define UART_BAUD            1200               // Tranceiver baud rate
#else
#define UART_BAUD            4800               // Tranceiver baud rate
#endif

#define T1_CLOCK             XTAL / 12          // Timer 1 mode 2 clock rate 
#define T1_RELOAD            256 - ((T1_CLOCK / 32) / UART_BAUD)

/*  ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
    º                            Private defines.                         º
    ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼  */
#define WIFI_CPU_CLOCK      WIFI_CPU_XTAL / 12    // 8051 clock rate (X1 mode)

// Delay routine timing parameters
#define WIFI_DELAY_CONST    9.114584e-5          // Delay routine constant
#define WIFI_DELAY_MULTPLR  (unsigned char)(WIFI_DELAY_CONST * WIFI_CPU_CLOCK)

// X1 CPU mode timing parameters
#define WIFI_T0_CLOCK             WIFI_CPU_XTAL / 12 // Timer 0 mode 1 clock rate
#define WIFI_T0_INT_RATE          24                // Timer 0 intrupt rate (Hz)
#define WIFI_T0_RELOAD            65536 - (WIFI_T0_CLOCK / WIFI_T0_INT_RATE)

/*********************** Defines CF Memory ***********************/

/*
 * Configuration Option Register (COR)
 */
#define COR_CONFIG_MASK		0x3f
#define COR_MFC_CONFIG_MASK	0x38
#define COR_FUNC_ENA		0x01
#define COR_ADDR_DECODE		0x02
#define COR_IREQ_ENA		0x04
#define COR_LEVEL_REQ		0x40
#define COR_SOFT_RESET		0x80
#define COR_SOFT_SET        0x00

/*
 * CIS Tuple Codes
 */
#define CISTPL_DEVICE              0x01
#define CISTPL_DEVICE_A            0x17
#define CISTPL_DEVICE_OC           0x1c
#define CISTPL_DEVICE_OA           0x1d
#define CISTPL_VERS_1              0x15
#define CISTPL_MANFID              0x20
#define CISTPL_FUNCID              0x21
#define CISTPL_FUNCE               0x22
#define CISTPL_CONFIG              0x1a
#define CISTPL_CFTABLE_ENTRY       0x1b
#define CISTPL_END                 0xff

//Pinos
#define CF_DATA_PORT    P0  // leitura/escrita de dados
#define CF_DATA_PORT_MASK 0xFF

// CF ADDR Port
#define CF_ADDR_PORT_LOW 	P2     //portos de enderecamento LSB dos registros da CF( A5..A0 )
#define CF_ADDR_PORT_LOW_MASK 0x30 //00110000

#define CF_ADDR_PORT_HIGH 	P3      //portos de enderecamento MSB dos registros da CF( A8..A6 ) 
#define CF_ADDR_PORT_HIGH_MASK 0x37 //00110111
									// OBS: A9=A8 via hardware, A10=GND	

#define CF_CS 		 CS_DSK	
#define CF_CS_ON() 	 CF_CS=1;
#define CF_CS_OFF()  CF_CS=0;	

#define CF_RD 		 RD	
#define CF_RD_ON() 	 CF_RD=1;
#define CF_RD_OFF()  CF_RD=0;

#define CF_WR 		 WR	
#define CF_WR_ON() 	 CF_WR=1;
#define CF_WR_OFF()  CF_WR=0;

/* IO control */
#define CF_REG       REG 
#define CF_REG_ON()  CF_REG=1; 
#define CF_REG_OFF() CF_REG=0;

#define CF_IOWR       IOWR
#define CF_IOWR_ON()  CF_IOWR=1;
#define CF_IOWR_OFF() CF_IOWR=0;

#define CF_IORD       IORD
#define CF_IORD_ON()  CF_IORD=1;
#define CF_IORD_OFF() CF_IORD=0;

#define CF_RESET       RESET
#define CF_RESET_ON()  CF_RESET=1;
#define CF_RESET_OFF() CF_RESET=0;

#define CF_CD1      CD1
#define NOP()		_nop_ ();	//função intrínsica de NOP

#define ON			1
#define OFF			0

/******************** /* specific defines ECG board ************************/
/* RED LED */
#define RED_ON() T2EX=0 //vermelho ( 0=aceso ) 
#define RED_OFF() T2EX=1 //vermelho ( 1=apagado )
#define RED_TOGGLE() T2EX=!T2EX
#define RED_LED T2EX

#define SET_TMR0_MODE1()   TMOD = (TMOD & 0xF0)|0x01; 
#define TMR0_START()       TR0 = 1; //TCON = (TCON & 0xEF)|0x10;
//#define TMR0_SETUP(value)  TH0  = (value<<8) && 0xFF
//						   TL0  = value && 0xFF

/* Power Supply Monitor Enable */
#define ENABLE_PSM 1;
#define DISABLE_PSM 0;
#define PSMI 0x20;
#define ENABLE_PSM_IRQ() PSMCON |= PSMI;

/* Power Down and idle mode */
#define PD 0x02;
#define ENABLE_POWER_DOWN() PCON = PCON | PD;
#define IDL 0x01;
#define ENABLE_IDLE_MODE() PCON = PCON | IDL;

#define	ENABLE_SERIAL_IRQ() ES = 1;   
#define	DISABLE_SERIAL_IRQ() ES = 0;   
#define ENABLE_INT1_IRQ() EX1 = 1;
#define DISABLE_INT1_IRQ() EX1 = 0;

// Timer 0 trigger value; 
// tick time = 12 (machine cycles)*256(Number of TH0 pulses)*DIV(TH0 overflow times) / CPU_CLK 
// 50ms tick time with 12.58 MHz clock requires DIV = 205
#define TMR0_DIV  205

// 50 ms ticks with 12.58 MHz clock requires 52417 timer pulses
// that means a timer0 pre-charge of 65536-52417 = 13119 = 333F
#define TIMER0_SET 0x333F

#define XRAM_ENABLE() CFG834 |= 0x01

/* RTC Configuration */
#define RTC_ENABLE() TIMECON |= 0x01;
#define SET_RTC_TMR(HOUR_VALUE, MIN_VALUE, SEC_VALUE) TIMECON &= 0x00; \ 
													  HOUR=(HOUR_VALUE); \
													  MIN=(MIN_VALUE); \
													  SEC = (SEC_VALUE); \

#define CONFIG_INTERVAL_COUNTER_HOURS() TIMECON = TIMECON & 0x0F; \
										TIMECON = TIMECON | 0x30; \

#define CONFIG_INTERVAL_COUNTER_MINUTES() TIMECON = TIMECON & 0x0F; \  
										  TIMECON = TIMECON | 0x20; \

#define CONFIG_INTERVAL_COUNTER_SECONDS() TIMECON = TIMECON & 0x0F; \
										  TIMECON = TIMECON | 0x10;	\

#define ENABLE_TIMER_INTERVAL_COUNTER() TIMECON = TIMECON & 0xFD; \
										TIMECON = TIMECON | 0x02; \


#define ENABLE_TIC_IRQ() IEIP2 = IEIP2 | 0x04;

/* WDT */
/* reset WDE bit to clear WDT*/		
#define RESET_WDE() DISABLE_GLOBAL_IRQ(); \
					ENABLE_WDT_WRITE();	\	
					WDE = 0; \
					ENABLE_GLOBAL_IRQ(); \

/*enable WDT for 2seg timeout*/
#define CONFIG_WDT_2SEG()	DISABLE_GLOBAL_IRQ(); \
							ENABLE_WDT_WRITE(); \		
							WDCON = 0x72; \
							ENABLE_GLOBAL_IRQ(); \

/* enable INT0 in power down mode */
#define ENABLE_INT0_PD() PCON = PCON | 0x20;

/* disable ALE output */
#define DISABLE_ALE_OUT() PCON = PCON | 0x10;

/* enable Oscillator in Power Down Mode */
#define ENABLE_OSC_PD() PLLCON = PLLCON & 0x7f;

/* tmp_buffer definition */
#define tmp_buff_size	64
extern unsigned int xdata tmp_buff[tmp_buff_size +1];

/*================================================================*/
/* 802.11 Constants */

/*--- Sizes/offsets -----------------------------------------------*/
#define WLAN_FCS_LEN			4
#define WLAN_ADDR_LEN			6
#define WLAN_CRC_LEN			4
#define WLAN_HDR_A3_LEN			24
#define WLAN_HDR_A4_LEN			30
#define WLAN_SSID_MAXLEN		32
#define WLAN_DATA_MAXLEN		2312

#define WIFI_FCS_LEN			2
#define WIFI_ADDR_LEN			3
#define WIFI_HDR_A3_LEN			12
#define WIFI_HDR_A4_LEN			15

#define FRAME_STRUCTURE_OFF		46			
#define ETH_HDR_OFF 			14			
#define MAC_LEN     			6
#define SNAP_LEN    			3
#define ETH_MTU_BUFF_SIZE		1500
#define TXFRAME_BUFF_SIZE		ETH_MTU_BUFF_SIZE + FRAME_STRUCTURE_OFF + ETH_HDR_OFF

#define TX_BSSID_OFF 			18		
#define TX_DEST_ADDR_OFF 		46
#define TX_SRC_ADDR_OFF 		52
#define RX_DST_ADDR_OFF  		46
#define RX_SRC_ADDR_OFF 		52
#define DATALEN_OFF     		58		
#define RXDATALEN_OFF   		44		
#define RXDATA_OFF  			60
#define TXDATA_OFF  			60

#define BAP_WRITE          1
#define BAP_READ           0 

/* API */
#define  BSS		  0x01
#define  IBSS		  0x02
#define  SSID		  0x03		
#define  MAX_LEN      0x04
#define  ENABLE		  0x05
#define  PM_ENABLE    0x06
#define  PM_MAX_SLEEP 0x07	
/*================================================================*/
//*	Ethernet Header Layout
//*================================================================*/
#define ETH_PCKT_LEN     	   0x00	 //enetpacketLen11	
#define ETH_PCKT_DST		   0x00  //0x01  //enetpacketDest
#define	ETH_PCKT_DST1   	   0x00  //0x01  //enetpacketDest01 destination mac address
#define	ETH_PCKT_DST2    	   0x01  //0x02  //enetpacketDest23
#define	ETH_PCKT_DST3          0x02  //0x03  //enetpacketDest45
#define ETH_PCKT_SRC           0x06  //0x04  //enetpacketSrc
#define	ETH_PCKT_SRC1		   0x03  //0x04  //enetpacketSrc01 source mac address
#define	ETH_PCKT_SRC2		   0x04  //0x05  //enetpacketSrc23
#define	ETH_PCKT_SRC3    	   0x05  //0x06  //enetpacketSrc45	
#define ETH_PCKT_LEN03		   0x0C  //0x07  //enetpacketLen03
#define ETH_PCKT_SNAP		   0x0D  //0x0E  //0x08  //enetpacketSnap
#define ETH_PCKT_CTRL		   0x0E  //0x10  //0x09  //enetpacketCntrl
#define ETH_PCKT_OUI		   0x0F  //0x12  //0x0A
#define	ETH_PCKT_TYPE		   0x10  //0x14  //0x0B  //enetpacketType type/length field
#define ETH_PCKT_DATA          0x11  //0x16  //0x0C  //enetpacketData  IP data area begins here

/*--- Command Code Constants --------------------------*/
/*--- Controller Commands --------------------------*/
#define		HFA384x_CMDCODE_INIT		0x0000 //((int)0x00)
#define		HFA384x_CMDCODE_ENABLE		0x0001 //((int)0x01)
#define		HFA384x_CMDCODE_DISABLE		0x0002 //((int)0x02)
#define		HFA384x_CMDCODE_DIAG		0x0003 //((int)0x03)

/*--- Buffer Mgmt Commands --------------------------*/
#define		HFA384x_CMDCODE_ALLOC		0x000A //((int)0x0A)
#define		HFA384x_CMDCODE_TX		    0x000B // ((int)0x0B)

/*--- Regulate Commands --------------------------*/
#define		HFA384x_CMDCODE_NOTIFY		0x0010
#define		HFA384x_CMDCODE_INQUIRE		0x0011

/*--- Configure Commands --------------------------*/
#define		HFA384x_CMDCODE_ACCESS		0x0021 //((int)0x21)

/*--- Result Codes Status Register ---------------------*/
#define		HFA384x_RES_SUCCESS			0x00
#define		HFA384x_RES_CARDFAIL		0x01
#define		HFA384x_RES_NOBUFF	    	0x05
#define     HFA384x_RES_COMMERR	    	0x7F    

/*--- Register Test/Get/Set Field macros ------------------------*/

#define		HFA384x_CMD_ISBUSY(value)			((int)(((int)value) & HFA384x_CMD_BUSY))
#define		HFA384x_CMD_AINFO_GET(value)		((int)(((int)(value) & HFA384x_CMD_AINFO) >> 8))
#define		HFA384x_CMD_AINFO_SET(value)		((int)((int)(value) << 8))
#define		HFA384x_CMD_MACPORT_GET(value)		((int)(HFA384x_CMD_AINFO_GET((int)(value) & HFA384x_CMD_MACPORT)))
#define		HFA384x_CMD_MACPORT_SET(value)		((int)HFA384x_CMD_AINFO_SET(value))
#define		HFA384x_CMD_RECL_SET(value)			((int)HFA384x_CMD_AINFO_SET(value))
#define		HFA384x_CMD_QOS_SET(value)			((int)((((int)(value)) << 12) & 0x3000)) // nao tem ref.
#define		HFA384x_STATUS_RESULT_GET(value)	((int)((((int)(value)) & HFA384x_STATUS_RESULT) >> 8))
#define		HFA384x_EVSTAT_ISCMD(value)			((int)(((int)(value)) & HFA384x_EVSTAT_CMD))
#define		HFA384x_CMD_CMDCODE_SET(value)		((int)(value))
#define		HFA384x_EVSTAT_ISALLOC(value)		((int)(((int)(value)) & HFA384x_EVSTAT_ALLOC))
#define		HFA384x_CMD_WRITE_SET(value)		((int)HFA384x_CMD_AINFO_SET((int)value))
#define		HFA384x_EVSTAT_ISALLOC(value)		((int)(((int)(value)) & HFA384x_EVSTAT_ALLOC))

#define		HFA384x_OFFSET_ISBUSY(value)		((int)(((int)(value)) & HFA384x_OFFSET_BUSY))
#define		HFA384x_OFFSET_ISERR(value)			((int)(((int)(value)) & HFA384x_OFFSET_ERR))

               /*Interrupt enable masks */
#define		HFA384x_INTEN_ISTICK(value)		((int)(((int)(value)) & HFA384x_INTEN_TICK))
#define		HFA384x_INTEN_TICK_SET(value)		((int)(((int)(value)) << 15))
#define		HFA384x_INTEN_ISWTERR(value)		((int)(((int)(value)) & HFA384x_INTEN_WTERR))
#define		HFA384x_INTEN_WTERR_SET(value)		((int)(((int)(value)) << 14))
#define		HFA384x_INTEN_ISINFDROP(value)		((int)(((int)(value)) & HFA384x_INTEN_INFDROP))
#define		HFA384x_INTEN_INFDROP_SET(value)	((int)(((int)(value)) << 13))
#define		HFA384x_INTEN_ISINFO(value)		((int)(((int)(value)) & HFA384x_INTEN_INFO))
#define		HFA384x_INTEN_INFO_SET(value)		((int)(((int)(value)) << 7))
#define		HFA384x_INTEN_ISDTIM(value)		((int)(((int)(value)) & HFA384x_INTEN_DTIM))
#define		HFA384x_INTEN_DTIM_SET(value)		((int)(((int)(value)) << 5))
#define		HFA384x_INTEN_ISCMD(value)		((int)(((int)(value)) & HFA384x_INTEN_CMD))
#define		HFA384x_INTEN_CMD_SET(value)		((int)(((int)(value)) << 4))
#define		HFA384x_INTEN_ISALLOC(value)		((int)(((int)(value)) & HFA384x_INTEN_ALLOC))
#define		HFA384x_INTEN_ALLOC_SET(value)		((int)(((int)(value)) << 3))
#define		HFA384x_INTEN_ISTXEXC(value)		((int)(((int)(value)) & HFA384x_INTEN_TXEXC))
#define		HFA384x_INTEN_TXEXC_SET(value)		((int)(((int)(value)) << 2))
#define		HFA384x_INTEN_ISTX(value)		((int)(((int)(value)) & HFA384x_INTEN_TX))
#define		HFA384x_INTEN_TX_SET(value)		((int)(((int)(value)) << 1))
#define		HFA384x_INTEN_ISRX(value)		((int)(((int)(value)) & HFA384x_INTEN_RX))
#define		HFA384x_INTEN_RX_SET(value)		((int)(((int)(value)) << 0))

#define		HFA384x_EVSTAT_ISTICK(value)		((int)(((int)(value)) & HFA384x_EVSTAT_TICK))
#define		HFA384x_EVSTAT_ISWTERR(value)		((int)(((int)(value)) & HFA384x_EVSTAT_WTERR))
#define		HFA384x_EVSTAT_ISINFDROP(value)		((int)(((int)(value)) & HFA384x_EVSTAT_INFDROP))
#define		HFA384x_EVSTAT_ISINFO(value)		((int)(((int)(value)) & HFA384x_EVSTAT_INFO))
#define		HFA384x_EVSTAT_ISDTIM(value)		((int)(((int)(value)) & HFA384x_EVSTAT_DTIM))
#define		HFA384x_EVSTAT_ISCMD(value)		((int)(((int)(value)) & HFA384x_EVSTAT_CMD))
#define		HFA384x_EVSTAT_ISALLOC(value)		((int)(((int)(value)) & HFA384x_EVSTAT_ALLOC))
#define		HFA384x_EVSTAT_ISTXEXC(value)		((int)(((int)(value)) & HFA384x_EVSTAT_TXEXC))
#define		HFA384x_EVSTAT_ISTX(value)		((int)(((int)(value)) & HFA384x_EVSTAT_TX))
#define		HFA384x_EVSTAT_ISRX(value)		 ((int)(((int)(value)) & HFA384x_EVSTAT_RX))

#define		HFA384x_EVACK_ISTICK(value)		((int)(((int)(value)) & HFA384x_EVACK_TICK))
#define		HFA384x_EVACK_TICK_SET(value)		((int)(((int)(value)) << 15))
#define		HFA384x_EVACK_ISWTERR(value)		((int)(((int)(value)) & HFA384x_EVACK_WTERR))
#define		HFA384x_EVACK_WTERR_SET(value)		((int)(((int)(value)) << 14))
#define		HFA384x_EVACK_ISINFDROP(value)		((int)(((int)(value)) & HFA384x_EVACK_INFDROP))
#define		HFA384x_EVACK_INFDROP_SET(value)	((int)(((int)(value)) << 13))
#define		HFA384x_EVACK_ISINFO(value)		((int)(((int)(value)) & HFA384x_EVACK_INFO))
#define		HFA384x_EVACK_INFO_SET(value)		((int)(((int)(value)) << 7))
#define		HFA384x_EVACK_ISDTIM(value)		((int)(((int)(value)) & HFA384x_EVACK_DTIM))
#define		HFA384x_EVACK_DTIM_SET(value)		((int)(((int)(value)) << 5))
#define		HFA384x_EVACK_ISCMD(value)		((int)(((int)(value)) & HFA384x_EVACK_CMD))
#define		HFA384x_EVACK_CMD_SET(value)		((int)(((int)(value)) << 4))
#define		HFA384x_EVACK_ISALLOC(value)		((int)(((int)(value)) & HFA384x_EVACK_ALLOC))
#define		HFA384x_EVACK_ALLOC_SET(value)		((int)(((int)(value)) << 3))
#define		HFA384x_EVACK_ISTXEXC(value)		((int)(((int)(value)) & HFA384x_EVACK_TXEXC))
#define		HFA384x_EVACK_TXEXC_SET(value)		((int)(((int)(value)) << 2))
#define		HFA384x_EVACK_ISTX(value)		((int)(((int)(value)) & HFA384x_EVACK_TX))
#define		HFA384x_EVACK_TX_SET(value)		((int)(((int)(value)) << 1))
#define		HFA384x_EVACK_ISRX(value)		((int)(((int)(value)) & HFA384x_EVACK_RX))
#define		HFA384x_EVACK_RX_SET(value)		((int)(((int)(value)) << 0))

/*--- Register Field Masks --------------------------*/
#define		HFA384x_CMD_BUSY		0x8000 //BIT15=1
#define		HFA384x_STATUS_RESULT	0x7F00 //BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 = 1
#define		HFA384x_EVSTAT_CMD		0x0010 //BIT4=1

#define		HFA384x_OFFSET_BUSY		0x8000 //((int)BIT15)
#define		HFA384x_OFFSET_ERR		0x4000 //((int)BIT14)

#define     HFA384x_INT_NORMAL           (HFA384x_EVSTAT_INFO|HFA384x_EVSTAT_RX|HFA384x_EVSTAT_TX|HFA384x_EVSTAT_TXEXC|HFA384x_EVSTAT_INFDROP|HFA384x_EVSTAT_ALLOC|HFA384x_EVSTAT_DTIM)

#define		HFA384x_INTEN_TICK		0x8000 //((int)BIT15)
#define		HFA384x_INTEN_WTERR		0x4000 //((int)BIT14)
#define		HFA384x_INTEN_INFDROP	0x2000 //((int)BIT13)
#define		HFA384x_INTEN_INFO		0x0080 //((int)BIT7)
#define		HFA384x_INTEN_DTIM		0x0020 //((int)BIT5)
#define		HFA384x_INTEN_CMD		0x0010 //((int)BIT4)
#define		HFA384x_INTEN_ALLOC		0x0008 //((int)BIT3)
#define		HFA384x_INTEN_TXEXC		0x0004 //((int)BIT2)
#define		HFA384x_INTEN_TX		0x0002 //((int)BIT1)
#define		HFA384x_INTEN_RX		0x0001 //((int)BIT0)

#define		HFA384x_EVACK_TICK		0x8000 //((int)BIT15)
#define		HFA384x_EVACK_WTERR		0x4000 //((int)BIT14)
#define		HFA384x_EVACK_INFDROP	0x2000 //((int)BIT13)
#define		HFA384x_EVACK_INFO		0x0080 //((int)BIT7)
#define		HFA384x_EVACK_DTIM		0x0020 //((int)BIT5)
#define		HFA384x_EVACK_CMD		0x0010 //((int)BIT4)
#define		HFA384x_EVACK_ALLOC		0x0008 //((int)BIT3)
#define		HFA384x_EVACK_TXEXC		0x0004 //((int)BIT2)
#define		HFA384x_EVACK_TX		0x0002 //((int)BIT1)
#define		HFA384x_EVACK_RX		0x0001 //((int)BIT0)

#define		HFA384x_EVSTAT_TICK		0x8000 //((int)BIT15)
#define		HFA384x_EVSTAT_WTERR	0x4000 //((int)BIT14)
#define		HFA384x_EVSTAT_INFDROP	0x2000 //((int)BIT13)
#define		HFA384x_EVSTAT_INFO		0x0080 //((int)BIT7)
#define		HFA384x_EVSTAT_DTIM		0x0020 //((int)BIT5)
#define		HFA384x_EVSTAT_CMD		0x0010 //((int)BIT4)
#define		HFA384x_EVSTAT_ALLOC	0x0008 //((int)BIT3)
#define		HFA384x_EVSTAT_TXEXC	0x0004 //((int)BIT2)
#define		HFA384x_EVSTAT_TX		0x0002 //((int)BIT1)
#define		HFA384x_EVSTAT_RX		0x0001 //((int)BIT0)
 
/*--- Register ID macros ------------------------*/

#define		HFA384x_CMD			HFA384x_CMD_OFF
#define		HFA384x_PARAM0		HFA384x_PARAM0_OFF
#define		HFA384x_PARAM1		HFA384x_PARAM1_OFF
#define		HFA384x_PARAM2		HFA384x_PARAM2_OFF
#define		HFA384x_STATUS		HFA384x_STATUS_OFF
#define		HFA384x_RESP0		HFA384x_RESP0_OFF
#define		HFA384x_RESP1		HFA384x_RESP1_OFF
#define		HFA384x_RESP2		HFA384x_RESP2_OFF
#define		HFA384x_INFOFID		HFA384x_INFOFID_OFF
#define		HFA384x_RXFID		HFA384x_RXFID_OFF
#define		HFA384x_ALLOCFID	HFA384x_ALLOCFID_OFF
#define		HFA384x_TXCOMPLFID	HFA384x_TXCOMPLFID_OFF
#define		HFA384x_SELECT0		HFA384x_SELECT0_OFF
#define		HFA384x_OFFSET0		HFA384x_OFFSET0_OFF
#define		HFA384x_DATA0		HFA384x_DATA0_OFF
#define		HFA384x_SELECT1		HFA384x_SELECT1_OFF
#define		HFA384x_OFFSET1		HFA384x_OFFSET1_OFF
#define		HFA384x_DATA1		HFA384x_DATA1_OFF
#define		HFA384x_EVSTAT		HFA384x_EVSTAT_OFF
#define		HFA384x_INTEN		HFA384x_INTEN_OFF       /* Interrupt enable offset */
#define		HFA384x_EVACK		HFA384x_EVACK_OFF
#define		HFA384x_CONTROL		HFA384x_CONTROL_OFF
#define		HFA384x_SWSUPPORT0	HFA384x_SWSUPPORT0_OFF
#define		HFA384x_SWSUPPORT1	HFA384x_SWSUPPORT1_OFF
#define		HFA384x_SWSUPPORT2	HFA384x_SWSUPPORT2_OFF
#define		HFA384x_AUXPAGE		HFA384x_AUXPAGE_OFF
#define		HFA384x_AUXOFFSET	HFA384x_AUXOFFSET_OFF
#define		HFA384x_AUXDATA		HFA384x_AUXDATA_OFF
#define		HFA384x_PCICOR		HFA384x_PCICOR_OFF
#define		HFA384x_PCIHCR		HFA384x_PCIHCR_OFF

/*--- Register I/O offsets --------------------------*/
#define		HFA384x_CMD_OFF			(0x0000)
#define		HFA384x_PARAM0_OFF		(0x0002)
#define		HFA384x_PARAM1_OFF		(0x0004)
#define		HFA384x_PARAM2_OFF		(0x0006)
#define		HFA384x_STATUS_OFF		(0x0008)
#define		HFA384x_RESP0_OFF		(0x000A)
#define		HFA384x_RESP1_OFF		(0x000C)
#define		HFA384x_RESP2_OFF		(0x000E)
#define		HFA384x_INFOFID_OFF		(0x0010)
#define		HFA384x_RXFID_OFF		(0x0020)
#define		HFA384x_ALLOCFID_OFF	(0x0022)
#define		HFA384x_TXCOMPLFID_OFF	(0x0024)
#define		HFA384x_SELECT0_OFF		(0x0018)
#define		HFA384x_OFFSET0_OFF		(0x001C)
#define		HFA384x_DATA0_OFF		(0x0036)
#define		HFA384x_SELECT1_OFF		(0x001A)
#define		HFA384x_OFFSET1_OFF		(0x001E)
#define		HFA384x_DATA1_OFF		(0x0038)
#define		HFA384x_EVSTAT_OFF		(0x0030)
#define		HFA384x_INTEN_OFF		(0x0032)
#define		HFA384x_EVACK_OFF		(0x0034)
#define		HFA384x_CONTROL_OFF		(0x0014)
#define		HFA384x_SWSUPPORT0_OFF	(0x0028)
#define		HFA384x_SWSUPPORT1_OFF	(0x002A)
#define		HFA384x_SWSUPPORT2_OFF	(0x002C)
#define		HFA384x_AUXPAGE_OFF		(0x003A)
#define		HFA384x_AUXOFFSET_OFF	(0x003C)
#define		HFA384x_AUXDATA_OFF		(0x003E)

/*--- Support Constants -----------------------------*/
#define		HFA384x_BAP_0    			0x0000
#define		HFA384x_BAP_1				0x0001
#define		HFA384x_PORTTYPE_IBSS		0x0000
#define		HFA384x_PORTTYPE_BSS		0x0001	//((int)1)
#define		HFA384x_PORTTYPE_WDS		0x0002	//((int)2)
#define		HFA384x_PORTTYPE_PSUEDOIBSS	0x0003	//((int)3)
#define		HFA384x_PORTTYPE_HOSTAP    	0x0006	//((int)6)

/*--- Just some symbolic names for legibility -------*/
#define		HFA384x_TXCMD_NORECL		0x0000//((int)0)
#define		HFA384x_TXCMD_RECL		    0x0001//((int)1)

/*------ Constants --------------------------------------------*/
/*--- Mins & Maxs -----------------------------------*/
#define		HFA384x_BAP_OFFSET_MAX		0x1000 //((int)4096)
#define		HFA384x_BAP_DATALEN_MAX		0x1000 //((int)4096)
#define		HFA384x_CMD_ALLOC_LEN_MIN	0x0004 //((int)4)
#define		HFA384x_CMD_ALLOC_LEN_MAX	0x0960 //((int)2400)

#define		HFA384x_DRVR_TXBUF_MAX		(sizeof(hfa384x_tx_frame_t) + \
						WLAN_DATA_MAXLEN - \
						WLAN_WEP_IV_LEN - \
						WLAN_WEP_ICV_LEN + 2)
#define		HFA384x_DRVR_MAGIC		(0x4a2d)
#define		HFA384x_INFODATA_MAXLEN		(sizeof(hfa384x_infodata_t))
#define		HFA384x_INFOFRM_MAXLEN		(sizeof(hfa384x_InfFrame_t))

/*--- Record ID Constants --------------------------*/
/*--------------------------------------------------------------------
Configuration RIDs: Network Parameters, Static Configuration Entities
--------------------------------------------------------------------*/
#define		HFA384x_RID_CNFPORTTYPE		0xFC00
#define		HFA384x_RID_CNFOWNMACADDR	0xFC01
#define		HFA384x_RID_CNFDESIREDSSID	0xFC02
#define		HFA384x_RID_CNFOWNCHANNEL	0xFC03
#define		HFA384x_RID_CNFOWNSSID		0xFC04
#define		HFA384x_RID_CNFOWNATIMWIN	0xFC05
#define		HFA384x_RID_CNFSYSSCALE		0xFC06
#define		HFA384x_RID_CNFMAXDATALEN	0xFC07
#define		HFA384x_RID_CNFWDSADDR		0xFC08
#define		HFA384x_RID_CNFPMENABLED	0xFC09

#define		HFA384x_RID_CNFMAXSLEEPDUR	0xFC0C

/*--------------------------------------------------------------------
Information RIDs:  MAC Information
--------------------------------------------------------------------*/
#define		HFA384x_RID_PORTSTATUS		0xFD40
#define		HFA384x_RID_CURRENTSSID		0xFD41
#define		HFA384x_RID_CURRENTBSSID	0xFD42
#define		HFA384x_RID_COMMSQUALITY	0xFD43
#define		HFA384x_RID_CURRENTTXRATE	0xFD44

/*--------------------------------------------------------------------
Configuration RIDs: Network Parameters, Dynamic Configuration Entities
--------------------------------------------------------------------*/
#define		HFA384x_RID_TXRATECNTL		0xFC84

/*--------------------------------------------------------------------
Information RIDs:  Modem Information
--------------------------------------------------------------------*/
#define		HFA384x_RID_CURRENTPOWERSTATE	0xFDC2

/*--------------------------------------------------------------------
Configuration RID lengths: Network Params, Static Config Entities
  This is the length of JUST the DATA part of the RID (does not 
  include the len or code fields)
--------------------------------------------------------------------*/
/* TODO: fill in the rest of these */
#define		HFA384x_RID_CNFPORTTYPE_LEN	    0x0002//((int)2)
#define		HFA384x_RID_CNFOWNMACADDR_LEN	0x0006//((int)6)
#define		HFA384x_RID_CNFDESIREDSSID_LEN	0x0012//((int)34) --> 18 words = 0x12
#define		HFA384x_RID_CNFOWNCHANNEL_LEN	0x0002//((int)2)
#define		HFA384x_RID_CNFMAXDATALEN_LEN	0x0002
#define     HFA384x_RID_TXRATE_LEN          0x0002
#define		HFA384x_RID_CNFPMENABLED_LEN	0x0002
#define		HFA384x_RID_CNFMAXSLEEPDUR_LEN	0x0002

/*--------------------------------------------------------------------
Communication Frames: Field Masks for Transmit Frames
--------------------------------------------------------------------*/
/*-- Status Field --*/
#define		HFA384x_TXSTATUS_ACKERR			0x0020//((int)BIT5)
#define		HFA384x_TXSTATUS_FORMERR		0x0008//((int)BIT3)
#define		HFA384x_TXSTATUS_DISCON			0x0004//((int)BIT2)
#define		HFA384x_TXSTATUS_AGEDERR		0x0002//((int)BIT1)
#define		HFA384x_TXSTATUS_RETRYERR		0x0001((int)BIT0)
/*-- Transmit Control Field --*/
#define		HFA384x_TX_CFPOLL			    0x1000//((int)BIT12)
#define		HFA384x_TX_PRST				    0x0800//((int)BIT11)
#define		HFA384x_TX_MACPORT			    0x0700//((int)(BIT10 | BIT9 | BIT8))
#define		HFA384x_TX_NOENCRYPT			0x0080//((int)BIT7)
#define		HFA384x_TX_RETRYSTRAT			0x0060//((int)(BIT6 | BIT5))
#define		HFA384x_TX_STRUCTYPE			0x0018//((int)(BIT4 | BIT3))
#define		HFA384x_TX_TXEX				    0x0004//((int)BIT2)
#define		HFA384x_TX_TXOK				    0x0001//((int)BIT1)
/*--------------------------------------------------------------------
Communication Frames: Test/Get/Set Field Values for Transmit Frames
--------------------------------------------------------------------*/
/*-- Status Field --*/
#define HFA384x_TXSTATUS_ISERROR(v)	\
	(((int)(v))&\
	(HFA384x_TXSTATUS_ACKERR|HFA384x_TXSTATUS_FORMERR|\
	HFA384x_TXSTATUS_DISCON|HFA384x_TXSTATUS_AGEDERR|\
	HFA384x_TXSTATUS_RETRYERR))

#define	HFA384x_TXSTATUS_ISACKERR(v)	((int)(((int)(v)) & HFA384x_TXSTATUS_ACKERR))
#define	HFA384x_TXSTATUS_ISFORMERR(v)	((int)(((int)(v)) & HFA384x_TXSTATUS_FORMERR))
#define	HFA384x_TXSTATUS_ISDISCON(v)	((int)(((int)(v)) & HFA384x_TXSTATUS_DISCON))
#define	HFA384x_TXSTATUS_ISAGEDERR(v)	((int)(((int)(v)) & HFA384x_TXSTATUS_AGEDERR))
#define	HFA384x_TXSTATUS_ISRETRYERR(v)	((int)(((int)(v)) & HFA384x_TXSTATUS_RETRYERR))

#define	HFA384x_TX_GET(v,m,s)		((((int)(v))&((int)(m)))>>((int)(s)))
#define	HFA384x_TX_SET(v,m,s)		((((int)(v))<<((int)(s)))&((int)(m)))

#define	HFA384x_TX_CFPOLL_GET(v)	HFA384x_TX_GET(v, HFA384x_TX_CFPOLL,12)
#define	HFA384x_TX_CFPOLL_SET(v)	HFA384x_TX_SET(v, HFA384x_TX_CFPOLL,12)
#define	HFA384x_TX_PRST_GET(v)		HFA384x_TX_GET(v, HFA384x_TX_PRST,11)
#define	HFA384x_TX_PRST_SET(v)		HFA384x_TX_SET(v, HFA384x_TX_PRST,11)
#define	HFA384x_TX_MACPORT_GET(v)	HFA384x_TX_GET(v, HFA384x_TX_MACPORT, 8)
#define	HFA384x_TX_MACPORT_SET(v)	HFA384x_TX_SET(v, HFA384x_TX_MACPORT, 8)
#define	HFA384x_TX_NOENCRYPT_GET(v)	HFA384x_TX_GET(v, HFA384x_TX_NOENCRYPT, 7)
#define	HFA384x_TX_NOENCRYPT_SET(v)	HFA384x_TX_SET(v, HFA384x_TX_NOENCRYPT, 7)
#define	HFA384x_TX_RETRYSTRAT_GET(v)	HFA384x_TX_GET(v, HFA384x_TX_RETRYSTRAT, 5)
#define	HFA384x_TX_RETRYSTRAT_SET(v)	HFA384x_TX_SET(v, HFA384x_TX_RETRYSTRAT, 5)
#define	HFA384x_TX_STRUCTYPE_GET(v)	HFA384x_TX_GET(v, HFA384x_TX_STRUCTYPE, 3)
#define	HFA384x_TX_STRUCTYPE_SET(v)	HFA384x_TX_SET(v, HFA384x_TX_STRUCTYPE, 3)
#define	HFA384x_TX_TXEX_GET(v)		HFA384x_TX_GET(v, HFA384x_TX_TXEX, 2)
#define	HFA384x_TX_TXEX_SET(v)		HFA384x_TX_SET(v, HFA384x_TX_TXEX, 2)
#define	HFA384x_TX_TXOK_GET(v)		HFA384x_TX_GET(v, HFA384x_TX_TXOK, 1)
#define	HFA384x_TX_TXOK_SET(v)		HFA384x_TX_SET(v, HFA384x_TX_TXOK, 1)

/*--------------------------------------------------------------------
Communication Frames: Field Masks for Receive Frames
--------------------------------------------------------------------*/
/*-- Offsets --------*/
#define		HFA384x_RX_DATA_LEN_OFF			0x002c//((int)44)
#define		HFA384x_RX_80211HDR_OFF			0x000E//((int)14)
#define		HFA384x_RX_DATA_OFF				0x003c//((int)60)

/*-- Status Fields --*/
#define		HFA384x_RXSTATUS_MSGTYPE		0xE000//((int)(BIT15 | BIT14 | BIT13))
#define		HFA384x_RXSTATUS_MACPORT		0x0700//((int)(BIT10 | BIT9 | BIT8))
#define		HFA384x_RXSTATUS_UNDECR			0x0002//((int)BIT1)
#define		HFA384x_RXSTATUS_FCSERR			0x0001//((int)BIT0)
/*--------------------------------------------------------------------
Communication Frames: Test/Get/Set Field Values for Receive Frames
--------------------------------------------------------------------*/
#define		HFA384x_RXSTATUS_MSGTYPE_GET(value)	((int)((((int)(value)) & HFA384x_RXSTATUS_MSGTYPE) >> 13))
#define		HFA384x_RXSTATUS_MSGTYPE_SET(value)	((int)(((int)(value)) << 13))
#define		HFA384x_RXSTATUS_MACPORT_GET(value)	((int)((((int)(value)) & HFA384x_RXSTATUS_MACPORT) >> 8))
#define		HFA384x_RXSTATUS_MACPORT_SET(value)	((int)(((int)(value)) << 8))
#define		HFA384x_RXSTATUS_ISUNDECR(value)	((int)(((int)(value)) & HFA384x_RXSTATUS_UNDECR))
#define		HFA384x_RXSTATUS_ISFCSERR(value)	((int)(((int)(value)) & HFA384x_RXSTATUS_FCSERR))

/*--------------------------------------------------------------------
 FRAME STRUCTURES: Information Types 
----------------------------------------------------------------------
Information Types
--------------------------------------------------------------------*/
#define		HFA384x_IT_HANDOVERADDR				0xF000
#define		HFA384x_IT_HANDOVERDEAUTHADDRESS	0xF001
#define		HFA384x_IT_COMMTALLIES				0xF100
#define		HFA384x_IT_SCANRESULTS				0xF101
#define		HFA384x_IT_CHINFORESULTS			0xF102
#define		HFA384x_IT_HOSTSCANRESULTS			0xF103
#define		HFA384x_IT_LINKSTATUS				0xF200


/* external variables */
extern char xdata cis_device;
extern char xdata cor_data;
extern unsigned int xdata cor_addr;

/* Function prototypes wifidev*/
extern void CFWriteIOByte(char val, unsigned int reg);
extern char CFReadIOByte(unsigned int reg);
extern void CFSetAddr(unsigned int CFaddr);
extern void CFWriteMemByte(char val, unsigned int reg);
extern char CFReadMemByte(unsigned int reg);

extern void delay_nops(int value);
extern bit CFConnected(void);

void hfa384x_setreg(int val, unsigned int reg);
int hfa384x_getreg(unsigned int reg);
int hfa384x_getdata(unsigned int datareg);
void hfa384x_setdata(int val, unsigned int datareg);

//void hfa384x_copy_to_bap(bit bapbuf, char* buff_data, unsigned int count);
char hfa384x_cmd(unsigned int cmd, unsigned int parm0);
char hfa384x_cmd_allocate(unsigned int len);
char hfa384x_alloc_tx_buff(void);
char hfa384x_alloc_info_buff(void);
char hfa384x_cmd_diagnose(void);

char get_free_bap(void);
void hfa384x_cmd_access(bit write, unsigned int id, unsigned int* buf, int len);
char hfa384x_rdwr_bap(bit write, int id, unsigned int offset, 
		       char* buf, int len);
void ReadCISTable(void);
void hfa384x_drvr_start(void);
void uIPwifi_api(unsigned int cmd, unsigned int val);
void config_chip(void);
void wifidev_init(void); 
unsigned int wifidev_read(void);
void hfa384x_event_ack(void);
void wifidev_send(void);
void find_CF_mode(void);
/*
ANSWER

You must use the optional characters B or b and L or l immediately preceding the 
type character to respectively specify char or long versions of the integer types d, i ,u, o, x and X. 

The following printf correctly prints 1, 2, and 3 as integers: 
printf ("%d %d %d", (int) 1, (int) 2, (int) 3);

The following printf correctly prints 1, 2, and 3 as long integers: 
printf ("%Ld %Ld %Ld", 1L, 2L, 3L);

The following printf correctly prints 1, 2, and 3 as chars: 
printf ("%Bd %Bd %Bd", (char) 1, (char) 2, (char) 3);

The type casts on the numeric constants are important because they specify the number of bytes 
required for each numeric constant. 
*/
