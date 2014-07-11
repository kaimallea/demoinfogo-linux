//====== Copyright (c) 2012, Valve Corporation, All rights reserved. ========//
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation 
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================//

#ifndef DEMOFILEDUMP_H
#define DEMOFILEDUMP_H

#include "demofile.h"

#include "generated_proto/netmessages_public.pb.h"

class CDemoFileDump
{
public:
	CDemoFileDump() : m_nFrameNumber( 0 ) {}
	~CDemoFileDump() {}

	bool Open( const char *filename ); 
	void DoDump();
	void HandleDemoPacket();

public:
	void DumpDemoPacket( const char* buf, size_t length );
	void DumpUserMessage( const void *parseBuffer, int BufferSize );
	//void PrintDemoHeader( EDemoCommands DemoCommand, int tick, int size );
	void MsgPrintf( const ::google::protobuf::Message& msg, int size, const char *fmt, ... );

public:
	CDemoFile m_demofile;
	CSVCMsg_GameEventList m_GameEventList;

	int m_nFrameNumber;
};

#endif // DEMOFILEDUMP_H

