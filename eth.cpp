//-----------------------------------------------------------------------
// 
// ./vgs_com/eth.cpp
//
// Description : ethernet class
// 
//-----------------------------------------------------------------------
#include "eth.hpp"
#include "packet.hpp"

using namespace std;

cEth::cEth(int iMode, int iProtocol, char *sIP)
{
	m_iMode = iMode;
	m_iProtocol = iProtocol;
	m_iStatus = VGS_ETH_VALID;
	
	//---------------------------------------------------------------------------
	// Messages for debugging
	char tmp[1024] = { 0 };
	// debug message ends here
	//---------------------------------------------------------------------------
	
	// initialize library
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		m_sLog += "Error : WSA Init failed\n";
		m_iStatus = VGS_ETH_INVALID;
	}

	// 
	// Server mode
	//
	if (m_iMode == VGS_ETH_MODE_SRV)
	{
		// tcp server
		if (m_iProtocol == VGS_ETH_TL_TCP)
		{
			// create socket
			if ((m_hSvSock = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
			{
				m_sLog += "Error : Failed to create socket\n";
				m_iStatus = VGS_ETH_INVALID;
			}

			// prepare address
			memset(&m_svAddr, 0, sizeof(m_svAddr));
			m_svAddr.sin_family = AF_INET;
			m_svAddr.sin_addr.s_addr = htonl(INADDR_ANY);
			m_svAddr.sin_port = htons(VGS_ETH_PORT_TCP);
			int iAddrSize = sizeof(m_svAddr);

			// bind socket
			if (bind(m_hSvSock, (SOCKADDR*)&m_svAddr, sizeof(m_svAddr)) == SOCKET_ERROR)
			{
				m_sLog += "Error : Failed to bind socket\n";
				m_iStatus = VGS_ETH_INVALID;
			}

			// listen
			if (listen(m_hSvSock, 5) == SOCKET_ERROR)
			{
				m_sLog += "Error : Failed to listen socket\n";
				m_iStatus = VGS_ETH_INVALID;
			}
			else
			{
				cout << "Server is waiting for connections\n";
			}

			// accept connection
			if ((m_hClSock = accept(m_hSvSock, (SOCKADDR*)&m_clAddr, &iAddrSize)) == SOCKET_ERROR)
			{
				m_sLog += "Error : Failed to accept connection to socket\n";
				m_iStatus = VGS_ETH_INVALID;
			}
			else
			{
				char tmp[100];
				sprintf_s(tmp, "Accepted connection through %s (%d)\n",
					inet_ntoa(m_clAddr.sin_addr), ntohs(m_svAddr.sin_port));
				m_sLog += tmp;
			}
		}
		// udp server
		else if (m_iProtocol == VGS_ETH_TL_UDP)
		{
			// create socket
			if ((m_hSvSock = socket(PF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
			{
				m_sLog += "Error : Failed to create socket\n";
				m_iStatus = VGS_ETH_INVALID;
			}

			// set socket buffer size
			int iBufLen = VGS_DATA_LEN_FRAME;
			if (setsockopt(m_hSvSock, SOL_SOCKET, SO_SNDBUF, (char*)&iBufLen, sizeof(iBufLen)))
			{
				m_sLog += "Error : Failed to set send buffer size\n";
				m_iStatus = VGS_ETH_INVALID;
			}
			if (setsockopt(m_hSvSock, SOL_SOCKET, SO_RCVBUF, (char*)&iBufLen, sizeof(iBufLen)))
			{
				m_sLog += "Error : Failed to set send buffer size\n";
				m_iStatus = VGS_ETH_INVALID;
			}

			// prepare address
			memset(&m_svAddr, 0, sizeof(m_svAddr));
			m_svAddr.sin_family = AF_INET;
			m_svAddr.sin_addr.s_addr = htonl(INADDR_ANY);
			m_svAddr.sin_port = htons(VGS_ETH_PORT_UDP);
			int iAddrSize = sizeof(m_svAddr);

			// bind socket
			if (bind(m_hSvSock, (SOCKADDR*)&m_svAddr, sizeof(m_svAddr)) == SOCKET_ERROR)
			{
				m_sLog += "Error : Failed to bind socket\n";
				m_iStatus = VGS_ETH_INVALID;
			}
			else
			{
				sprintf_s(tmp, "Socket bound at port %d\n", ntohs(m_svAddr.sin_port));
				m_sLog += tmp;
			}
			int optlen = sizeof(int);
			int optval;
			getsockopt(m_hSvSock, SOL_SOCKET, SO_SNDBUF, (char*)&optval, &optlen);
			cout << optval << "/" << iBufLen << endl;
			getsockopt(m_hSvSock, SOL_SOCKET, SO_RCVBUF, (char*)&optval, &optlen);
			cout << optval << "/" << iBufLen << endl;
		} 
		// unknown protocol
		else
		{
			m_sLog += "Error : Unknown communication protocol\n";
			m_iStatus = VGS_ETH_INVALID;
		}
		cout << "Server is ready at port " << "(" << ntohs(m_svAddr.sin_port) << ")" << endl;
	}
	//
	// Client mode
	//
	else if (m_iMode == VGS_ETH_MODE_CLT)
	{
		// tcp client
		if (m_iProtocol == VGS_ETH_TL_TCP)
		{
			// create socket
			if ((m_hClSock = socket(PF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
			{
				m_sLog += "Error : Failed to create socket\n";
				m_iStatus = VGS_ETH_INVALID;
			}

			// prepare address
			memset(&m_svAddr, 0, sizeof(m_svAddr));
			m_svAddr.sin_family = AF_INET;
			m_svAddr.sin_addr.s_addr = inet_addr(sIP);
			m_svAddr.sin_port = htons(VGS_ETH_PORT_TCP);

			// connect to server
			if (connect(m_hClSock, (struct sockaddr*)&m_svAddr, sizeof(m_svAddr)) == SOCKET_ERROR)
			{
				m_sLog += "Error : Failed to connect to server\n";
				m_iStatus = VGS_ETH_INVALID;
			}
			else
			{
				char tmp[100];
				sprintf_s(tmp, "Connected through %s (%d)\n",
					inet_ntoa(m_svAddr.sin_addr), ntohs(m_svAddr.sin_port));
				m_sLog += tmp;
			}
		}
		// udp client
		else if (m_iProtocol == VGS_ETH_TL_UDP)
		{
			// create socket
			if ((m_hClSock = socket(PF_INET, SOCK_DGRAM, 0)) == SOCKET_ERROR)
			{
				m_sLog += "Error : Failed to create socket\n";
				m_iStatus = VGS_ETH_INVALID;
			}
			// set socket buffer size
			int iBufLen = VGS_DATA_LEN_FRAME;
			if (setsockopt(m_hClSock, SOL_SOCKET, SO_SNDBUF, (char*)&iBufLen, sizeof(iBufLen)))
			{
				m_sLog += "Error : Failed to set send buffer size\n";
				m_iStatus = VGS_ETH_INVALID;
			}
			if (setsockopt(m_hClSock, SOL_SOCKET, SO_RCVBUF, (char*)&iBufLen, sizeof(iBufLen)))
			{
				m_sLog += "Error : Failed to set send buffer size\n";
				m_iStatus = VGS_ETH_INVALID;
			}

			// prepare address
			memset(&m_svAddr, 0, sizeof(m_svAddr));
			m_svAddr.sin_family = AF_INET;
			m_svAddr.sin_addr.s_addr = inet_addr(sIP);
			m_svAddr.sin_port = htons(VGS_ETH_PORT_UDP);

			// create connected udp socket (address fixed)
			connect(m_hClSock, (struct sockaddr*)&m_svAddr, sizeof(m_svAddr));
			char tmp[100];
			sprintf_s(tmp, "Connected through %s (%d)\n",
				inet_ntoa(m_svAddr.sin_addr), ntohs(m_svAddr.sin_port));
			m_sLog += tmp;
		}
		// unknown protocol
		else
		{
			m_sLog += "Error : Unknown communication protocol\n";
			m_iStatus = VGS_ETH_INVALID;
		}

	}
	else
	{
		m_sLog += "Error : Unknown communication mode\n";
		m_iStatus = VGS_ETH_INVALID;
	}
}

cEth::~cEth()
{
	if (m_iMode == VGS_ETH_MODE_SRV)
	{
		closesocket(m_hSvSock);
	}
	closesocket(m_hClSock);
	WSACleanup();
	dump_log();
}

vector<BYTE> cEth::recv_data()
{
	//---------------------------------------------------------------------------
	// Messages for debugging
	m_sLog += "Receive data\n";
	char tmp[1024] = { 0 };
	// debug message ends here
	//---------------------------------------------------------------------------
	
	// default return value
	vector<BYTE> err = DWORDToBytes((DWORD)VGS_ETH_ERROR);

	// socket is initialized properly
	if (m_iStatus == VGS_ETH_VALID)
	{
		// initialize
		int iCodeLen = VGS_ETH_PACKET_LEN_CMD;
		int iLenLen = VGS_ETH_PACKET_LEN_LEN;
		int iBufLen = VGS_DATA_LEN_FRAME + iCodeLen + iLenLen;
		int iStrLen = 0;		// incoming dgram or packet size

		char *sRecvBuf = (char*)malloc(iBufLen);
		memset(sRecvBuf, 0, iBufLen);

		vector<BYTE> out;		// output buffer

		// tcp protocol
		if (m_iProtocol == VGS_ETH_TL_TCP)
		{
			int iRcnt = 0, iRlen = 0;
	
			while (iRlen < iBufLen)
			{
				iRcnt = recv(m_hClSock, sRecvBuf + iRlen, iBufLen, 0);
				iRlen += iRcnt;
			}
			out = ArrayToBytes(sRecvBuf, iBufLen);
			free(sRecvBuf);
			return out;
		}
		// udp protocol
		else if (m_iProtocol == VGS_ETH_TL_UDP)
		{
			int iClAddrSize = sizeof(m_clAddr);
			//
			// udp server using {recvfrom, sendto}
			// 
			if (m_iMode == VGS_ETH_MODE_SRV)
			{
				// get datagram
				iStrLen = recvfrom(m_hSvSock, sRecvBuf, iBufLen, 0, (SOCKADDR*)&m_clAddr, &iClAddrSize);
				if (iStrLen < 0)
				{
					cout<< "Error : Failed to receive datagram" << endl;
					free(sRecvBuf);
					return err;
				}
				else
				{ 
					// decode packet and get full length of data
					cPckt recvPckt(ArrayToBytes(sRecvBuf, iStrLen));
					out = recvPckt.m_sPckt;
					sprintf_s(tmp,"UDP Server : %d bytes receieved from %s (%d)\n"
						, iStrLen, inet_ntoa(m_clAddr.sin_addr), ntohs(m_clAddr.sin_port));
					m_sLog += tmp;
					free(sRecvBuf);
					return out;
				}
			}
			//
			// udp client using {recv, send} (connected udp)
			//
			else
			{
				// get datagram
				if ((iStrLen = recv(m_hClSock, sRecvBuf, iBufLen, 0)) == SOCKET_ERROR)
				{
					cout << "Error : Failed to receive datagram" << endl;
					free(sRecvBuf);
					return err;
				}
				else
				{
					// decode packet and get full length of data
					cPckt recvPckt(ArrayToBytes(sRecvBuf, iStrLen));
					out = recvPckt.m_sPckt;
					sprintf_s(tmp, "UDP Client : %d bytes receieved from %s (%d)\n"
						, iStrLen, inet_ntoa(m_svAddr.sin_addr), ntohs(m_svAddr.sin_port));
					m_sLog += tmp;
					free(sRecvBuf);
					return out;
				}
			}
		}
	}
	// socket initialization failed
	else if (m_iStatus == VGS_ETH_INVALID)
	{
		m_sLog += "Error : Socket is not initialized properly\n";
		dump_log();
		return err;
	}
	// socket in unknown state
	else
	{
		m_sLog += "Error : Socket is not initialized properly\n";
		dump_log();
		return err;
	}

}

int cEth::send_data(vector<BYTE> value)
{
	if (m_iStatus == VGS_ETH_VALID)
	{
		//---------------------------------------------------------------------------
		// Messages for debugging
		m_sLog += "Send data\n";
		char tmp[1024] = { 0 };
		// debug message ends here
		//---------------------------------------------------------------------------
		
		int iBufLen = VGS_ETH_MTU;
		int iCodeLen = VGS_ETH_PACKET_LEN_CMD;
		int iLenLen = VGS_ETH_PACKET_LEN_LEN;
		int iStrLen = value.size();		// incoming dgram or packet size

		// tcp protocol
		if (m_iProtocol == VGS_ETH_TL_TCP)
		{
			// send packet
			if (send(m_hClSock, (char*)value.data(), iStrLen, 0) == SOCKET_ERROR)
			{
				m_sLog += "Error : Failed to send packet\n";
				return VGS_ETH_ERROR;
			}
			else
			{
				sprintf_s(tmp, "TCP : %d bytes sent to %s (%d)\n"
					, iStrLen, inet_ntoa(m_clAddr.sin_addr), ntohs(m_clAddr.sin_port));
				m_sLog += tmp;
				return VGS_ETH_SUCCESS;
			}
		}
		// udp protocol
		else
		{
			int iClAddrSize = sizeof(m_clAddr);

			// make fragment
			int iNumDgrams = ceil((double)iStrLen / (double)iBufLen);
			int iEndDgramSize = (iStrLen == 0) ? 0 : iStrLen - (iNumDgrams - 1)*iBufLen;
			

			// 
			// udp server
			//
			if (m_iMode == VGS_ETH_MODE_SRV)
			{
				// send N dgrams
				for (int k = 0; k < iNumDgrams - 1; k++)
				{
					vector<BYTE> buf(value.begin() + (k*iBufLen), value.begin() + ((k + 1)*iBufLen));
					if (sendto(m_hSvSock, (char*)buf.data(), iBufLen, 0,
						(SOCKADDR*)&m_clAddr, sizeof(m_clAddr)) == SOCKET_ERROR)
					{
						m_sLog += "Failed to send datagram\n";
					}
					else
					{
						sprintf_s(tmp, "UDP Server : %d bytes sent to %s (%d)\n"
							, iBufLen, inet_ntoa(m_clAddr.sin_addr), ntohs(m_clAddr.sin_port));
						m_sLog += tmp;
					}
				}
				vector<BYTE> buf(value.end() - iEndDgramSize, value.end());
				if (sendto(m_hSvSock, (char*)buf.data(), iEndDgramSize, 0,
					(SOCKADDR*)&m_clAddr, sizeof(m_clAddr)) == SOCKET_ERROR)
				{
					m_sLog += "Failed to send datagram\n";
					return VGS_ETH_ERROR;
				}
				else
				{
					sprintf_s(tmp, "UDP Server : %d bytes sent to %s (%d)\n"
						, iEndDgramSize, inet_ntoa(m_clAddr.sin_addr), ntohs(m_clAddr.sin_port));
					m_sLog += tmp;
					return VGS_ETH_SUCCESS;
				}
			}
			// 
			// udp client
			//
			else
			{
				// send N dgrams
				for (int k = 0; k < iNumDgrams - 1; k++)
				{
					vector<BYTE> buf(value.begin() + (k*iBufLen), value.begin() + ((k + 1)*iBufLen));
					if (send(m_hClSock, (char*)buf.data(), iBufLen, 0) == SOCKET_ERROR)
					{
						m_sLog += "Failed to send datagram\n";
					}
					else
					{
						sprintf_s(tmp, "UDP Server : %d bytes sent to %s (%d)\n"
							, iBufLen, inet_ntoa(m_clAddr.sin_addr), ntohs(m_clAddr.sin_port));
						m_sLog += tmp;
					}
				}
				vector<BYTE> buf(value.end() - iEndDgramSize, value.end());
				if (send(m_hClSock, (char*)buf.data(), iEndDgramSize, 0) == SOCKET_ERROR)
				{
					m_sLog += "Failed to send datagram\n";
					return VGS_ETH_ERROR;
				}
				else
				{
					sprintf_s(tmp, "UDP Server : %d bytes sent to %s (%d)\n"
						, iEndDgramSize, inet_ntoa(m_clAddr.sin_addr), ntohs(m_clAddr.sin_port));
					m_sLog += tmp;
					return VGS_ETH_SUCCESS;
				}
			}
		}
	}
	else if (m_iStatus == VGS_ETH_INVALID)
	{
		m_sLog += "Error : Socket is not initialized properly\n";
		dump_log();
		return VGS_ETH_ERROR;
	}
	// socket in unknown state
	else
	{
		m_sLog += "Error : Socket is not initialized properly\n";
		dump_log();
		return VGS_ETH_ERROR;
	}
	
}

void cEth::dump_log()
{
	cout << m_sLog;
	m_sLog.assign("");
}

void cEth::dump_log(ofstream ofs)
{
	ofs << m_sLog;
	m_sLog.assign("");
}