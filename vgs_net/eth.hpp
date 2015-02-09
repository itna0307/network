//-----------------------------------------------------------------------
// 
// ./vgs_com/eth.hpp
// 
//-----------------------------------------------------------------------

#ifndef _VGS_ETH_H_
#define _VGS_ETH_H_

// local includes
#include "com_params.h"

// project general includes
#include <util.h>
#include <ctrl_params.h>

// global includes
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <WinSock2.h>

#pragma comment (lib, "Ws2_32.lib")

#define DllExport   __declspec( dllexport ) 

class DllExport cEth
{
	// socket variables
	WSADATA m_wsaData;
	SOCKADDR_IN m_svAddr, m_clAddr;
	SOCKET m_hSvSock, m_hClSock;

	int m_iStatus;						// module is initialized properly or not
	int m_iProtocol;					// udp or tcp
	int m_iMode;						// server or client

	// log
	string m_sLog;
public:
	// constructor and destructor
	cEth(int iMode, int iProtocol, char *sIP);
	~cEth();

	// read/write
	vector<BYTE> recv_data();		// receive data through socket
	int send_data(vector<BYTE> value);		// send data through socket

	// dump log
	void dump_log();					// dump log to console
	void dump_log(ofstream ofs);		// dump log to target file
};

#endif