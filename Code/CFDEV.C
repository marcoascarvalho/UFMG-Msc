/********************************************************************
* Filename: cfdev.c
* Description: This file implements the compact flash functions for
				CF mode
* Developed by: Marco Carvalho
* Created on: 10/03/05
/*********************************************************************/

#include "cfdev.h"
#include "wifidev.h"
#include <ctype.h>
//#include "main.h"
#include <stdio.h>

unsigned char xdata CFDrvNumber=0;
unsigned long int xdata CFNumSectors=0;

//unsigned long xdata maxsect;

unsigned char CFSendCommand(unsigned char cmd, unsigned long address,unsigned char nSectors)
{
	unsigned char tmp_data;

	CF_WAIT_BUSY();
	/* Set the drive head */
	CFWriteCommonMemByte((0xe0|CFDrvNumber)|(0xf&(address>>24)),DRIVE_HEAD);
	/* Set cylinder high */
	CFWriteCommonMemByte(0xff&(address>>16),CYLINDER_HI);
	/* Set cylinder low */
    CFWriteCommonMemByte(0xff&(address>>8),CYLINDER_LO);
	/* Set sector number */
    CFWriteCommonMemByte(0xff&address,SECTOR_NO);
    /* Set sector quantity */
	CFWriteCommonMemByte(nSectors,SECTOR_CNT);
    /* Set command */
	CFWriteCommonMemByte(cmd,COMMAND);
    
	CF_WAIT_BUSY();
	CF_WAIT_DRQ();
	tmp_data = CF_WAIT_BUSY();
	if (tmp_data)
	{
		printf("Erro no registrador de comandos \n");
		return 1;
	}
	else
		return 0;
		
} 

/*----------------------------------------------------------------
* CFReadSector(unsigned long address)
*
* Read one sector from CF card
* The sector read will be found in uip_buf after call this 
* function
----------------------------------------------------------------*/
bit CFReadSector(unsigned long address)
{
	unsigned int xdata i;

	/* send Read sector command */
	if(CFSendCommand(CF_READ_SEC,address,1)) 
		return 1;

	/* read one sector */
	for(i=0;i<BYTE_PER_SEC;i++) 
		uip_buf[i]= CFReadCommonMemByte(RD_DATA);
		
	return 0;
}

/*----------------------------------------------------------------
* CFWriteSector(unsigned long address)
*
* The sector array to be write must be placed in uip_buf before 
* call this function
----------------------------------------------------------------*/
bit CFWriteSector(unsigned long address)
{
	unsigned int xdata i;

	/* send write sector command */
	if(CFSendCommand(CF_WRITE_SEC,address,1)) 
  		return 1;

    /* write one sector = 512 bytes */
	for(i=0;i<BYTE_PER_SEC;i++) 
		CFWriteCommonMemByte(uip_buf[i],WR_DATA);

	return 0;

}

/*----------------------------------------------------------------
*CFWait(void)
* Wait for some bit masked by cf_stacom_mask until its value becomes
* cf_stacom_ok_mask
----------------------------------------------------------------*/
unsigned char CFWait(unsigned char cf_stacom_mask, unsigned char cf_stacom_ok_mask)
{
	unsigned char xdata tmp_data;

	tmp_data=CFReadCommonMemByte(CF_STACOM);	//status register read
	if(tmp_data & CF_STACOM_ERROR_MASK) return 1; //error if tmp = 0x01 !

	do
	{
		tmp_data=CFReadCommonMemByte(CF_STACOM);	//leitura do conteudo do registro de status da CF 
		tmp_data&=cf_stacom_mask;	//
  	}while(tmp_data!=cf_stacom_ok_mask/*cont !=1000*/); 	
  
	return 0; 
}

/*----------------------------------------------------------------
* IDDrive()
* Identify drive to discover head/cylinder/sectors configuration
----------------------------------------------------------------*/
char CFIDDrive(void)
{
	unsigned int xdata i;
	unsigned char xdata Num_sectors[4];
	
	/* Get CF drive number */
	CFDrvNumber=0x1&(CFReadMemByte(SOCKET_COPY_REG)>>4);
	printf("Drive number is %Bx\n",(char)CFDrvNumber);               // --> Precisa verificar se precisa setar o modo LBA no
                                                              // --> registrador Drive/Head  - offset 6   
	/* Set Operation mode = Memory mapped */
	cor_data = CFReadMemByte(cor_addr);
	delay_nops(5000);
	cor_data = MEMORY_MAPPED|(cor_data&0xe0);	
	//cor_data = IO_MAPPED|(cor_data&0xe0);	
	CFWriteMemByte(cor_data,cor_addr);
	
	if(CFSendCommand(CF_ID_DRIVE,0,0)) 
	{
		printf("Identify drive command error\n");
		return 1;
	}

	for(i=0;i<=123;i++)
    {
		uip_buf[i]=CFReadCommonMemByte(RD_DATA);
		//printf("uip_buf[%d] = %Bx\n", i,(char)uip_buf[i]);
 	    if (i>=120)
			Num_sectors[i-120] = uip_buf[i];	
	}
  
	CFNumSectors = (unsigned long)(Num_sectors[3]<<24)|(unsigned long)(Num_sectors[2]<<16)|(unsigned long)(Num_sectors[1]<<8)|(unsigned long)(Num_sectors[0]);
	
	printf("Mumber of sectors (LBA mode) = 0x%Lx\n", CFNumSectors);
			
	return 0;
}

/*----------------------------------------------------------------
* CFWriteCommonMemByte
*
* write a byte in the CF Common memory
----------------------------------------------------------------*/
static void CFWriteCommonMemByte(char val, unsigned int reg)
{

	CF_REG_ON();
	
	CFSetAddr(reg);	// Set Mem reg to write in CF

 	CF_DATA_PORT=val;		//escrita no PORT0=buf do 8051 

    NOP();
 	CF_WR_OFF(); // write=0
	NOP();
	NOP();
	NOP();
 	CF_WR_ON();	// write=1

    CF_DATA_PORT = CF_DATA_PORT_MASK;

 	return ;
}

/*----------------------------------------------------------------
* CFReadCommonMemByte
*
* Read a byte from the CF common memory
----------------------------------------------------------------*/
char CFReadCommonMemByte(unsigned int reg)
{
	char xdata val;

	CF_REG_ON();
 
 	CFSetAddr(reg);	// Set Mem reg to read from CF

 	//NOP();
 	CF_RD_OFF();		//read=0
	NOP();
	NOP();
	NOP();
 	
	val = CF_DATA_PORT;  // val = P0 (8051)	
 	CF_RD_ON();		//read=1
 
 	return val;
} 


