#include "lwip/opt.h"

// #include "client.h"

/* OPTEE includes */
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <ta_client_side.h>


#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/tcp.h"
#include "lwip/err.h"
#include "lwip/init.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/inet_chksum.h"
#include "lwip/prot/ip4.h"

#include "lwip/etharp.h" 

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


enum client_states
{

  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

struct client_state
{
  u8_t state;
  u8_t retries;
  struct tcp_pcb *pcb;
  struct pbuf *p;
};

int counter = 0;

uint8_t data[100];

/* create a struct to store data */

struct client_state *esTx = 0;

struct tcp_pcb *pcbTx = 0;



static err_t tcp_client_raw_connected(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_client_raw_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_client_raw_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_client_raw_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);


static void tcp_client_raw_send(struct tcp_pcb *tpcb, struct client_state *es);

static void tcp_client_connection_close(struct tcp_pcb *tpcb, struct client_state *es);

static void tcp_client_handle(struct tcp_pcb *tpcb, struct client_state *es);


void
client_init(void)

{
  struct tcp_pcb *client = tcp_new_ip_type(IPADDR_TYPE_ANY);
  ip_addr_t destIPADDR;
  IP_ADDR4(&destIPADDR, 127, 0, 0, 1);
  printf("test1\n");
  tcp_connect(client, &destIPADDR, 7, tcp_client_raw_connected);
}



/* tcp를 연결 */

static err_t tcp_client_raw_connected(void *arg, struct tcp_pcb *newpcb, err_t err) {

  err_t ret;

  struct client_state *es; /* es가 무엇을 의미할까?? */



  LWIP_UNUSED_ARG(arg);

  LWIP_UNUSED_ARG(err);


  /* tcp 정보를 유지하기 위해서는 메모리 할당을 통한 유지 필요 */

  es = (struct client_state *)mem_malloc(sizeof(struct client_state));



  printf("tcp client raw connected try\n");

  if (es != NULL) {

    es->state = ES_ACCEPTED;

    es->pcb = newpcb;

    es->retries = 0;

    es->p = NULL;



    tcp_arg(newpcb, es);

    tcp_recv(newpcb, tcp_client_raw_recv);

    tcp_poll(newpcb, tcp_client_raw_poll, 0);

    tcp_sent(newpcb, tcp_client_raw_sent);



    tcp_client_handle(newpcb, es);



	printf("open connection success\n");



    ret = ERR_OK;

  } else {

    tcp_client_connection_close(newpcb, es);

    ret = ERR_MEM;

	printf("Error occur at open connection\n");

  }

  return ret;

}





/* 클라이언트 recv */

static err_t tcp_client_raw_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {

  struct client_state *es;

  err_t ret;



  LWIP_ASSERT("arg != NULL", arg != NULL);

  es = (struct client_state *)arg;



  if (p == NULL) {

    es->state = ES_CLOSING;

    if (es->p == NULL) {

      tcp_client_connection_close(tpcb, es);

    } else {

      tcp_client_raw_send(tpcb, es);

    }

    ret = ERR_OK;

  } else if (err != ERR_OK) {

    LWIP_ASSERT("no pbuf expected here", p == NULL);

    ret = err;

  } else if (es->state == ES_ACCEPTED) {

    es->state = ES_RECEIVED;

    es->p = p;

    tcp_client_raw_send(tpcb, es);

    ret = ERR_OK;

  } else if (es->state == ES_RECEIVED) {

    /* read some more data */

    if(es->p == NULL) {

      es->p = p;

      tcp_client_raw_send(tpcb, es);

    } else {

      struct pbuf *ptr;



      /* chain pbufs to the end of what we recv'ed previously  */

      ptr = es->p;

      pbuf_cat(ptr,p);

    }

    ret = ERR_OK;

  } else {

    tcp_recved(tpcb, p->tot_len);

    pbuf_free(p);

    ret = ERR_OK;

  }

  return ret;

}



static err_t tcp_client_raw_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {

  struct client_state *es;

  LWIP_UNUSED_ARG(len);



  es = (struct client_state *)arg;

  es->retries = 0;



  if (es->p != NULL) {

    tcp_sent(tpcb, tcp_client_raw_sent);

    tcp_client_raw_send(tpcb, es);

  } else {

    if (es->state == ES_CLOSING) {

      tcp_client_connection_close(tpcb, es);

    }

  }

  return ERR_OK;

}







/* 할당된 메모리를 해제 */

static void tcp_client_raw_free(struct client_state *es) { 

  if (es != NULL) {

    if (es->p) {

      pbuf_free(es->p);

    }

    mem_free(es);

  }

}



static void tcp_client_connection_close(struct tcp_pcb *tpcb, struct client_state *es) {

  tcp_arg(tpcb, NULL);

  tcp_recv(tpcb, NULL);

  tcp_sent(tpcb, NULL);

  tcp_err(tpcb, NULL);

  tcp_poll(tpcb, NULL, 0);



  tcp_client_raw_free(es);



  tcp_close(tpcb);

}







/* 페이로드의 데이터를 전달 */

static void tcp_client_raw_send(struct tcp_pcb *tpcb, struct client_state *es) {

  struct pbuf *ptr;

  err_t ret = ERR_OK;



  while ((ret == ERR_OK) && (es->p != NULL) && (es->p->len <= tcp_sndbuf(tpcb))) {

    ptr = es->p; /* 전달할 패킷의 패이로드를 포인팅 */



    ret = tcp_write(tpcb, ptr->payload, ptr->len, 1); /* 페이로드 부분을 이용하여 tcp_write를 수행 */

    if (ret == ERR_OK) {

      u16_t plen;



      plen = ptr->len; /* 보내야 할 데이터가 fragmentation이 발생하는 경우 */

      es->p = ptr->next;



      if (es->p != NULL) { /* buffer의 래퍼런스를 다음 페킷으로 이동 */

        pbuf_ref(es->p);

      }



      pbuf_free(ptr); /* chop first pbuf from chain */

      tcp_recved(tpcb, plen); /* we can read more data now */

    } else if (ret == ERR_MEM) {

      es->p = ptr;

    } else {



    }

  }

}







static err_t tcp_client_raw_poll(void *arg, struct tcp_pcb *tpcb) {

  err_t ret;

  struct client_state *es;



  es = (struct client_state *)arg;

  if (es != NULL) { /* arg 파라미터가 NULL이 아닌 경우 */

    if (es->p != NULL) {



    } else {

      if (es->state == ES_CLOSING) /* 상태가 닫혀있는 상태인 경우 */

      {

        tcp_client_connection_close(tpcb, es);

      }

    }

    ret = ERR_OK;

  }

  else { /* arg 파라미터가 NULL인 경우 */

    tcp_abort(tpcb); 

    ret = ERR_ABRT;

  }

  return ret;

}



static void tcp_client_handle (struct tcp_pcb *tpcb, struct client_state *es)

{

	/* get the Remote IP */

	ip_addr_t inIP = tpcb->remote_ip;

	uint16_t inPort = tpcb->remote_port;



	/* Extract the IP */

	char *remIP = ipaddr_ntoa(&inIP);

	printf("%s", remIP);

	printf("%d", inPort);

	esTx = es;

	pcbTx = tpcb;



	counter++;

}

TEE_Result TA_CreateEntryPoint(void) {
	DMSG("has been called");

	return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {
	DMSG("shut down client");
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

   IMSG("Hello client!\n");

   return TEE_SUCCESS;

}

void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx) {

   (void)&sess_ctx;

   IMSG("BYE client!\n");

}

static TEE_Result pseudo_main(uint32_t param_types, TEE_Param params[4]) {

   uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                                                   TEE_PARAM_TYPE_NONE,
                                                   TEE_PARAM_TYPE_NONE,
                                                   TEE_PARAM_TYPE_NONE);
   lwip_init();
   client_init();

   while(!LWIP_EXAMPLE_APP_ABORT()) {
	   sys_check_timeouts();
	   netif_poll_all();
	   printf("%s\n", __func__);
   	   TEE_Wait(300);
   }

   DMSG("has been called");

   if( param_types != exp_param_types ) {
      return TEE_ERROR_BAD_PARAMETERS;
   }



   TEE_Wait(500);
   return TEE_SUCCESS;
}

TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx, uint32_t cmd_id,
                        uint32_t param_types, TEE_Param params[4])

{
        (void)&sess_ctx; /* Unused parameter */

        switch (cmd_id) {
        case TA_CLIENT_INIT:
                return pseudo_main(param_types, params);
        default:
                return TEE_ERROR_BAD_PARAMETERS;
        }
}

