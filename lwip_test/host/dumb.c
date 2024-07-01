#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tee_client_api.h>

#define SERVER_IP "192.168.5.100"
#define SERVER_PORT 8080

int main() {
	TEEC_Result res;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    uint32_t err_origin;

    // Initialize context
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        fprintf(stderr, "TEEC_InitializeContext failed with code 0x%x\n", res);
        return EXIT_FAILURE;
    }

    // Open session with the TA
    res = TEEC_OpenSession(&ctx, &sess, &TA_UUID, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    if (res != TEEC_SUCCESS) {
        fprintf(stderr, "TEEC_OpenSession failed with code 0x%x origin 0x%x\n", res, err_origin);
        TEEC_FinalizeContext(&ctx);
        return EXIT_FAILURE;
    }

    // Prepare data to send
    const char* message = "Hello, world!";
    size_t message_len = strlen(message);

    // Set up operation
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, 0, 0);
    op.params[0].tmpref.buffer = (void*)message;
    op.params[0].tmpref.size = message_len;

    // Invoke the TA to send data
    res = TEEC_InvokeCommand(&sess, 0, &op, &err_origin);
    if (res != TEEC_SUCCESS) {
        fprintf(stderr, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", res, err_origin);
        TEEC_CloseSession(&sess);
        TEEC_FinalizeContext(&ctx);
        return EXIT_FAILURE;
    }

    // Print received message
    printf("Received from server: %.*s\n", (int)op.params[1].tmpref.size, (char*)op.params[1].tmpref.buffer);

    // Close session and finalize context
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);

    return EXIT_SUCCESS;

}
