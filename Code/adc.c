/*============================================================================
Arquivo: ini.c
Descricao: Este modulo contem as funcoes relativas ao ADC 7718 da Analog Devices 
Elaborado por: Fabio Lucio
Data: 2 Jun 2003
=============================================================================*/
/*===========================================================================
                            Descricao:         

            O ADC 7718 possui os seguintes registros internos :

               A3 A2 A1 A0 Registros
 
               0  0  0  0  Communications Register during a Write Operation
               0  0  0  0  Status Register during a Read Operation
               0  0  0  1  Mode Register
               0  0  1  0  ADC Control Register
               0  0  1  1  Filter Register
               0  1  0  0  ADC Data Register
               0  1  0  1  ADC Offset Register
               0  1  1  0  ADC Gain Register
               0  1  1  1  I/O C ontrol Register
               1  0  0  0  Undefined
               1  0  0  1  Undefined
               1  0  1  0  Undefined
               1  0  1  1  Undefined
               1  1  0  0  Test 1 Register
               1  1  0  1  Test 2 Register
               1  1  1  0  Undefined
               1  1  1  1  ID Register
           
          A configuracao dos port do ADU834 que controla a serial do ADC
          é a seguinte :
  
          P2.7 ADC CS\
          P1.7 ADCout (in 8051)
          P3.6 ADCin   (out 8051)
          P3.7 ADCsclk (out 8051)
          P3.3 ADCrdy\ (int1 ou in 8051)
=============================================================================*/
/*============================================================================
			      Includes
=============================================================================*/
#include <stdio.h>
#include "adc.h"
/*============================================================================
			      Definicoes
=============================================================================*/
#define IOCOM             0x30  /* IO Control Register*/
#define FILTER_REGISTER   0x0D  /* Filter Register*/
#define ADCCON            0x07  /* ADC Control Register*/
#define ADC_MODE          0x03  /* ADC Control Register*/

#define END_IOCOM             0x07  /* Endereco IO Control Register*/
#define END_FILTER_REGISTER   0x03  /* Endereco Filter Register*/
#define END_ADCCON            0x02  /* Endereco ADC Control Register*/
#define END_ADC_MODE          0x01  /* Endereco ADC Control Register*/
#define END_ID_REG            0x4F  /* Endereco ID Register*/

#define MAX_COUNT       1000 /* numero maximo de iteracoes para o ADC ready (RDY)*/
#define MAX_RST_COUNT   40   /* numero maximo de iteracoes para o loop de iniciacao do ADC*/
#define MAX_BIT_COUNT   8    /* numero maximo de bits a ser transmitido ou recebido*/
/*============================================================================
			      Variaveis Globais Externas
=============================================================================*/
 
/*============================================================================
			      Prototipos
=============================================================================*/
/*============================================================================
			      Funcoes
=============================================================================*/
void delay(char periodo){
   while (periodo >=0) 
      periodo--;
}


/*---------------------------------------------------------------------ini_adc                                                                                                                       
Descricao     : Rotina de iniciacao do conversor ADC
Parametros    : nenhum
Retorna       : nenhum         
Comentarios   : 
----------------------------------------------------------------------------*/ 
void ini_adc(void)
{
   rst_adc();
   /* configura os registros do ADC*/
   /* escreve no communication register configurando next para 
   IOCOM*/
   CS_ADC = 0;   // abaixa o pino de chip select
   esc_adc(END_IOCOM); 
   esc_adc(IOCOM); /* pinos de IO configurado como saida e igual a zero*/
   CS_ADC = 1;   // levanta o pino de chip select
   
   /* escreve no communication register configurando next para 
   FILTER_REGISTER*/
   CS_ADC = 0;   // abaixa o pino de chip select
   esc_adc(END_FILTER_REGISTER); 
   esc_adc(FILTER_REGISTER);  
   CS_ADC = 1;   // levanta o pino de chip select
   
   /* escreve no communication register configurando next para 
   ADCCON*/
   CS_ADC = 0;   // abaixa o pino de chip select
   esc_adc(END_ADCCON); 
   esc_adc(ADCCON); 
   CS_ADC = 1;   // levanta o pino de chip select

   /* escreve no communication register configurando next para 
   ADC_MODE*/
   CS_ADC = 0;   // abaixa o pino de chip select
   esc_adc(END_ADC_MODE); 
   esc_adc(ADC_MODE);
   CS_ADC = 1;   // levanta o pino de chip select
}

/*---------------------------------------------------------------------rst_adc                                                                                                                      
Descricao     : Rotina de reset do conversor ADC
Parametros    : nenhum
Retorna       : nenhum         
Comentarios   : Para resetar o ADC eh preciso gerar 40 ciclos de clock com 
                o pino Dout em '1'.
----------------------------------------------------------------------------*/ 
void rst_adc(void){
   char xdata i;
/* iniciacao dos pinos dos ports para o ADC8*/ 
   CS_ADC = 1;   // levanta o pino de chip select 
   CLK = 1;      // levanta pino de clock
   CS_ADC = 0;   // abaixa o pino de chip select
   DOUT = 1;     // mantem sinal DIN alto   
   for(i=0;i<MAX_RST_COUNT;i++){
      CLK = 0;   // toggle pino de clock
	  CLK = 1;
   }
   CS_ADC = 1;   // abaixa o pino de chip select
}

/*---------------------------------------------------------------------esc_adc                                                                                                                     
Descricao     : Gera o ciclo de ecrista  do ADC
Parametros    : Byte a ser transmitido pela serial emulada
Retorna       : nenhum         
Comentarios   : Serializa o byte de entrada transmitindo primeiramente o MSB. 
----------------------------------------------------------------------------*/ 
void esc_adc(char dado)
{
   char xdata i;
   bit get_bit;
   CLK = 1;   // levanta pino de clock
   for(i=0;i<MAX_BIT_COUNT;i++){
      get_bit = (dado&(0x01<<(7-i))); //transmite MSB primeiro
      DOUT = get_bit;
	  CLK = 0;      // toggle pino de clock 
 	  CLK =  1;     // toggle pino de clock  
   }
}
 
/*---------------------------------------------------------------------le_ID_adc                                                                                                                      
Descricao     : Rotina de leitura do codigo de identificacao do ADC 
Parametros    : nenhum
Retorna       : nenhum         
Comentarios   : 
----------------------------------------------------------------------------*/
char le_ID_adc(void)
{
   char xdata i,j=0,dado=0;

   erro_adc = FALSE; /* inicializa flag de erro*/ 
   while (MAX_COUNT){
      j++;
      if (!RDY){
	     CS_ADC = 0;   // abaixa o pino de chip select
		 esc_adc(END_ID_REG); 
         for (i=0;i<MAX_BIT_COUNT;i++){
	        CLK = 0;      // abaixa pino de clock
 	        delay(10);    // aguarda dado estabilizar
		    dado = dado << 1 ;
		    dado |= DIN ;	 
            CLK = 1;      // levanta pino de clock
         }
		 CS_ADC = 1;       // levanta o pino de chip select
		 return(dado);
      }         
   }
   erro_adc = TRUE;
   rst_adc();             // reseta o ADC em caso de falha
} 
 
/*---------------------------------------------------------------------adc_dado                                                                                                                     
Descricao     : Rotina de leitura do Data register do ADC 
Parametros    : nenhum
Retorna       : nenhum         
Comentarios   :  recebe 8 bits por iteracao ( total 24 bits)
                 ; .. ver det da serial pag.11 datasheet Analog Devices
----------------------------------------------------------------------------*/
char adc_dado(void)
{
   char xdata *amostra,i,k,j=0,dado=0;

   amostra = adc_amostra;
   esc_adc(END_ID_REG); 
   while (j<MAX_COUNT){
      j++;
      if (!RDY){
         CS_ADC = 0;       // abaixa o pino de chip select
         for (i=0;i<(MAX_BIT_COUNT-5);i++) { 
            for (k=0;k<MAX_BIT_COUNT;k++){
	           CLK = 0;      // abaixa pino de clock
 	           //delay(10);    // aguarda dado estabilizar
		       dado = dado << 1 ;
		       dado |= DIN ;	 
               CLK = 1;      // levanta pino de clock
            }
            *(amostra++) = dado;

         }
         CS_ADC = 1;       // levanta o pino de chip select
         return(amostra);
      }         
   }   
   rst_adc();             // reseta o ADC em caso de falha*/ 
   return(FALSE);          
} 








