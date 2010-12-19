
#define USE_DHCPD

#include <stdint.h>

#include "endian.c"

/* lpc2148 board */
#include <lpc2000/io.h>
#include <lpc2000/interrupt.h>

/* interrupts driver */
#include "drivers/vic.c"

/*******************************************************/
/* Timing                                              */
/*******************************************************/

#define CLOCKS_PCLK 60000000

#define ARP_TIMER_IN_MSEC (1000*10)
#define IP_TIMER_IN_MSEC (500)

static uint16_t arp_timer_counter = 0;
volatile static uint16_t arp_timer_expired = 0;

static uint16_t ip_timer_counter = 0;
volatile static uint16_t ip_timer_expired =0 ;

static          uint32_t debounce14 = 0;
static volatile uint32_t switch14   = 1;
static volatile uint32_t state_change = 0;

enum {
  NOT_CONNECTED,
  CONNECTED
} state = NOT_CONNECTED;

static volatile uint32_t udp_retrans = 0;
static volatile uint32_t udp_sync    = 0;
static volatile uint32_t udp_disconnected = 0;
static volatile uint32_t udp_poll_request = 0;

void systick_periodic_task(void) {

  if (udp_disconnected) udp_disconnected--;
  if (udp_retrans     ) udp_retrans     --;
  if (udp_sync        ) udp_sync        --;

  if ((udp_retrans==0 || udp_sync==0) && state==CONNECTED) udp_poll_request=1;

  /* check switches */
  if (debounce14) debounce14--;
  if (switch14==0 && debounce14==0 && (FIO0PIN & BIT14) == 0) {
    debounce14 = 50;
    switch14 = 1;
    state_change = 1;
    udp_poll_request = 1;
  }
  if (switch14==1 && debounce14==0 && (FIO0PIN & BIT14) != 0) {
    debounce14 = 50;
    switch14 = 0;
    state_change = 1;
    udp_poll_request = 1;
  }

  //switch14 = (IOPIN0 & BIT14);

  arp_timer_counter++;
  ip_timer_counter++;

  if (arp_timer_counter >= ARP_TIMER_IN_MSEC) {
    arp_timer_counter = 0;
    arp_timer_expired = 1;
  }
  if (ip_timer_counter >= IP_TIMER_IN_MSEC) {
    ip_timer_counter = 0;
    ip_timer_expired = 1;
  }
}

int  net_timers_ip_expired()  { return ip_timer_expired;  }
void net_timers_ip_reset()    { ip_timer_expired = 0;     }
int  net_timers_arp_expired() { return arp_timer_expired; }
void net_timers_arp_reset()   { arp_timer_expired = 0;    }

#define SYSTICK_ISR_PRIORITY 3
#include "../systick_timer0.c"

#include "../busywait.c"


#define lcd_en_rs_out()  FIO1DIR |= (BIT24 | BIT25)
#define lcd_rw_out()     FIO0DIR |= BIT22
#define lcd_data4_out()  FIO1DIR |=  (BIT23 | BIT22 | BIT21 | BIT20)
#define lcd_data4_in()   FIO1DIR &= ~(BIT23 | BIT22 | BIT21 | BIT20)
#define lcd_data4_set(x) FIO1CLR |=  (BIT23 | BIT22 | BIT21 | BIT20); \
                         FIO1SET |= ((x & 0x0F) << 20)
#define lcd_data4_get(x) x = (FIO1PIN >> 20) & 0x0F
#define lcd_rs_set()     FIO1SET |= BIT24
#define lcd_rs_clr()     FIO1CLR |= BIT24
#define lcd_en_set()     FIO1SET |= BIT25
#define lcd_en_clr()     FIO1CLR |= BIT25
#define lcd_rw_set()     FIO0SET |= BIT22
#define lcd_rw_clr()     FIO0CLR |= BIT22
#define lcd_backlight_on() { FIO0DIR |= BIT30; FIO0SET = BIT30; }
#include "../char-lcd.c"
/*******************************************************/
/* Other modules                                       */
/*******************************************************/

/* serial driver */
#define UART0_BAUD_RATE 19200
#include "drivers/uart0-polling.c"

#define putchar(c) uart0SendByte(c)
#include "printf.c"

//#define DBG(fmt, args...) printf(fmt "%s", ##args, "\n")
//#define DBGN(fmt, args...) printf(fmt "%s", ##args, " ")
#define DBG(fmt, args...)
#define TRACE_ENTER(func) printf("-> %s\n", func)
#define TRACE_RETURN(func) printf("<- %s\n", func)
#define ASSERT(x)

typedef int bool;

#include "drivers/usb.h"

//#define uip_udp_appstate_t uint32_t;

void uip_log(char *m) {
  printf("uIP log message: %s\n", m);
}

#include "uip.h"
#include "uip_arp.h"

#include "usb-eth_dev.c"

#include "drivers/usb.c"

//#include "net_timers.c"

uint8_t ipaddr_set = 0;
uip_ipaddr_t ipaddr;
const struct uip_eth_addr ethaddr = {{ 0xDE, 0xAD, 0xBE, 0xEF, 0xAB, 0xBA }};

//void telnetd_init(void);

#define USE_UDP
#ifdef USE_UDP
static struct uip_udp_conn* app_conn;
static struct uip_udp_conn* bc_conn;
#else
static struct uip_conn* app_conn;
#endif

#ifdef USE_DHCPD

uip_ipaddr_t dhcpd_client_ipaddr;
uip_ipaddr_t dhcpd_broadcast_ipaddr;

static void dhcpd_address_assigned() {
#ifdef USE_UDP
  if (app_conn) uip_udp_remove(app_conn);
  printf("setting up app connection ");

  app_conn = uip_udp_new(&dhcpd_client_ipaddr, HTONS(1208) /* remote port */);
  if (app_conn != NULL) {
    uip_udp_bind(app_conn, HTONS(1208) /* local port */);
  }
  bc_conn = uip_udp_new(&dhcpd_broadcast_ipaddr, HTONS(1208) /* remote port */);
  if (bc_conn != NULL) {
    uip_udp_bind(bc_conn, HTONS(1208) /* local port */);
  }
#else
  // we can only close the current connection, so we can't do it here; must do it in a poll handler or something
  //if (app_conn) uip_close(app_conn);
  printf("setting up app connection ");
  app_conn = uip_connect(&dhcpd_client_ipaddr, HTONS(1208) /* remote port */);
#endif
}
//#define dhcpd_client_ipaddr
#include "dhcpd.c"
#endif

/*******************************************************/
/* UDP Appcall                                         */
/*******************************************************/

uint8_t  tx_buffer[32];
uint8_t  tx_len = 0;

void appHandlePacket(void) {
  printf("(((got a packet)))");

  udp_disconnected = 3000; /* disconnect in three seconds */

  //uint16_t command = uint16FromLittleEndian(uip_appdata);
  uint8_t command = ((uint8_t*) uip_appdata)[0];

  switch (command) {
  case 'i':
    printf(" info ack!!! ");
    {
      lcdClear();
      lcdGoto(0);
      uint8_t* msg = "connected";
      int i;
      for (i=0; msg[i]; i++) lcdPrintChar(msg[i]);
    }
    state = CONNECTED;
    state_change = 1; /* send our state to the host */
    break;
  case 's':
    printf(" state-change ack!!! ");
    state_change = 0;
    break;
  case 'H':
    // this is just for reseting the countdown timer
    printf(" host heartbeat ");
    break;
  case 'R':
    // reset
    state = NOT_CONNECTED;
    printf("protocol reset");
    {
      lcdClear();
      lcdGoto(0);
      uint8_t* msg = "protocol reset";
      int i;
      for (i=0; msg[i]; i++) lcdPrintChar(msg[i]);
    }
    break;
  case 'P':
    printf("print");
    break;
  case 'L':
    lcdClear();
    lcdGoto(0);
    int i;
    for (i=1; i<uip_len; i++)
      lcdPrintChar(((uint8_t*) uip_appdata)[i]);
    uip_send(uip_appdata,1); /* acknowledge the command */
    udp_sync = 1000; /* send sync in one second */
    break;
  default:
    printf("<what is this command? %u, len=%d>",command,uip_len);
    break;
  }
}

static const struct uip_eth_addr broadcast_ethaddr = {{0xff,0xff,0xff,0xff,0xff,0xff}};

void UIP_UDP_APPCALL(void) {
  if (uip_flags & UIP_NEWDATA) {
    printf("UDP: UIP NEWDATA");
    if (uip_udp_conn == bc_conn)  printf("oops, this should not happen...");
    if (uip_udp_conn == app_conn) appHandlePacket();
#ifdef USE_DHCPD
    if (uip_udp_conn == dhcpd_request_conn) dhcpdHandlePacket();
    if (uip_udp_conn == dhcpd_reply_conn) { printf("packet to the reply connection?!?"); while (1); }
#endif
    return;
  }

  if (uip_flags & UIP_POLL) {
#ifdef USE_UDP
    if (state==CONNECTED && udp_disconnected==0) {
      state = NOT_CONNECTED;

      lcdClear();
      lcdGoto(0);
      uint8_t* msg = "disconnected";
      int i;
      for (i=0; msg[i]; i++) lcdPrintChar(msg[i]);
    }

    //printf("{{{UDP: UIP POLL}}}");
    if (state==NOT_CONNECTED && uip_udp_conn == bc_conn) {
     /* UIP does not know about UDP broadcasts, so we fix the ARP table manually
      * This function is declared static by default, so it needs to be made non-static to
      * call from here.
      */
     uip_arp_update(&dhcpd_broadcast_ipaddr, &broadcast_ethaddr);
     uip_send("iLPC2148 Education Board\0", 25);
     udp_sync = 1000; /* send sync in one second */

      //((uint8_t*) uip_appdata)[0] = 0;
      //uip_send(uip_appdata, 1);
      printf("<<< sending udp info packet >>>");
      return;
    }
    if (state==CONNECTED && uip_udp_conn == app_conn) {
     if (state_change) {
       ((uint8_t*)uip_appdata)[0] = 's';
       ((uint8_t*)uip_appdata)[1] = (uint8_t) switch14;
       uip_send(uip_appdata, 2);
       udp_sync = 1000; /* send sync in one second */
       printf("<<< sending udp state packet >>>");
       return;
     }
     if (udp_sync==0) {
       uip_send("h", 1); /* heartbeat */
       udp_sync = 1000; /* send sync in one second */
     }
   }
#endif
    if (uip_udp_conn == dhcpd_reply_conn) dhcpdPollHandler();
    return;
  }
}

/*******************************************************/
/* UIP Appcall                                         */
/*******************************************************/

#ifndef USE_UDP
uint32_t tcp_idle_time    = 0;
uint8_t  tcp_reply        = 0;

uint8_t  tcp_tx_buffer[32];
uint8_t  tcp_tx_len;

void appHandlePacket(void) {
  printf("(((got a packet)))");

  //uint16_t command = uint16FromLittleEndian(uip_appdata);
  uint8_t command = ((uint8_t*) uip_appdata)[0];

  switch (command) {
  case 'P':
    printf("print");
    break;
  case 'L':
    lcdClear();
    lcdGoto(0);
    int i;
    for (i=1; i<uip_len; i++)
      lcdPrintChar(((uint8_t*) uip_appdata)[i]);
    break;
  default:
    printf("<what is this command? %u, len=%d>",command,uip_len);
    break;
  }
}

void UIP_APPCALL(void) {
  if (uip_poll()) {
    tcp_idle_time++;

    if (tcp_idle_time > 10) { /* half second units */
      printf("{{{Too much idle time in TCP connection}}}");
      uip_close();
      tcp_idle_time = 0;
      app_conn = uip_connect(&dhcpd_client_ipaddr, HTONS(1208) /* remote port */); /* try again */
    }
  }

  if (uip_acked()) {
    tcp_idle_time = 0;
    if (tcp_reply) {
      printf("{{{ACKED, sending reply to a request <%c>}}}",tcp_reply);
      tcp_tx_buffer[0] = tcp_reply;
      tcp_tx_len = 1;
      uip_send(tcp_tx_buffer, tcp_tx_len);
    } else {
      printf("{{{ACKED, idle}}}");
      tcp_tx_len = 0;
    }
  }

  if (uip_aborted() || uip_closed() || uip_timedout()) {
    printf("{{{TIMEDOUT,CLOSED,ABORTED}}}");
    uip_close();
    tcp_idle_time = 0;
    app_conn = uip_connect(&dhcpd_client_ipaddr, HTONS(1208) /* remote port */); /* try again */
  }

  if (uip_rexmit()) {
    printf("{{{REXMIT}}}");
    uip_send(tcp_tx_buffer, tcp_tx_len);
  }

  if (uip_newdata()) {
    printf("<<<TCP: UIP NEWDATA len=%d firstchar=%c>>>", uip_len,
        ((uint8_t*) uip_appdata)[0]);
    tcp_idle_time = 0;
    appHandlePacket();
    if (tcp_tx_len == 0) {
      tcp_tx_len = 1;
      tcp_tx_buffer[0] = ((uint8_t*) uip_appdata)[0];
      uip_send(tcp_tx_buffer, tcp_tx_len);
    } else {
      // we'll send it when the packet we are now sending is acked
      tcp_reply = ((uint8_t*) uip_appdata)[0];
      printf("...setting tcp reply to <%c>", tcp_reply);
    }
  }

  /*
   * if we put this before the test for new data,
   * it causes the message we send here to appear
   * as new data.
   */
  if (uip_connected()) {
    tcp_idle_time = 0;
    printf("{{{CONNECTED}}}");
    tcp_tx_len = 26;
    memcpy(tcp_tx_buffer,"iLPC 2148 Education Board\0", tcp_tx_len);
    uip_send(tcp_tx_buffer, tcp_tx_len);
  }


}
#else
void UIP_APPCALL(void) {
}
#endif
/*******************************************************/
/* Main program                                        */
/*******************************************************/

int main(int argc, char *argv[])
{
  uint16_t eth_hdr_type;
  int i;

  SCS      = BIT0 | BIT1;   /* use fast I/O registers    */

  VPBDIV = 0x00000001;  /* PCLK = CCLK */
  PLLCFG  = 0x00000024; /* Fosc=12MHz, CCLK=60MHz */
  PLLCON  = 0x00000001; /* enable the PLL  */
  PLLFEED = 0x000000AA; /* feed sequence   */
  PLLFEED = 0x00000055;
  while (!(PLLSTAT & 0x00000400));
  PLLCON = 3;     // enable and connect
  PLLFEED = 0xAA;
  PLLFEED = 0x55;

  busywaitInit();
  systickInit();
  lcdInit();

  vicInit();
  uart0Init();

  DBG("Initializing USB Stack");
  usbInit();

  rndisInit();
  DBG("init 2");
  network_device_init();

  DBG("init 3");
  uip_init();

  /*
  ipaddr_set = 1;
  uip_ipaddr(ipaddr, 192, 168, 12, 1);
  uip_ipaddr(dhcpd_client_ipaddr, 192, 168, 12, 2);
  uip_sethostaddr(ipaddr);
*/
  uip_setethaddr(ethaddr);

  //uip_setethaddr(eaddr);

  //telnetd_init();

  //net_timers_init();

#ifdef USE_DHCPD
  dhcpdInit();
#endif


  DBG("Starting USB Stack");
  interruptsEnable();
  usbConnect();

  lcdOn();
  lcdClear();

  //int xyz = 0;
  //printf("Frame number %u ",usbFrameNumber());

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

    /*
     * we reply to DHCP requests on another connection, and the connection
     * is faster if we don't wait for expiration of the timer.
     * I tried to call uip_udp_periodic_conn(dhcpd_reply_conn) instead of
     * the loop, but it did not work.
     */
    //if (dhcpd_state != DHCPD_IDLE) {
    if (udp_poll_request) {
      udp_poll_request = 0;
      for(i = 0; i < UIP_UDP_CONNS; i++) {
        uip_udp_periodic(i);
        if(uip_len > 0) {
          uip_arp_out();
          network_device_send();
        }
      }
      //uip_udp_periodic_conn(dhcpd_reply_conn);
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
#define USE_UDP_PERIODIC
#ifdef USE_UDP_PERIODIC
      for(i = 0; i < UIP_UDP_CONNS; i++) {
        uip_udp_periodic(i);
        if(uip_len > 0) {
          uip_arp_out();
          network_device_send();
        }
      }
#endif
      if(net_timers_arp_expired()) {
        net_timers_arp_reset();
        uip_arp_timer();
      }
    }
  }
  return 0;
}

/*******************************************************/
/* The end                                             */
/*******************************************************/

