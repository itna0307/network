//-----------------------------------------------------------------------
// 
// ./vgs_com/com_params.h
//
// Description : communication module parameter definitions
// 
//-----------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Preprocessor
// -----------------------------------------------------------------------------

#ifndef _VGS_COM_PARAMS_H_
#define _VGS_COM_PARAMS_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS

// -----------------------------------------------------------------------------
// Ethernet Module General Definition
// -----------------------------------------------------------------------------

// 
// Validity
// 

#define VGS_ETH_INVALID		6
#define VGS_ETH_VALID		7


// 
// Operation mode
//

// server
#define VGS_ETH_MODE_SRV	0

// client
#define VGS_ETH_MODE_CLT	1


//
// Transport layer definition
//

// TCP
#define VGS_ETH_TL_TCP		0

// UDP
#define VGS_ETH_TL_UDP		1


//
// IP address and port
//

// Uncomment if you want to use default address
// #define VGS_ETH_FIXED_IP
#ifdef VGS_ETH_FIXED_IP
#define VGS_ETH_IP_DEFAULT	192.168.1.125		
#endif

#define VGS_ETH_PORT_TCP	1000
#define VGS_ETH_PORT_UDP	3000


// 
// Image-based window size (in bytes)
//

// overheads (reserved for headers, checksum, end of message... etc)
#define VGS_ETH_PACKET_LEN_CMD		4
#define VGS_ETH_PACKET_LEN_LEN		4
#define VGS_ETH_PACKET_LEN_CRC		16 			// reserved

// control message data
#define VGS_ETH_PACKET_LEN_QUERY	32
#define VGS_ETH_PACKET_LEN_RETVAL	32


//
// Ethernet buffer definition
//

// maximum packet size
#define VGS_ETH_MTU					1500		// standard
#define VGS_ETH_MTU_JGRAM			65535		// udp jumbogram

// -----------------------------------------------------------------------------
// Packet Header Definition
// -----------------------------------------------------------------------------

//
// Command definition (Host to Sensor)
//

// sensor reset
#define VGS_CMD_SRST		0 	

// sensor initialization
#define VGS_CMD_SINIT		2 	

// sensor mode
#define VGS_CMD_SMODE		4 	

// sensor status
#define VGS_CMD_SSTAT		8 	

// sensor register write
#define VGS_CMD_CAWR		16 	
#define VGS_CMD_IRWR		18

// sensor register read
#define VGS_CMD_CARR		32
#define VGS_CMD_IRRR		34

// FWU
#define VGS_CMD_SFWU_RDY	64
#define VGS_CMD_SFWU_PROG	66

// query capture
#define VGS_CMD_CAPTURE		128

// Image data
#define VGS_CMD_IMG			129

//
// Acknowledgement (Sensor to Host)
//
#define VGS_ACK_MSG			1

// -----------------------------------------------------------------------------
// Packet Header Definition
// -----------------------------------------------------------------------------

// packet mode
#define VGS_PCKT_UNKNOWN	-1
#define VGS_PCKT_RECV		0
#define VGS_PCKT_SEND		1


// -----------------------------------------------------------------------------
// Exceptions
// -----------------------------------------------------------------------------

// ethernet functions failure
#define VGS_ETH_SUCCESS		1024
#define VGS_ETH_ERROR		-2

// packet status unknown
#define VGS_PCKT_ERROR		-3
#define VGS_PCKT_NO_DATA	-4

// invalid/unknown oper

#endif