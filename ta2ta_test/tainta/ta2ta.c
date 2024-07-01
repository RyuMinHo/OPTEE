#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <ta2ta_ta.h>


TEE_Result test(void) {
	IMSG("Hello TA in TA");
	return TEE_SUCCESS;
}


TEE_Result TA_CreateEntryPoint(void)
{
	DMSG("TA2: TA_CreateEntryPoint has been called");

	return TEE_SUCCESS;
}


TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
				    TEE_Param __maybe_unused params[4],
				    void __maybe_unused **sess_ctx) {


	IMSG("TA2: TA_OpenSessionEntryPoint has been called");

	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
					    TEE_PARAM_TYPE_NONE,
					    TEE_PARAM_TYPE_NONE,
					    TEE_PARAM_TYPE_NONE);

	if(param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	// Unused parameters 
	(void)&params;
	(void)&sess_ctx;
	
	IMSG("TA2: Hello TA2!");

	return TEE_SUCCESS;
}


void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx) {
	(void)&sess_ctx;
	IMSG("TA2: Goodbye TA2");
}


TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx, uint32_t cmd_id, uint32_t param_types, TEE_Param params[4]) {

	(void)&sess_ctx;
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);
        
	if( param_types != exp_param_types ){
		IMSG("TA2: Failed to InvokeCommandEntryPoint in TA2");
		return TEE_ERROR_BAD_PARAMETERS;
	}

	switch(cmd_id) {
	case TA_TA2TA_CMD:
		IMSG("TA2: TA_TA2TA_CMD has been invoked by TA1");

		return TEE_SUCCESS;
	default:
		return TEE_SUCCESS;
	}
	
}

void TA_DestroyEntryPoint(void) {
        DMSG("TA2: DestroyEntryPoint");
}

