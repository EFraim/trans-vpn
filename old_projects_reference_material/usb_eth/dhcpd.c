/******************************************************************************/
/* DHCP Server                                                                */
/* A minimal server for point-to-point networks only.                         */
/* Based on the DHCP client in uIP.                                           */
/******************************************************************************/

struct dhcpc_state {
  //struct pt pt;
  char state;
  struct uip_udp_conn *conn;
  //struct timer timer;
  u16_t ticks;
  const void *mac_addr;
  int mac_len;

  u8_t serverid[4];

  u16_t lease_time[2];
  u16_t ipaddr[2];
  u16_t netmask[2];
  u16_t dnsaddr[2];
  u16_t default_router[2];
};
static struct dhcpc_state s;
/* code and definitions from uIP 1.0, dhcpc.c */

struct dhcp_msg {
  u8_t op, htype, hlen, hops;
  u8_t xid[4];
  u16_t secs, flags;
  u8_t ciaddr[4];
  u8_t yiaddr[4];
  u8_t siaddr[4];
  u8_t giaddr[4];
  u8_t chaddr[16];
#ifndef UIP_CONF_DHCP_LIGHT
  u8_t sname[64];
  u8_t file[128];
#endif
  u8_t options[312];
};

struct dhcp_header_st {
  u8_t op, htype, hlen, hops;
  u8_t xid[4];
  u16_t secs, flags;
  u8_t ciaddr[4];
  u8_t yiaddr[4];
  u8_t siaddr[4];
  u8_t giaddr[4];
  u8_t chaddr[16];
} dhcp_header;

enum {
  DHCPD_IDLE,
  DHCPD_SEND_OFFER,
  DHCPD_SEND_ACK
} dhcpd_state;

#define BOOTP_BROADCAST 0x8000

#define DHCP_REQUEST        1
#define DHCP_REPLY          2
#define DHCP_HTYPE_ETHERNET 1
#define DHCP_HLEN_ETHERNET  6
#define DHCP_MSG_LEN      236

#define DHCPC_SERVER_PORT  67
#define DHCPC_CLIENT_PORT  68

#define DHCPDISCOVER  1
#define DHCPOFFER     2
#define DHCPREQUEST   3
#define DHCPDECLINE   4
#define DHCPACK       5
#define DHCPNAK       6
#define DHCPRELEASE   7

#define DHCP_OPTION_PAD           0
#define DHCP_OPTION_SUBNET_MASK   1
#define DHCP_OPTION_ROUTER        3
#define DHCP_OPTION_DNS_SERVER    6
#define DHCP_OPTION_DOMAIN_NAME  15
#define DHCP_OPTION_REQ_IPADDR   50
#define DHCP_OPTION_LEASE_TIME   51
#define DHCP_OPTION_MSG_TYPE     53
#define DHCP_OPTION_SERVER_ID    54
#define DHCP_OPTION_REQ_LIST     55
#define DHCP_OPTION_END         255

static const u8_t xid[4] = {0xad, 0xde, 0x12, 0x23};
static const u8_t magic_cookie[4] = {99, 130, 83, 99};
/*---------------------------------------------------------------------------*/
static u8_t *
add_msg_type(u8_t *optptr, u8_t type)
{
  *optptr++ = DHCP_OPTION_MSG_TYPE;
  *optptr++ = 1;
  *optptr++ = type;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_server_id(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_SERVER_ID;
  *optptr++ = 4;
  memcpy(optptr, s.serverid, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_server_id_myself(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_SERVER_ID;
  *optptr++ = 4;
  memcpy(optptr, uip_hostaddr, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_domain_name(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_DOMAIN_NAME;
  *optptr++ = 4;
  memcpy(optptr, "usb", 3);
  return optptr + 3;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_dns_server_myself(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_DNS_SERVER;
  *optptr++ = 4;
  memcpy(optptr, uip_hostaddr, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_req_ipaddr(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_REQ_IPADDR;
  *optptr++ = 4;
  memcpy(optptr, s.ipaddr, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_subnetmask(u8_t *optptr, u8_t m0, u8_t m1, u8_t m2, u8_t m3)
{
  *optptr++ = DHCP_OPTION_SUBNET_MASK;
  *optptr++ = 4;
  optptr[0] = m0;
  optptr[1] = m1;
  optptr[2] = m2;
  optptr[3] = m3;
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_lease_time(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_LEASE_TIME;
  *optptr++ = 4;
  optptr[0] = 0xff;
  optptr[1] = 0xff;
  optptr[2] = 0xff;
  optptr[3] = 0xff;
  return optptr + 4;
}

/*---------------------------------------------------------------------------*/
static u8_t *
add_req_options(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_REQ_LIST;
  *optptr++ = 3;
  *optptr++ = DHCP_OPTION_SUBNET_MASK;
  *optptr++ = DHCP_OPTION_ROUTER;
  *optptr++ = DHCP_OPTION_DNS_SERVER;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_end(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_END;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static void
create_msg(register struct dhcp_msg *m)
{
  m->op = DHCP_REQUEST;
  m->htype = DHCP_HTYPE_ETHERNET;
  m->hlen = s.mac_len;
  m->hops = 0;
  memcpy(m->xid, xid, sizeof(m->xid));
  m->secs = 0;
  m->flags = HTONS(BOOTP_BROADCAST); /*  Broadcast bit. */
  /*  uip_ipaddr_copy(m->ciaddr, uip_hostaddr);*/
  memcpy(m->ciaddr, uip_hostaddr, sizeof(m->ciaddr));
  memset(m->yiaddr, 0, sizeof(m->yiaddr));
  memset(m->siaddr, 0, sizeof(m->siaddr));
  memset(m->giaddr, 0, sizeof(m->giaddr));
  memcpy(m->chaddr, s.mac_addr, s.mac_len);
  memset(&m->chaddr[s.mac_len], 0, sizeof(m->chaddr) - s.mac_len);
#ifndef UIP_CONF_DHCP_LIGHT
  memset(m->sname, 0, sizeof(m->sname));
  memset(m->file, 0, sizeof(m->file));
#endif

  memcpy(m->options, magic_cookie, sizeof(magic_cookie));
}
/*---------------------------------------------------------------------------*/
static void
send_discover(void)
{
  u8_t *end;
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;

  create_msg(m);

  end = add_msg_type(&m->options[4], DHCPDISCOVER);
  end = add_req_options(end);
  end = add_end(end);

  uip_send(uip_appdata, end - (u8_t *)uip_appdata);
}
/*---------------------------------------------------------------------------*/
static void
send_request(void)
{
  u8_t *end;
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;

  create_msg(m);

  end = add_msg_type(&m->options[4], DHCPREQUEST);
  end = add_server_id(end);
  end = add_req_ipaddr(end);
  end = add_end(end);

  uip_send(uip_appdata, end - (u8_t *)uip_appdata);
}
/*---------------------------------------------------------------------------*/
static u8_t
parse_options(u8_t *optptr, int len)
{
  u8_t *end = optptr + len;
  u8_t type = 0;

  while(optptr < end) {
    switch(*optptr) {
    case DHCP_OPTION_PAD:
      optptr += 1;
      continue; // continue with the loop
    case DHCP_OPTION_SUBNET_MASK:
      memcpy(s.netmask, optptr + 2, 4);
      break;
    case DHCP_OPTION_ROUTER:
      memcpy(s.default_router, optptr + 2, 4);
      break;
    case DHCP_OPTION_DNS_SERVER:
      memcpy(s.dnsaddr, optptr + 2, 4);
      break;
    case DHCP_OPTION_MSG_TYPE:
      type = *(optptr + 2);
      break;
    case DHCP_OPTION_SERVER_ID:
      memcpy(s.serverid, optptr + 2, 4);
      break;
    case DHCP_OPTION_LEASE_TIME:
      memcpy(s.lease_time, optptr + 2, 4);
      break;
    case DHCP_OPTION_END:
      return type;
    default:
      printf("unknown DHCP option %d ",*optptr);
      break;
    }

    optptr += optptr[1] + 2;
  }
  return type;
}
/*---------------------------------------------------------------------------*/
static u8_t
parse_msg(void)
{
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;


  if(m->op == DHCP_REPLY &&
     memcmp(m->xid, xid, sizeof(xid)) == 0 &&
     memcmp(m->chaddr, s.mac_addr, s.mac_len) == 0) {
    memcpy(s.ipaddr, m->yiaddr, 4);
    return parse_options(&m->options[4], uip_datalen());
  }
  return 0;
}
/*---------------------------------------------------------------------------*/

static struct uip_udp_conn* dhcpd_request_conn;
static struct uip_udp_conn* dhcpd_reply_conn;

static void dhcpdInit(void) {
  uip_ipaddr_t remote_ipaddr;
  uip_ipaddr(remote_ipaddr, 0,0,0,0);
  //uip_ipaddr(remote_ipaddr, 192,168,12,2);
  dhcpd_request_conn = uip_udp_new(&remote_ipaddr, HTONS(68) /* remote port */);
  if(dhcpd_request_conn != NULL) {
    uip_udp_bind(dhcpd_request_conn, HTONS(67) /* local port */);
    printf("BIND OK");
  } else {
    printf("BIND ERROR!");
  }


  uip_ipaddr(remote_ipaddr, 255,255,255,255);
  //uip_ipaddr(remote_ipaddr, 192,168,12,2);
  dhcpd_reply_conn = uip_udp_new(&remote_ipaddr, HTONS(68) /* remote port */);
  if(dhcpd_reply_conn != NULL) {
    uip_udp_bind(dhcpd_reply_conn, HTONS(67) /* local port */);
    printf("BIND OK");
  } else {
    printf("BIND ERROR!");
  }

  dhcpd_state = DHCPD_IDLE;
}

//const uip_ipaddr_t ipaddr;
//const struct uip_eth_addr ethaddr = {{ 0xDE, 0xAD, 0xBE, 0xEF, 0xAB, 0xBA }};


static void dhcpdHandlePacket(void) {
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;

  if (!ipaddr_set) {
    uip_ipaddr( ipaddr,                 10, 200, 0, 1);
    uip_ipaddr( dhcpd_client_ipaddr,    10, 200, 0, 2);
    uip_ipaddr( dhcpd_broadcast_ipaddr, 10, 200, 0, 3);
    uip_sethostaddr(ipaddr);
    ipaddr_set = 1;
  }

  printf("This is a DHCP broadcast op=%d ",m->op);
  if (m->op == DHCP_REQUEST) {
    //printf("xid = %02x %02x %02x %02x ",(m->xid)[0],(m->xid)[1],(m->xid)[2],(m->xid)[3]);
    //printf("secs = %02x ",m->secs);
    //printf("flags (bc?) = %02x ",m->flags);
    if (memcmp(m->options,magic_cookie,4)==0)
      printf("found magic cookie ");
    else {
      printf("magic cookie missing! ");
      return;
    }
    int type = parse_options(&m->options[4], uip_datalen());
    //printf("DHCP message type %d ",type);
    if (type == DHCPDISCOVER) {
      printf("<<discover>> ");
      memcpy((uint8_t*) &dhcp_header,m,sizeof(dhcp_header));
      dhcpd_state = DHCPD_SEND_OFFER;
      udp_poll_request = 1;
      return;


      m->op = DHCP_REPLY;
      uip_ipaddr_t ipaddr;
      //uip_ipaddr( m->yiaddr, 192, 168, 12, 2);
      uip_ipaddr_copy(m->yiaddr,dhcpd_client_ipaddr);
      memset(m->ciaddr,0,4);
      //memset(m->siaddr,0,4);
      uip_ipaddr_copy(m->siaddr,uip_hostaddr);

      u8_t *end;
      end = add_msg_type(&m->options[4], DHCPOFFER);
      //end = add_subnetmask(end, 255,255,255,0);
      end = add_subnetmask(end, 255,255,255, 0xfc); // a /30 address
      end = add_lease_time(end);
      end = add_server_id_myself(end);
      //end = add_dns_server_myself(end);
      //end = add_domain_name(end);
      end = add_end(end);

      uip_send(uip_appdata, end - (u8_t *)uip_appdata);
    }
    if (type == DHCPREQUEST) {
      printf("<<request>> ");
      memcpy((uint8_t*) &dhcp_header,m,sizeof(dhcp_header));
      dhcpd_state = DHCPD_SEND_ACK;
      udp_poll_request = 1;
      return;

      m->op = DHCP_REPLY;
      uip_ipaddr_t ipaddr;
      //uip_ipaddr( m->yiaddr,  192, 168, 12, 2);
      uip_ipaddr_copy(m->yiaddr,dhcpd_client_ipaddr);
      memset(m->ciaddr,0,4);
      //memset(m->siaddr,0,4);
      uip_ipaddr_copy(m->siaddr,uip_hostaddr);


      u8_t *end;
      end = add_msg_type(&m->options[4], DHCPACK);
      //end = add_subnetmask(end, 255,255,255,0);
      //end = add_subnetmask(end, 255,255,255, 0xfC); // a /30 address
      //end = add_subnetmask(end, 255,255,255, 0xfe); // a /31 address
      end = add_subnetmask(end, 255,255,255, 0xfc); // a /30 address
      end = add_lease_time(end);
      end = add_server_id_myself(end);
      //end = add_dns_server_myself(end);
      //end = add_domain_name(end);
      end = add_end(end);

      uip_send(uip_appdata, end - (u8_t *)uip_appdata);
      dhcpd_address_assigned(); // notify the rest of teh system
    }
  }
}

void dhcpdPollHandler(void) {
  if (dhcpd_state == DHCPD_IDLE) return;

  printf("<<dhcp poll>> ");

  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;
  memcpy(m,(uint8_t*) &dhcp_header,sizeof(dhcp_header));

  m->op = DHCP_REPLY;
  //uip_ipaddr_t ipaddr;
  //uip_ipaddr( m->yiaddr, 192, 168, 12, 2);
  uip_ipaddr_copy(m->yiaddr,dhcpd_client_ipaddr);
  memset(m->ciaddr,0,4);
  //memset(m->siaddr,0,4);
  uip_ipaddr_copy(m->siaddr,uip_hostaddr);


  memcpy(m->options, magic_cookie, sizeof(magic_cookie));
  u8_t *end;
  switch (dhcpd_state) {
  case DHCPD_SEND_OFFER:
    end = add_msg_type(&m->options[4], DHCPOFFER);
    break;
  case DHCPD_SEND_ACK:
    end = add_msg_type(&m->options[4], DHCPACK);
    dhcpd_address_assigned(); // notify the rest of teh system
    break;
  default:
    return;
  }

  //end = add_subnetmask(end, 255,255,255,0);
  end = add_subnetmask(end, 255,255,255, 0xfc); // a /30 address
  end = add_lease_time(end);
  end = add_server_id_myself(end);
  //end = add_dns_server_myself(end);
  //end = add_domain_name(end);
  end = add_end(end);
  uip_send(uip_appdata, end - (u8_t *)uip_appdata);
  dhcpd_state = DHCPD_IDLE;
}

/******************************************************************************/
