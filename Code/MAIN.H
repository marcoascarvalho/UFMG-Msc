/*       浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様融
         �    TITLE:  Keil C51 v7.00 port of Adam Dunkels' uIP v0.9 �
         � REVISION:  VER 0.0                                       �
         � REV.DATE:  30-01-05                                      �
         �  ARCHIVE:                                                �
         �   AUTHOR:  Murray R. Van Luyn, 2005.                     �
         藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様夕        */

/*  敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳�
    �  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS  � 
    �  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED  � 
    �  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE � 
    �  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY    � 
    �  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL � 
    �  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  � 
    �  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS      � 
    �  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,       � 
    �  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING          � 
    �  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS � 
    �  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.       � 
    青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳�  */


#ifndef MAIN_H
#define MAIN_H

#include "uip.h"
//#include "etherdev.h"
#include "wifidev.h"
#include "uip_arp.h"
//#include "webclient.h"
#include "ecg_app.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

#endif /* MAIN_H */
