/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* For the UUID (found in the TA's h-file(s)) */
#include <hello_world_ta.h>
#include <rtst_socket.h>

int main(void)
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_HELLO_WORLD_UUID;
	uint32_t err_origin;
	printf("ctx address: %p\n", &ctx);
	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/*
	 * Open a session to the "hello world" TA, the TA will print "hello
	 * world!" in the log when the session is created.
	 */
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

	/*
	 * Execute a function in the TA by invoking it, in this case
	 * we're incrementing a number.
	 *
	 * The value of command ID part and how the parameters are
	 * interpreted is part of the interface provided by the TA.
	 */

	/* Clear the TEEC_Operation struct */
	memset(&op, 0, sizeof(op));

	/*
	 * Prepare the argument. Pass a value in the first parameter,
	 * the remaining three parameters are unused.
	 */
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 42;

	/*
	 * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
	 * called.
	 */
	printf("Invoking TA to increment %d\n", op.params[0].value.a);
	res = TEEC_InvokeCommand(&sess, TA_HELLO_WORLD_CMD_INC_VALUE, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
	printf("TA incremented value to %d\n", op.params[0].value.a);
/*
	char buf[1024];
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_INPUT, )
*/	
	TEEC_Operation op_init;
	TEEC_SharedMemory ip_shm;
	uint32_t port = 7777; // 원하는 포트 번호

	// IP 주소를 위한 shared memory 설정
	ip_shm.size = 16; // IPv4 주소의 최대 길이 + 1 (null 종료 문자)
	ip_shm.flags = TEEC_MEM_INPUT;
	res = TEEC_AllocateSharedMemory(&ctx, &ip_shm);
	if (res != TEEC_SUCCESS) {
	    // 에러 처리
	    return 1;
	}

	// IP 주소 복사
	strncpy(ip_shm.buffer, "127.0.0.1", ip_shm.size);

	// Operation 구조체 설정
	memset(&op_init, 0, sizeof(op_init));
	op_init.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_WHOLE, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);
	op_init.params[0].memref.parent = &ip_shm;
	op_init.params[0].memref.size = strlen(ip_shm.buffer) + 1;
	op_init.params[1].value.a = port;

	// 명령 호출
	res = TEEC_InvokeCommand(&sess, TA_UDPSERVERINIT_CMD, &op_init, &err_origin);
	
	// 사용 후 shared memory 해제
	TEEC_ReleaseSharedMemory(&ip_shm);
	
/*	TEEC_SharedMemory txbuf;

	txbuf.size = 30;
	tx.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
	res = TEEC_AllocateSharedMemory(&ctx, &txbuf);
	if(res != TEEC_SUCCESS) {
		printf("TEEC_AllocateSharedMemory failed with code 0x%x\n", res);
                TEEC_CloseSession(&sess);
                TEEC_FinalizeContext(&ctx);
                return 1;
	}

	printf("Enter string: ");
	if( fgets((char *)shm.buffer, 30, stdin) == NULL ) {
                printf("Error reading input\n");
                TEEC_ReleaseSharedMemory(&shm);
                TEEC_CloseSession(&sess);
                TEEC_FinalizeContext(&ctx);
                return 1;
        }

	int len = strlen((char *)txbuf.buffer);
	if(len > 0 && ((char *)txbuf.buffer)[len - 1] == '\n') {
		((char *)txbuf.buffer)[len - 1] = '\0';
	}

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_WHOLE, TEEC_NONE, TEEC_NONE, TEEC_NONE);
	op.params[0].memref.parent = &txbuf;
	op.params[0].memref.size = txbuf.size;
	op.params[0].memref.offset = 0;*/

	TEEC_Operation op_send;
	TEEC_SharedMemory msg_shm;

	// 보낼 메시지를 위한 shared memory 설정
	msg_shm.size = 1024; // 적절한 크기로 조정
	msg_shm.flags = TEEC_MEM_INPUT;
	res = TEEC_AllocateSharedMemory(&ctx, &msg_shm);
	if (res != TEEC_SUCCESS) {
	    // 에러 처리
	    return 1;
	}

	// 메시지 입력 받기
	printf("Enter message to send: ");
	if (fgets(msg_shm.buffer, msg_shm.size, stdin) == NULL) {
	    // 에러 처리
	    TEEC_ReleaseSharedMemory(&msg_shm);
	    return 1;
	}

	// 개행 문자 제거
	char *char_buffer = (char *)msg_shm.buffer;
	size_t len = strlen(char_buffer);
	
	if (len > 0 && char_buffer[len-1] == '\n') {
    		char_buffer[len-1] = '\0';
	}

	// Operation 구조체 설정
	memset(&op_send, 0, sizeof(op_send));
	op_send.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_WHOLE, TEEC_NONE, TEEC_NONE, TEEC_NONE);
	op_send.params[0].memref.parent = &msg_shm;
	op_send.params[0].memref.size = strlen(msg_shm.buffer) + 1;

	// 명령 호출
	res = TEEC_InvokeCommand(&sess, TA_UDPSOCKETSEND_CMD, &op_send, &err_origin);

	// 사용 후 shared memory 해제
	TEEC_ReleaseSharedMemory(&msg_shm);

	/*
	 * We're done with the TA, close the session and
	 * destroy the context.
	 *
	 * The TA will print "Goodbye!" in the log when the
	 * session is closed.
	 */

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return 0;
}
