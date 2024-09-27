/* C runtime includes */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* lwIP core includes */
#include "lwip/opt.h"

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

/* OPTEE includes */
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <ta_server_side.h>
#include <ta_client_side.h>


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


#define ONBOARD 	0
//static struct netif loop_netif;
//static struct tcp_pcb *client_pcb = NULL;
//static err_t client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
//static err_t client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
//static char *data_to_send = NULL;

TEE_Result res;
TEE_TASessionHandle session = TEE_HANDLE_NULL;
TEE_UUID uuid = { 0x59b49e72, 0xa721, 0x49a4, \
                { 0x8d, 0x21, 0x70, 0x80, 0x11, 0xce, 0xc4, 0x99} };

static void _get_monotonic_time(TEE_Time time) {
	TEE_GetSystemTime(&time);
}

uint32_t
sys_now(void) {

  TEE_Time t;
  uint32_t now;
  printf("%s\n", __func__);

  //TEE_MemFill(t, 0, sizeof(TEE_Time));

  TEE_GetSystemTime(&t); 
  now = (uint32_t)(t.seconds * 1000L + t.millis);
  return now;
}


#if LWIP_NETIF_STATUS_CALLBACK

static void
status_callback(struct netif *state_netif)
{
   printf("%s\n", __func__);
   if (netif_is_up(state_netif)) {
#if LWIP_IPV4
    printf("status_callback==UP, local interface IP is %s\n", ip4addr_ntoa(netif_ip4_addr(state_netif)));
#else
    printf("status_callback==UP\n");
#endif
  } else {
    printf("status_callback==DOWN\n");
  }
}

#endif /* LWIP_NETIF_STATUS_CALLBACK */

#if LWIP_NETIF_LINK_CALLBACK

static void
link_callback(struct netif *state_netif)
{
   printf("%s\n", __func__);
  if (netif_is_link_up(state_netif)) {
    printf("link_callback==UP\n");
  } else {
    printf("link_callback==DOWN\n");
  }
}

#endif /* LWIP_NETIF_LINK_CALLBACK */


static struct netif my_netif;
ip_addr_t server_ip;

#define NETIF_ADDRS ipaddr, netmask, gw,

/*
static void init_my_netif(void) {
        ip4_addr_t ipaddr, netmask, gw;

        printf("%s\n", __func__);
	ip4_addr_set_zero(&gw);
        ip4_addr_set_zero(&ipaddr);
        ip4_addr_set_zero(&netmask);

        LWIP_PORT_INIT_GW(&gw);
        LWIP_PORT_INIT_IPADDR(&ipaddr);
        LWIP_PORT_INIT_NETMASK(&netmask);

	printf("1st settings:\n");
	printf("IP Address: %s\n", ip4addr_ntoa(&ipaddr));
	printf("Netmask: %s\n", ip4addr_ntoa(&netmask));
	printf("Gateway: %s\n", ip4addr_ntoa(&gw));

	netif_add(&my_netif, &ipaddr, &netmask, &gw, NULL, netif_loopif_init, netif_input);

	printf("2nd settings:\n");
        printf("IP Address: %s\n", ip4addr_ntoa(&(my_netif.ip_addr)));
        printf("Netmask: %s\n", ip4addr_ntoa(&(my_netif.netmask)));
        printf("Gateway: %s\n", ip4addr_ntoa(&(my_netif.gw)));
        //netif_add(&my_netif, &ipaddr, &netmask, &gw, NULL, tapif_init, netif_input);
        netif_set_default(&my_netif);

        netif_set_status_callback(&my_netif, status_callback);

	printf("3rd settings:\n");
        printf("IP Address: %s\n", ip4addr_ntoa(&(my_netif.ip_addr)));
        printf("Netmask: %s\n", ip4addr_ntoa(&(my_netif.netmask)));
        printf("Gateway: %s\n", ip4addr_ntoa(&(my_netif.gw)));

        netif_set_link_callback(&my_netif, link_callback);

	printf("4th settings:\n");
        printf("IP Address: %s\n", ip4addr_ntoa(&(my_netif.ip_addr)));
        printf("Netmask: %s\n", ip4addr_ntoa(&(my_netif.netmask)));
        printf("Gateway: %s\n", ip4addr_ntoa(&(my_netif.gw)));

        netif_set_up(&my_netif);

        printf("Starting custom lwIP, local interface IP is %s\n", ip4addr_ntoa(&ipaddr));
        printf("Netmask: %s, Gateway: %s\n", ip4addr_ntoa(&netmask), ip4addr_ntoa(&gw));
}
*/

void
default_netif_poll(void)
{
     printf("%s\n", __func__);
     netif_poll(&my_netif);
}

void
default_netif_shutdown(void)
{
	 printf("%s\n", __func__);
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
        printf("%s\n", __func__);
	if( es != NULL ) {
                if( es->p ) {
                        pbuf_free(es->p);
                }

        mem_free(es);
        }
}

static void my_tcpecho_raw_close(struct tcp_pcb *tpcb, struct tcpecho_raw_state *es) {
        printf("%s\n", __func__);
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
        printf("%s\n", __func__);
        while( wr_err == ERR_OK && es->p != NULL && es->p->len <= tcp_sndbuf(tpcb) ) {
                ptr = es->p;
                printf("%d $$$$$$$$$$$$ %s $$$$$$$$$$$$\n", cnt++ ,(char *)ptr->payload);
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
                        printf("Unknown error\n");
                }
        }
}

static void my_tcpecho_raw_error(void *arg, err_t err) {
        struct tcpecho_raw_state* es;
        LWIP_UNUSED_ARG(err);
	printf("%s\n", __func__);
        es = (struct tcpecho_raw_state *)arg;
        my_tcpecho_raw_free(es);
}


static err_t my_tcpecho_raw_poll(void *arg, struct tcp_pcb* tpcb) {
        err_t ret_err;
        struct tcpecho_raw_state* es;

        printf("%s\n", __func__);
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

        printf("%s\n", __func__);
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

        printf("%s\n", __func__);
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
        printf("%s\n", __func__);
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
        printf("%s\n", __func__);
	if(my_tcpecho_raw_pcb != NULL) {
                err_t err;

                IP4_ADDR(&server_ip, 127, 0, 0, 1);
                err = tcp_bind(my_tcpecho_raw_pcb, &server_ip, 7);
                if( err == ERR_OK ) {
                        printf("TCP bind successful on port 7\n");
                        my_tcpecho_raw_pcb = tcp_listen(my_tcpecho_raw_pcb);
                        tcp_accept(my_tcpecho_raw_pcb, my_tcpecho_raw_accept);
                } else {
                        printf("Binding new pcb has failed with errno %d\n", err);
                        return;
                }
        } else {
                printf("Creating new pcb has failed.\n");
                return;
        }
}

static void my_init(void *arg) {
	//uint32_t seed;
	//TEE_GenerateRandom(&seed, sizeof(seed));
	//srand(seed);
        printf("%s\n", __func__);
	LWIP_UNUSED_ARG(arg);
        //init_my_netif();
        my_tcpecho_raw_init();
}


static void main_loop(void) {
        printf("%s\n", __func__);
	lwip_init();
        my_init(NULL);
        while(!LWIP_EXAMPLE_APP_ABORT()) {
		sys_check_timeouts();
                netif_poll_all();
		TEE_Wait(1000);
        }
        /* netif_poll_all(); */
        default_netif_shutdown();
}


TEE_Result TA_CreateEntryPoint(void) {

   DMSG("has been called");
   
   return TEE_SUCCESS;
}



void TA_DestroyEntryPoint(void) {

   printf("\n\n%d\n\n", netif_is_up(&my_netif));

   DMSG("has been called");

}



TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
                TEE_Param __maybe_unused params[4], void __maybe_unused **sess_ctx) {

   uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE,
					      TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

   DMSG("has been called");

   if ( param_types != exp_param_types ) {
      return TEE_ERROR_BAD_PARAMETERS;
   }

   (void)&params;
   (void)&sess_ctx;

   IMSG("Hello lwIP!\n");

   return TEE_SUCCESS;

}



void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx) {

   (void)&sess_ctx;

   IMSG("BYE lwIP!\n");

}



static TEE_Result pseudo_main(uint32_t param_types, TEE_Param params[4]) {

   uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                                                   TEE_PARAM_TYPE_NONE,
                                                   TEE_PARAM_TYPE_NONE,
                                                   TEE_PARAM_TYPE_NONE);

   for(int i = 1; i <= 100; i++) {
        printf("server_TA: %d\n", i);
   }

   DMSG("has been called");

   if( param_types != exp_param_types ) {
      return TEE_ERROR_BAD_PARAMETERS;
   }

   //char *fromREE = (char *)params[0].memref.buffer;
   
   //IMSG("got [%s] from Normal World", fromREE);

   /*
    IMSG("Before Listening");
    server.pcb = tcp_listen(server.pcb);

    if(server.pcb == NULL) {
      printf("server.pcb = tcp_listen(server.pcb) went wrong\n");
        return TEE_ERROR_GENERIC;
   }

    IMSG("After Listening");
    IMSG("open session to client TA for setting up tcp_accept");
*/

      main_loop();


      //tcp_accept(server.pcb, tcp_accept_callback);

   TEE_Wait(500);
   TEE_CloseTASession(session);
   return TEE_SUCCESS;

}



TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx, uint32_t cmd_id,
                        uint32_t param_types, TEE_Param params[4])

{
        (void)&sess_ctx; /* Unused parameter */

        switch (cmd_id) {
        case TA_LWIP_INIT:
                return pseudo_main(param_types, params);
        default:
                return TEE_ERROR_BAD_PARAMETERS;
        }
}







/* This function is only required to prevent arch.h including stdio.h
 * (which it does if LWIP_PLATFORM_ASSERT is undefined)
 */
/*
void lwip_example_app_platform_assert(const char *msg, int line, const char *file)
{
  printf("Assertion \"%s\" failed at line %d in %s\n", msg, line, file);
  //fflush(NULL);
  abort();
}
*/
