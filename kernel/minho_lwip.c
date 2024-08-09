/* C runtime includes */
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

/* Kernel includes */
#include <initcall.h>
#include <kernel/interrupt.h>
#include <kernel/misc.h>
#include <kernel/panic.h>
#include <kernel/thread.h>
#include <kernel/unwind.h>
#include <trace.h>
#include <crypto/crypto.h> // generate random

/* lwIP core includes */
#include "lwip/opt.h"

#include "kernel_siro.h"

#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/api.h"

#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"

/* lwIP netif includes */
#include "lwip/etharp.h"
#include "netif/ethernet.h"
#include "lwip/ip.h"
#include "lwip/ip4_frag.h"
#include "lwip/igmp.h"

#include "lwip/def.h"
#include "lwip/ip_addr.h"
#include "lwip/init.h"
#include "lwipopts.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"
#include "netif/ethernet.h"
//#include "netif/tapif.h"

#include "lwipcfg.h"

#include "lwip/def.h"

#include <kernel/tee_time.h>

#ifndef LWIP_EXAMPLE_APP_ABORT
#define LWIP_EXAMPLE_APP_ABORT() 0
#endif

/** Define this to 1 to enable a port-specific ethernet interface as default interface. */
#ifndef USE_DEFAULT_ETH_NETIF
#define USE_DEFAULT_ETH_NETIF 1
#endif

/** Define this to 1 to enable a PPP interface. */
#ifndef USE_PPP
#define USE_PPP 0
#endif

/** Define this to 1 or 2 to support 1 or 2 SLIP interfaces. */
#ifndef USE_SLIPIF
#define USE_SLIPIF 0
#endif

/** Use an ethernet adapter? Default to enabled if port-specific ethernet netif or PPPoE are used. */
#ifndef USE_ETHERNET
#define USE_ETHERNET  (USE_DEFAULT_ETH_NETIF || PPPOE_SUPPORT)
#endif

/** Use an ethernet adapter for TCP/IP? By default only if port-specific ethernet netif is used. */
#ifndef USE_ETHERNET_TCPIP
#define USE_ETHERNET_TCPIP  (USE_DEFAULT_ETH_NETIF)
#endif

void default_netif_poll(void);
void default_netif_shutdown(void);
void tcpecho_raw_init(void);
void my_tcpecho_raw_init(void);

/*
uint32_t
lwip_port_rand(void)
{
  uint8_t rand_int[4];
  printf("%s\n", __func__);

  crypto_rng_read(rand_int, sizeof(rand_int));

  uint32_t res = (rand_int[0] << 24) | (rand_int[1] << 16) | (rand_int[2] << 8) | rand_int[3];

  return res;
}


uint32_t
sys_now(void) {

  TEE_Time t;

  uint32_t now;

  //TEE_MemFill(t, 0, sizeof(TEE_Time));

   get_monotonic_time(t);
  tee_time_get_sys_time(&t);
  now = (uint32_t)(t.seconds * 1000L + t.millis);
  return now;
}
*/

#if LWIP_NETIF_STATUS_CALLBACK

static void
status_callback(struct netif *state_netif)
{
  IMSG("**********status_callback()**********\n");
  if (netif_is_up(state_netif)) {
#if LWIP_IPV4
    IMSG("status_callback==UP, local interface IP is %s\n", ip4addr_ntoa(netif_ip4_addr(state_netif)));
#else
    IMSG("status_callback==UP\n");
#endif
  } else {
    IMSG("status_callback==DOWN\n");
  }
}

#endif /* LWIP_NETIF_STATUS_CALLBACK */

#if LWIP_NETIF_LINK_CALLBACK

static void
link_callback(struct netif *state_netif)
{
  IMSG("**********link_callback()**********\n");
  if (netif_is_link_up(state_netif)) {
    IMSG("link_callback==UP\n");
  } else {
    IMSG("link_callback==DOWN\n");
  }
}

#endif /* LWIP_NETIF_LINK_CALLBACK */

/* static struct netif my_netif; */
ip_addr_t server_ip;
struct netif my_netif;

#define NETIF_ADDRS ipaddr, netmask, gw,

static void init_my_netif(void) {
	ip4_addr_t ipaddr, netmask, gw;
	
	IMSG("**********init_my_netif()**********\n");
	ip4_addr_set_zero(&gw);
	ip4_addr_set_zero(&ipaddr);
	ip4_addr_set_zero(&netmask);
	
	LWIP_PORT_INIT_GW(&gw);
	LWIP_PORT_INIT_IPADDR(&ipaddr);
	LWIP_PORT_INIT_NETMASK(&netmask);

	netif_add(&my_netif, &ipaddr, &netmask, &gw, NULL, netif_init, netif_input);
	netif_set_default(&my_netif);
	
	netif_set_status_callback(&my_netif, status_callback);
	netif_set_link_callback(&my_netif, link_callback);
	
	netif_set_up(&my_netif);

  	IMSG("Starting custom lwIP, local interface IP is %s\n", ip4addr_ntoa(&ipaddr));
	IMSG("Netmask: %s, Gateway: %s\n", ip4addr_ntoa(&netmask), ip4addr_ntoa(&gw));
}

void
default_netif_poll(void)
{
  netif_poll(&my_netif);
}

void
default_netif_shutdown(void)
{
  IMSG("**********default_netif_shutdown()**********\n");
}



/* tcpecho server */

static struct tcp_pcb* my_tcpecho_raw_pcb;

enum my_tcpecho_raw_states {
	ES_NONE,
	ES_ACCEPTED,
	ES_RECEIVED,
	ES_CLOSING
};

struct tcpecho_raw_state {
	u8_t state;
	u8_t retries;
	struct tcp_pcb *pcb;
	struct pbuf *p;
};


static void my_tcpecho_raw_free(struct tcpecho_raw_state* es) {
	IMSG("**********my_tcpecho_raw_free()**********\n");
	if( es != NULL ) {
		if( es->p ) {
			pbuf_free(es->p);
		}
	
	mem_free(es);
	}
}

static void my_tcpecho_raw_close(struct tcp_pcb *tpcb, struct tcpecho_raw_state *es) {
	IMSG("**********my_tcpecho_raw_close()**********\n");
	tcp_arg(tpcb, NULL);
	tcp_sent(tpcb, NULL);
	tcp_recv(tpcb, NULL);
	tcp_err(tpcb, NULL);
	tcp_poll(tpcb, NULL, 0);

	my_tcpecho_raw_free(es);
	tcp_close(tpcb);
}

static void my_tcpecho_raw_send(struct tcp_pcb *tpcb, struct tcpecho_raw_state *es) {
	struct pbuf* ptr;
	err_t wr_err = ERR_OK;
	int cnt = 0;
	while( wr_err == ERR_OK && es->p != NULL && es->p->len <= tcp_sndbuf(tpcb) ) {
		ptr = es->p;
		IMSG("%d $$$$$$$$$$$$ %s $$$$$$$$$$$$\n", cnt++ ,(char *)ptr->payload);
		wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
		if( wr_err == ERR_OK ) {
			u16_t plen;
			plen = ptr->len;

			es->p = ptr->next;
			if( es->p != NULL ) {
				pbuf_ref(es->p);
			}
			tcp_recved(tpcb, plen);
		} else if( wr_err == ERR_MEM ) {
			es->p = ptr;
		} else {
			IMSG("Unknown error\n");
		}
	}
}

static void my_tcpecho_raw_error(void *arg, err_t err) {
	struct tcpecho_raw_state* es;
	LWIP_UNUSED_ARG(err);
	es = (struct tcpecho_raw_state *)arg;
	my_tcpecho_raw_free(es);
}

static err_t my_tcpecho_raw_poll(void *arg, struct tcp_pcb* tpcb) {
	err_t ret_err;
	struct tcpecho_raw_state* es;

	es = (struct tcpecho_raw_state *)arg;
	if( es != NULL ) {
		if( es->p != NULL ) {
			my_tcpecho_raw_send(tpcb, es);
		} else {
			if( es->state == ES_CLOSING ) {
				my_tcpecho_raw_close(tpcb, es);
			}
		}
	ret_err = ERR_OK;
	} else {
		tcp_abort(tpcb);
		ret_err = ERR_ABRT;
	}
	return ret_err;
}

static err_t my_tcpecho_raw_sent(void *arg, struct tcp_pcb* tpcb, u16_t len) {
	struct tcpecho_raw_state *es;
	
	LWIP_UNUSED_ARG(len);

	es = (struct tcpecho_raw_state *)arg;
	es->retries = 0;

	if( es->p != NULL ) {
		tcp_sent(tpcb, my_tcpecho_raw_sent);
		my_tcpecho_raw_send(tpcb, es);
	} else {
		if( es->state == ES_CLOSING ) {
			my_tcpecho_raw_close(tpcb, es);
		}
	}
	return ERR_OK;
}

static err_t my_tcpecho_raw_recv(void *arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err) {
	struct tcpecho_raw_state *es;
	err_t ret_err;

	LWIP_ASSERT("arg != NULL", arg != NULL);
	es = (struct tcpecho_raw_state *)arg;
	if( p == NULL ) {
		es->state = ES_CLOSING;
		if( es->p == NULL ) {
			my_tcpecho_raw_close(tpcb, es);
		} else {
			my_tcpecho_raw_send(tpcb, es);
		}
		ret_err = ERR_OK;
	} else if( err != ERR_OK ) {
		LWIP_ASSERT("no pbuf expected here", p == NULL);
		ret_err = err;
	} else if( es->state == ES_ACCEPTED ) {
		es->state = ES_RECEIVED;
		es->p = p;
		my_tcpecho_raw_send(tpcb, es);
		ret_err = ERR_OK;
	} else if( es->state == ES_RECEIVED ) {
		if( es->p == NULL ) {
			es->p = p;
			my_tcpecho_raw_send(tpcb, es);
		} else {
			struct pbuf* ptr;

			ptr = es->p;
			pbuf_cat(ptr, p);
		}
		ret_err = ERR_OK; 
	} else {
		tcp_recved(tpcb, p->tot_len);
		pbuf_free(p);
		ret_err = ERR_OK;
	}
	return ret_err;
}

static err_t my_tcpecho_raw_accept(void *arg, struct tcp_pcb* newpcb, err_t err) {
	err_t ret_err;
	struct tcpecho_raw_state *es;
	LWIP_UNUSED_ARG(arg);
	if( err != ERR_OK || newpcb == NULL ) {
		return ERR_VAL;
	}

	tcp_setprio(newpcb, TCP_PRIO_MIN);

	es = (struct tcpecho_raw_state *)mem_malloc(sizeof(struct tcpecho_raw_state));
	if( es != NULL ) {
		es->state = ES_ACCEPTED;
		es->pcb = newpcb;
		es->retries = 0;
		es->p = NULL;
		
		tcp_arg(newpcb, es);
		tcp_recv(newpcb, my_tcpecho_raw_recv);
		tcp_err(newpcb, my_tcpecho_raw_error);
		tcp_poll(newpcb, my_tcpecho_raw_poll, 0);
		tcp_sent(newpcb, my_tcpecho_raw_sent);
		ret_err = ERR_OK;
	} else {
		ret_err = ERR_MEM;
	}
	return ret_err;
}

void my_tcpecho_raw_init(void) {
	my_tcpecho_raw_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if(my_tcpecho_raw_pcb != NULL) {
		err_t err;

		err = tcp_bind(my_tcpecho_raw_pcb, IP_ADDR_ANY, 7);
		if( err == ERR_OK ) {
			IMSG("TCP bind successful on port 7\n");
			my_tcpecho_raw_pcb = tcp_listen(my_tcpecho_raw_pcb);
			tcp_accept(my_tcpecho_raw_pcb, my_tcpecho_raw_accept);
		} else { 
			IMSG("Binding new pcb has failed with errno %d\n", err);
			return;
		}
	} else {
		IMSG("Creating new pcb has failed.\n");
		return;	
	}
}

static void my_init(void *arg) {
	LWIP_UNUSED_ARG(arg);
	/*srand((unsigned int)time(NULL));*/
	//init_my_netif();
	my_tcpecho_raw_init();
}

void main_loop(void) {
	lwip_init();
	my_init(NULL);
	while(!LWIP_EXAMPLE_APP_ABORT()) {
		sys_check_timeouts(); 
		default_netif_poll();
		/*netif_poll_all();*/
	}
	/* netif_poll_all(); */
	default_netif_shutdown();
}

int main(void) {
/*	lwip_init();
	my_init(NULL); */
	main_loop();
	/*
	lwip_init();
	init_my_netif();
	tcpecho_raw_init();
	*/
	return 0;
}

void lwip_example_app_platform_assert(const char *msg, int line, const char *file)
{
  IMSG("Assertion \"%s\" failed at line %d in %s\n", msg, line, file);
  //fflush(NULL);
  panic();
}

