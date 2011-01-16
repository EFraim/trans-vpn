
#include <stdint.h>
#include <stdbool.h>

/* lpc2148 board */
#include <lpc2000/io.h>
#include <lpc2000/interrupt.h>

#define DBG(fmt, args...)
#define TRACE_ENTER(func) printf("-> %s\n", func)
#define TRACE_RETURN(func) printf("<- %s\n", func)
#define ASSERT(x)
void UIP_APPCALL() {}
void uip_log(char* msg) { (void)msg; }

#include "uip.h"
#include "uip_arp.h"
#include "endian.c"
#include "drivers/vic.c"
#include "drivers/net_timers.c"
#include "drivers/usb.h"
#include "drivers/usb-eth_dev.c"
#include "drivers/usb.c"

const struct uip_eth_addr ethaddr = {{ 0xDE, 0xAD, 0xBE, 0xEF, 0xAB, 0xBA }};


int main(int argc, char *argv[])
{
  uint16_t eth_hdr_type;
  int i;

  VPBDIV = 0x00000001;  /* PCLK = CCLK */
  PLLCFG  = 0x00000024; /* Fosc=12MHz, CCLK=60MHz */
  PLLCON  = 0x00000001; /* enable the PLL  */
  PLLFEED = 0x000000AA; /* feed sequence   */
  PLLFEED = 0x00000055;
  while (!(PLLSTAT & 0x00000400));
  PLLCON = 3;     // enable and connect
  PLLFEED = 0xAA;
  PLLFEED = 0x55;

  vicInit();

  DBG("Initializing USB Stack");
  usbInit();

  rndisInit();
  DBG("init 2");
  network_device_init();

  DBG("init 3");
  uip_init();

  uip_setethaddr(ethaddr);

  DBG("Starting USB Stack");
  interruptsEnable();
  usbConnect();

  while(1) {
    //if (xyz) DBG("is there a packet? bulk ep status = %x",_usbGetEPStatus(RNDIS_BULK_EP | USB_EP_OUT));
    uip_len = network_device_read();
    //if (xyz) DBG("len=%d",uip_len);
    if (uip_len > 0) {
      eth_hdr_type = (uip_buf[0xc] << 8) | uip_buf[0xd];
      if(eth_hdr_type == UIP_ETHTYPE_IP) {
        //DBG("Hey, got an IP packet!!!");
        uip_arp_ipin();
        uip_input();
        if(uip_len > 0) {
          uip_arp_out();
          //DBG("IP Reply! (%d bytes)",uip_len);
          //int j;
          //for (j=0; j<uip_len; j++) {
          //  DBG(">%d %02x ",j,uip_buf[j]);
          //}
          //DBG("");
          network_device_send();
        }
      }
      else if (eth_hdr_type == UIP_ETHTYPE_ARP) {
        //DBG("Hey, got an ARP packet!!!");
        uip_arp_arpin();
        //xyz = 1;
        if(uip_len > 0) {
          //DBG("ARP Reply!");
          network_device_send();
        }
      } else {
        DBG("Hey, got some weird packet, not IP, not ARP, type=%x",eth_hdr_type);
      }

    }


    if (net_timers_ip_expired()) {
      net_timers_ip_reset();

      for(i = 0; i < UIP_CONNS; i++) {
        uip_periodic(i);
        if(uip_len > 0) {
          uip_arp_out();
          network_device_send();
        }
      }
      if(net_timers_arp_expired()) {
        net_timers_arp_reset();
        uip_arp_timer();
      }
    }
  }
  return 0;
}
