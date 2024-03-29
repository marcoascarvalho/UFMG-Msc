
/* config.h
 Description: This file set the 8051 microcontroller registers
 Created by: Marco Carvalho
 Last revised: 02/01/05
*/

sfr P0	= 0x80;
sfr SP	= 0x81;
sfr DPL	= 0x82;
sfr DPH	= 0x83;
sfr DPP	= 0x84;
sfr PCON	= 0x87;
sfr TCON	= 0x88;
sfr TMOD	= 0x89;
sfr TL0	= 0x8A;
sfr TL1	= 0x8B;
sfr TH0	= 0x8C;
sfr TH1	= 0x8D;
sfr P1	= 0x90;
sfr SCON	= 0x98;
sfr SBUF	= 0x99;
sfr T3FD	= 0x9D;
sfr T3CON	= 0x9E;
sfr P2	= 0xA0;
sfr TIMECON = 0xA1;
sfr HTHSEC  = 0xA2;
sfr SEC     = 0xA3;
sfr MIN	    = 0xA4;
sfr HOUR	= 0xA5;
sfr INTVAL	= 0xA6;
sfr DPCON	= 0xA7;
sfr IE	= 0xA8;
sfr IEIP2	= 0xA9;
sfr PWCON	= 0xAE;
sfr CFG834	= 0xAF;
sfr P3	= 0xB0;
sfr PWM0L	= 0xB1;
sfr PWM0H	= 0xB2;
sfr PWM1L	= 0xB3;
sfr PWM1H	= 0xB4;
sfr SPH	= 0xB7;
sfr IP	= 0xB8;
sfr ECON    = 0xB9;
sfr EDATA1  = 0xBC;
sfr EDATA2  = 0xBD;
sfr EDATA3  = 0xBE;
sfr EDATA4  = 0xBF;
sfr WDCON   = 0xC0;
sfr CHIPID  = 0xC2;
sfr EADRL   = 0xC6;
sfr EADRH   = 0xC7;
sfr T2CON	= 0xC8;
sfr RCAP2L	= 0xCA;
sfr RCAP2H	= 0xCB;
sfr TL2	= 0xCC;
sfr TH2	= 0xCD;
sfr PSW	= 0xD0;
sfr ADCMODE  = 0xD1;
sfr ADC0CON	= 0xD2;
sfr ADC1CON	= 0xD3;
sfr SF	= 0xD4;
sfr ICON	= 0xD5;
sfr PLLCON	= 0xD7;
sfr ADCSTAT	= 0xD8;
sfr ADC0L	= 0xD9;
sfr ADC0M	= 0xDA;
sfr ADC0H	= 0xDB;
sfr ADC1L	= 0xDC;
sfr ADC1H	= 0xDD;
sfr PSMCON  = 0xDF;
sfr ACC	= 0xE0;
sfr OF0L	= 0xE1;
sfr OF0M	= 0xE2;
sfr OF0H	= 0xE3;
sfr OF1L	= 0xE4;
sfr OF1H	= 0xE5;
sfr HCOPCON	= 0xE8;
sfr GN0L	= 0xE9;
sfr GN0M	= 0xEA;
sfr GN0H	= 0xEB;
sfr GN1L	= 0xEC;
sfr GN1H	= 0xED;
sfr B		= 0xF0;
sfr SPIDAT	= 0xF7;
sfr SPICON	= 0xF8;
sfr DACL	= 0xFB;
sfr DACH	= 0xFC;
sfr DACCON	= 0xFD;
/*============================================================================
			     Registros do ADU834 (BITS) 
=============================================================================*/
/*  PSW  */
sbit CY    = PSW^7;
sbit AC    = PSW^6;
sbit F0    = PSW^5;
sbit RS1   = PSW^4;
sbit RS0   = PSW^3;
sbit OV    = PSW^2;
sbit P     = PSW^0;

/*  TCON  */
sbit TF1   = TCON^7;
sbit TR1   = TCON^6;
sbit TF0   = TCON^5;
sbit TR0   = TCON^4;
sbit IE1   = TCON^3;
sbit IT1   = TCON^2;
sbit IE0   = TCON^1;
sbit IT0   = TCON^0;

/*  IE  */
sbit EA    = IE^7;
sbit EADC  = IE^6;
sbit ET2   = IE^5;
sbit ES    = IE^4;
sbit ET1   = IE^3;
sbit EX1   = IE^2;
sbit ET0   = IE^1;
sbit EX0   = IE^0;

/*  IP  */
//sbit PS1   = IP^7;
sbit PADC  = IP^6;
sbit PT2   = IP^5;
sbit PS    = IP^4;
sbit PT1   = IP^3;
sbit PX1   = IP^2;
sbit PT0   = IP^1;
sbit PX0   = IP^0;

/*  SCON  */
sbit SM0   = SCON^7;
sbit SM1   = SCON^6;
sbit SM2   = SCON^5;
sbit REN   = SCON^4;
sbit TB8   = SCON^3;
sbit RB8   = SCON^2;
sbit TI    = SCON^1;
sbit RI    = SCON^0;
             
/*  T2CON  */
sbit TF2    = T2CON^7;
sbit EXF2   = T2CON^6;
sbit RCLK   = T2CON^5;
sbit TCLK   = T2CON^4;
sbit EXEN2  = T2CON^3;
sbit TR2    = T2CON^2;
sbit CNT2   = T2CON^1;
sbit CAP2	= T2CON^0;

/*  WDCON  */
sbit PRE3	= WDCON^7;
sbit PRE2	= WDCON^6;
sbit PRE1	= WDCON^5;
sbit PRE0	= WDCON^4;
sbit WDIR	= WDCON^3;
sbit WDS	= WDCON^2;
sbit WDE	= WDCON^1;
sbit WDWR	= WDCON^0;

/*  ADCSTAT  */
sbit RDY0	= ADCSTAT^7;
sbit RDY1	= ADCSTAT^6;
sbit CAL	= ADCSTAT^5;
sbit NOXREF	= ADCSTAT^4;
sbit ERR0	= ADCSTAT^3;
sbit ERR1	= ADCSTAT^2;

/*  HCOPCON  */
sbit HCOP1	= HCOPCON^7;
sbit HCOP1EN= HCOPCON^6;
sbit HCOP0	= HCOPCON^5;
sbit HCOP0EN= HCOPCON^3;

/*  SPICON  */
sbit ISPI	= SPICON^7;
sbit WCOL	= SPICON^6;
sbit SPE	= SPICON^5;
sbit SPIM	= SPICON^4;
sbit CPOL	= SPICON^3;
sbit CPHA	= SPICON^2;
sbit SPR1	= SPICON^1;
sbit SPR0	= SPICON^0;	

/* PSMCON */
/*sbit CMPD	= PSMCON^7;
sbit CMPA	= PSMCON^6;
sbit PSMI	= PSMCON^5;
sbit TPD1	= PSMCON^4;
sbit TPD0	= PSMCON^3;
sbit TPA1	= PSMCON^2;
sbit TPA0	= PSMCON^1;
sbit PSMEN	= PSMCON^0;	*/

/*  TIMECON */
/*sbit ITS1	= TIMECON^5;
sbit ITS0	= TIMECON^4;
sbit STI	= TIMECON^3;
sbit TII	= TIMECON^2;
sbit TIEN	= TIMECON^1;
sbit TCEN	= TIMECON^0;*/

/* Definicao dos PORTS- ver diagrama eletrico */

/*  P0  */
sbit AD7   = P0^7; 
sbit AD6   = P0^6;
sbit AD5   = P0^5;
sbit AD4   = P0^4; 
sbit AD3   = P0^3;
sbit AD2   = P0^2;
sbit AD1   = P0^1;  
sbit AD0   = P0^0; 

/*  P1  */
sbit DIN   = P1^7; 
sbit PDET0 = P1^6;
//sbit RDY   = P1^5;
sbit WAIT  = P1^5;
sbit CD1   = P1^4; 
sbit LDCH  = P1^3;
//sbit RXD1  = P1^2;
//sbit WR = P1^2;
//sbit T2EX  = P1^1;
sbit IOWR  = P1^1;
//sbit T2	   = P1^0;
sbit IORD  = P1^0;
 
/*  P2  */
//sbit CS_ADC  = P2^7; 
sbit A5      = P2^7; 
//sbit CS_DSK  = P2^6;  // original da placa
sbit A4      = P2^6;
sbit WR      = P2^5;   
sbit RD      = P2^4; 
sbit A0      = P2^3;
sbit A1      = P2^2;
sbit A2      = P2^1;  
sbit A3      = P2^0;  

/*  P3  */
sbit A8       = P3^7;
sbit A7      = P3^6; 
//sbit CS_DSP    = P3^5;
sbit REG       = P3^5;
//sbit CS_ASIC   = P3^4;
//sbit CS_DSK    = P3^4;  // modificado por fio
sbit RESET       = P3^4;  // trocado nao precisa de CE1
//sbit TXD1      = P3^3;
sbit A6      = P3^3;
sbit LED2      = P3^2;
sbit TXD       = P3^1; // serial
sbit RXD       = P3^0; // serial





