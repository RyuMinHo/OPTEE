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

typedef enum {
    STATE_DISCONNECTED,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_ERROR
} client_state_t;

typedef struct {
    struct tcp_pcb *pcb;
    client_state_t state;
	char message[100] = "Hello Client\n";
} client_t;

// global var
client_t client = {
	.pcb = NULL, 
	.state = STATE_DISCONNECTED 
};


ip_addr_t client_ip;


// 1. init client pcb
static TEE_Result client_init() {
	client.pcb = tcp_new();
	printf("INIT CLIENT PCB\n");
	return TEE_SUCCESS;
}

/* 2. client connect setting */
static TEE_Result client_connect(uint32_t param_types, TEE_Param params[4]) {
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                                   TEE_PARAM_TYPE_NONE,
                                                   TEE_PARAM_TYPE_NONE,
                                                   TEE_PARAM_TYPE_NONE);

	if (param_types != exp_param_types) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    strncpy(client.message, (char *)params[0].memref.buffer, sizeof(client.message) - 1);
    client.message[sizeof(client.message) - 1] = '\0';
	
	printf("strcpy clear\n");

    lwip_init();
    // client.pcb = tcp_new();
    if (!client.pcb) {
        return TEE_ERROR_OUT_OF_MEMORY;
    }
	ip4_addr_t ipaddr, netmask, gw;
	IP4_ADDR(&ipaddr, 192, 168, 1, 101);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw, 192, 168, 1, 1);
    // IP4_ADDR(&client.server_ip, 192, 168, 1, 100);
    client.port = 7;

    tcp_connect(client.pcb, &client.server_ip, client.port, client_connected); 

    return TEE_SUCCESS;
}

/* 3. call back function by 2 */
static err_t client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
	struct tcp_pcb = tcp_new(); 
    if(err != ERR_OK) {
		return err;
	}
	tcp_write(tpcb, client.message, strlen(client.message), TCP_WRITE_FLAG_COPY);
    tcp_close(tpcb);
	printf("CLIENT CONNECTED\n");
	return ERR_OK;
}



TEE_Result TA_CreateEntryPoint(void) {
	DMSG("in client side");
	return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {
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

TEE_Result TA_InvokeTACommandEntryPoint(void *sess_ctx, uint32_t cmd_id,
					uint32_t param_types, TEE_Param params[4]) {
	(void)&sess_ctx;

	switch(cmd_id) {
		case TA_CLIENT_INIT:
			printf("TA client init\n");
			return client_init();
		case TA_CLIENT_CONNECT:
			return client_connect(param_types, params);
		default:
			return TEE_ERROR_BAD_PARAMETERS;
	}
}

