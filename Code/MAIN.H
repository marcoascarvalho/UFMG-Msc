/*       ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
         บ    TITLE:  Keil C51 v7.00 port of Adam Dunkels' uIP v0.9 บ
         บ REVISION:  VER 0.0                                       บ
         บ REV.DATE:  30-01-05                                      บ
         บ  ARCHIVE:                                                บ
         บ   AUTHOR:  Murray R. Van Luyn, 2005.                     บ
         ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ        */

/*  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
    ณ  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS  ณ 
    ณ  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED  ณ 
    ณ  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ณ 
    ณ  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY    ณ 
    ณ  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL ณ 
    ณ  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  ณ 
    ณ  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS      ณ 
    ณ  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,       ณ 
    ณ  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING          ณ 
    ณ  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS ณ 
    ณ  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.       ณ 
    ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู  */


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
