/********************************************************************
* Filename: wifidev.c
* Description: This file implements the prism device driver for 8051
*              microcontroller 
* Developed by: Marco Carvalho
* Created on: 30/03/05
/*********************************************************************/

#include "wifidev.h"

/*control message print*/
#define PRINT_WIFI 0

//tx buffers
#define		HFA384x_DRVR_FIDSTACKLEN_MAX	3

/****************************Global Variables *****************************/

int xdata txfid_queue[HFA384x_DRVR_FIDSTACKLEN_MAX]; // 3 tx buffers
int xdata infofid_queue[1];
char xdata infofid;
char xdata txfid;
unsigned int xdata cor_addr, cmd_data;
char xdata cis_device = 0;
char xdata cor_data;

#if PRINT_WIFI
char xdata bssidc[6];
#endif

char xdata txflag=0;

/* Buffers */
unsigned int xdata tmp_buff[tmp_buff_size +1];
char xdata comframe_buff[60];


/*----------------------------------------------------------------
* delay_nops
*
* Delay a value number os nops function
----------------------------------------------------------------*/
void delay_nops(int value)
{
	for (;value;value--)
		NOP();
}

/*----------------------------------------------------------------
* CFConnected
*
* Check if CF card is connected
----------------------------------------------------------------*/
bit CFConnected(void)
{
 	bit connected;

 	connected=CF_CD1;	//leitura do pino que informa se CF esta conectada
 	if(connected) return 0;	//Erro se P1.4=1 !
 	else return 1;
}

/*----------------------------------------------------------------
* CFSetAddr
*
* Set addr to CF
*   A8  |  A7  |  A6  |  A5  |  A4  |  A3  |  A2  |  A1  |  A0
*  P3.7 | P3.6 | P3.3 | P2.7 | P2.6 | P2.0 | P2.1 | P2.2 | P2.3
----------------------------------------------------------------*/
void CFSetAddr(unsigned int CFaddr)
{
	A0 = (CFaddr)&0x0001;
	A1 = (CFaddr >> 1)&0x0001;
	A2 = (CFaddr >> 2)&0x0001;
	A3 = (CFaddr >> 3)&0x0001;
	A4 = (CFaddr >> 4)&0x0001;
	A5 = (CFaddr >> 5)&0x0001;
	A6 = (CFaddr >> 6)&0x0001;
	A7 = (CFaddr >> 7)&0x0001;
	A8 = (CFaddr >> 8)&0x0001;
}
	
/*----------------------------------------------------------------
* CFWriteIOByte()
*
* Write some data (val) in a register (reg) in IO mode
----------------------------------------------------------------*/
static void CFWriteIOByte(char val, unsigned int reg)
{

	CFSetAddr(reg);	// Set IO reg to write in CF

	CF_DATA_PORT=val;		//write in PORT0

	CF_REG_OFF();
	CF_IOWR_OFF(); // IOwrite=0
	CF_IOWR_ON();	// IOwrite=1
 	CF_REG_ON();

	// Set Data outputs as inputs
    CF_DATA_PORT = CF_DATA_PORT_MASK;

	return ;
}

/*----------------------------------------------------------------
* CFReadIOByte()
*
* Read some data From a register (reg) in IO mode
----------------------------------------------------------------*/
char CFReadIOByte(unsigned int reg)
{
	char xdata val;

 	CFSetAddr(reg);	// Set IO reg to read from CF

 	CF_REG_OFF();		//chip select=0
	CF_IORD_OFF();		//IOread=0
	val = CF_DATA_PORT;  // val = P0 (8051)	
 	CF_IORD_ON();		//IOread=1
 	CF_REG_ON();

 	return val;
 
}

/*----------------------------------------------------------------
* CFWriteMemByte
*
* write a byte in the CF attribute memory
----------------------------------------------------------------*/
void CFWriteMemByte(char val, unsigned int reg)
{

 	CFSetAddr(reg);	// Set Mem reg to write in CF

 	CF_DATA_PORT=val;		//escrita no PORT0=buf do 8051 

 	CF_REG_OFF();
 	CF_WR_OFF(); // write=0
 	CF_WR_ON();	// write=1
   	CF_REG_ON();

    CF_DATA_PORT = CF_DATA_PORT_MASK;

 	return ;
}

/*----------------------------------------------------------------
* CFReadMemByte
*
* Read a byte from the CF attribute memory
----------------------------------------------------------------*/
char CFReadMemByte(unsigned int reg)
{
	char xdata val;
 
 	CFSetAddr(reg);	// Set Mem reg to read from CF
 
 	CF_REG_OFF();		//chip select=0
 	CF_RD_OFF();		//read=0
 	val = CF_DATA_PORT;  // val = P0 (8051)	
 	CF_RD_ON();		//read=1
 	CF_REG_ON();
 
 	return val;
} 

/*----------------------------------------------------------------
* hfa384x_setreg
*
* Set the value of one of the MAC registers.  Done here
* because different PRISM2 MAC parts use different buses and such.
----------------------------------------------------------------*/
void hfa384x_setreg(int val, unsigned int reg)
{
	/* Write the low byte firt */
	CFWriteIOByte((char)val&0xFF, reg);
	CFWriteIOByte((char)(val>>8), reg+1);
	return;
}

void hfa384x_setdata(int val, unsigned int datareg)
{
	/* Write the High byte first */
	CFWriteIOByte((char)(val>>8), datareg);
	CFWriteIOByte((char)val&0xFF, datareg+1);
	return;
}

/*----------------------------------------------------------------
* hfa384x_getreg
*
* Retrieve the value of one of the MAC registers.  Done here
* because different PRISM2 MAC parts use different buses and such.
----------------------------------------------------------------*/
int hfa384x_getreg(unsigned int reg)
{
	int xdata val=0;

	val = CFReadIOByte(reg)&0x00FF;
	val |= CFReadIOByte(reg+1)<<8;
	return val;

}

int hfa384x_getdata(unsigned int datareg)
{
	int xdata val=0;

	val = CFReadIOByte(datareg)<<8;
	val |= CFReadIOByte(datareg+1)&0x00FF;
	return val;

}

/*----------------------------------------------------------------
* hfa384x_cmd
*
* Issues the cmd command 
*
* Arguments:
*	cmd		command to set
*   parm0   parameter 0
*
* Returns: 
*	result		
----------------------------------------------------------------*/
char hfa384x_cmd(unsigned int cmd, unsigned int parm0)
{
	char xdata  result;
	unsigned int xdata cmd_status, evstat_data;
	unsigned int xdata timeout_count;

	/* Wait for busy bit */
	do{
		cmd_data = hfa384x_getreg(HFA384x_CMD); 
		timeout_count++;
	}while((HFA384x_CMD_ISBUSY(cmd_data)) && (timeout_count<10)); 
	timeout_count = 0;

	/* Write command and param0 */
	hfa384x_setreg(parm0, HFA384x_PARAM0); 
	hfa384x_setreg(cmd, HFA384x_CMD); 
	
	/* Wait for busy bit */
	do{
		cmd_data = hfa384x_getreg(HFA384x_CMD); 
		timeout_count++;
	}while((HFA384x_CMD_ISBUSY(cmd_data)) && (timeout_count<10)); 
	timeout_count = 0;
	
	/* Wait for command complete event */
	do{
		evstat_data = hfa384x_getreg(HFA384x_EVSTAT);
 	    timeout_count++;
	}while((!(HFA384x_EVSTAT_ISCMD(evstat_data))) && (timeout_count<5000)); 
	timeout_count = 0;

	/* Read Command result through status register */
	cmd_status = hfa384x_getreg(HFA384x_STATUS); 
	result = HFA384x_STATUS_RESULT_GET(cmd_status);
	/* acknowledge the cmd event */
	hfa384x_setreg(HFA384x_EVACK_CMD, HFA384x_EVACK); 

	return(result);
}

/*----------------------------------------------------------------
* hfa384x_cmd_allocate
*
* Issues the allocate command instructing the firmware to allocate
* a 'frame structure buffer' in MAC controller RAM.  This command
* does not provide the result, it only initiates one of the f/w's
* asynchronous processes to construct the buffer.  When the 
* allocation is complete, it will be indicated via the Alloc
* bit in the EvStat register and the FID identifying the allocated
* space will be available from the AllocFID register.  Some care
* should be taken when waiting for the Alloc event.  If a Tx or 
* Notify command w/ Reclaim has been previously executed, it's
* possible the first Alloc event after execution of this command
* will be for the reclaimed buffer and not the one you asked for.
* This case must be handled in the Alloc event handler.
*
* Arguments:
*	len		allocation length, must be an even value
*			in the range [4-2400]. (host order)
*
* Returns: 
*	0		success
*	1		f/w reported failure - f/w status code
*
----------------------------------------------------------------*/
char hfa384x_cmd_allocate(unsigned int len)
{
	char xdata result;

	result = hfa384x_cmd(HFA384x_CMDCODE_ALLOC, len);
	return result;
}

/*----------------------------------------------------------------
* hfa384x_alloc_tx_buff()
* allocate some tx FIDs in order to transmit data
----------------------------------------------------------------*/
char hfa384x_alloc_tx_buff(void)
{
	unsigned int xdata fid, evstat_data;
	char xdata i, result;

	/*Reset tx buffer positions and txfid (number of free fids)*/
    for(i=0;i<HFA384x_DRVR_FIDSTACKLEN_MAX;++i)
		txfid_queue[i] = 0;
	txfid = 0;
 
	// Create a common buffer structure between all TX buffers */
	/* Reset all comframe_buff positions */
	for(i=0;i<(FRAME_STRUCTURE_OFF + ETH_HDR_OFF);++i)
		comframe_buff[i] = 0x00;

	/* Setup TXControl field in the frame structure */
	comframe_buff[12] = 0x00;
	comframe_buff[13] = 0x06; //comframe_buff[13] = HFA384x_TX_TXOK_SET(1)|HFA384x_TX_TXEX_SET(1);

	/* SWSupport bytes */
	comframe_buff[6] = 0x55;
	comframe_buff[7] = 0xaa;
	comframe_buff[8] = 0x55;
	comframe_buff[9] = 0xaa;

	/* Alocate Tx buffers */
	for (i=0; i<HFA384x_DRVR_FIDSTACKLEN_MAX; i++)
	{
		result = hfa384x_cmd_allocate(TXFRAME_BUFF_SIZE);	
		if (result)
			return 1;
		/* Wait for alloc bit in the event stat register to set */
		do{
			evstat_data = hfa384x_getreg(HFA384x_EVSTAT);
	  	}while(!(HFA384x_EVSTAT_ISALLOC(evstat_data)));
		/* get FID */	
		fid = hfa384x_getreg(HFA384x_ALLOCFID);

        /* acknowledge the Alloc event in the event ack register */
		hfa384x_setreg(HFA384x_EVACK_ALLOC_SET(1), HFA384x_EVACK);
		/* Prepare BAP with 802.11 pre header*/	    
		result = hfa384x_rdwr_bap( BAP_WRITE, fid, 0x0000, comframe_buff, FRAME_STRUCTURE_OFF + ETH_HDR_OFF);
		if (result)
	   		return 1;	  
  		/* Put FID in a TX FID Buffer */
		txfid_queue[i] = fid;
		txfid++;		
	}
	return 0;
}

/*----------------------------------------------------------------
* hfa384x_alloc_info_buff()
* allocate some info FIDs in order to retrieve information data
----------------------------------------------------------------*/
char hfa384x_alloc_info_buff(void)
{
	unsigned int xdata fid, evstat_data;
	char xdata i, result;

	/*Reset tx buffer positions and txfid (number of free fids)*/
    for(i=0;i<1;++i)
		infofid_queue[i] = 0;
	infofid = 0;
 
	// Create a common buffer structure between all info buffers */
	/* Reset all comframe_buff positions */
	for(i=0;i<(FRAME_STRUCTURE_OFF + ETH_HDR_OFF);++i)
		comframe_buff[i] = 0x00;
  
	/* Alocate Info buffers */
	for (i=0; i<1; i++)
	{
		result = hfa384x_cmd_allocate(10);	
		if (result)
			return 1;
		/* Wait for alloc bit in the event stat register to set */
		do{
			evstat_data = hfa384x_getreg(HFA384x_EVSTAT);
	  	}while(!(HFA384x_EVSTAT_ISALLOC(evstat_data)));
		/* get FID */	
		fid = hfa384x_getreg(HFA384x_ALLOCFID);

        /* acknowledge the Alloc event in the event ack register */
		hfa384x_setreg(HFA384x_EVACK_ALLOC_SET(1), HFA384x_EVACK);

		if (result)
	   		return 1;	  
  		/* Put FID in a TX FID Buffer */
		infofid_queue[i] = fid;				
	}
	infofid++;
	return 0;
}


/*----------------------------------------------------------------
* hfa384x_cmd_diagnose
*
* Issues the diagnose command to test the: register interface,
* MAC controller (including loopback), External RAM, Non-volatile
* memory integrity, and synthesizers.  Following execution of this
* command, MAC/firmware are in the 'initial state'.  Therefore,
* the Initialize command should be issued after successful
* completion of this command.  This function may only be called
* when the MAC is in the 'communication disabled' state.
*
* Arguments:
*
* Returns: 
*	0		success
*	1		f/w reported failure - f/w status code
*
----------------------------------------------------------------*/
#define DIAG_PATTERNA ((int)0xaaaa)
#define DIAG_PATTERNB ((int)~0xaaaa)

char hfa384x_cmd_diagnose(void)
{
	unsigned int xdata result_resp0 = 0;
	unsigned int xdata result_resp1 = 0;
	char xdata result = 1;
	unsigned int xdata timeout_count;

	unsigned int xdata cmd_status, evstat_data;

	/* Wait for busy bit */
	do{
		cmd_data = hfa384x_getreg(HFA384x_CMD); //0x0000
		timeout_count++;
	}while((HFA384x_CMD_ISBUSY(cmd_data))&&(timeout_count<10)); //0x8000
	timeout_count = 0;

	/* Write command, param0 and param1 */
	hfa384x_setreg(DIAG_PATTERNB, HFA384x_PARAM1); //0x0004
	hfa384x_setreg(DIAG_PATTERNA, HFA384x_PARAM0); //0x0002
	hfa384x_setreg(HFA384x_CMD_CMDCODE_SET(HFA384x_CMDCODE_DIAG), HFA384x_CMD); //0x0000
	
	/* Wait for busy bit */
	do{
		cmd_data = hfa384x_getreg(HFA384x_CMD); //0x0000
		timeout_count++;
	}while((HFA384x_CMD_ISBUSY(cmd_data))&&(timeout_count<10)); //0x8000
	timeout_count = 0;
	
	/* Wait for command complete event */
	do{
		evstat_data = hfa384x_getreg(HFA384x_EVSTAT); //0x0030
	}while(!(HFA384x_EVSTAT_ISCMD(evstat_data))); //0x0010

	/* Read Command result through status register */
	cmd_status = hfa384x_getreg(HFA384x_STATUS); //0x0008
	result = HFA384x_STATUS_RESULT_GET(cmd_status);
	
	/* acknowledge the cmd event */
	hfa384x_setreg(HFA384x_EVACK_CMD, HFA384x_EVACK); //0x0010, 0x0034

	result_resp0 = hfa384x_getreg(HFA384x_RESP0);
	result_resp1 = hfa384x_getreg(HFA384x_RESP1);

	if ((result_resp0 != DIAG_PATTERNB)||(result_resp1 != DIAG_PATTERNA))
		result = 1;
	
	return(result);
}

/*----------------------------------------------------------------
* Get Free bap
----------------------------------------------------------------*/
char get_free_bap(void)
{
 	 unsigned int xdata temp;
	 char xdata result;
 
	result = 2;	 	  
    temp = hfa384x_getreg(HFA384x_OFFSET1);
	if(!(HFA384x_OFFSET_ISBUSY(temp)))
		result = 1;
	temp = hfa384x_getreg(HFA384x_OFFSET0);
	if(!(HFA384x_OFFSET_ISBUSY(temp)))
	 	result = 0;
	return(result);
}	 

/*----------------------------------------------------------------
* hfa384x_cmd_access
*
* Requests that a given record be copied to/from the record 
* buffer.  If we're writing from the record buffer, the contents
* must previously have been written to the record buffer via the 
* bap.  If we're reading into the record buffer, the record can
* be read out of the record buffer after this call.
*
* Arguments:
*	write		[0|1] copy the record buffer to the given
*			configuration record. 
*	rid_fid	RID or fid of the record to read/write. 
*           Write to bap = rid
*           Read from bap = fid 
*	buf		host side record buffer.  Upon return it will
*			contain the body portion of the record (minus the 
*			RID and len).
*	len		buffer length (in bytes, should match record length)
----------------------------------------------------------------*/
void hfa384x_cmd_access(bit write, unsigned int id, unsigned int* buf, int len)
{
	char xdata result = 0;
	
	if (write)
	{
		result = hfa384x_rdwr_bap(BAP_WRITE, id, 0x0000, (char*) buf, (len+1)<<1);
    	result = hfa384x_cmd(HFA384x_CMDCODE_ACCESS | HFA384x_CMD_WRITE_SET(write), id);
	}
	else
	{
		result = hfa384x_cmd(HFA384x_CMDCODE_ACCESS, id);
		result = hfa384x_rdwr_bap(BAP_READ, id, 0x0000, NULL, 0x0000);	
	}
	
	return;
}
/*----------------------------------------------------------------
* hfa384x_rdwr_bap
*
* Copies a collection of bytes to the MAC controller memory via
* one set of BAP registers.
*
* Arguments:
*	write	[0|1] read/write to BAp control
*	id		FID or RID, destined for the select register (host order)
*	offset		An _even_ offset into the buffer for the given
*			FID/RID.  We haven't the means to validate this,
*			so be careful. (host order)
*	buf		ptr to array of bytes
*	len		length of data to transfer (in bytes)
*
* Returns: 
*	0		success
*	1		f/w reported failure - value of offset reg.
----------------------------------------------------------------*/
char hfa384x_rdwr_bap(bit write, int id, unsigned int offset, 
		       char* buf, int len)
{
	char	xdata result = 0;
	unsigned int xdata fid_ptr;
	unsigned int xdata	reg;
    unsigned int xdata fidlen, i;
	char xdata bap;
	char xdata data_lo;
	int xdata *dataptr = (int*)buf;

	/* Disable all IRQs */
	DISABLE_GLOBAL_IRQ();

	bap = get_free_bap(); 

	if (write) // if write
	{
		if (bap == 1) // if bap = 1
 	    {
			/* Write id to select reg */
			hfa384x_setreg(id, HFA384x_SELECT1);
			/* Write offset to offset reg */
	   		hfa384x_setreg(offset, HFA384x_OFFSET1);
	   		/* Wait for offset[busy] to clear */
			do{
		  		reg = hfa384x_getreg(HFA384x_OFFSET1);
	  		}while(HFA384x_OFFSET_ISBUSY(reg));
	   		/* Check for error bit in offset reg if not error write in buffer path*/
			reg = hfa384x_getreg(HFA384x_OFFSET1);

 			if(HFA384x_OFFSET_ISERR(reg))
	    		result = HFA384x_RES_CARDFAIL;
	   		else
			{
				if (txflag)
				{	
					for(i=0;i<(len&0xFFFE);)
					{	
						hfa384x_setdata(*dataptr++, HFA384x_DATA1);
						i+=2;
					}
					if(len % 2)
					{
						data_lo = buf[i++];
						hfa384x_setreg(data_lo, HFA384x_DATA1); 
					}
				}
				else // if !txflag
				{
					for(i=0;i<(len&0xFFFE);)
					{	
						hfa384x_setreg(*dataptr++, HFA384x_DATA1);
						i+=2;
					}
					if(len % 2)
					{
						data_lo = buf[i++];
						hfa384x_setreg(data_lo, HFA384x_DATA1);
					}
				} 
			}
		}
		else if (bap == 0)
		{
			/* Write id to select reg */
			hfa384x_setreg(id, HFA384x_SELECT0);
			/* Write offset to offset reg */
	   		hfa384x_setreg(offset, HFA384x_OFFSET0);
			/* Wait for offset[busy] to clear */
			do{
		  		reg = hfa384x_getreg(HFA384x_OFFSET0);
	   		}while(HFA384x_OFFSET_ISBUSY(reg));
	   		/* Check for error bit in offset reg if not error write in buffer path*/
			reg = hfa384x_getreg(HFA384x_OFFSET0);

			if(HFA384x_OFFSET_ISERR(reg))
	    		result = HFA384x_RES_CARDFAIL;
	   		else
			{
				if (txflag)
				{	
					for(i=0;i<(len&0xFFFE);)
					{	
						hfa384x_setdata(*dataptr++, HFA384x_DATA0);
						i+=2;
					}
					if(len % 2)
					{
						data_lo = buf[i++];
						hfa384x_setreg(data_lo, HFA384x_DATA0); 
					}
				}
				else // if !txflag
				{
					for(i=0;i<(len&0xFFFE);)
					{	
						hfa384x_setreg(*dataptr++, HFA384x_DATA0);
						i+=2;
					}
					if(len % 2)
					{
						data_lo = buf[i++];
						hfa384x_setreg(data_lo, HFA384x_DATA0);
					}
				} 
			}
		}
	}
	else //if read
	{
		if (bap) // if bap = 1
	    {
	    	/* Write id to select reg */
			hfa384x_setreg(id, HFA384x_SELECT1);
			/* Write offset to offset reg */
	    	hfa384x_setreg(offset, HFA384x_OFFSET1);
	  	   	/* Wait for offset[busy] to clear */
			do{
		  		reg = hfa384x_getreg(HFA384x_OFFSET1);
	  		 }while(HFA384x_OFFSET_ISBUSY(reg));
			/* Check for error bit in offset reg if not error */
			reg = hfa384x_getreg(HFA384x_OFFSET1);

 			if(HFA384x_OFFSET_ISERR(reg))
	    		result = HFA384x_RES_CARDFAIL;
	   		else
      		{
	   			fidlen = hfa384x_getreg(HFA384x_DATA1);
	   			tmp_buff[0] = fidlen;
	   			if(fidlen)
	   			{
					for(fid_ptr=0;fid_ptr<fidlen;fid_ptr++)
	    			{
		 				tmp_buff[fid_ptr+1] = hfa384x_getreg(HFA384x_DATA1);
	    			}
	   			} 
	  		}
		}
	 	else // if bap = 0	 
	    {
	    	/* Write id to select reg */
			hfa384x_setreg(id, HFA384x_SELECT0);
			/* Write offset to offset reg */
	    	hfa384x_setreg(offset, HFA384x_OFFSET0);
	  	   	/* Wait for offset[busy] to clear */
			do{
		  		reg = hfa384x_getreg(HFA384x_OFFSET0);
	  		}while(HFA384x_OFFSET_ISBUSY(reg));
			/* Check for error bit in offset reg if not error */
			reg = hfa384x_getreg(HFA384x_OFFSET0);

 			if(HFA384x_OFFSET_ISERR(reg))
	    		result = HFA384x_RES_CARDFAIL;
	   		else
      		{
	   			fidlen = hfa384x_getreg(HFA384x_DATA0);
	   			tmp_buff[0] = fidlen;
	   			if(fidlen)
	   			{
					for(fid_ptr=0;fid_ptr<fidlen;fid_ptr++)
	    			{
		 				tmp_buff[fid_ptr+1] = hfa384x_getreg(HFA384x_DATA0);
	    			}
	   			} 
	  		}
		}
	}

	/* Enable all IRQs */
	ENABLE_GLOBAL_IRQ();
	
	return(result);
}

/*----------------------------------------------------------------
* ReadCISTable
* Read The Card Information Structure (CIS)
----------------------------------------------------------------*/
void ReadCISTable(void)
{
	char xdata CISFlag;
	char xdata cor_addr_lo,cor_addr_hi;
//	char xdata cis_device = 0;
	unsigned int xdata cis_addr = 0;  	
	char xdata code_tuple=0;			
    char xdata link_tuple;
    char xdata tuple_link_cnt;
	char xdata tuple_data;			
    char xdata funce_cnt = 0;      	 	
    char xdata mac_cnt = 0;

	/* Wait until read the first CIS tuple */
    CISFlag = 0;
	do{
		delay_nops(2);
		code_tuple = CFReadMemByte(0x0000);	

		if(code_tuple == 1)
			CISFlag = 1;		
	  }while(!CISFlag);

//	printf("Reading CIS Table\n")
	cor_addr = 0;
	CISFlag = 0;
	do{
   		code_tuple = CFReadMemByte(cis_addr);
        cis_addr+=2;	

        link_tuple = CFReadMemByte(cis_addr);		
        cis_addr+=2;  

//#if PRINT_WIFI
		printf("code_tuple[%2i] = 0x%Bx\n", cis_addr-4, (char)code_tuple);
//		printf("link_tuple[%2i] = 0x%Bx\n", cis_addr-2, (char)link_tuple);
//#endif

		if(code_tuple+1!=0) //if(code_tuple != CISTPL_END)
 		{
       		for(tuple_link_cnt=0;tuple_link_cnt<link_tuple;++tuple_link_cnt)		
       		{
				tuple_data = CFReadMemByte(cis_addr);	
         		cis_addr+=2;

#if PRINT_WIFI
				printf("CISTable[%2i] = 0x%Bx\n", cis_addr-2, (char)tuple_data);
#endif

		        switch(code_tuple)	
				{
          	 		case CISTPL_DEVICE:			break;	
             		case CISTPL_DEVICE_A:		break;	
             		case CISTPL_DEVICE_OC:		break;	
             		case CISTPL_DEVICE_OA:		break;	
             		case CISTPL_VERS_1:   		break;
             		case CISTPL_MANFID:	    	break;	
             		case CISTPL_FUNCID:			
               			if(tuple_link_cnt == 0)
							cis_device = tuple_data;
 						/* Stop read CIS table if card is a fixed disk */
						if (cis_device == CISTPL_FUNCID_FIXED)
							CISFlag = 1;
			    								break;
             		case CISTPL_FUNCE: 			
						if(funce_cnt == 6) // Prism Datasheet = 4
							if(tuple_link_cnt>1)
								uip_ethaddr.addr[mac_cnt++] = tuple_data; 
						if(tuple_link_cnt == (link_tuple-1))	
               	  			funce_cnt++;        		
               									break;
             		case CISTPL_CONFIG:			
               			if(tuple_link_cnt == 2)
               				cor_addr_lo = tuple_data;		  	
               			if(tuple_link_cnt == 3)              
			   			{
               				cor_addr_hi = tuple_data;
							cor_addr = ((unsigned int)cor_addr_hi<<8)+((unsigned int)cor_addr_lo&0x00FF);
			   			}
            									break;
			   
             		case CISTPL_CFTABLE_ENTRY:	break;
             		default:					break;
				} 
       		} 
		}
		else
		{
			CISFlag = 1;
		}
	}while(!CISFlag);

#if PRINT_WIFI
	printf("COR address = 0x%x\n", cor_addr); 
	printf("CIS device = 0x%Bx\n", (char)cis_device);
#endif

	return;
}
/*----------------------------------------------------------------
* hfa384x_drvr_start
*
* Issues the MAC initialize command, sets up some data structures,
* and enables the interrupts.  After this function completes, the
* low-level stuff should be ready for any/all commands.
*
----------------------------------------------------------------*/
void hfa384x_drvr_start(void)
{
	char xdata result = 0;
	char xdata cor_data;

	/* Check if CF is connected */
//	if(!CFConnected())
//	{
//		printf("CF not connected. Connect the card and Try again.\n");
//		return;		
//	}

	/* Reset CF card */
	CF_RESET_ON() 
	delay_nops(4);
	CF_RESET_OFF();

	/* Read CIS table */
	//ReadCISTable();

	/* Set CF card to IO mode */
	cor_data = CFReadMemByte(cor_addr);
	delay_nops(5000);
	cor_data |= COR_FUNC_ENA;	//0x01
	CFWriteMemByte(cor_data,cor_addr);

	/* Initialize command */
	result = hfa384x_cmd(HFA384x_CMDCODE_INIT, 0); 
	switch(result)
	{
		case HFA384x_RES_SUCCESS:	
		    printf("Card Initialized\r\n");
		    break;
		case HFA384x_RES_CARDFAIL:
			printf("Card Failure\r\n");
			break;
		case HFA384x_RES_NOBUFF:
			printf("No buffer space\r\n");
			break;
		case HFA384x_RES_COMMERR:
			printf("Command error\r\n");
			break;
		default:
			printf("Result Code = %x",result);
			break;
	}

	/* make sure interrupts are disabled and any events cleared */
	hfa384x_setreg(0, HFA384x_INTEN);
	hfa384x_setreg(0xffff, HFA384x_EVACK);

	return;
}

/*----------------------------------------------------------------
* uIPwifi_api
*
* Implement an api for wifi parameters configuration
----------------------------------------------------------------*/
void uIPwifi_api(unsigned int cmd, unsigned int val) 
{
    char xdata i,j,k;
	char xdata SSID_name[] = "linksys"; // change it to support different SSIDs
	char xdata result = 0;

	switch(cmd)
	{
		case BSS:
			tmp_buff[0] = HFA384x_RID_CNFPORTTYPE_LEN;
			tmp_buff[1] = HFA384x_RID_CNFPORTTYPE;
			tmp_buff[2] = HFA384x_PORTTYPE_BSS;
			hfa384x_cmd_access(BAP_WRITE, HFA384x_RID_CNFPORTTYPE,
				                tmp_buff, 
				                HFA384x_RID_CNFPORTTYPE_LEN);	
         	break;

		case IBSS:		
			tmp_buff[0] = HFA384x_RID_CNFPORTTYPE_LEN	;
			tmp_buff[1] = HFA384x_RID_CNFPORTTYPE;
			tmp_buff[2] = HFA384x_PORTTYPE_IBSS;
			hfa384x_cmd_access(BAP_WRITE, HFA384x_RID_CNFPORTTYPE,
				                tmp_buff, 
				                HFA384x_RID_CNFPORTTYPE_LEN);	
         	break;
			
	    case SSID:
			tmp_buff[0] = HFA384x_RID_CNFDESIREDSSID_LEN;
			tmp_buff[1] = HFA384x_RID_CNFDESIREDSSID;
			tmp_buff[2] = strlen(SSID_name);
			/* assembly string vector in tmp_buff */
			j = strlen(SSID_name);
			i=0;
			k=3;
			while(j > 1) /* if SSID_name is even */
			{
	 			tmp_buff[k] = (SSID_name[i+1]) << 8 | SSID_name[i];
	 			i+=2;
	 			++k;
	 			j-=2; 	
			}
			while(j > 0) /* if SSID name is odd put the last single byte in tmp_buff */
			{
	 			tmp_buff[k] = SSID_name[i];
	 			--j;	
			}
			hfa384x_cmd_access(BAP_WRITE, HFA384x_RID_CNFDESIREDSSID,
				                tmp_buff, 
				                HFA384x_RID_CNFDESIREDSSID_LEN);	
			break;
		
		case MAX_LEN:
			tmp_buff[0] = HFA384x_RID_CNFMAXDATALEN_LEN;
			tmp_buff[1] = HFA384x_RID_CNFMAXDATALEN;
			tmp_buff[2] = val; 
			hfa384x_cmd_access(BAP_WRITE, HFA384x_RID_CNFMAXDATALEN, 
								tmp_buff, 
								HFA384x_RID_CNFMAXDATALEN_LEN);
         	break;

		case ENABLE:
			if (val)
			{
				/*enable the chip to work --> MACPORT = 0 */
				if (result = hfa384x_cmd(HFA384x_CMDCODE_ENABLE | HFA384x_CMD_MACPORT_SET(0), 0)) 
				{
					printf("Enable macport failed, result=0x%x.\n", result);
				}
			}
			else
 		    {
				/*disable the chip to work --> MACPORT = 0 */
				if (result = hfa384x_cmd(HFA384x_CMDCODE_DISABLE | HFA384x_CMD_MACPORT_SET(0), 0)) 
				{
					printf("Disable macport failed, result=0x%x.\n", result);
				}
			}
			break;

		case PM_ENABLE:
			tmp_buff[0] = HFA384x_RID_CNFPMENABLED_LEN;
			tmp_buff[1] = HFA384x_RID_CNFPMENABLED;
			tmp_buff[2] = val; 
			hfa384x_cmd_access(BAP_WRITE, HFA384x_RID_CNFPMENABLED, 
								tmp_buff, 
								HFA384x_RID_CNFPMENABLED_LEN);
			break;

		case PM_MAX_SLEEP:
			tmp_buff[0] = HFA384x_RID_CNFMAXSLEEPDUR_LEN;
			tmp_buff[1] = HFA384x_RID_CNFMAXSLEEPDUR;
			tmp_buff[2] = val; 
			hfa384x_cmd_access(BAP_WRITE, HFA384x_RID_CNFMAXSLEEPDUR, 
								tmp_buff, 
								HFA384x_RID_CNFMAXSLEEPDUR_LEN);
			break;

		default:
			break;
	}
	return;
}

/*----------------------------------------------------------------
* static void config_chip(void)
*
* Config some 8051 features 
-----------------------------------------------------------------*/
void config_chip(void)
{

#if PLL_DPM
	PLLCON &=0x00;
	PLLCON = PLLCON | 0x07; //CPU clk = 0,09MHz
#else
	PLLCON &= 0x00;   /* configura o clock interno = 12MHz */
#endif
	PLLCON = PLLCON | 0x08; /*configure Fast interrupt feature */ 

	/* Disable Ale output */
	DISABLE_ALE_OUT();

	// Serial Configuration
	TR1  = 0;        // Halt Timer 1.
	ET1  = 0;        // Disable Timer 1 interrupt
	SCON = 0x50;     /* mode 1, 8-bit uart, receptor enable */		
    TMOD = 0x20;     /* timer 1, modo 2, 8-bit reload */		
	TH1  = T1_RELOAD-1; // ?? bps
	TR1  = 1;        /* Start Serial Communication */
	TI   = 1;        /* Interrupt Flag set */ 

	/* RTC Configuration */
	SET_RTC_TMR(0,0,0);
	RTC_ENABLE();

	/* Power Supply Monitor Configuration */
	/*               Bit7  Bit6  Bit5  Bit4  Bit3  Bit2  Bit1  Bit0 */
	/* PSMCON =      CMPD  CMPA  PSMI  TPD1  TPD0  TPA1  TPA0  PSMEN */
	PSMCON = 0x1E; // 0      0     0     1     1     1     1      0   --> Trip Point = 2.63V (AVdd & DVdd)	

    /* Initialize Timer 0 to generate a periodic 24Hz interrupt. */
    // Stop timer/ counter 0.                                         
    TR0  = 0;          
	// Set timer/ counter 0 as mode 1 16 bit timer.      
    TMOD &= 0xF0;
    TMOD |= 0x01;
    // Preload for 24Hz periodic interrupt.    
    TH0 = WIFI_T0_RELOAD >> 8; 
    TL0 = WIFI_T0_RELOAD;

/* Ports Configuration */
/* Port 1 */
/* All pins configured as inputs */
	DIN   = 0; 
	PDET0 = 0;
	WAIT   = 0;
	CD1   = 0; 
	LDCH  = 0;

	LED2 = 1;
	/* Port 0 - Set Data as inputs*/
    CF_DATA_PORT = CF_DATA_PORT_MASK;

	/* Cf signals */
	CF_WR_ON();
	CF_RD_ON();
	CF_REG_ON();
	CF_RESET_OFF();	
	CF_IORD_ON();
	CF_IOWR_ON();

	printf("Starting 8051 WIFI uIP TCP/IP Stack\n");
	
	/* enable INT0 Interrupt */
	ENABLE_INT0_IRQ();
	// Restart timer/ counter 0 running.
    TR0 = 1;
    // Enable timer/ counter 0 overflow interrupt.            
    ET0 = 1;
    // Enable global interrupt.
    ENABLE_GLOBAL_IRQ();

}

/*----------------------------------------------------------------
/ wifidev_init()
/ 
/ This function init the wifi CF card
----------------------------------------------------------------*/
void wifidev_init(void)
{
	char xdata result = 0;
	char xdata i;
	unsigned int xdata timeout_count;

#if PLL_DPM
	/*configure PLL to 12,58MHz frequency */
	PLLCON &= 0xF8; 
#endif

	/*enable WDT for 2seg timeout*/
	CONFIG_WDT_2SEG();

	/*start drv performing CF initialization and initialization command*/
	hfa384x_drvr_start();

	/* Diagnose card before enable */
	result = hfa384x_cmd_diagnose();
	if (result)
	{
		printf("Diagnose card failed, result= 0x%x.\n", result);
		return;
	}
#if PRINT_WIFI
	else
		printf("Diagnose card passed!!!!\n");
#endif	
	
	/*configure the PRISM MAC for STA operation - Infra Structure mode*/
	/*set MAC port to 1 */
	uIPwifi_api(BSS, 1);

	/* Set the desired SSID */ 
 	uIPwifi_api(SSID, 1);

	/* lets extend the data length a bit */
 	uIPwifi_api(MAX_LEN, 0x05dc); // 1500 bytes = Ethernet MAC data size

	/* Read card MAC address and set as board MAC address */
	hfa384x_cmd_access(BAP_READ, HFA384x_RID_CNFOWNMACADDR, NULL, 0);
	for(i=0;i<MAC_LEN;i++)
    {
		if(i%2)
			uip_ethaddr.addr[i] = tmp_buff[2+i/2]>>8;
		else
			uip_ethaddr.addr[i] = tmp_buff[2+i/2]& 0xFF;
    }

#if PRINT_WIFI
   	printf("MAC Address:\n");
	for(i=0;i<MAC_LEN;i++)
		printf("uip_ethaddr.addr[%Bd] = 0x%Bx\n", (char)i, (char)uip_ethaddr.addr[i]);
#endif

	/* Alloc TX Buffers */
	result = hfa384x_alloc_tx_buff();
	
	/* Alloc Info Buffers */
	result = hfa384x_alloc_info_buff();
	
	/* Acknolegde all events */
	hfa384x_setreg(0xffff, HFA384x_EVACK);

	/*Read the current power mode */
	hfa384x_cmd_access(BAP_READ, HFA384x_RID_CURRENTPOWERSTATE, NULL, 0);

#if PRINT_WIFI
	printf("Power mode is 0x%x\n", tmp_buff[2]); 
#endif

	/*enable the chip to work --> MACPORT = 0 */
	uIPwifi_api(ENABLE, ON);

	/* Read MAC Port Status */
	do{
		hfa384x_cmd_access(BAP_READ, HFA384x_RID_PORTSTATUS, NULL, 0);
		timeout_count++;
	}while((tmp_buff[2]!=4)&&(timeout_count<30000)); // 4 = Connected to ESS
	timeout_count = 0;

	if (tmp_buff[2]!=4)
		printf("Wireless Connection Failed. Please Try again later. \n");
	
#if PRINT_WIFI
	/* Read The current BSSID which the station is connected */
	hfa384x_cmd_access(BAP_READ, HFA384x_RID_CURRENTBSSID, NULL, 0);
	for(i=0;i<MAC_LEN;i++)
	{
		if(i%2)
			bssidc[i] = tmp_buff[2+i/2]>>8;
		else
			bssidc[i] = tmp_buff[2+i/2]& 0xFF;
	}

	for(i=0;i<MAC_LEN;i++)
		printf("BSSID[%Bd] = 0x%Bx\n", (char)i, (char)bssidc[i]);

	/* Read CurrentTXrate fid */
	hfa384x_cmd_access(BAP_READ, HFA384x_RID_CURRENTTXRATE, NULL, 0);
	printf("Tx rate is %d\n", tmp_buff[2]);

	/* Read Commsquality */
	hfa384x_cmd_access(BAP_READ, HFA384x_RID_COMMSQUALITY, NULL, 0);
	printf("CQ.currBSS is %d\n", tmp_buff[2]);
	printf("ASL.currBSS is %d\n", tmp_buff[3]);
	printf("ANL.currFC is %d\n", tmp_buff[4]);
#endif

	/* reset WDE bit to clear WDT*/			
	RESET_WDE();

 	return;
}	

/*----------------------------------------------------------------
/ wifidev_read()
/ 
/ This function read data from wifi CF card
----------------------------------------------------------------*/
unsigned int wifidev_read(void)
{
	int xdata		rxfid;
	int xdata i, j;
	int xdata	reg;
	char xdata result = 0;
	char xdata bap;
	unsigned int xdata data_len;

	/*enable WDT for 2seg timeout*/
	CONFIG_WDT_2SEG();
	
	/* reset uip_len and data_len*/
	uip_len =0;
	data_len = 0;

	/* read the EvStat register for enabled events */
	reg = hfa384x_getreg(HFA384x_EVSTAT);	

	/* if event RX proceed receiving data */
	if (HFA384x_EVSTAT_ISRX(reg))
	{
#if PRINT_WIFI
		printf("I'm receiving something\n");
#endif
		/* Get the FID */
		rxfid = hfa384x_getreg(HFA384x_RXFID);
		
		bap = get_free_bap(); 

		if (bap) //if bap =1
		{
			/* Write id to select reg */
			hfa384x_setreg(rxfid, HFA384x_SELECT1);
			/* Write offset to offset reg to start from DataLen in the structure */
			hfa384x_setreg(RXDATALEN_OFF, HFA384x_OFFSET1);
			/*wait for offset busy bit to set */ 
 			do{
		 		reg = hfa384x_getreg(HFA384x_OFFSET1);
	   		}while(HFA384x_OFFSET_ISBUSY(reg));
			/* Check for errors */
			if (HFA384x_OFFSET_ISERR(reg))
			{
				result = HFA384x_RES_CARDFAIL;
				/* acknowledge RX event in EvAck Reg */
				hfa384x_setreg(HFA384x_EVACK_RX_SET(1), HFA384x_EVACK);
			}
			else // if not error
			{
				/* Read packet data len */ 
				data_len = hfa384x_getreg(HFA384x_DATA1);

				/* Check if Data len is bigger than UIP_BUFSIZE, 
				if yes set data len to UIP_BUFSIZE */
				if(data_len > UIP_BUFSIZE)
       				data_len = UIP_BUFSIZE-UIP_LLH_LEN;

      			/* Start to fill uip_buf */
				/* Read Destination MAC address */ 
				j=0;
				for(i=0;i<3;++i) 
				{
					*(int*)(&(uip_buf[ETH_PCKT_DST+j])) = hfa384x_getdata(HFA384x_DATA1);
					j+=2;
				}
				/* Read Source MAC address */	
			   	j=0;
				for(i=0;i<3;++i) 
				{
					*(int*)(&(uip_buf[ETH_PCKT_SRC+j])) = hfa384x_getdata(HFA384x_DATA1);
					j+=2;
				}
				
				/* read to get data Len 802.3 and snap */
				j=0;
				for(i=ETH_PCKT_LEN03;i<ETH_PCKT_DATA;++i)
				{
					*(int*)(&(uip_buf[ETH_PCKT_LEN03+j])) = hfa384x_getdata(HFA384x_DATA1);
					j+=2;
				}
				
				/* Read data(even) and put in uip_buf */
				j=0;
				for(i=0;j<(data_len & 0xfffe);++i)  
				{
					*(int*)(&(uip_buf[UIP_LLH_LEN+j])) = hfa384x_getdata(HFA384x_DATA1);
					j+=2;
				}
				/* If len odd, handle last byte */
				if ( data_len % 2 )
				{
					reg = hfa384x_getreg(HFA384x_DATA1);
					uip_buf[data_len + UIP_LLH_LEN -1] = ((char*)(&reg))[0];
				}
			}
		}
  	    else // if bap =0
		{
			/* Write id to select reg */
			hfa384x_setreg(rxfid, HFA384x_SELECT0);
			/* Write offset to offset reg to start from DataLen in the structure */
			hfa384x_setreg(RXDATALEN_OFF, HFA384x_OFFSET0);
			/*wait for offset busy bit to set */ 
 			do{
		 		reg = hfa384x_getreg(HFA384x_OFFSET0);
	   		}while(HFA384x_OFFSET_ISBUSY(reg));
			/* Check for errors */
			if (HFA384x_OFFSET_ISERR(reg))
			{
				result = HFA384x_RES_CARDFAIL;
				/* acknowledge RX event in EvAck Reg */
				hfa384x_setreg(HFA384x_EVACK_RX_SET(1), HFA384x_EVACK);
				//return (int)result;
			}
			else // if not error
			{
				/* Read packet data len */ 
				data_len = hfa384x_getreg(HFA384x_DATA0);
#if PRINT_WIFI
				printf("data_len = 0x%x\n", data_len);
#endif
				/* Check if Data len is bigger than UIP_BUFSIZE, 
				if yes set data len to UIP_BUFSIZE */
				if(data_len > UIP_BUFSIZE)
       				data_len = UIP_BUFSIZE-UIP_LLH_LEN;
	  		
      			/* Start to fill uip_buf */
				/* Read Destination MAC address */ 
				j=0;
				for(i=0;i<3;++i) 
				{
					*(int*)(&(uip_buf[ETH_PCKT_DST+j])) = hfa384x_getdata(HFA384x_DATA0);
					j+=2;
				}
				/* Read Source MAC address */	
			   	j=0;
				for(i=0;i<3;++i) 
				{
					*(int*)(&(uip_buf[ETH_PCKT_SRC+j])) = hfa384x_getdata(HFA384x_DATA0);
					j+=2;
				}
				
				/* read to get data Len 802.3 and snap */
				j=0;
				for(i=ETH_PCKT_LEN03;i<ETH_PCKT_DATA;++i)
				{
					*(int*)(&(uip_buf[ETH_PCKT_LEN03+j])) = hfa384x_getdata(HFA384x_DATA0);
					j+=2;
				}

				/* Read data(even) and put in uip_buf */
				j=0;
				for(i=0;j<(data_len & 0xfffe);++i)  
				{
					*(int*)(&(uip_buf[UIP_LLH_LEN+j])) = hfa384x_getdata(HFA384x_DATA0);
					j+=2;
				}
				/* If len odd, handle last byte */
				if ( data_len % 2 )
				{
					reg = hfa384x_getreg(HFA384x_DATA0);
					uip_buf[data_len + UIP_LLH_LEN -1] = ((char*)(&reg))[0];
				}
			}
		}
	 	/* acknowledge RX event in EvAck Reg */
		hfa384x_setreg(HFA384x_EVACK_RX_SET(1), HFA384x_EVACK);

		/* Setup type field 802.3 header */
		/* Copy Snap type field over Ether header len */
		/*in order to create a original ethernet header */
		uip_buf[ETH_PCKT_LEN03] = uip_buf[0x14];
		uip_buf[ETH_PCKT_LEN03+1] = uip_buf[0x15];
	}
	else // if not event RX
	{
		hfa384x_event_ack();
	}

	/* reset WDE bit to clear WDT*/			
	RESET_WDE();

	if (data_len > 0 )
	{
  		return (data_len+UIP_LLH_LEN);
	}
	else
		return 0;
}

/*----------------------------------------------------------------
* hfa384x_event_ack
* Perform ack on Events
----------------------------------------------------------------*/
void hfa384x_event_ack(void)
{
	unsigned int xdata evstat_data, fid;

	evstat_data = hfa384x_getreg(HFA384x_EVSTAT);
	
	/* Ack TX event */
	if(HFA384x_EVSTAT_ISTX(evstat_data))
	{
		hfa384x_setreg(HFA384x_EVACK_TX_SET(1), HFA384x_EVACK);
	}
	/* Ack TXEXC event */
	if(HFA384x_EVSTAT_ISTXEXC(evstat_data))
	{
		hfa384x_setreg(HFA384x_EVACK_TXEXC_SET(1), HFA384x_EVACK);
	}
	/* Ack ALLOC event */
	if(HFA384x_EVSTAT_ISALLOC(evstat_data))
	{
		hfa384x_setreg(HFA384x_EVACK_ALLOC_SET(1), HFA384x_EVACK);
	}
	/* Ack CMD event */
	if(HFA384x_EVSTAT_ISCMD(evstat_data))
	{
		hfa384x_setreg(HFA384x_EVACK_CMD_SET(1), HFA384x_EVACK);
	}
	/* Ack INFO event */
	if(HFA384x_EVSTAT_ISINFO(evstat_data))
	{
		fid = hfa384x_getreg(HFA384x_INFOFID); // TBU
		hfa384x_setreg(HFA384x_EVACK_INFO_SET(1), HFA384x_EVACK);
	}
	/* Ack INFDROP event */ 
	if(HFA384x_EVSTAT_ISINFDROP(evstat_data))
	{
		hfa384x_setreg(HFA384x_EVACK_INFDROP_SET(1), HFA384x_EVACK);
	}
	/* Ack WTERR event */
	if(HFA384x_EVSTAT_ISWTERR(evstat_data))
	{
		hfa384x_setreg(HFA384x_EVACK_WTERR_SET(1), HFA384x_EVACK);
	}
}


/*----------------------------------------------------------------
* wifidev_send()
* 
* This function write data to wifi CF card
----------------------------------------------------------------*/
void wifidev_send(void)
{
	char xdata result = 0;
	int	xdata	fid = 0;
	int xdata	reg;
	int xdata i=0;
	unsigned int xdata timeout_count;

	/*enable WDT for 2seg timeout*/
	CONFIG_WDT_2SEG();

#if PRINT_WIFI
	printf("I'm transmiting something\n");
	printf("uip_len = 0x%x\n", uip_len);
#endif

	/* Change Type field in eth header to length field in uip_buf*/
	uip_buf[12] = (char)(uip_len>>8);
	uip_buf[13] = (char)(uip_len&0xFF);

	/* fill uip_buf with tcp appdata */
	if(uip_buf[31] == UIP_PROTO_TCP)
	{
		for(i=40 + UIP_LLH_LEN; i< uip_len; i++)
		{
			uip_buf[i] = *uip_appdata;
			uip_appdata++;
		}
	}

	/* Allocate TX FID */
	fid = txfid_queue[--txfid];
	if(txfid==0)
		txfid= HFA384x_DRVR_FIDSTACKLEN_MAX;

	/*copy data to bap using fid*/
	txflag=1;
	result = hfa384x_rdwr_bap(BAP_WRITE, fid, TX_DEST_ADDR_OFF, 
		       				  uip_buf, uip_len);
	txflag=0;

	/* issue TX command */
	result = hfa384x_cmd(HFA384x_CMDCODE_TX | HFA384x_CMD_RECL_SET(1), fid);

	/* Check for TXOk bit */
	do{ 
		reg = hfa384x_getreg(HFA384x_EVSTAT);
		timeout_count++;
	}while((!HFA384x_EVSTAT_ISTX(reg)) && (timeout_count<10));
	timeout_count = 0;

	/* if TX command fail restore fid */
	if (result)
    {
		txfid++;
		printf("TX command failed. Result = 0x%x\n", result); 
	}

	/*ack TX and other events */
	hfa384x_event_ack();

	/* reset uip_len */
	uip_len =0;

	/* reset WDE bit to clear WDT*/			
	RESET_WDE();

	return;
}


/*----------------------------------------------------------------
* find_CF_mode()
* 
* This function find the CF mode: fixed disk(memory) or IO network
----------------------------------------------------------------*/
void find_CF_mode(void)
{

	unsigned int xdata connect_timeout = 0;

	/* Check if CF is connected for 10 seconds*/
	while ((connect_timeout <50)&&(!CFConnected()))
	{
		if(!CFConnected())
		{
			printf("CF not connected. Connect the card and Try again.\n");
			//return;		
		}
		connect_timeout++;
	}

	if(!CFConnected())
	{
		cis_device = CISTPL_FUNCID_NOT_SUPPORTED;
    	return;
	}

	/* Reset CF card */
	CF_RESET_ON() 
	delay_nops(4);
	CF_RESET_OFF();

	/* Read CIS table */
	ReadCISTable();

	/* if card not supported reset cis_device variable*/
	if ((cis_device != CISTPL_FUNCID_NETWORK)&&(cis_device != CISTPL_FUNCID_FIXED))
		cis_device = CISTPL_FUNCID_NOT_SUPPORTED;

	/*if cf card = fixed disk disable ECG send button switch */
	if (cis_device == CISTPL_FUNCID_FIXED)
	{
		DISABLE_INT0_IRQ();
		if (CFIDDrive())
			printf("Identify drive command error\n");
	}

//#if PRINT_WIFI
	printf("cis_device = 0x%Bx\n", cis_device);
//#endif

	return;

}

