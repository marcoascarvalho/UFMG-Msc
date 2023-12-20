/*============================================================================
Arquivo: asic.h
Descricao: Arquivo cabecalho   
Elaborado por: Fabio Lucio
Data: 24 Jun 2003
=============================================================================*/
/*============================================================================
			      Includes
=============================================================================*/
/*============================================================================
			      Definicoes
=============================================================================*/
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
#define DATA_EMD  0x04 // configura 5 lead mode e liga drivers
#define DATA_LDR  0x00 // **************** testar outros valores
#define DATA_EOS  0x00 // **************** testar outros valores
#define DATA_PDL  0x80 // **************** testar outros valores
#define DATA_EST  0x00 // **************** ECG auto test nao usar 
#define DATA_RSP  0x00 // **************** testar outros valores

#define MAX_BIT_COUNT   8    /* numero maximo de bits a ser transmitido ou recebido*/
/*============================================================================
			      Variaveis Globais externas
=============================================================================*/
 
/*============================================================================
			      Prototipos
=============================================================================*/
void ini_asic(void);
void esc_asic(char add_reg, char dado);
char le_asic(char add_reg);

