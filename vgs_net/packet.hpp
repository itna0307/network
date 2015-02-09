//-----------------------------------------------------------------------
// 
// ./vgs_com/packet.hpp
// 
//-----------------------------------------------------------------------

#ifndef _VGS_PACKET_H_
#define _VGS_PACKET_H_

// local includes
#include "com_params.h"

// project general includes
#include <ctrl_params.h>
#include <util.h>

// global includes
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

// for dll build
#define DllExport   __declspec( dllexport ) 

//
// Base packet class
//
struct DllExport PACKET_FIELD
{
	DWORD dwCode = 0;
	DWORD dwLen = 0;
	vector<BYTE> sData;
};

class DllExport cPckt
{
protected:
	// error log
	string m_sLog;

public:
	// constructor/destructor
	cPckt(DWORD code, vector<BYTE> data);			// send constructor
	cPckt(vector<BYTE> in);							// recv constructor

	// packet field data
	PACKET_FIELD m_pcktFd; 

	// formatted packet
	vector<BYTE> m_sPckt;

	// packet direction : send/recv
	//int m_iMode;

	// dump log
	void dump_log();					// dump log to console
	void dump_log(ofstream ofs);		// dump log to target file
};

// 
// Control data packet
// 
/*
class DllExport cCtrlPckt : public cPckt
{
public:
	// constructor
	//cCtrlPckt(DWORD code, vector<BYTE> data) : cPckt(code, data){}
	//cCtrlPckt(vector<BYTE> in) : cPckt(in){}
	
	// decode packet
	//int pckt_dec();

	// format packet
	//int pckt_fmt();
};*/

// 
// Image data packet : TBD
//
/*
class DllExport cImgPckt : public cPckt
{
	
};
*/

#endif