/// @file protoconst.h
#if !defined(PROTOCONST_H)
#define PROTOCONST_H

/*

Possible messages:

 - | circuitid | memory    | afu | size |
 - | circuitid | compute   | afu | size | ea |
 - | circuitid | mresponse | afu |      | ea | error |
 - | circuitid | cresponse | afu | size |    | error |

*/

/*! \def    SOCK_PATH
 * \brief default linux socket path used for communication with libthymesisflow
 * server
 */
#define SOCK_PATH "/tmp/thymesisflow.sock"

/*! \def CIRCUIT_ID_SIZE
 * \brief circuit uuid lenght
 */
#define CIRCUIT_ID_SIZE 36

/*! \def MSGTYPE_SIZE
 * \brief maximum size for section identifying the type of message sent/received
 */
#define MSGTYPE_SIZE 16

/*! \def AFUNAME_SIZE
 * \brief maximum size for section identifying the AFU name (or path)
 */
#define AFUNAME_SIZE 256

/*! \def IPORT_SIZE
 * \brief maximum size for section reporting the AFU ports to be used
 * (current maximum is 8 given from the binary representation)
 */
#define IPORT_SIZE sizeof(uint8_t)

/*! \def IPORT_SIZE
 * \brief maximum size for section reporting the remote memory to be attached
 */
#define MEM_SIZE sizeof(uint64_t)

/*! \def EA_SIZE
 * \brief maximum size for sectiction containing the effective address
 */
#define EA_SIZE sizeof(uint64_t)

/*! \def ERROR_SIZE
 * \brief maximum size for the error section
 */
#define ERROR_SIZE sizeof(uint32_t)

/*! \def MSG_SIZE
 * \brief maximum message size
 */
#define MSG_SIZE                                                               \
    CIRCUIT_ID_SIZE + MSGTYPE_SIZE + AFUNAME_SIZE + IPORT_SIZE + MEM_SIZE +    \
        EA_SIZE + ERROR_SIZE

#define CIRCUIT_ID_OFFSET 0

#define MSGTYPE_OFFSET CIRCUIT_ID_OFFSET + CIRCUIT_ID_SIZE

#define AFUNAME_OFFSET MSGTYPE_OFFSET + MSGTYPE_SIZE

#define IPORT_OFFSET AFUNAME_OFFSET + AFUNAME_SIZE

#define MEM_SIZE_OFFSET IPORT_OFFSET + IPORT_SIZE

#define EA_OFFSET MEM_SIZE_OFFSET + MEM_SIZE

#define ERROR_OFFSET EA_OFFSET + EA_SIZE

#define MEMORY_ATTACH "memory_attach"

#define COMPUTE_ATTACH "compute_attach"

#define ERROR_ATTACH "error_attach"

#define MRESPONSE_ATTACH "mresponse_attach"

#define CRESPONSE_ATTACH "cresponse_attach"

#define MEMORY_DETACH "memory_detach"

#define COMPUTE_DETACH "compute_detach"

#define ERROR_DETACH "error_detach"

#define MRESPONSE_DETACH "mresponse_detach"

#define CRESPONSE_DETACH "cresponse_detach"

/*! \def    ERR_MSGTYPE
 * \brief message type is not one of the default message types
 * specified (MEMORY_ATTACH/MEMORY_DETACH/COMPUTE_ATTACH/COMPUTE_DETACH)
 */
#define ERR_MSGTYPE -3

/*! \def    ERR_MSGNULL
 * \brief provided a null reference to a message
 */
#define ERR_MSGNULL -2

/*! \def    ERR_NULL_PARAM
 * \brief provided a null reference input parameter
 */
#define ERR_NULL_PARAM -1

/*! \def    ERR_EMPTY_PARAM
 * \brief provided an empty input parameter
 */
#define ERR_EMPTY_PARAM -4

/*! \def    ERR_INT_PARAM
 * \brief provided a negative or zero input parameter
 */
#define ERR_INT_PARAM -5

/*! \def    ERR_MEMBLOCK_SIZE
 * \brief memory hotplugged needs to be a multiple of MEMBLOCK_SIZE
 * (/sys/devices/system/memory/block_size_bytes)
 */
#define ERR_MEMBLOCK_SIZE -5
#endif
