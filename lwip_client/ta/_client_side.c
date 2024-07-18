#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <ta_client_side.h>


#include "lwip/init.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwipopts.h"
#include "tcpecho_raw.h"
#include "lwip/ip.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"
#include "netif/ethernet.h"

#define ONBOARD 0

typedef enum {
    STATE_DISCONNECTED,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_ERROR
} client_state_t;

typedef struct {
    struct tcp_pcb *pcb;
    client_state_t state;
    char message[100];
} client_t;

client_t client = {
    .pcb = NULL,
    .state = STATE_DISCONNECTED,
    .message = "Hello Client\n"
};


struct netif client_netif;

static err_t tcp_connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err);

static err_t client_sent(void *arg, struct tcp_pcb *tpcb, uint16_t len);


static err_t my_netif_init(struct netif *netif) {

	printf("Client network interface init\n");
	netif->hwaddr_len = 6;

#if ONBOARD
	netif->hwaddr[0] = 0x00;
	netif->hwaddr[0] = 0x04;
	netif->hwaddr[0] = 0x9F;
	netif->hwaddr[0] = 0x07;
	netif->hwaddr[0] = 0x57;
	netif->hwaddr[0] = 0x65;
#else
	netif->hwaddr[0] = 0x52;
	netif->hwaddr[0] = 0x54;
	netif->hwaddr[0] = 0x00;
	netif->hwaddr[0] = 0x12;
	netif->hwaddr[0] = 0x34;
	netif->hwaddr[0] = 0x56;
#endif

   netif->mtu = 1500;

    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_UP | NETIF_FLAG_LINK_UP;

}


struct tcp_pcb *client_pcb;

ip_addr_t client_ip;
ip_addr_t ip_to_connect;


// 1. init client pcb

static TEE_Result client_init() {

   lwip_init();

   for( int i = 1; i <= 100; i++ ) {
         printf("client TA: %d\n", i);
      }

   /* NEW */
   client.pcb = tcp_new();

   if ( client.pcb == NULL ) {
      printf("Client PCB is NULL\n");
        return TEE_ERROR_SECURITY;
   }

   IP4_ADDR(&ip_to_connect, 127, 0, 0, 1);
   /*
   tcp_arg(client_pcb, 0);
   tcp_sent(client_pcb, tcp_sent_callback);
   tcp_recv(client_pcb, tcp_recved_callback);
   tcp_err(client_pcb, tcp_err_callback);
   tcp_poll(client_pcb, tcp_poll_callback, 0);
   */

   printf("Init Client pcb Success\n");
   return TEE_SUCCESS;

}


static void connect_timeout(void *arg) {
        if(client.state == STATE_CONNECTING) {
                printf("Connection attempt has timed out\n");
                tcp_abort(client.pcb);
                client.state = STATE_DISCONNECTED;
        }
}


/* 2. client connect setting */

static TEE_Result client_connect() {

   err_t err;
/*
   ip4_addr_t ipaddr, netmask, gw;
   IP4_ADDR(&ipaddr, 192, 168, 1, 101);
   IP4_ADDR(&netmask, 255, 255, 255, 0);
   IP4_ADDR(&gw, 192, 168, 1, 1);

   IP4_ADDR(&client_pcb->local_ip, 192, 168, 1, 101);

   netif_add(&client_netif, &client_pcb->local_ip, &netmask, &gw, NULL, my_netif_init, ethernet_input);

   netif_set_default(&client_netif);
   netif_set_link_up(&client_netif);
   netif_set_default(&client_netif);
   netif_set_up(&client_netif);
   

   uint16_t port = 7;
*/
   printf("Attempting to connect...\n");
   client.state = STATE_CONNECTING;
   err = tcp_connect(client.pcb, &ip_to_connect, 7, tcp_connect_callback); 
   sys_timeout(5000, connect_timeout, NULL);


   if (err != ERR_OK) {   // tcp connection failed
      printf("Attemp to connect has failed with error code : %d\n", err);
      client.state = STATE_ERROR;
      return TEE_ERROR_NO_DATA;
   }

   /*

   printf("Before write message\n");
   
   err = tcp_write(client.pcb, client.message, strlen(client.message), TCP_WRITE_FLAG_COPY);

   if (err != ERR_OK) {
      printf("ERROR TCP WRITE!   %d\n", err);
      return TEE_ERROR_NO_DATA;
   }

  
   tcp_sent(client.pcb, client_sent);


   printf("After tcp_sent\n");
*/

      return TEE_SUCCESS;

}



/* 3. call back function by 2 */

static err_t tcp_connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {


  	printf("BEFORE Logic Start\n");
	
	LWIP_UNUSED_ARG(arg);

	printf("Connection established, trying to send message..\n");
	client.state = STATE_CONNECTED;

	err = tcp_write(tpcb, client.message, strlen(client.message), TCP_WRITE_FLAG_COPY);
	if(err != ERR_OK) {
		printf("ERROR TCP WRITE with error code %d\n", err);
		client.state = STATE_ERROR;
		return err;
	}
/*
	tcp_arg(pcb_new, 0); //no argument is used
	tcp_sent(pcb_new, tcp_callback_sent); //register send callback
	tcp_recv(pcb_new, tcp_callback_received);  //register receive callback
	tcp_err(pcb_new, tcp_callback_error); //register error callback
	tcp_poll(pcb_new, tcp_callback_poll, 0); //register poll callback
*/

//   printf("BEFORE CLIENT CONNECTED\n");
//   tcp_write(tpcb, client.message, strlen(client.message), TCP_WRITE_FLAG_COPY);
//   tcp_close(tpcb);
//   printf("CLIENT CONNECTED\n");
     tcp_sent(tpcb, client_sent);
     return ERR_OK;

}



static err_t client_sent(void *arg, struct tcp_pcb *tpcb, uint16_t len) {

   printf("In Cleint Sent\n");   



   err_t err = tcp_output(client.pcb);

   if (err != ERR_OK) {

      printf("Client Send callback error code : %d\n", err);

      return err;

   }
   tcp_close(tpcb);
   return ERR_OK;

}

/*
static void connect_timeout(void *arg) {
	if(client.state == STATE_CONNECTING) {
		printf("Connection attempt has timed out\n");
		tcp_abort(client.pcb);
		client.state = STATE_DISCONNECTED;
	}
}
*/



TEE_Result TA_CreateEntryPoint(void) {

   DMSG("in client side");

   return TEE_SUCCESS;

}



void TA_DestroyEntryPoint(void) {

   printf("client net interface : %d\n", netif_is_up(&client_netif));

   DMSG("has been called");

}



TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,

                TEE_Param __maybe_unused params[4],

                void __maybe_unused **sess_ctx) {

   uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,

                     TEE_PARAM_TYPE_NONE,

                     TEE_PARAM_TYPE_NONE,

                     TEE_PARAM_TYPE_NONE);

   DMSG("has been called");

   if( param_types != exp_param_types ) {

      return TEE_ERROR_BAD_PARAMETERS;

   }



   (void)&params;

   (void)&sess_ctx;



   IMSG("IN CLIENT!\n");



   return TEE_SUCCESS;

}



void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx) {

   (void)&sess_ctx;

   IMSG("BYE CLIENT");

}



TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd_id,

               uint32_t param_types, TEE_Param params[4]) {

   (void)&sess_ctx;



   switch(cmd_id) {

      case TA_CLIENT_INIT:

         printf("TA client init\n");

         return client_init();

      case TA_CLIENT_CONNECT:

         printf("TA client connect\n");

         return client_connect();

      default:

         return TEE_ERROR_BAD_PARAMETERS;

   }

}




