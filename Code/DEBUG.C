/*----------------------------------------------------------------
* hfa384x_cmd_transmit
*
* Instructs the firmware to transmit a frame previously copied
* to a given buffer.  This function returns immediately, the Tx
* results are available via the Tx or TxExc events (if the frame
* control bits are set).  The reclaim argument specifies if the 
* FID passed will be used by the f/w tx process or returned for
* use w/ another transmit command.  If reclaim is set, expect an 
* Alloc event signalling the availibility of the FID for reuse.
*
* NOTE: hw->cmdlock MUST BE HELD before calling this function!
*
* Arguments:
*	hw		device structure
*	reclaim		[0|1] indicates whether the given FID will
*			be handed back (via Alloc event) for reuse.
*			(host order)
*	qos		[0-3] Value to put in the QoS field of the 
*			tx command, identifies a queue to place the 
*			outgoing frame in.
*			(host order)
*	fid		FID of buffer containing the frame that was
*			previously copied to MAC memory via the bap.
*			(host order)
*
* Returns: 
*	0		success
*	>0		f/w reported failure - f/w status code
*	<0		driver reported error (timeout|bad arg)
*
* Side effects:
*	hw->resp0 will contain the FID being used by async tx
*	process.  If reclaim==0, resp0 will be the same as the fid
*	argument.  If reclaim==1, resp0 will be the different and
*	is the value to watch for in the Tx|TxExc to indicate completion
*	of the frame passed in fid.
*
* Call context:
*	process thread 
----------------------------------------------------------------*/
//int hfa384x_cmd_transmit(/*hfa384x_t *hw,*/ int reclaim, int qos, int fid)
//{
//	int	result = 0;
//	hfa384x_metacmd_t cmd;
//
//	cmd.cmd = HFA384x_CMD_CMDCODE_SET(HFA384x_CMDCODE_TX) |
//		HFA384x_CMD_RECL_SET(reclaim) |
//		HFA384x_CMD_QOS_SET(qos); // manual nao faz referencia a qos
//	cmd.parm0 = fid;
//	cmd.parm1 = 0;
//	cmd.parm2 = 0;
//
//	result = hfa384x_docmd_wait(/*hw,*/ &cmd);
//	
//	return result;
//}

/*----------------------------------------------------------------
* hfa384x_drvr_txframe
*
* Takes a frame from prism2sta and queues it for transmission.
*
* Arguments:
*	hw		device structure
*	skb		packet buffer struct.  Contains an 802.11
*			data frame.
*   p80211_hdr      points to the 802.11 header for the packet.
* Returns: 
*	0		Success and more buffs available
*	1		Success but no more buffs
*	2		Allocation failure
*	3		MAC Tx command failed
*	4		Buffer full or queue busy
*
* Side effects:
*
* Call context:
*	process thread
----------------------------------------------------------------*/
//int hfa384x_drvr_txframe(/*hfa384x_t *hw,*/
//						 struct sk_buff *skb, p80211_hdr_t *p80211_hdr/*, 
//						 p80211_metawep_t *p80211_wep*/)
//{
// verificar nesta funcao como vai tratar a questao do buffer skb
//	hfa384x_tx_frame_t	txdesc;
//	int			macq = 0;
//	int			fid;
//	int			result;

	/* Build Tx frame structure */
	/* Set up the control field */
//	memset(&txdesc, 0, sizeof(txdesc));

/* Tx complete and Tx exception disable per dleach.  Might be causing 
 * buf depletion 
 */
/*Setup TxControl Field */
// #define DOBOTH 1
//#if DOBOTH
//	txdesc.tx_control = 
//		HFA384x_TX_MACPORT_SET(0) | HFA384x_TX_STRUCTYPE_SET(1) | 
//		HFA384x_TX_TXEX_SET(1) | HFA384x_TX_TXOK_SET(1);	
//#elif DOEXC
//	txdesc.tx_control = 
//		HFA384x_TX_MACPORT_SET(0) | HFA384x_TX_STRUCTYPE_SET(1) |
//		HFA384x_TX_TXEX_SET(1) | HFA384x_TX_TXOK_SET(0);	
//#else
//	txdesc.tx_control = 
//		HFA384x_TX_MACPORT_SET(0) | HFA384x_TX_STRUCTYPE_SET(1) |
//		HFA384x_TX_TXEX_SET(0) | HFA384x_TX_TXOK_SET(0);	
//#endif

	/* nao vou suportar wep por enquanto */
	
	/* if we're using host WEP, increase size by IV+ICV */
	//if (p80211_wep->data) {
	//	txdesc.data_len = host2hfa384x_16(skb->len+8);
	//	//		txdesc.tx_control |= HFA384x_TX_NOENCRYPT_SET(1);
	//} else {
	//	txdesc.data_len =  host2hfa384x_16(skb->len);
	//}

	//txdesc.tx_control = host2hfa384x_16(txdesc.tx_control);
	
	/* copy the header over to the txdesc */
//	memcpy(&(txdesc.frame_control), p80211_hdr, sizeof(p80211_hdr_t));

	/* Since tbusy is set whenever the stack is empty, there should 
	 * always be something on the stack if we get to this point.
	 * [MSM]: NOT TRUE!!!!! so I added the test of fid below.
	 */

	/* Allocate FID */

	//fid = txfid_queue_remove(hw);
//	fid = txfid_queue[0]; // mudar depois aqui
	
//	if ( fid == 0 ) { /* stack or queue was empty */
//		return 4;
//	}

	/* now let's get the cmdlock */
	//spin_lock(&hw->cmdlock); //ver que porra eh essa

	/* Copy descriptor+payload to FID */
        //if (p80211_wep->data) { 
		//result = hfa384x_copy_to_bap4(hw, HFA384x_BAP_PROC, fid, 0,
		//			      &txdesc, sizeof(txdesc),
		//			      p80211_wep->iv, sizeof(p80211_wep->iv),
		//			      p80211_wep->data, skb->len,
		//			      p80211_wep->icv, sizeof(p80211_wep->icv));
	//} else {
//		result = hfa384x_copy_to_bap4(/*hw,*/ HFA384x_BAP_PROC, fid, 0,
//					      &txdesc, sizeof(txdesc),
//					      /*skb->data*/ NULL, /*skb->len*/ 0,
//					      NULL, 0, NULL, 0);
	//}

// if ( result ) {
//		printf("copy_to_bap(%04x, %d, %d) failed, result=0x%x\n", 
//			fid,
//		 	sizeof(txdesc), 
	 		/*skb->len,*/
//			result);

		/* put the fid back in the queue */
		//txfid_queue_add(hw, fid); // ver como manipular isto depois

//		result = 3;
		//goto failed;
//	}

	/* Issue Tx command */
//	result = hfa384x_cmd_transmit(/*hw,*/ HFA384x_TXCMD_RECL, macq, fid);
		
//	if ( result != 0 ) {
		//txfid_queue_add(hw, fid);

//		printf("cmd_tx(%04x) failed, result=%d\n", fid, result);
//		result = 3;
		//goto failed;
//	}
	
	/* indicate we haven't any buffers, int_alloc will clear */
	//result = txfid_queue_empty(hw); //ver o que é isto direito
//failed:

	//spin_unlock(&hw->cmdlock);

//	return result;
//}








/*----------------------------------------------------------------
* hfa384x_interrupt
*
* Driver interrupt handler. Associate with Irq IO pin
*
* Arguments:
*	irq		irq number
*	dev_id		pointer to the device
*	regs		registers
*
* Returns: 
*	nothing
*
* Side effects:
*	May result in a frame being passed up the stack or an info
*	frame being handled.  
*
* Call context:
*	Ummm, could it be interrupt?
----------------------------------------------------------------*/
void hfa384x_interrupt(void) interrupt 2 using 1
{
	int xdata			reg;
	int xdata			ev_read = 0;
	
	for (;;ev_read++) 
	{
		if (ev_read >= prism2_irq_evread_max)
			break;

		/* Check swsupport reg magic # for card presence */
		reg = hfa384x_getreg(HFA384x_SWSUPPORT0);
		if ( reg != HFA384x_DRVR_MAGIC) 
		{
			printf("no magic.  Card removed?.\n");
			break;
		}		

		/* read the EvStat register for interrupt enabled events */
		reg = hfa384x_getreg(HFA384x_EVSTAT);
		
		/* AND with the enabled interrupts */
		reg &= hfa384x_getreg(HFA384x_INTEN);

		/* Handle the events */
		switch(reg)
		{
			case HFA384x_EVSTAT_RX: {
				hfa384x_int_rx();
				hfa384x_setreg(HFA384x_EVACK_RX_SET(1), HFA384x_EVACK);
				break;
			}
			case HFA384x_EVSTAT_WTERR: {
				printf("Error: WTERR interrupt received (unhandled).\n");
				hfa384x_setreg(HFA384x_EVACK_WTERR_SET(1), HFA384x_EVACK);
				break;
			}	
			case HFA384x_EVSTAT_INFDROP: {
//				hfa384x_int_infdrop();
				hfa384x_setreg(HFA384x_EVACK_INFDROP_SET(1), HFA384x_EVACK);
				break;
			}
//			case HFA384x_EVSTAT_ISBAP_OP(reg) {
//			/* Disable the BAP interrupts */
//			hfa384x_events_nobap(hw);
//			}
			case HFA384x_EVSTAT_ALLOC: {
//				hfa384x_int_alloc();
				hfa384x_setreg(HFA384x_EVACK_ALLOC_SET(1), HFA384x_EVACK);
				break;
			}
			case HFA384x_EVSTAT_DTIM: {
//				hfa384x_int_dtim();
				hfa384x_setreg(HFA384x_EVACK_DTIM_SET(1), HFA384x_EVACK);
				break;
			}
#ifdef CMD_IRQ
			case HFA384x_EVSTAT_CMD: {
				hfa384x_int_cmd();
				hfa384x_setreg(HFA384x_EVACK_CMD_SET(1), HFA384x_EVACK);
				break;
			}
#endif

		/* allow the evstat to be updated after the evack */
		delay_nops(20);
		}
	}

}

#ifdef CMD_IRQ
/*----------------------------------------------------------------
* hfa384x_int_cmd
*
* Handles command completion event.
*
* Arguments:
*	wlandev		wlan device structure
*
* Returns: 
*	nothing
*
* Side effects:
*
* Call context:
*	interrupt
----------------------------------------------------------------*/
void hfa384x_int_cmd()
{
	
	cmdflag = 1; 

	return;
}
#endif

/*----------------------------------------------------------------
* hfa384x_int_dtim
*
* Handles the DTIM early warning event.
*
* Arguments:
*	wlandev		wlan device structure
*
* Returns: 
*	nothing
*
* Side effects:
*
* Call context:
*	interrupt
----------------------------------------------------------------*/
//void hfa384x_int_dtim(wlandevice_t *wlandev)
//{
//#if 0
//	hfa384x_t		*hw = wlandev->priv;
//#endif
//	prism2sta_ev_dtim(wlandev);
//	return;
//}


/*----------------------------------------------------------------
* hfa384x_int_infdrop
*
* Handles the InfDrop event.
*
* Arguments:
*	wlandev		wlan device structure
*
* Returns: 
*	nothing
*
* Side effects:
*
* Call context:
*	interrupt
----------------------------------------------------------------*/
//void hfa384x_int_infdrop(wlandevice_t *wlandev)
//{
//#if 0
//	hfa384x_t		*hw = wlandev->priv;
//#endif
//
//	prism2sta_ev_infdrop(wlandev);
//
//	return;
//}


/*----------------------------------------------------------------
* hfa384x_int_info
*
* Handles the Info event.
*
* Arguments:
*	wlandev		wlan device structure
*
* Returns: 
*	nothing
*
* Side effects:
*
* Call context:
*	tasklet
----------------------------------------------------------------*/
//void hfa384x_int_info(wlandevice_t *wlandev)
//{
//	hfa384x_t		*hw = wlandev->priv;
//	UINT16			reg;
//	hfa384x_InfFrame_t	inf;
//	int			result;
//	DBFENTER;
//	/* Retrieve the FID */
//	reg = hfa384x_getreg(hw, HFA384x_INFOFID);
//
//	/* Retrieve the length */
//	result = hfa384x_copy_from_bap( hw, 
//		HFA384x_BAP_INT, reg, 0, &inf.framelen, sizeof(UINT16));
//	if ( result ) {
//		WLAN_LOG_DEBUG(1, 
//			"copy_from_bap(0x%04x, 0, %d) failed, result=0x%x\n", 
//			reg, sizeof(inf), result);
//		goto failed;
//	}
//	inf.framelen = hfa384x2host_16(inf.framelen);
//
//	/* Retrieve the rest */
//	result = hfa384x_copy_from_bap( hw, 
//		HFA384x_BAP_INT, reg, sizeof(UINT16),
//		&(inf.infotype), inf.framelen * sizeof(UINT16));
//	if ( result ) {
//		WLAN_LOG_DEBUG(1, 
//			"copy_from_bap(0x%04x, 0, %d) failed, result=0x%x\n", 
//			reg, sizeof(inf), result);
//		goto failed;
//	}

//	prism2sta_ev_info(wlandev, &inf);
//failed:
//	DBFEXIT;
//	return;
//}


/*----------------------------------------------------------------
* hfa384x_int_txexc
*
* Handles the TxExc event.  A Transmit Exception event indicates
* that the MAC's TX process was unsuccessful - so the packet did
* not get transmitted.
*
* Arguments:
*	wlandev		wlan device structure
*
* Returns: 
*	nothing
*
* Side effects:
*
* Call context:
*	tasklet  
----------------------------------------------------------------*/
//void hfa384x_int_txexc(/*wlandevice_t *wlandev*/)
//{
//	hfa384x_t		*hw = wlandev->priv;
//	int			status;
//	int			fid;
//	int			result = 0;
	
	/* Collect the status and display */
//	fid = hfa384x_getreg(/*hw,*/ HFA384x_TXCOMPLFID);
//	result = hfa384x_copy_from_bap(/*hw,*/ HFA384x_BAP_INT, fid, 0, &status, sizeof(status));
//	if ( result ) {
//		printf("copy_from_bap(0x%04x, 0, %d) failed, result=0x%x\n", 
//			fid, sizeof(status), result);
		//goto failed;
//	}
	//status = hfa384x2host_16(status);
//	prism2sta_ev_txexc(/*wlandev,*/ status);
//failed:

//	return;
//}


/*----------------------------------------------------------------
* hfa384x_int_tx
*
* Handles the Tx event.
*
* Arguments:
*	wlandev		wlan device structure
*
* Returns: 
*	nothing
*
* Side effects:
*
* Call context:
*	tasklet  
----------------------------------------------------------------*/
//void hfa384x_int_tx(/*wlandevice_t *wlandev*/)
//{
//	//hfa384x_t		*hw = wlandev->priv;
//	int			fid;
//	int			status;
//	int			result = 0;
//	
//	fid = hfa384x_getreg(/*hw,*/ HFA384x_TXCOMPLFID);
//	result = hfa384x_copy_from_bap(/*hw,*/ HFA384x_BAP_INT, fid, 0, &status, sizeof(status));
//	if ( result ) {
//		printf("copy_from_bap(0x%04x, 0, %d) failed, result=0x%x\n", 
//			fid, sizeof(status), result);
//		//goto failed;
//	}
//	//status = hfa384x2host_16(status);
//	prism2sta_ev_tx(/*wlandev,*/ status);
////failed:

//	return;
//}

/*----------------------------------------------------------------
* hfa384x_int_rx
*
* Handles the Rx event.
*
* Arguments:
*
* Returns: 
*	nothing
*
* Side effects:
*
* Call context:
*	tasklet  
----------------------------------------------------------------*/
int hfa384x_int_rx()
{

	int xdata		rxfid;
	hfa384x_rx_frame_t xdata	rxdesc;
	int xdata		result;
	int xdata       hdrlen;
#if USING_HEADERLEN
	int xdata       fc;
#endif
//	p80211_rxmeta_t	xdata *rxmeta; 
	int xdata       *skb = NULL;
	int xdata *datap;
	int xdata i;
	
	/* Get the FID */
	rxfid = hfa384x_getreg(HFA384x_RXFID);
	/* Get the descriptor (including headers) */
	result = hfa384x_copy_from_bap(HFA384x_BAP_INT, 
			rxfid, 
			0, 
			&rxdesc, 
			sizeof(rxdesc));
	if (result) 
	{
		printf("copy_from_bap(0x%04x, %d, %d) failed, result=0x%x\n", 
			rxfid, 
			0, 
			sizeof(rxdesc),
			result);
		return;
	}

	/* Now handle frame based on port# */
	if( HFA384x_RXSTATUS_MACPORT_GET(rxdesc.status)==0)
	{
#if USING_HEADERLEN		
		fc = rxdesc.frame_control;
		
		/* If exclude and we receive an unencrypted, drop it */
		hdrlen = p80211_headerlen(fc);
#endif

		/* Allocate the buffer, note CRC (aka FCS). pballoc */
		/* assumes there needs to be space for one */
		if (rxdesc.data_len + WLAN_HDR_A4_LEN + WLAN_CRC_LEN + 2 < UIP_BUFSIZE)
		{	
			skb = malloc(rxdesc.data_len +
#if USING_HEADERLEN	
											hdrlen 
#else
											WLAN_HDR_A4_LEN	
#endif
															+ WLAN_CRC_LEN + 2); /* a little extra */

			if ( ! skb ) 
			{
				printf("alloc_skb failed.\n");
			}

			/* theoretically align the IP header on a 32-bit word. */
			//if ( hdrlen == WLAN_HDR_A4_LEN )
			//	skb_reserve(skb, 2); // verificar como tratar isto
	
			/* Copy the 802.11 hdr to the buffer */
			//datap = skb_put(skb, WLAN_HDR_A3_LEN);
			//skb_tmp = nao sei como tratar isto. datap deve ter a ultima posicao ocupada de skb 
			datap = skb;
			memcpy(datap, &rxdesc.frame_control, WLAN_HDR_A3_LEN);
	
			/* Snag the A4 address if present */
			if (hdrlen == WLAN_HDR_A4_LEN) 
			{
				datap = skb+WLAN_HDR_A3_LEN;
				//skb_put(skb, WLAN_ADDR_LEN);
				memcpy(datap, &rxdesc.address4, WLAN_ADDR_LEN);
			}

			/* Copy the payload data to the buffer */
			if ( rxdesc.data_len > 0 ) {
				//datap = skb_put(skb, rxdesc.data_len); // nao sei como tratar ainda
				datap = skb + WLAN_HDR_A4_LEN + 2;
				result = hfa384x_copy_from_bap(
					HFA384x_BAP_INT, rxfid, HFA384x_RX_DATA_OFF, 
					datap, rxdesc.data_len);

				if ( result ) 
				{
					printf("copy_from_bap(0x%04x, %d, %d) failed, result=0x%x\n", 
						rxfid, 
						HFA384x_RX_DATA_OFF, 
						rxdesc.data_len,
						result);
					return;
				}
			}
			/* copy content of skb->data to rxbuff */
			for (i=0 ;rxdesc.data_len; i++)
			{
				*(uip_buf + i) = *(datap++);
			}
			/* the prism2 cards don't return the FCS */
			//datap = skb_put(skb, WLAN_CRC_LEN); nao sei como tratar ainda
			//memset (datap, 0xff, WLAN_CRC_LEN);
			//skb->mac.raw = skb->data; // nao sei o q eh isto tratar depois

			//prism2sta_ev_rx(/*wlandev,*/ skb);
		}
		else
		{
			printf("Could not allocate buf: datalen > BUFSIZE");
		}
	}		
	else
	{
			printf("Received frame on unsupported port=%d\n",
				HFA384x_RXSTATUS_MACPORT_GET(rxdesc.status) );
	}	
	
	free(skb);

	return rxdesc.data_len;
}

/*----------------------------------------------------------------
* prism2sta_ev_txexc
*
* Handles the TxExc event.  A Transmit Exception event indicates
* that the MAC's TX process was unsuccessful - so the packet did
* not get transmitted.
*
* Arguments:
*	wlandev		wlan device structure
*	status		tx frame status word
*
* Returns: 
*	nothing
*
* Side effects:
*
* Call context:
*	interrupt
----------------------------------------------------------------*/
//void prism2sta_ev_txexc(/*wlandevice_t *wlandev,*/ int status)
//{
//	
//	printf("TxExc status=0x%x.\n", status);
//
//	return;
//}


/*----------------------------------------------------------------
* prism2sta_ev_tx
*
* Handles the Tx event.
*
* Arguments:
*	wlandev		wlan device structure
*	status		tx frame status word
* Returns: 
*	nothing
*
* Side effects:
*
* Call context:
*	interrupt
----------------------------------------------------------------*/
//void prism2sta_ev_tx(/*wlandevice_t *wlandev,*/ int status)
//{
//	printf("Tx Complete, status=0x%04x\n", status);
//	/* update linux network stats */
//	//wlandev->linux_stats.tx_packets++;
//	return;
//}

#if USING_HEADERLEN
/*---------------------------------------------------------------
* 802.11 functions
* Calculate 802.11 header lenght
*----------------------------------------------------------------
/* ftcl in HOST order */
static int p80211_headerlen(int fctl)
{
	int xdata hdrlen = 0;

	switch ( WLAN_GET_FC_FTYPE(fctl) ) {
	case WLAN_FTYPE_MGMT:
		hdrlen = WLAN_HDR_A3_LEN;
		break;
	case WLAN_FTYPE_DATA:
		hdrlen = WLAN_HDR_A3_LEN;
		if ( WLAN_GET_FC_TODS(fctl) && WLAN_GET_FC_FROMDS(fctl) ) {
			hdrlen += WLAN_ADDR_LEN;
		}
		break;
	case WLAN_FTYPE_CTL:
		hdrlen = WLAN_CTL_FRAMELEN(WLAN_GET_FC_FSTYPE(fctl)) - WLAN_FCS_LEN; 
		break;
	default:
		hdrlen = WLAN_HDR_A3_LEN;
	}
	
	return hdrlen;
}
#endif

/*----------------------------------------------------------------
* hfa384x_copy_to_bap
*
* Copies a collection of bytes to the MAC controller memory via
* one set of BAP registers.
*
* Arguments:
*	hw		device structure
*	bap		[0|1] which BAP to use
*	id		FID or RID, destined for the select register (host order)
*	offset		An _even_ offset into the buffer for the given
*			FID/RID.  We haven't the means to validate this,
*			so be careful. (host order)
*	buf		ptr to array of bytes
*	len		length of data to transfer (in bytes)
*
* Returns: 
*	0		success
*	>0		f/w reported failure - value of offset reg.
*	<0		driver reported error (timeout|bad arg)
*
* Side effects:
*
* Call context:
*	process thread
*	interrupt
----------------------------------------------------------------*/
//char hfa384x_copy_to_bap(/*hfa384x_t *hw,*/ int bap, int id, int offset,
//				void *buf, int len)
//{
//	return hfa384x_copy_to_bap4(/*hw,*/ bap, id, offset, buf, len, NULL, 0, NULL, 0, NULL, 0);
//}
char hfa384x_copy_to_bap4(int bap, int id, int offset,
			 void *buf, char len1, void* buf2, char len2,
			 void *buf3, char len3, void *buf4, char len4)
{
	char xdata		result = 0;
	char xdata		*d;
	char xdata selectreg;
	char xdata offsetreg;
	char xdata datareg;
	char xdata i;
	int xdata			reg;
	int xdata data_;



	/* Validate bap, offset, buf, and len */
	if ( (bap > 1) || 
	     (offset > HFA384x_BAP_OFFSET_MAX) || 
	     (offset % 2) ||
	     (buf == NULL) ||
	     (len1+len2+len3+len4 > HFA384x_BAP_DATALEN_MAX) )
	{
	     	return 1;
	} 
	else 
	{
		selectreg = (bap == 1) ? HFA384x_SELECT1 : HFA384x_SELECT0;
		offsetreg = (bap == 1) ? HFA384x_OFFSET1 : HFA384x_OFFSET0;
		datareg =   (bap == 1) ? HFA384x_DATA1   : HFA384x_DATA0;

		/* Write id to select reg */
		hfa384x_setreg(id, selectreg);
		delay_nops(10);

		/* Write offset to offset reg */
		hfa384x_setreg(offset, offsetreg);

		/* Wait for offset[busy] to clear (see BAP_TIMEOUT) */
		i = 0; 
		do {
			reg = hfa384x_getreg(offsetreg);
			if ( i > 0 ) delay_nops(10);
			i++;
//#if SIMULATE
//			reg = 0x0000;
//#endif
		} while ( i < prism2_bap_timeout && HFA384x_OFFSET_ISBUSY(reg));

		if ( HFA384x_OFFSET_ISBUSY(reg) )
		{
			/* If timeout, return reg */
			result = reg;
		} 
		else if ( HFA384x_OFFSET_ISERR(reg) )
		{
			/* If offset[err] == 1, return reg */
			result = reg;
		} 
		else 
		{
			d = (char*)buf;
			/* Write even(len1) buf contents to data reg */
			for ( i = 0; i < (len1 & 0xfffe); i+=2 ) 
			{
				hfa384x_setreg(*(int*)(&(d[i])), datareg); 
			}
			if (len1 & 1) 
			{
				char *b = (char *) &data_;
				b[0] = d[len1-1];
				if (buf2 != NULL) 
				{
					d = (char*)buf2;
					b[1] = d[0];
					len2--;		
//					buf2++;
				}
				hfa384x_setreg(data_, datareg);
			}
			if ((buf2 != NULL) && (len2 > 0)) 
			{
				/* Write even(len2) buf contents to data reg */
				d = (char*)buf2;
				for ( i = 0; i < (len2 & 0xfffe); i+=2 ) 
				{
					hfa384x_setreg(*(int*)(&(d[i])), datareg);
				}
			}	
			if (len2 & 1) 
			{
				char *b = (char *) &data_;
				b[0] = d[len2-1];
				if (buf3 != NULL) 
				{
					d = (char*)buf3;
					b[1] = d[0];
					len3--;	
//					buf3++;
				}
				hfa384x_setreg(data_, datareg);
			}
		}

		if ((buf3 != NULL) && (len3 > 0)) 
		{
			/* Write even(len3) buf contents to data reg */
				d = (char*)buf3;
			for ( i = 0; i < (len3 & 0xfffe); i+=2 ) 
			{
				hfa384x_setreg(*(int*)(&(d[i])), datareg);
			}
			if (len3 & 1) 
			{
				char *b = (char *) &data_;
				b[0] = d[len3-1];
				if (buf4 != NULL) 
				{
					d = (char*)buf4;
					b[1] = d[0];
					len4--;
//						buf4++;
				}
				hfa384x_setreg(data_, datareg);
			}
		}
		if ((buf4 != NULL) && (len4 > 0)) 
		{
			/* Write even(len4) buf contents to data reg */
			d = (char*)buf4;
			for ( i = 0; i < (len4 & 0xfffe); i+=2 ) 
			{
				hfa384x_setreg(*(int*)(&(d[i])), datareg);
			}
			if (len4 & 1) 
			{
				char *b = (char *) &data_;
				b[0] = d[len4-1];
				b[1] = 0;
				hfa384x_setreg(data_, datareg);
			}
		}
		printf("ctb2 %d id %04x o %d %d %d %d %d\n", bap, id, offset, len1, len2, len3, len4);
	}
	
	if (result)
		printf("copy_to_bap() failed.\n");

	return result;
}


/*----------------------------------------------------------------
 hfa384x_copy_from_bap

* Copies a collection of bytes from the MAC controller memory via
* one set of BAP registers.
*
* Arguments:
*	bap		[0|1] which BAP to use
*	id		FID or RID, destined for the select register (host order)
*	offset		An _even_ offset into the buffer for the given
*			FID/RID.  We haven't the means to validate this,
*			so be careful. (host order)
*	buf		ptr to array of bytes
*	len		length of data to transfer in bytes
*
* Returns: 
*	0		success
*	>0		f/w reported failure - value of offset reg.
*	<0		driver reported error (timeout|bad arg)
*
* Side effects:
*
* Call context:
*	process thread
*	interrupt
----------------------------------------------------------------*/
char hfa384x_copy_from_bap(int bap, int id, int offset,
				void *buf, int len)
{
	char xdata 	result = 0;
	char xdata	*d = (char*)buf;
	char xdata	selectreg;
    char xdata	offsetreg;
    char xdata	datareg;
	char xdata	i;
	int xdata	reg = 0;
	
	/* Validate bap, offset, buf, and len */
	if ( (bap > 1) || 
	     (offset > HFA384x_BAP_OFFSET_MAX) || 
	     (offset % 2) ||
	     (buf == NULL) ||
	     (len > HFA384x_BAP_DATALEN_MAX) )
	{
	     	result = 1;
	} 
	else 
	{
		selectreg = (bap == 1) ?  HFA384x_SELECT1 : HFA384x_SELECT0 ;
		offsetreg = (bap == 1) ?  HFA384x_OFFSET1 : HFA384x_OFFSET0 ;
		datareg =   (bap == 1) ?  HFA384x_DATA1 : HFA384x_DATA0 ;

		/* Write id to select reg */
		hfa384x_setreg(id, selectreg);
		/* Write offset to offset reg */
		hfa384x_setreg(offset, offsetreg);
		/* Wait for offset[busy] to clear (see BAP_TIMEOUT) */
		i = 0; 
		do {
			reg = hfa384x_getreg(offsetreg);
			if ( i > 0 ) delay_nops(10);
			i++;
//#if SIMULATE
//			reg = 0x0000;
//#endif
		} while ( i < prism2_bap_timeout && HFA384x_OFFSET_ISBUSY(reg));

		if ( HFA384x_OFFSET_ISBUSY(reg) )
		{
			/* If timeout, return -ETIMEDOUT */
			result = reg;
		} 
		else if ( HFA384x_OFFSET_ISERR(reg) )
		{
			/* If offset[err] == 1, return -EINVAL */
			result = reg;
		} 
		else 
		{
		/* Read even(len) buf contents from data reg */
			for ( i = 0; i < (len & 0xfffe); i+=2 ) 
			{
				*(int*)(&(d[i])) = hfa384x_getreg(datareg);
			}
			/* If len odd, handle last byte */
			if ( len % 2 )
			{
				reg = hfa384x_getreg(datareg);
				d[len-1] = ((char*)(&reg))[0];
			}
		}

		/* According to Intersil errata dated 9/16/02:
	
		"In PRISM PCI MAC host interface, if both BAPs are concurrently 
		 requesing memory access, both will accept the Ack.   There is no
		 firmware workaround possible.  To prevent BAP access failures or
		 hang conditions the host MUST NOT access both BAPs in sucession
		 unless at least 5us elapses between accesses.  The safest choice
		 is to USE ONLY ONE BAP for all data movement operations."
	
		 What this means:

		 We have to serialize ALL BAP accesses, and furthermore, add a 5us
		 delay after access if we're using a PCI platform.
	
		 Unfortunately, this means we have to lock out interrupts througout
		 the entire BAP copy.
	
		 It remains to be seen if "BAP access" means "BAP setup" or the more
		 literal definition of "copying data back and forth"  I'm erring for
		 the latter, safer definition.  -- SLP.
	
		*/


	}

	if (result) {
		printf("copy_from_bap(0x%04x, 0, %d) failed, result=0x%x\n", 
			  reg, len, result);
	}

	return result;
}


