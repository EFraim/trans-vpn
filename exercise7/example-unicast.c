/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * $Id: example-unicast.c,v 1.2 2009/03/12 21:58:21 adamdunkels Exp $
 */

/**
 * \file
 *         Best-effort single-hop unicast example
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "net/rime.h"
#include "dev/cc2420.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "Example unicast");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, rimeaddr_t *from)
{
  printf("unicast message received from %d.%d\n",
	 from->u8[0], from->u8[1]);
}

static void print_stats() {
  printf("Tx: %lu, Rx: %lu, reliabletx: %lu, reliablerx: %lu,"
	 "rexmit: %lu, acktx: %lu, noacktx: %lu, ackrx: %lu, timedout: %lu, badackrx: %lu,"
	 "toolong: %lu, tooshort: %lu, badsynch: %lu, badcrc: %lu, contentiondrop: %lu, sendingdrop: %lu, "
	 "lltx: %lu, llrx: %lu\n",
	 rimestats.tx, rimestats.rx,rimestats.reliabletx, rimestats.reliablerx,rimestats.rexmit,rimestats.acktx, rimestats.noacktx, rimestats.ackrx, rimestats.timedout, rimestats.badackrx,rimestats.toolong,rimestats.tooshort,rimestats.badsynch, rimestats.badcrc, rimestats.contentiondrop, rimestats.sendingdrop, rimestats.lltx, rimestats.llrx);
}


static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(unicast_close(&uc);)
    
  PROCESS_BEGIN();

  cc2420_set_txpower(CC2420_TXPOWER_MAX);
  cc2420_set_channel(30);
  //cc2420_on();
  unicast_open(&uc, 128, &unicast_callbacks);

  while(1) {
    static struct etimer et;
    rimeaddr_t addr;
    
    etimer_set(&et, CLOCK_SECOND);
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    packetbuf_clear();
    packetbuf_copyfrom("Good bye cruel world! This is somewhat longer message", 1);
    //addr.u8[0] = 5;    addr.u8[1] = 240; //First node
    addr.u8[0] = 230;		addr.u8[1] = 110; //Second node
    printf("my address is %d.%d\n", rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1]);
    unicast_send(&uc, &addr);
    print_stats();

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
