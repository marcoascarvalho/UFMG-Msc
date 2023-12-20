/********************************************************************
* Filename: webclient.c
* Description: This file implements a HTTP client for ECG board
* Developed by: Marco Carvalho
* Created on: 30/03/05
/*********************************************************************/

#include <stdio.h>

#include "uip.h"
#include "webclient.h"

#include <string.h>

#define WEBCLIENT_TIMEOUT 100

#define WEBCLIENT_STATE_STATUSLINE 0
#define WEBCLIENT_STATE_HEADERS    1
#define WEBCLIENT_STATE_DATA       2
#define WEBCLIENT_STATE_CLOSE      3
#define WEBCLIENT_STATE_SENDDATA   4

#define HTTPFLAG_NONE   0
#define HTTPFLAG_OK     1
#define HTTPFLAG_MOVED  2
#define HTTPFLAG_ERROR  3

#define ISO_nl       0x0a
#define ISO_cr       0x0d
#define ISO_space    0x20

#define WEBCLIENT_CLOSED()  ENABLE_TMR0_IRQ()
	
#define WEBCLIENT_ABORTED() ENABLE_TMR0_IRQ(); \
							s.state = WEBCLIENT_STATE_CLOSE; \

#define WEBCLIENT_TIMEDOUT() WEBCLIENT_ABORTED()
							
xdata struct webclient_state s;

unsigned char xdata ecg_send_button;

/*-----------------------------------------------------------------------------------*/
void
webclient_init(void)
{
	ecg_send_button = 0;
	uip_flags = UIP_CLOSE;
	s.state = WEBCLIENT_STATE_STATUSLINE;
	ecg_state = ECG_STATE_DPM_IDLE;
	tick_count = 1;  
}

/*-----------------------------------------------------------------------------------*/
unsigned char
webclient_post(char *host , u16_t port, char *file)
{
	struct uip_conn xdata *conn;
	static u16_t xdata ipaddr[2];
  
	/* open a uip connection */
    uip_ipaddr(ipaddr, 192,168,1,50); 
  	conn = uip_connect(ipaddr, htons(port));

	printf("Try to open a connection with the server\n");

	if(conn == NULL) 
	{
    	printf("conexão nula retornando 0\n");
		return 0;
	}

	/* Setup s structure */  
	s.port = port;
	strncpy(s.file, file, sizeof(s.file));
	strncpy(s.host, host, sizeof(s.host));
    s.postrequestleft = sizeof(http_post) - 1 + 1 +
                    	sizeof(http_10) - 1 +
    	                sizeof(http_crnl) - 1 +
    	                sizeof(http_host) - 1 +
    	                sizeof(http_crnl) - 1 +
		                sizeof(http_content_type) - 1 +
		                sizeof(http_app_wwwformurlenc_type) -1 +
		                sizeof(http_crnl) - 1 +
    	                strlen(http_user_agent_fields) +
		                sizeof(http_crnl) - 1 +
    	                strlen(s.file) + strlen(s.host) +
		                sizeof(http_crnl) - 1 + sizeof(http_crnl) - 1 +
		                sizeof(http_param) - 1;
    s.postrequestptr = 0;
	s.httpheaderlineptr = 0;

  	return 1;
}
/*-----------------------------------------------------------------------------------*/
static unsigned char copy_string(unsigned char *dest,
	    const unsigned char *src, unsigned char len)
{
	strcpy(dest, src);
	return len;

}
/*-----------------------------------------------------------------------------------
/ static void senddata(void)
/ Send http post header
/*-----------------------------------------------------------------------------------*/
static void senddata(void)
{
  u16_t xdata len;
  char xdata *postrequest;
  char xdata *cptr;
 
	if(s.postrequestleft > 0) 
	{
		//cptr = postrequest = (char *)uip_appdata;

		cptr = (char *)uip_appdata;
		postrequest = (char *)uip_appdata;

		/*create http post header */
		/* POST ecg.php http/1.0 */
		cptr += copy_string(cptr, http_post, sizeof(http_post) - 1);  //POST 
    	cptr += copy_string(cptr, s.file, strlen(s.file));			 //ecg.php
    	*cptr++ = ISO_space;										 // 
    	cptr += copy_string(cptr, http_10, sizeof(http_10) - 1);	     //http/1.0

	    cptr += copy_string(cptr, http_crnl, sizeof(http_crnl) - 1);  //<CR> <NL>

	    /* Host: 192.168.1.50 */
    	cptr += copy_string(cptr, http_host, sizeof(http_host) - 1);  //Host:
    	cptr += copy_string(cptr, s.host, strlen(s.host));			 //192.168.1.50

	    cptr += copy_string(cptr, http_crnl, sizeof(http_crnl) - 1);  //<CR> <NL>

		/* User-Agent: ECG Webclient */
    	cptr += copy_string(cptr, http_user_agent_fields,             //User-Agent: ECG Webclient
		       strlen(http_user_agent_fields));

		cptr += copy_string(cptr, http_crnl, sizeof(http_crnl) - 1);  //<CR> <NL>

		/*Content-Type: application/x-www-form-urlencoded */
		cptr += copy_string(cptr, http_content_type, sizeof(http_content_type) - 1);  //Content-Type:
		cptr += copy_string(cptr, http_app_wwwformurlenc_type, 	       //application/x-www-form-urlencoded
		        sizeof(http_app_wwwformurlenc_type) - 1); 

		cptr += copy_string(cptr, http_crnl, sizeof(http_crnl) - 1);  //<CR> <NL>
		cptr += copy_string(cptr, http_crnl, sizeof(http_crnl) - 1);  //<CR> <NL>
		
		/* ecg_data= */
		cptr += copy_string(cptr, http_param, sizeof(http_param) - 1);  //http_ecgdata
		
		/* call ecg prepare data function */
		ecg_prepare_data();

		/*format post request lenght */    
    	len = s.postrequestleft+uip_len > uip_mss()? uip_mss():s.postrequestleft+uip_len;
		//len = s.postrequestleft > uip_mss()? uip_mss():s.postrequestleft;		

		//printf("dentro de senddata() - len = %d\n", len);

	    /* send http post header */
		uip_send(&(postrequest[s.postrequestptr]), len);
	}
}  
/*-----------------------------------------------------------------------------------*/
// nao entendi o que esta funcao faz
static void acked(void)
{
	u16_t xdata len;
  
	if(s.postrequestleft > 0) 
	{
    	len = s.postrequestleft > uip_mss()? uip_mss():s.postrequestleft;
    	s.postrequestleft -= len;
    	s.postrequestptr += len;
	}
}
/*-----------------------------------------------------------------------------------*/
static u16_t parse_statusline(u16_t len)
{
	char xdata *cptr;
  
	while(len > 0 && s.httpheaderlineptr < sizeof(s.httpheaderline)) 
	{
    	s.httpheaderline[s.httpheaderlineptr] = *uip_appdata;
    	++uip_appdata;
    	--len;
    	if(s.httpheaderline[s.httpheaderlineptr] == ISO_nl) 
		{

			if((strncmp(s.httpheaderline, http_10, sizeof(http_10) - 1) == 0) ||
	 		   (strncmp(s.httpheaderline, http_11, sizeof(http_11) - 1) == 0)) 
			{
				cptr = &(s.httpheaderline[9]);
				s.httpflag = HTTPFLAG_NONE;
				if(strncmp(cptr, http_200, sizeof(http_200) - 1) == 0) 
				{
	  				/* 200 OK */
	  				s.httpflag = HTTPFLAG_OK;
					printf("Received 200 OK\n"); 
				} 
				else if(strncmp(cptr, http_301, sizeof(http_301) - 1) == 0 ||
		  			    strncmp(cptr, http_302, sizeof(http_302) - 1) == 0) 
				{
	  				/* 301 Moved permanently or 302 Found. Location: header line
	     			will contain the new location. */
	  				s.httpflag = HTTPFLAG_MOVED;
				} 
				else 
				{
	  				s.httpheaderline[s.httpheaderlineptr - 1] = 0;
				}
      		} 
			else 
			{
				uip_abort();
				//webclient_aborted();
				WEBCLIENT_ABORTED();
				return 0;
      		}
      
      		/* We're done parsing the status line, so we reset the pointer
	 		and start parsing the HTTP headers.*/
      		s.httpheaderlineptr = 0;
      		s.state = WEBCLIENT_STATE_HEADERS;
      		break;
		} 
		else 
		{
     		++s.httpheaderlineptr;
    	}
	}
	
	return len;
}

/*-----------------------------------------------------------------------------------*/
static void newdata(void)
{
	u16_t xdata len;

	len = uip_datalen();

	if(s.state == WEBCLIENT_STATE_STATUSLINE) 
	{
		/* parse http status line and check if the data was received OK */
	    len = parse_statusline(len);
		/* if httpflag received was http OK, close connection */		
		if (s.httpflag == HTTPFLAG_OK)
		{
			s.state = WEBCLIENT_STATE_CLOSE;
		}
	}
}

/*-----------------------------------------------------------------------------------
/ void webclient_appcall(void)
/ Main function to http post application. It manages all the steps during the http post
/ open connection
/ We use the uip_ test functions to deduce why we were called. If uip_connected() is 
/ non-zero, we were called because a remote host has connected to us. If uip_newdata() 
/ is non-zero, we were called because the remote host has sent us new data, and if 
/ uip_acked() is non-zero, the remote host has acknowledged the data we	previously sent 
/ to it. 
-------------------------------------------------------------------------------------*/
void webclient_appcall(void)
{

	char xdata file[]="/ecg.php";
	char xdata host[]="192.168.1.50";

	/* at the first time we are not connected to the server. This is done when the 
	TCP/IP stack connect to the server */
	if(uip_connected()) 
	{
    	printf("Connection accepted by the server\n");
		s.timer = 0;
    	//s.state = WEBCLIENT_STATE_SENDDATA;
		s.state = WEBCLIENT_STATE_STATUSLINE;
    	return;
  	}

	//if(s.state == WEBCLIENT_STATE_SENDDATA)
	//{
	//	printf("Estou em if s.state == WEBCLIENT_STATE_SENDDATA\n");
		//senddata();
	//	s.state = WEBCLIENT_STATE_STATUSLINE;
	//}

	/* if Webclient state is close, abort connection and continue to capture data through */
	/* Timer 0 IRQ */
	if(s.state == WEBCLIENT_STATE_CLOSE) 
	{
		//printf("Estou em if s.state == WEBCLIENT_STATE_CLOSE\n");
		WEBCLIENT_CLOSED(); //enable TMR0 interrupt to continue acquire data
    	uip_abort();
		uip_flags = UIP_CLOSE;
		s.state = WEBCLIENT_STATE_STATUSLINE;
    	return;
  	}    
  
	/* If uIP tcp/IP aborted connection, continue acquiring data */
	if(uip_aborted()) 
	{
    	//printf("Estou em if uip_aborted()\n");
		WEBCLIENT_ABORTED();  //enable TMR0 interrupt to continue acquire data
		uip_flags = UIP_CLOSE;
		s.state = WEBCLIENT_STATE_STATUSLINE;
	}

	/* if uIP timeout, so try again */
	if(uip_timedout()) 
	{
    	//printf("Estou em if uip_timedout()\n");
		WEBCLIENT_TIMEDOUT(); 
		s.timer = 0;
		uip_flags = UIP_CLOSE;
		s.state = WEBCLIENT_STATE_STATUSLINE;
  	}
  
	if(uip_acked()) 
	{
    	//printf("Estou em if uip_acked()\n");
		s.timer = 0;
    	acked();
  	}

	/* if uIP request for newdata proceed with it */
	if(uip_newdata()) 
	{
    	//printf("Estou em if uip_newdata()\n");
		s.timer = 0;
		newdata();   //decode http protocol and call ecg function to send data
  	}

	//if(uip_rexmit() || uip_acked()) 
	//{
    //	printf("Estou em if uip_rexmit() || uip_acked()\n");
	//	printf("uip_rexmit() = %Bd\n", (char)uip_rexmit());
	//	printf("uip_acked() = %Bd\n", (char)uip_acked());
	//	senddata();
	//} 
	/*else */if(uip_poll()) 
	{
		//printf("Estou em if uip_poll()\n");
		++s.timer;
		if(s.timer == WEBCLIENT_TIMEOUT) 
		{
			//printf("s.timer == WEBCLIENT_TIMEOUT dentro de if uip_poll()\n");
			WEBCLIENT_TIMEDOUT();   //nao tratado ainda
			uip_abort();
			uip_flags = UIP_CLOSE;
			s.state = WEBCLIENT_STATE_STATUSLINE;
      		return;
		}
		senddata();
   	}

	/* if connection is closed and ecg_send_button pressed */
	if(uip_closed() && ecg_send_button) 
	{
		printf("Send HTTP POST request\n");
		s.port = 80;
		webclient_post(host, s.port, file);
		ecg_send_button = 0;
	}
	else
	{
		if(uip_closed())
		{
		    printf("Enter in power down mode through application\n");

			/* Enable switch IRQ */
			ENABLE_INT0_IRQ();

			/* reset WDE bit to clear WDT*/			
			DISABLE_GLOBAL_IRQ();
			ENABLE_WDT_WRITE();		
			WDE = 0;
			ENABLE_GLOBAL_IRQ();

			/* Change the state to Power Down */
			ecg_state = ECG_STATE_DPM_PWRDWN;;
		}
	}
}

