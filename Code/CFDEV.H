/********************************************************************
* Filename: cfdev.c
* Description: This file implements the header file compact flash 
*              functions in IO mode 
* Developed by: Marco Carvalho
* Created on: 30/03/05
/*********************************************************************/


#ifndef __COMPACT_FLASH_H	
#define __COMPACT_FLASH_H

//defines

//CF memory registers
#define CF_IO		0x00	//registro para IO
#define CF_FEATURES	0x01	//registro de Errors (Out) e Features (In)
#define CF_ERROR	0x01	//registro de Errors (Out) e Features (In)
#define CF_SECCOUNT	0x02	//numero de setores transferidos
#define CF_LBA0		0x03	//LBA 0-7
#define CF_LBA1		0x04	//LBA 8-15
#define CF_LBA2		0x05	//LBA 16-23
#define CF_LBA3		0x06	//LBA 24-27 -> LBA bit = 1
#define CF_STACOM	0x07	//registro de Status (Out) e Command (In)

//CF memory Comands
#define CF_READ_SEC	0x20	//read
#define CF_WRITE_SEC    0x30	//write
#define CF_DRIVE_DIAG   0x90 
#define CF_ID_DRIVE  0xec // identify drive


#define CF_STACOM_ERROR_MASK   0x01
#define CF_STACOM_DRQ_OK_MASK  0x58 //D6=RDY=1 D4=DSC=1 D3=DRQ=1
#define CF_STACOM_DRQ_MASK     0xF8  
#define CF_STACOM_BUSY_MASK    0x80
#define CF_STACOM_BUSY_OK_MASK 0x00  //D8=0 
#define CF_STACOM_RDY_MASK     0xF0
#define CF_STACOM_RDY_OK_MASK  0x50 // D6=RDY=1 e D4=DSC=1

#define CF_WAIT_DRQ()  CFWait(CF_STACOM_DRQ_MASK, CF_STACOM_DRQ_OK_MASK)
#define CF_WAIT_RDY()  CFWait(CF_STACOM_RDY_MASK, CF_STACOM_RDY_OK_MASK)
#define CF_WAIT_BUSY() CFWait(CF_STACOM_BUSY_MASK, CF_STACOM_BUSY_OK_MASK)

// registers

#define RD_DATA 0x0
#define WR_DATA 0x0
#define ERROR_REG 0x1
#define FEATURES 0x1
#define SECTOR_CNT 0x2
#define SECTOR_NO 0x3
#define CYLINDER_LO 0x4
#define CYLINDER_HI 0x5
#define DRIVE_HEAD 0x6
#define STATUS 0x7
#define COMMAND 0x7
#define ALT_STATUS 0xe
#define DEVICE_CTRL 0xe
#define DRIVE_ADDRESS 0xf

// register bits

#define BAD_BLOCK 0x80
#define UNCORRECTABLE 0x40
#define SECTOR_ID_ERROR 0x10
#define ABORT 0x04
#define GENERAL_ERROR 0x01

#define LBA 0x40
#define DRIVE_NO 0x10

#define BUSY 0x80
#define READY 0x40
#define WRITE_FAULT 0x20
#define CARD_READY 0x10
#define DATA_REQUEST 0x08
#define CORRECTABLE_ERROR 0x04
#define ERROR 0x01

#define BYTE_PER_SEC 512

// CF registers

#define COR_REG 0x0200
#define SOCKET_COPY_REG 0x0206
//#define SOCKET_COPY_REG cor_addr+6


// COR register values

#define MEMORY_MAPPED 0
#define IO_MAPPED 1

/* external variables */
extern unsigned long xdata CFNumSectors;

/* Function Prototypes CF */
unsigned char CFSendCommand(unsigned char cmd, unsigned long address,unsigned char nSectors);

bit CFReadSector(unsigned long address);
bit CFWriteSector(unsigned long address);

unsigned char CFWait(unsigned char cf_stacom_mask, unsigned char cf_stacom_ok_mask);
char CFIDDrive(void);
void CFWriteCommonMemByte(char val, unsigned int reg);
char CFReadCommonMemByte(unsigned int reg);

/* end Function prototypes CF memory */


#endif /* COMPACT_FLASH_H */
