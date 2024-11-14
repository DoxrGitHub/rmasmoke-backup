// #include <sys/cdefs.h>
#include <tss2/tss2_tpm2_types.h>
#include <cstddef>
typedef struct TSS2_TCTI_OPAQUE_CONTEXT_BLOB TSS2_TCTI_CONTEXT;
typedef struct _TPM20_Header_Out {
  TPM2_ST tag;
  UINT32 responseSize;
  UINT32 responseCode;
} TPM20_Header_Out;
typedef struct _TPM20_Header_In {
  TPM2_ST tag;
  UINT32 commandSize;
  UINT32 commandCode;
} TPM20_Header_In;

 struct  _TSS2_SYS_CONTEXT_BLOB {
    TSS2_TCTI_CONTEXT *tctiContext;
    UINT8 *cmdBuffer;
    UINT32 maxCmdSize;
    UINT8 cmd_header[sizeof(TPM20_Header_In)]; /* Copy of the cmd header to allow reissue */
    TPM20_Header_Out rsp_header;

    TPM2_CC commandCode;    /* In host endian */
    UINT32 cpBufferUsedSize;
    UINT8 *cpBuffer;
    UINT32 *rspParamsSize;
    UINT8 previousStage;
    UINT8 authsCount;
    UINT8 numResponseHandles;

    struct
    {
        UINT16 decryptAllowed:1;
        UINT16 encryptAllowed:1;
        UINT16 decryptNull:1;
        UINT16 authAllowed:1;
    };

    /* Offset to next data in command/response buffer. */
    size_t nextData;
  } ;
enum cmdStates {CMD_STAGE_INITIALIZE,
                CMD_STAGE_PREPARE,
                CMD_STAGE_SEND_COMMAND,
                CMD_STAGE_RECEIVE_RESPONSE,
                CMD_STAGE_ALL = 0xff };
typedef _TSS2_SYS_CONTEXT_BLOB TSS2_SYS_CONTEXT_INT  ;