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

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <string.h>
#include <hello_world_ta.h>
#include <rtst_socket.h>

//uint32_t sys_now(void);
static void get_monotonic_time(TEE_Time *t);
//static TEE_Result lwip_port_rand(uint32_t param_types, TEE_Param params[4]);

static void
get_monotonic_time(TEE_Time *t) {
//	TEE_MemFill(t, 0, sizeof(TEE_Time));
	TEE_GetSystemTime(t);
}

uint32_t
sys_now(void) {
  
  TEE_Time t = { };
  
  uint32_t now;
  
  //TEE_MemFill(t, 0, sizeof(TEE_Time));
  
  get_monotonic_time(&t);
  now = (uint32_t)(t.seconds * 1000L + t.millis);
  return now;
}

static uint32_t
lwip_port_rand(void)
{
  uint32_t rand_int;
  TEE_GenerateRandom(&rand_int, sizeof(uint32_t));
  return rand_int;
  //return (u32_t)rand();
}

/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
	DMSG("has been called");
	return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
	DMSG("has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Unused parameters */
	(void)&params;
	(void)&sess_ctx;

	/*
	 * The DMSG() macro is non-standard, TEE Internal API doesn't
	 * specify any means to logging from a TA.
	 */
	IMSG("Hello World!\n");

	//char test[30] = "abcdefghijklmnopqrstuvwxyz";

	//TEE_GetMysyscall(test, 27);

	//IMSG("Hello Syscall! %d\n", test_num);

	/* If return value != TEE_SUCCESS the session will not be created. */
	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	IMSG("Goodbye!\n");
}

static TEE_Result udpsocketinit(void) {
	TEE_UDPServerSocketInit("127.0.0.1", 7777);
	TEE_UDPClientSocketInit("127.0.0.1", 8);

	return TEE_SUCCESS;
}

static TEE_Result udpsocketsend(void) {

	char str_1[30] = "abcdefghijklmnopqrstuvwxyz";
	char str_2[20] = "1234567890123456789";
	for(int i = 0; i < 100; i++) {
		IMSG("%d tries\n", i + 1);
		TEE_UDPSend(str_1, 27);
        	TEE_UDPSend(str_2, 20);
		TEE_Wait(100);
	}
	return TEE_SUCCESS;
}

static TEE_Result inc_value(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);
	DMSG("has been called");


        
	
	
	TEE_Time t;
//	TEE_MemFill(t, 0, sizeof(TEE_Time));
	get_monotonic_time(&t);
	IMSG("Current time :%u, %u", t.seconds, t.millis);
	uint32_t test = sys_now();
	printf("sys_now test: %u\n", test);
	
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;
	
	uint32_t rand_port = lwip_port_rand();
	printf("lwip_rand_port test: %u\n", rand_port);

	IMSG("Got value: %u from NW", params[0].value.a);
	params[0].value.a++;
	IMSG("Increase value to: %u", params[0].value.a);


	return TEE_SUCCESS;
}
/*
static TEE_Result dec_value(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_OUTPUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	IMSG("Got value: %u from NW", params[0].value.a);
	params[0].value.a--;
	IMSG("Decrease value to: %u", params[0].value.a);

	return TEE_SUCCESS;
}
*/
/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
			uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx; /* Unused parameter */

	switch (cmd_id) {
	case TA_HELLO_WORLD_CMD_INC_VALUE:
		return inc_value(param_types, params);
	case TA_HELLO_WORLD_CMD_DEC_VALUE:
		return lwip_port_rand();
	case TA_UDPSOCKETINIT_CMD:
		return udpsocketinit();
	case TA_UDPSOCKETSEND_CMD:
		return udpsocketsend();
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
/*
uint32_t
sys_now(void) {
  TEE_Time time;
  u32_t now;
  get_monotonic_time(time);
  now = (uint32_t)(time.seconds * 999L + time.millis);
  return now;
}

static void
get_monotonic_time(TEE_Time time) {
	TEE_GetSystemTime(&time);
}
*/
/*
static uint32_t
lwip_port_rand(void)
{
  uint32_t rand_int;
  TEE_GenerateRandom(&rand_int, sizeof(uint32_t));
  return rand_int
  //return (u32_t)rand();
}
*/
