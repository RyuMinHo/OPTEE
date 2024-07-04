#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* OPTEE includes */
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <lwip_test_ta.h>

#include "lwip/init.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "lwipopts.h"
#include "tcpecho_raw.h"
#include "lwip/ip.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"

//static struct netif loop_netif;
//static struct tcp_pcb *client_pcb = NULL;
//static err_t client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
//static err_t client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
//static char *data_to_send = NULL;

typedef enum {
    CLIENT_STATE_DISCONNECTED,
    CLIENT_STATE_CONNECTING,
    CLIENT_STATE_CONNECTED,
    CLIENT_STATE_ERROR
} client_state_t;

typedef struct {
    struct tcp_pcb *pcb;
    client_state_t state;
} client_t;

client_t client = { .pcb = NULL, .state = CLIENT_STATE_DISCONNECTED };

void event_loop() {
    while (client.state != CLIENT_STATE_CONNECTED && client.state != CLIENT_STATE_ERROR) {
        sys_check_timeouts(); // lwIP 타임아웃 처리

        if (client.state == CLIENT_STATE_ERROR) {
            printf("Connection failed\n");
            break;
        }

        TEE_Wait(100); // 0.1초 대기
    }
    if (client.state == CLIENT_STATE_CONNECTED) {
        printf("Connection successful\n");
        // 연결 성공 후 추가 작업 수행
    }

    if (client.state == CLIENT_STATE_CONNECTING) {
    	printf("Trying to Connect..\n");
    }
}


static err_t server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    IMSG("starting point of server_recv()");
    if (err != ERR_OK) {
        printf("Receive error\n");
        return err;
    }

    err_t write_err = tcp_write(tpcb, p->payload, p->len, 0);
    if (write_err != ERR_OK) {
        printf("Failed to write echo data\n");
        return write_err;
    }

    tcp_recved(tpcb, p->tot_len);
    printf("%s\n", p->payload);
    pbuf_free(p);

    IMSG("end of server_recv()");

    return ERR_OK;
}


static err_t server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    if (err != ERR_OK) {
        printf("Accept failed\n");
        return err;
    }

    client.pcb = newpcb;
    client.state = CLIENT_STATE_CONNECTED;

    tcp_recv(newpcb, server_recv);

    return ERR_OK;
}

static err_t server_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    IMSG("starting point of server_connected()");
    if (err != ERR_OK) {
        printf("Connection failed\n");
        client.state = CLIENT_STATE_ERROR;
        return err;
    }

    printf("Connected to server\n");
    //client.state = CLIENT_STATE_CONNECTED;
    printf("client.state = %d\n", client.state);
    tcp_recv(tpcb, server_recv);

    IMSG("end of server_connected()");
    
    return ERR_OK;
}

void connect_to_server(ip_addr_t server_ip) {
    IMSG("In connect_to_server()");
    /*
    ip_addr_t server_ip;
    IP_ADDR4(&server_ip, 127, 0, 0, 1);
    */
    client.pcb = tcp_new();
    if(client.pcb == NULL) {
    	IMSG("Failed to create client.pcb");
	return ;
    }

    IMSG("Attempting to connect server at 127.0.0.1:7");

    err_t err = tcp_connect(client.pcb, &server_ip, 7, server_connected);
    printf("ERRNO: %d\n", err);
    if (err != ERR_OK) {
        printf("Failed to connect to server: %d\n", err);
        client.state = CLIENT_STATE_ERROR;
    } else {
        client.state = CLIENT_STATE_CONNECTING;
	printf("Connecting to server...\n");
    }

    IMSG("End of connect_to_server()");
}

static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  if (err != ERR_OK) {
    IMSG("Receive error");
    return err;
  }

  if (p == NULL) {
    IMSG("Connection closed by server");
    tcp_close(tpcb);
    return ERR_OK;
  }

  IMSG("Received packet: %.*s", p->len, (char *)p->payload);

  tcp_recved(tpcb, p->tot_len);
  pbuf_free(p);

  return ERR_OK;
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
  if (err != ERR_OK) {
    IMSG("Connection failed");
    return err;
  }


  IMSG("Connected to server");

  char *message = "Hello from Secure World!";
  err = tcp_write(tpcb, message, strlen(message), TCP_WRITE_FLAG_COPY);
  if (err != ERR_OK) {
    IMSG("Failed to send message");
    return err;
  }

  tcp_recv(tpcb, tcp_client_recv);

  return ERR_OK;
}


/*****************************************************************
 * 			OPTEE setting
 *****************************************************************/


TEE_Result TA_CreateEntryPoint(void) {
	DMSG("has been called");
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

static TEE_Result my_init(uint32_t param_types, TEE_Param params[4]) {
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                                   TEE_PARAM_TYPE_NONE,
                                                   TEE_PARAM_TYPE_NONE,
                                                   TEE_PARAM_TYPE_NONE);
	DMSG("has been called");

	if( param_types != exp_param_types ) {
		return TEE_ERROR_BAD_PARAMETERS;
	}

	char *fromREE = (char *)params[0].memref.buffer;
	//int fromREE_str_len = params[0].memref.size;

	IMSG("Starting lwIP Initializing\n");
	
	lwip_init();
/*
	IMSG("Initialized lwIP\n");

//	netif_init();

//	ip4_addr_t loop_ip;
//	IP4_ADDR(&loop_ip, 127, 0, 0, 1);
//	netif_add(&loop_netif, &loop_ip, IP4_ADDR_BROADCAST, IP4_ADDR_BROADCAST, NULL, NULL, NULL);
//	netif_set_up(&loop_netif);

	IMSG("tcpecho_raw_init() start");
	tcpecho_raw_init();
	IMSG("tcpecho_raw_init() complete");
		
	IMSG("got [%s] from Normal World", fromREE);
	
//	data_to_send = fromREE;

//	client_pcb = tcp_new();
//	if(client_pcb == NULL) {
//		EMSG("Failed to create client PCB");
//		return TEE_ERROR_GENERIC;
//	}

//	ip_addr_t dest_ip;
//	IP4_ADDR(&dest_ip, 127, 0, 0, 1);

//	err_t err = tcp_connect(client_pcb, &dest_ip, 7, client_connected);
//	if( err != ERR_OK ) {
//		EMSG("Failed to connect to server");
//		return TEE_ERROR_GENERIC;
//	}

//	}	
	IMSG("start creating pcb");
	client_pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if( client_pcb == NULL ) {
		IMSG("Failed to create tcp pcb");
		return TEE_ERROR_GENERIC;
	}
	IMSG("Created pcb");

	

	ip_addr_t loc_ip;
	IP4_ADDR(&loc_ip, 127, 0, 0, 1);
	client_pcb->next = NULL;
	
	//printf("ip addr: %d\n", loc_ip);
	

	IMSG("start connect");
	err_t err = tcp_connect(client_pcb, &loc_ip, 7, tcp_client_connected);
	
	if( err != ERR_OK ) {
		IMSG("Failed to connect");
		return TEE_ERROR_GENERIC;
	}
	IMSG("completed connect");

	err = tcp_write(client_pcb, fromREE, strlen(fromREE), TCP_WRITE_FLAG_MORE);

	if( err != ERR_OK ) {
		IMSG("Failed to write on pcb");
		return TEE_ERROR_GENERIC;
	}

	tcp_recv(client_pcb, my_recv);

*/
	check_netif_status();
	
	ip_addr_t server_ip;
	IP4_ADDR(&server_ip, 127, 0, 0, 1);
	struct tcp_pcb *server_pcb = tcp_new();
	err_t err = tcp_bind(server_pcb, &server_ip, 7);

	if( err != ERR_OK ) {
		printf("Failed to bind server socket\n");
		return TEE_ERROR_GENERIC;
	}

//	server_pcb = tcp_listen(server_pcb);
//	tcp_accept(server_pcb, server_accept);
	IMSG("Before Listening");
	server_pcb = tcp_listen(server_pcb);
	IMSG("After Listening");

	client.pcb = tcp_new();
    	if(client.pcb == NULL) {
        	IMSG("Failed to create client.pcb");
        	return ;
    	}

	connect_to_server(server_ip);
	
	IMSG("Before Accept");
        tcp_accept(server_pcb, server_accept);
	IMSG("After Accept");

/*
	while(1) {
		sys_check_timeouts();
		TEE_Wait(100);
	}
*/
	IMSG("Before event loop");
	event_loop();

	return TEE_SUCCESS;

}

TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
                        uint32_t cmd_id,
                        uint32_t param_types, TEE_Param params[4])
{
        (void)&sess_ctx; /* Unused parameter */
	
        switch (cmd_id) {
        case TA_LWIP_INIT:
                return my_init(param_types, params);
        default:
                return TEE_ERROR_BAD_PARAMETERS;
        }
}



/* This function is only required to prevent arch.h including stdio.h
 * (which it does if LWIP_PLATFORM_ASSERT is undefined)
 */
void lwip_example_app_platform_assert(const char *msg, int line, const char *file)
{
  printf("Assertion \"%s\" failed at line %d in %s\n", msg, line, file);
  //fflush(NULL);
  abort();
}
/*
static err_t client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        EMSG("Connection failed");
        return err;
    }

    err = tcp_write(tpcb, data_to_send, strlen(data_to_send), TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        EMSG("Failed to write data");
        return err;
    }

    err = tcp_output(tpcb);
    if (err != ERR_OK) {
        EMSG("Failed to send data");
        return err;
    }

    tcp_recv(tpcb, client_recv);
    return ERR_OK;
}

static err_t client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (err != ERR_OK || p == NULL) {
        EMSG("Failed to receive data or connection closed");
        tcp_close(tpcb);
        return err;
    }

    char *received_data = (char *)malloc(p->len + 1);
    memcpy(received_data, p->payload, p->len);
    received_data[p->len] = '\0';

    IMSG("Received: %s", received_data);

    free(received_data);
    pbuf_free(p);
    tcp_close(tpcb);

    return ERR_OK;
}
*/
