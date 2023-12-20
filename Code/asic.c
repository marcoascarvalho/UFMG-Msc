/*============================================================================
Arquivo: asic.c
Descricao: Este modulo contem funcoes do CI ECG ASIC.    
Elaborado por: Fabio Lucio
Data: 2 Jun 2003
=============================================================================*/
/*============================================================================
			      Includes
=============================================================================*/
#include "asic.h"
/*============================================================================
			      Definicoes
=============================================================================*/
#if 0
/* Enderecos dos registros de escrita do CI ASIC */
#define WRITE_EMD  0xF0
#define WRITE_LDR  0xF1
#define WRITE_EOS  0xF2
#define WRITE_PDL  0xF3
#define WRITE_EST  0xF5
#define WRITE_RSP  0xF6
/* Enderecos dos registros de leitura do CI ASIC */ 
#define READ_EMD  0x00
#define READ_LDR  0x01
#define READ_EOS  0x02
#define READ_PDL  0x03
#define READ_LFS  0x04
#define READ_EST  0x05
#define READ_RSP  0x06 
/* Dados a serem escritos nos registros de leitura do CI ASIC */ 
#define DATA_EMD  0x0C // configura 5 lead mode e liga drivers
#define DATA_LDR  0x00 // **************** testar outros valores
#define DATA_EOS  0x00 // **************** testar outros valores
#define DATA_PDL  0x01 // **************** testar outros valores
#define DATA_EST  0x00 // **************** ECG auto test nao usar 
#define DATA_RSP  0x00 // **************** testar outros valores

#define MAX_BIT_COUNT   8    /* numero maximo de bits a ser transmitido ou recebido*/
#endif
/*===========================================================================
			      Variaveis Globais Externas
=============================================================================*/
 
/*============================================================================
			      Prototipos
=============================================================================*/
void esc_asic(char add_reg,char dado);
/*============================================================================
			      Funcoes
=============================================================================*/
/*---------------------------------------------------------------------ini_asic                                                                                                                      
Descricao     : Rotina de iniciacao do ECG ASIC
Parametros    : nenhum
Retorna       : nenhum         
Comentarios   : 
----------------------------------------------------------------------------*/ 
void ini_asic(void){
   /* escreve no registro 
   EMD- liga ASIC*/
   esc_asic(WRITE_EMD,DATA_EMD);
  
   /* escreve no registro 
   LDR*/
   esc_asic(WRITE_LDR,DATA_LDR);  

   /* escreve no registro 
   EOS*/
   esc_asic(WRITE_EOS,DATA_EOS);  
 
   /* escreve no registro 
   PDL*/
   esc_asic(WRITE_PDL,DATA_PDL);  
 
   /* escreve no registro 
   RSP*/
   esc_asic(WRITE_RSP,DATA_RSP); 
}
/*---------------------------------------------------------------------esc_asic                                                                                                                     
Descricao     : Gera o ciclo de escrita para o asic
Parametros    : endereco e Byte a ser transmitido pela serial emulada
Retorna       : nenhum         
Comentarios   : Serializa os bytes de entrada transmitindo primeiramente o MSB. 
----------------------------------------------------------------------------*/ 
void esc_asic(char add_reg, char dado)
{
   char xdata i;
   bit get_bit;

   CS_ASIC = 1;       // levanta o pino de chip select
   CLK = 1;           // levanta pino de clock
   delay(10);         // +/- 200 ns
   CS_ASIC = 0;       // abaixa o pino de chip select
   delay(10);         // +/- 200 ns
   /* trasmite o comando e endereco do registro*/
   for(i=0;i<MAX_BIT_COUNT;i++)
   {
      get_bit = (add_reg&(0x01<<(7-i))); //transmite MSB primeiro
      DOUT = get_bit;
      delay(10);         // +/- 200 ns
	  CLK = 0;      // toggle pino de clock 
      delay(10);         // +/- 900 ns
 	  CLK =  1;     // toggle pino de clock
      delay(10);         // +/- 900 ns 
   }
   /* trasmite o dado a ser escrito*/
   for(i=0;i<MAX_BIT_COUNT;i++)
   {
      get_bit = (dado&(0x01<<(7-i))); //transmite MSB primeiro
      DOUT = get_bit;
	  delay(10);         // +/- 200 ns
	  CLK = 0;      // toggle pino de clock 
      delay(10);         // +/- 900 ns
 	  CLK =  1;     // toggle pino de clock
      delay(10);         // +/- 900 ns  
   }
   CS_ASIC = 1;       // levanta o pino de chip select
}
 
/*---------------------------------------------------------------------le_asic                                                                                                                     
Descricao     : Gera o ciclo de leitura para o asic
Parametros    : endereco do registro a ser lido pela serial emulada
Retorna       : nenhum         
Comentarios   : Serializa os bytes de entrada transmitindo primeiramente o MSB. 
----------------------------------------------------------------------------*/ 
char le_asic(char add_reg){
   char xdata i,dado=0;
   bit get_bit;

   CS_ASIC = 1;       // levanta o pino de chip select
   CLK = 1;           // levanta pino de clock
   delay(10);         // +/- 200 ns
   CS_ASIC = 0;       // abaixa o pino de chip select
   delay(10);         // +/- 200 ns
   /* trasmite o comando e endereco do registro*/
   for(i=0;i<MAX_BIT_COUNT;i++){
      get_bit = (add_reg&(0x01<<(7-i))); //transmite MSB primeiro
      DOUT = get_bit;
      delay(10);         // +/- 200 ns
	  CLK = 0;      // toggle pino de clock 
      delay(10);         // +/- 900 ns
 	  CLK =  1;     // toggle pino de clock
      delay(10);         // +/- 900 ns 
   }
   /* le o dado do ASIC*/
   for (i=0;i<MAX_BIT_COUNT;i++){
      CLK = 0;      // abaixa pino de clock
 	  delay(10);    // aguarda dado estabilizar
      dado = dado << 1 ;
	  dado |= DIN ;	
 	  delay(10); 
      CLK = 1;      // levanta pino de clock
	  delay(10); 
   }
   CS_ASIC = 1;       // levanta o pino de chip select
   return(dado);
}
 
