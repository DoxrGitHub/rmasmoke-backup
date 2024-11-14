#include <tss2/tss2_esys.h>
#include <tss2/tss2_sys.h>
#include <tss2/tss2_tpm2_types.h>

#define ESYS_MAX_SIZE_METADATA 3072
typedef struct ESYS_CRYPTO_CONTEXT_BLOB ESYS_CRYPTO_CONTEXT_BLOB;

typedef UINT32 TSS2_ESYS_RC;


#ifndef TSS2_RC_SUCCESS
#define TSS2_RC_SUCCESS 0
#endif
/** Type of resource
 */
typedef UINT32 IESYSC_RESOURCE_TYPE_CONSTANT;
#define IESYSC_KEY_RSRC                1    /**< Tag for key resource */
#define IESYSC_NV_RSRC                 2    /**< Tag for NV Ram resource */
#define IESYSC_SESSION_RSRC            3    /**< Tag for session resources */
#define IESYSC_DEGRADED_SESSION_RSRC   4    /**< Tag for degraded session resources */
#define IESYSC_WITHOUT_MISC_RSRC       0    /**< Tag for other resources, e.g. PCR register, hierarchies */

/** Type to indicate parameter encryption (by TPM)
 */
typedef UINT32 IESYSC_PARAM_ENCRYPT;
#define ENCRYPT                        1    /**< Parameter encryption by TPM */
#define NO_ENCRYPT                     0    /**< No parameter encryption by TPM */

/** Type to indicate parameter decryption (by TPM)
 */
typedef UINT32 IESYSC_PARAM_DECRYPT;
#define DECRYPT                        1    /**< Parameter decryption by TPM */
#define NO_DECRYPT                     0    /**< No parameter decryption by TPM */

/** Type of policy authorization
 */
typedef UINT32 IESYSC_TYPE_POLICY_AUTH;
#define POLICY_PASSWORD                2    /**< Marker to include auth value of the authorized object */
#define POLICY_AUTH                    1    /**< Marker to include the auth value in the HMAC key */
#define NO_POLICY_AUTH                 0    /**< no special handling */


/** Type for representing TPM-Session
 */
typedef struct {
    TPM2B_NAME                             bound_entity;    /**< Entity to which the session is bound */
    TPM2B_ENCRYPTED_SECRET                encryptedSalt;    /**< Encrypted salt which can be provided by application */
    TPM2B_DATA                                     salt;    /**< Salt computed if no encrypted salt is provided */
    TPMT_SYM_DEF                              symmetric;    /**< Algorithm selection for parameter encryption */
    TPMI_ALG_HASH                              authHash;    /**< Hashalg used for authorization */
    TPM2B_DIGEST                             sessionKey;    /**< sessionKey used for KDFa to compute symKey */
    TPM2_SE                                 sessionType;    /**< Type of the session (HMAC, Policy) */
    TPMA_SESSION                      sessionAttributes;    /**< Flags which define the session behaviour */
    TPMA_SESSION                  origSessionAttributes;    /**< Copy of flags which define the session behaviour */
    TPM2B_NONCE                             nonceCaller;    /**< Nonce computed by the ESAPI for every session call */
    TPM2B_NONCE                                nonceTPM;    /**< Nonce which is returned by the TPM for every session call */
    IESYSC_PARAM_ENCRYPT                        encrypt;    /**< Indicate parameter encryption by the TPM */
    IESYSC_PARAM_DECRYPT                        decrypt;    /**< Indicate parameter decryption by the TPM */
    IESYSC_TYPE_POLICY_AUTH         type_policy_session;    /**< Field to store markers for policy sessions */
    UINT16                             sizeSessionValue;    /**< Size of sessionKey plus optionally authValue */
    BYTE                 sessionValue [2*sizeof(TPMU_HA)];    /**< sessionKey || AuthValue */
    UINT16                                sizeHmacValue;    /**< Size of sessionKey plus optionally authValue */
} IESYS_SESSION;

/** Selector type for esys resources
 */
typedef UINT32                  IESYSC_RESOURCE_TYPE;

/** Type for representing public info of a TPM-Resource
 */
typedef union {
    TPM2B_PUBLIC                           rsrc_key_pub;    /**< Public info for key objects */
    TPM2B_NV_PUBLIC                         rsrc_nv_pub;    /**< Public info for NV ram objects */
    IESYS_SESSION                          rsrc_session;    /**< Internal esapi session information */
    TPMS_EMPTY                               rsrc_empty;    /**< no specialized date for resource */
} IESYS_RSRC_UNION;

/** Type for representing TPM-Resource
 */
typedef struct {
    TPM2_HANDLE                                  handle;    /**< Handle used by TPM */
    TPM2B_NAME                                     name;    /**< TPM name of the object */
    IESYSC_RESOURCE_TYPE                       rsrcType;    /**< Selector for resource type */
    IESYS_RSRC_UNION                               misc;    /**< Resource specific information */
} IESYS_RESOURCE;

/**  Esys resource with size field
 */
typedef struct {
    UINT16                                         size;    /**< size of the operand buffer */
    IESYS_RESOURCE                                 data;    /**< Esys resource data */

} IESYS_METADATA;

/** Type for representing ESYS metadata
 */
typedef struct {
    UINT32                                     reserved;    /**< Must allways be zero */
    TPM2B_CONTEXT_DATA                       tpmContext;    /**< Context information computed by tpm */
    IESYS_METADATA                         esysMetadata;    /**< Meta data of the ESY_TR object */
} IESYS_CONTEXT_DATA;

/** actual header esys_int.h*/
/** Linked list type for object meta data.
 *
 * This structure represents a linked list to store meta data information of
 * type IESYS_RESOURCE.
 */
typedef struct RSRC_NODE_T {
    ESYS_TR esys_handle;        /**< The ESYS_TR handle used by the application
                                     to reference this entry. */
    TPM2B_AUTH auth;            /**< The authValue for this resource object. */
    IESYS_RESOURCE rsrc;        /**< The meta data for this resource object. */
    size_t reference_count;     /**< Reference Count for Esys_TR_FromTPMPublic */
    struct RSRC_NODE_T * next;  /**< The next object in the linked list. */
} RSRC_NODE_T;

typedef struct {
    ESYS_TR tpmKey;
    ESYS_TR bind;
    TPM2_SE sessionType;
    TPMI_ALG_HASH authHash;
    TPM2B_NONCE *nonceCaller;
    TPM2B_NONCE nonceCallerData;
    TPMT_SYM_DEF *symmetric;
    TPMT_SYM_DEF symmetricData;
} StartAuthSession_IN;

typedef struct {
    TPM2B_SENSITIVE_CREATE *inSensitive;
    TPM2B_SENSITIVE_CREATE inSensitiveData;
} CreatePrimary_IN;

typedef struct {
    TPM2B_SENSITIVE_CREATE *inSensitive;
    TPM2B_SENSITIVE_CREATE inSensitiveData;
} Create_IN;

typedef struct {
    ESYS_TR saveHandle;
} ContextSave_IN;

typedef struct {
    TPMS_CONTEXT *context;
    TPMS_CONTEXT contextData;
} ContextLoad_IN;

typedef struct {
    TPM2B_PUBLIC *inPublic;
    TPM2B_PUBLIC inPublicData;
} Load_IN;

typedef struct {
    TPM2B_PUBLIC *inPublic;
    TPM2B_PUBLIC inPublicData;
} LoadExternal_IN;

typedef struct {
    TPM2B_SENSITIVE_CREATE *inSensitive;
    TPM2B_SENSITIVE_CREATE inSensitiveData;
    TPM2B_TEMPLATE *inPublic;
    TPM2B_TEMPLATE inPublicData;
} CreateLoaded_IN;

typedef struct {
    ESYS_TR objectHandle;
    TPMI_DH_PERSISTENT persistentHandle;
} EvictControl_IN;

typedef struct {
    TPM2B_AUTH authData;
} HMAC_Start_IN;

typedef HMAC_Start_IN MAC_Start_IN;

typedef struct {
    ESYS_TR authHandle;
    TPM2B_AUTH newAuth;
} HierarchyChangeAuth_IN;

typedef struct {
    ESYS_TR sequenceHandle;
} SequenceComplete_IN;

typedef struct {
    ESYS_TR policySession;
} Policy_IN;

typedef struct {
    ESYS_TR nvIndex;
    TPM2B_AUTH authData;
    TPM2B_NV_PUBLIC *publicInfo;
    TPM2B_NV_PUBLIC publicInfoData;
} NV_IN;

typedef struct {
    ESYS_TR flushHandle;
} FlushContext_IN;

typedef struct {
    ESYS_TR pcrHandle;
    TPM2B_AUTH authData;
} PCR_IN;

/** Union for input parameters.
 *
 * The input parameters of a command need to be stored if they are needed
 * in corresponding _Finish() function.
 */
typedef union {
    StartAuthSession_IN StartAuthSession;
    CreatePrimary_IN CreatePrimary;
    Create_IN Create;
    ContextSave_IN ContextSave;
    ContextLoad_IN ContextLoad;
    Load_IN Load;
    LoadExternal_IN LoadExternal;
    CreateLoaded_IN CreateLoaded;
    EvictControl_IN EvictControl;
    HMAC_Start_IN HMAC_Start;
    MAC_Start_IN MAC_Start;
    HierarchyChangeAuth_IN HierarchyChangeAuth;
    SequenceComplete_IN SequenceComplete;
    Policy_IN Policy;
    NV_IN NV;
    FlushContext_IN FlushContext;
    PCR_IN PCR;
} IESYS_CMD_IN_PARAM;

/** The states for the ESAPI's internal state machine */
enum _ESYS_STATE {
    _ESYS_STATE_INIT = 0,     /**< The initial state after creation or after
                                   finishing a command. A new command can only
                                   be issued in this state. */
    _ESYS_STATE_SENT,         /**< The state after sending a command to the TPM
                                   before receiving a response. */
    _ESYS_STATE_RESUBMISSION, /**< The state after receiving a response from the
                                   TPM that requires resending of the command.*/
    _ESYS_STATE_INTERNALERROR /**< A non-recoverable error occured within the
                                   ESAPI code. */
};

/*
 * Crypto Backend Support
 */

/** Provide the context for the computation of a hash digest.
 *
 * The context will be created and initialized according to the hash function.
 * @param[out] context The created context (callee-allocated).
 * @param[in] hashAlg The hash algorithm for the creation of the context.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success.
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_HASH_START_FNP)(
        ESYS_CRYPTO_CONTEXT_BLOB ** context,
        TPM2_ALG_ID hashAlg,
        void *userdata);

/** Update the digest value of a digest object from a byte buffer.
 *
 * The context of a digest object will be updated according to the hash
 * algorithm of the context. <
 * @param[in,out] context The context of the digest object which will be updated.
 * @param[in] buffer The data for the update.
 * @param[in] size The size of the data buffer.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success.
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_HASH_UPDATE_FNP)(
        ESYS_CRYPTO_CONTEXT_BLOB * context,
        const uint8_t *buffer,
        size_t size,
        void *userdata);

/** Get the digest value of a digest object and close the context.
 *
 * The digest value will written to a passed buffer and the resources of the
 * digest object are released.
 * @param[in,out] context The context of the digest object to be released
 * @param[out] buffer The buffer for the digest value (caller-allocated).
 * @param[out] size The size of the digest.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success.
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_HASH_FINISH_FNP)(
        ESYS_CRYPTO_CONTEXT_BLOB **context,
        uint8_t *buffer,
        size_t *size,
        void *userdata);

/** Release the resources of a digest object.
 *
 * The assigned resources will be released and the context will be set to NULL.
 * @param[in,out] context The context of the digest object.
 * @param[in/out] userdata information.
 */
typedef void
    (*ESYS_CRYPTO_HASH_ABORT_FNP)(
        ESYS_CRYPTO_CONTEXT_BLOB **context,
        void *userdata);

/** Provide the context an HMAC digest object from a byte buffer key.
 *
 * The context will be created and initialized according to the hash function
 * and the used HMAC key.
 * @param[out] context The created context (callee-allocated).
 * @param[in] hashAlg The hash algorithm for the HMAC computation.
 * @param[in] key The byte buffer of the HMAC key.
 * @param[in] size The size of the HMAC key.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success.
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_HMAC_START_FNP)(
        ESYS_CRYPTO_CONTEXT_BLOB **context,
        TPM2_ALG_ID hashAlg,
        const uint8_t *key,
        size_t size,
        void *userdata);

/** Update and HMAC digest value from a byte buffer.
 *
 * The context of a digest object will be updated according to the hash
 * algorithm and the key of the context.
 * @param[in,out] context The context of the digest object which will be updated.
 * @param[in] buffer The data for the update.
 * @param[in] size The size of the data buffer.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success.
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_HMAC_UPDATE_FNP)(
        ESYS_CRYPTO_CONTEXT_BLOB *context,
        const uint8_t *buffer,
        size_t size,
        void *userdata);

/** Write the HMAC digest value to a byte buffer and close the context.
 *
 * The digest value will written to a passed buffer and the resources of the
 * HMAC object are released.
 * @param[in,out] context The context of the HMAC object.
 * @param[out] buffer The buffer for the digest value (caller-allocated).
 * @param[out] size The size of the digest.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success.
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_HMAC_FINISH_FNP)(
        ESYS_CRYPTO_CONTEXT_BLOB **context,
        uint8_t *buffer,
        size_t *size,
        void *userdata);

/** Release the resources of an HMAC object.
 *
 * The assigned resources will be released and the context will be set to NULL.
 * @param[in,out] context The context of the HMAC object.
 * @param[in/out] userdata information.
 */
typedef void
    (*ESYS_CRYPTO_HMAC_ABORT_FNP)(
        ESYS_CRYPTO_CONTEXT_BLOB **context,
        void *userdata);

/** Compute random TPM2B data.
 *
 * The random data will be generated and written to a passed TPM2B structure.
 * @param[out] nonce The TPM2B structure for the random data (caller-allocated).
 * @param[in] num_bytes The number of bytes to be generated.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success.
 * @retval USER_DEFINED user defined errors on failure.
 * @note: the TPM should not be used to obtain the random data
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_GET_RANDOM2B_FNP)(
        TPM2B_NONCE *nonce,
        size_t num_bytes,
        void *userdata);

/** Computation of an ephemeral ECC key and shared secret Z.
 *
 * According to the description in TPM spec part 1 C 6.1 a shared secret
 * between application and TPM is computed (ECDH). An ephemeral ECC key and a
 * TPM key are used for the ECDH key exchange.
 * @param[in] key The key to be used for ECDH key exchange.
 * @param[in] max_out_size the max size for the output of the public key of the
 *            computed ephemeral key.
 * @param[out] Z The computed shared secret.
 * @param[out] Q The public part of the ephemeral key in TPM format.
 * @param[out] out_buffer The public part of the ephemeral key will be marshaled
 *             to this buffer.
 * @param[out] out_size The size of the marshaled output.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_GET_ECDH_POINT_FNP)(
        TPM2B_PUBLIC *key,
        size_t max_out_size,
        TPM2B_ECC_PARAMETER *Z,
        TPMS_ECC_POINT *Q,
        BYTE *out_buffer,
        size_t *out_size,
        void *userdata);

/** Encrypt data with AES.
 *
 * @param[in] key key used for AES.
 * @param[in] tpm_sym_alg AES type in TSS2 notation (must be TPM2_ALG_AES).
 * @param[in] key_bits Key size in bits.
 * @param[in] tpm_mode Block cipher mode of opertion in TSS2 notation (CFB).
 *            For parameter encryption only CFB can be used.
 * @param[in,out] buffer Data to be encrypted. The encrypted date will be stored
 *                in this buffer.
 * @param[in] buffer_size size of data to be encrypted.
 * @param[in] iv The initialization vector.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_AES_ENCRYPT_FNP)(
        uint8_t *key,
        TPM2_ALG_ID tpm_sym_alg,
        TPMI_AES_KEY_BITS key_bits,
        TPM2_ALG_ID tpm_mode,
        uint8_t *buffer,
        size_t buffer_size,
        uint8_t *iv,
        void *userdata);

/** Decrypt data with AES.
 *
 * @param[in] key key used for AES.
 * @param[in] tpm_sym_alg AES type in TSS2 notation (must be TPM2_ALG_AES).
 * @param[in] key_bits Key size in bits.
 * @param[in] tpm_mode Block cipher mode of opertion in TSS2 notation (CFB).
 *            For parameter encryption only CFB can be used.
 * @param[in,out] buffer Data to be decrypted. The decrypted date will be stored
 *                in this buffer.
 * @param[in] buffer_size size of data to be encrypted.
 * @param[in] iv The initialization vector.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_AES_DECRYPT_FNP)(
        uint8_t *key,
        TPM2_ALG_ID tpm_sym_alg,
        TPMI_AES_KEY_BITS key_bits,
        TPM2_ALG_ID tpm_mode,
        uint8_t *buffer,
        size_t buffer_size,
        uint8_t *iv,
        void *userdata);

/** Encrypt data with SM4.
 *
 * @param[in] key key used for SM4.
 * @param[in] tpm_sym_alg SM4 type in TSS2 notation (must be TPM2_ALG_SM4).
 * @param[in] key_bits Key size in bits.
 * @param[in] tpm_mode Block cipher mode of opertion in TSS2 notation (CFB).
 *            For parameter encryption only CFB can be used.
 * @param[in,out] buffer Data to be encrypted. The encrypted date will be stored
 *                in this buffer.
 * @param[in] buffer_size size of data to be encrypted.
 * @param[in] iv The initialization vector.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_SM4_ENCRYPT_FNP)(
        uint8_t *key,
        TPM2_ALG_ID tpm_sym_alg,
        TPMI_SM4_KEY_BITS key_bits,
        TPM2_ALG_ID tpm_mode,
        uint8_t *buffer,
        size_t buffer_size,
        uint8_t *iv,
        void *userdata);

/** Decrypt data with SM4.
 *
 * @param[in] key key used for SM4.
 * @param[in] tpm_sym_alg SM4 type in TSS2 notation (must be TPM2_ALG_SM4).
 * @param[in] key_bits Key size in bits.
 * @param[in] tpm_mode Block cipher mode of opertion in TSS2 notation (CFB).
 *            For parameter encryption only CFB can be used.
 * @param[in,out] buffer Data to be decrypted. The decrypted date will be stored
 *                in this buffer.
 * @param[in] buffer_size size of data to be encrypted.
 * @param[in] iv The initialization vector.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_SM4_DECRYPT_FNP)(
        uint8_t *key,
        TPM2_ALG_ID tpm_sym_alg,
        TPMI_SM4_KEY_BITS key_bits,
        TPM2_ALG_ID tpm_mode,
        uint8_t *buffer,
        size_t buffer_size,
        uint8_t *iv,
        void *userdata);

/** Encryption of a buffer using a public (RSA) key.
 *
 * Encrypting a buffer using a public key is used for example during
 * Esys_StartAuthSession in order to encrypt the salt value.
 * @param[in] pub_tpm_key The key to be used for encryption.
 * @param[in] in_size The size of the buffer to be encrypted.
 * @param[in] in_buffer The data buffer to be encrypted.
 * @param[in] max_out_size The maximum size for the output encrypted buffer.
 * @param[out] out_buffer The encrypted buffer.
 * @param[out] out_size The size of the encrypted output.
 * @param[in] label The label used in the encryption scheme.
 * @param[in/out] userdata information.
 * @retval TSS2_RC_SUCCESS on success
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC
    (*ESYS_CRYPTO_PK_RSA_ENCRYPT_FNP)(
        TPM2B_PUBLIC * pub_tpm_key,
        size_t in_size,
        BYTE *in_buffer,
        size_t max_out_size,
        BYTE *out_buffer,
        size_t *out_size,
        const char *label,
        void *userdata);

/** Initialize crypto backend.
 *
 * Initialize internal tables of crypto backend.
 *
 * @param[in/out] userdata Optional userdata pointer.
 *
 * @retval TSS2_RC_SUCCESS ong success.
 * @retval USER_DEFINED user defined errors on failure.
 */
typedef TSS2_RC (*ESYS_CRYPTO_INIT_FNP)(void *userdata);

typedef struct ESYS_CRYPTO_CALLBACKS ESYS_CRYPTO_CALLBACKS;
struct ESYS_CRYPTO_CALLBACKS2 {
    ESYS_CRYPTO_PK_RSA_ENCRYPT_FNP rsa_pk_encrypt;
    ESYS_CRYPTO_HASH_START_FNP hash_start;
    ESYS_CRYPTO_HASH_UPDATE_FNP hash_update;
    ESYS_CRYPTO_HASH_FINISH_FNP hash_finish;
    ESYS_CRYPTO_HASH_ABORT_FNP hash_abort;
    ESYS_CRYPTO_HMAC_START_FNP hmac_start;
    ESYS_CRYPTO_HMAC_UPDATE_FNP hmac_update;
    ESYS_CRYPTO_HMAC_FINISH_FNP hmac_finish;
    ESYS_CRYPTO_HMAC_ABORT_FNP hmac_abort;
    ESYS_CRYPTO_GET_RANDOM2B_FNP get_random2b;
    ESYS_CRYPTO_GET_ECDH_POINT_FNP get_ecdh_point;
    ESYS_CRYPTO_AES_ENCRYPT_FNP aes_encrypt;
    ESYS_CRYPTO_AES_DECRYPT_FNP aes_decrypt;
    ESYS_CRYPTO_SM4_ENCRYPT_FNP sm4_encrypt;
    ESYS_CRYPTO_SM4_DECRYPT_FNP sm4_decrypt;
    ESYS_CRYPTO_INIT_FNP init;
    void *userdata;
};
struct ESYS_CONTEXT {
    enum _ESYS_STATE state;      /**< The current state of the ESAPI context. */
    TSS2_SYS_CONTEXT *sys;       /**< The SYS context used internally to talk to
                                      the TPM. */
    ESYS_TR esys_handle_cnt;     /**< The next free ESYS_TR number. */
    RSRC_NODE_T *rsrc_list;      /**< The linked list of all ESYS_TR objects. */
    int32_t timeout;             /**< The timeout to be used during
                                      Tss2_Sys_ExecuteFinish. */
    ESYS_TR session_type[3];     /**< The list of TPM session handles in the
                                      current command execution. */
    RSRC_NODE_T *session_tab[3]; /**< The list of TPM session meta data in the
                                      current command execution. */
    int encryptNonceIdx;         /**< The index of the encrypt session. */
    TPM2B_NONCE *encryptNonce;   /**< The nonce of the encrypt session, or NULL
                                      if no encrypt session exists. */
    int authsCount;              /**< The number of session provided during the
                                      command. */
    int submissionCount;         /**< The current number of submissions of this
                                      command to the TPM. */
    TPM2B_DATA salt;             /**< The salt used during a StartAuthSession.*/
    IESYS_CMD_IN_PARAM in;       /**< Temporary storage for Input parameters
                                      needed in corresponding _Finish function*/
    ESYS_TR esys_handle;         /**< Temporary storage for the object's TPM
                                      handle during Esys_TR_FromTPMPublic. */
    TSS2_TCTI_CONTEXT *tcti_app_param;/**< The TCTI context provided by the
                                           application during Esys_Initialize()
                                           to be returned from Esys_GetTcti().*/
    void *dlhandle;              /**< The handle of dlopen if the tcti was
                                      automatically loaded. */
    IESYS_SESSION *enc_session;  /**< Ptr to the enc param session.
                                      Used to restore session attributes */
    ESYS_TR sav_session1;        /**< Used to store session for cases where call
                                      with ESYS_TR_NONE is needed to determine object
                                      name */
    ESYS_TR sav_session2;
    ESYS_TR sav_session3;

    ESYS_CRYPTO_CALLBACKS2 crypto_backend; /**< The backend function pointers to use
                                              for crypto operations */
};
