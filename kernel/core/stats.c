/**
 * @file
 * Statistics module
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/opt.h"

#if LWIP_STATS /* don't build if not configured for use in lwipopts.h */

#include "lwip/def.h"
#include "lwip/stats.h"
#include "lwip/mem.h"
#include "lwip/debug.h"

#include <string.h>
#include <trace.h>

struct stats_ lwip_stats;

void
stats_init(void)
{
#ifdef LWIP_DEBUG
#if MEM_STATS
  lwip_stats.mem.name = "MEM";
#endif /* MEM_STATS */
#endif /* LWIP_DEBUG */
}

#if LWIP_STATS_DISPLAY
void
stats_display_proto(struct stats_proto *proto, const char *name)
{
  DMSG(("\n%s\n\t", name));
  DMSG(("xmit: %"STAT_COUNTER_F"\n\t", proto->xmit));
  DMSG(("recv: %"STAT_COUNTER_F"\n\t", proto->recv));
  DMSG(("fw: %"STAT_COUNTER_F"\n\t", proto->fw));
  DMSG(("drop: %"STAT_COUNTER_F"\n\t", proto->drop));
  DMSG(("chkerr: %"STAT_COUNTER_F"\n\t", proto->chkerr));
  DMSG(("lenerr: %"STAT_COUNTER_F"\n\t", proto->lenerr));
  DMSG(("memerr: %"STAT_COUNTER_F"\n\t", proto->memerr));
  DMSG(("rterr: %"STAT_COUNTER_F"\n\t", proto->rterr));
  DMSG(("proterr: %"STAT_COUNTER_F"\n\t", proto->proterr));
  DMSG(("opterr: %"STAT_COUNTER_F"\n\t", proto->opterr));
  DMSG(("err: %"STAT_COUNTER_F"\n\t", proto->err));
  DMSG(("cachehit: %"STAT_COUNTER_F"\n", proto->cachehit));
}

#if IGMP_STATS || MLD6_STATS
void
stats_display_igmp(struct stats_igmp *igmp, const char *name)
{
  DMSG(("\n%s\n\t", name));
  DMSG(("xmit: %"STAT_COUNTER_F"\n\t", igmp->xmit));
  DMSG(("recv: %"STAT_COUNTER_F"\n\t", igmp->recv));
  DMSG(("drop: %"STAT_COUNTER_F"\n\t", igmp->drop));
  DMSG(("chkerr: %"STAT_COUNTER_F"\n\t", igmp->chkerr));
  DMSG(("lenerr: %"STAT_COUNTER_F"\n\t", igmp->lenerr));
  DMSG(("memerr: %"STAT_COUNTER_F"\n\t", igmp->memerr));
  DMSG(("proterr: %"STAT_COUNTER_F"\n\t", igmp->proterr));
  DMSG(("rx_v1: %"STAT_COUNTER_F"\n\t", igmp->rx_v1));
  DMSG(("rx_group: %"STAT_COUNTER_F"\n\t", igmp->rx_group));
  DMSG(("rx_general: %"STAT_COUNTER_F"\n\t", igmp->rx_general));
  DMSG(("rx_report: %"STAT_COUNTER_F"\n\t", igmp->rx_report));
  DMSG(("tx_join: %"STAT_COUNTER_F"\n\t", igmp->tx_join));
  DMSG(("tx_leave: %"STAT_COUNTER_F"\n\t", igmp->tx_leave));
  DMSG(("tx_report: %"STAT_COUNTER_F"\n", igmp->tx_report));
}
#endif /* IGMP_STATS || MLD6_STATS */

#if MEM_STATS || MEMP_STATS
void
stats_display_mem(struct stats_mem *mem, const char *name)
{
  DMSG(("\nMEM %s\n\t", name));
  DMSG(("avail: %"MEM_SIZE_F"\n\t", mem->avail));
  DMSG(("used: %"MEM_SIZE_F"\n\t", mem->used));
  DMSG(("max: %"MEM_SIZE_F"\n\t", mem->max));
  DMSG(("err: %"STAT_COUNTER_F"\n", mem->err));
}

#if MEMP_STATS
void
stats_display_memp(struct stats_mem *mem, int idx)
{
  if (idx < MEMP_MAX) {
    stats_display_mem(mem, mem->name);
  }
}
#endif /* MEMP_STATS */
#endif /* MEM_STATS || MEMP_STATS */

#if SYS_STATS
void
stats_display_sys(struct stats_sys *sys)
{
  DMSG(("\nSYS\n\t"));
  DMSG(("sem.used:  %"STAT_COUNTER_F"\n\t", sys->sem.used));
  DMSG(("sem.max:   %"STAT_COUNTER_F"\n\t", sys->sem.max));
  DMSG(("sem.err:   %"STAT_COUNTER_F"\n\t", sys->sem.err));
  DMSG(("mutex.used: %"STAT_COUNTER_F"\n\t", sys->mutex.used));
  DMSG(("mutex.max:  %"STAT_COUNTER_F"\n\t", sys->mutex.max));
  DMSG(("mutex.err:  %"STAT_COUNTER_F"\n\t", sys->mutex.err));
  DMSG(("mbox.used:  %"STAT_COUNTER_F"\n\t", sys->mbox.used));
  DMSG(("mbox.max:   %"STAT_COUNTER_F"\n\t", sys->mbox.max));
  DMSG(("mbox.err:   %"STAT_COUNTER_F"\n", sys->mbox.err));
}
#endif /* SYS_STATS */

void
stats_display(void)
{
  s16_t i;

  LINK_STATS_DISPLAY();
  ETHARP_STATS_DISPLAY();
  IPFRAG_STATS_DISPLAY();
  IP6_FRAG_STATS_DISPLAY();
  IP_STATS_DISPLAY();
  ND6_STATS_DISPLAY();
  IP6_STATS_DISPLAY();
  IGMP_STATS_DISPLAY();
  MLD6_STATS_DISPLAY();
  ICMP_STATS_DISPLAY();
  ICMP6_STATS_DISPLAY();
  UDP_STATS_DISPLAY();
  TCP_STATS_DISPLAY();
  MEM_STATS_DISPLAY();
  for (i = 0; i < MEMP_MAX; i++) {
    MEMP_STATS_DISPLAY(i);
  }
  SYS_STATS_DISPLAY();
}
#endif /* LWIP_STATS_DISPLAY */

#endif /* LWIP_STATS */
