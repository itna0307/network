//-----------------------------------------------------------------------
// 
// ./vgs_com/packet.cpp
//
// Description : TCP/UDP packet generation following app protocol
// 
//-----------------------------------------------------------------------

#include "packet.hpp"

using namespace std;

// 
// Constructor and destructor
//

// Send packet constructor
cPckt::cPckt(DWORD code, vector<BYTE> data)
{
	m_pcktFd.dwCode = code;
	m_pcktFd.sData = data;
	
	int iLen = VGS_ETH_PACKET_LEN_CMD + VGS_ETH_PACKET_LEN_LEN + data.size();
	m_pcktFd.dwLen = (DWORD)iLen;
	//m_iMode = VGS_PCKT_SEND;

	// get length
	int iCodeLen = VGS_ETH_PACKET_LEN_CMD;
	int iLenLen = VGS_ETH_PACKET_LEN_LEN;

	// put field values
	vector<BYTE> ucCode = DWORDToBytes(m_pcktFd.dwCode);
	vector<BYTE> ucLen = DWORDToBytes(m_pcktFd.dwLen);
	vector<BYTE> sData = m_pcktFd.sData;

	// make packet
	int iPcktSize = ucCode.size() + ucLen.size() + sData.size();
	vector<BYTE> out;
	out.reserve(iPcktSize);
	out.insert(out.end(), ucCode.begin(), ucCode.end());
	out.insert(out.end(), ucLen.begin(), ucLen.end());
	out.insert(out.end(), sData.begin(), sData.end());

	m_sPckt = out;
}

// Receive packet constructor
cPckt::cPckt(vector<BYTE> in)
{
	m_sPckt = in;

	//m_iMode = VGS_PCKT_RECV;

	// get length
	int iCodeLen = VGS_ETH_PACKET_LEN_CMD;
	int iLenLen = VGS_ETH_PACKET_LEN_LEN;

	// parse
	vector<BYTE> ucCode(m_sPckt.begin(), m_sPckt.begin() + iCodeLen);
	vector<BYTE> ucLen(m_sPckt.begin() + iCodeLen, m_sPckt.begin() + iCodeLen + iLenLen);
	vector<BYTE> sData(m_sPckt.begin() + iCodeLen + iLenLen, m_sPckt.end());

	m_pcktFd.dwCode = BytesToDWORD(ucCode);
	m_pcktFd.dwLen = BytesToDWORD(ucLen);
	m_pcktFd.sData = sData;
}

void cPckt::dump_log()
{
	cout << m_sLog;
	m_sLog.assign("");
}

void cPckt::dump_log(ofstream ofs)
{
	ofs << m_sLog;
	m_sLog.assign("");
}