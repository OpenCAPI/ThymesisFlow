/// @file errcode.h
#if !defined(THYMESISFLOW_ERR_H)
#define THYMESISFLOW_ERR_H
#define THYMESISFLOW_ERR_H

/*! \def ATTACH_OK
 * \brief Successfully attached memory
 */
#define ATTACH_OK 100

/*! \def ERR_MLOCK
 * \brief Error locking memory
 */
#define ERR_MLOCK 101

/*! \def ERR_AFU_OPEN
 * \brief Error opening AFU
 */
#define ERR_AFU_OPEN 102

/*! \def ERR_AFU_ATTACH
 * \brief Error attaching AFU
 */
#define ERR_AFU_ATTACH 103

/*! \def ERR_MMIO_MAP_GLOBAL
 * \brief Error mapping global memory area
 */
#define ERR_MMIO_MAP_GLOBAL 104

/*! \def ERR_MMIO_MAP_LOCAL
 * \brief Error mapping per process MMIO Area
 */
#define ERR_MMIO_MAP_LOCAL 105

/*! \def ERR_WRITE_CTRL_REGISTER
 * \brief Error writing control register
 */
#define ERR_WRITE_CTRL_REGISTER 106

/*! \def ERR_ENABLING_NETWORK
 * \brief Error enabling network
 */
#define ERR_ENABLING_NETWORK 107

/*! \def ERR_PORT_UNSUPPORTED
 * \brief Only single port setup supported
 */
#define ERR_PORT_UNSUPPORTED 108

/*! \def ERR_AURORA_CHAN
 * \brief Error bringing up the AURORA channel
 */
#define ERR_AURORA_CHAN 109

/*! \def ERR_ATTACH_SEG
 * \brief Error while attaching the memory segment
 */
#define ERR_ATTACH_SEG 110

/*! \def ERR_PROBE
 * \brief Error while probing the memory
 */
#define ERR_PROBE 111

/*! \def    DETACH_OK
 * \brief Successfully detached memory
 */
#define DETACH_OK 200

/*! \def    ERR_REGISTER_DETACH
 * \brief error while updating the internal connection status
 */
#define ERR_REGISTER_DETACH 201

/*! \def    ERR_MISSING_CID
 * \brief Missing connection related to circuit id
 */
#define ERR_MISSING_CID 201

#endif
