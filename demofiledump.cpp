//====== Copyright (c) 2014, Valve Corporation, All rights reserved. ========//
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

#include <stdarg.h>
#include "demofile.h"
#include "demofiledump.h"

#include "google/protobuf/descriptor.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/descriptor.pb.h"

#include "generated_proto/cstrike15_usermessages_public.pb.h"
#include "generated_proto/netmessages_public.pb.h"


__attribute__ (( noreturn )) void fatal_errorf( const char* fmt, ... )
{
    va_list  vlist;
    char buf[ 1024 ];

    va_start( vlist, fmt);
    vsnprintf( buf, sizeof( buf ), fmt, vlist );
	buf[ sizeof( buf ) - 1 ] = 0;
    va_end( vlist );

    fprintf( stderr, "\nERROR: %s\n", buf );
    exit( -1 );
}

bool CDemoFileDump::Open( const char *filename )
{
	if ( !m_demofile.Open( filename ) )
	{
		fprintf( stderr, "Couldn't open '%s'\n", filename );
		return false;
	}

	return true;
}

void CDemoFileDump::MsgPrintf( const ::google::protobuf::Message& msg, int size, const char *fmt, ... )
{
	va_list vlist;
	const std::string& TypeName = msg.GetTypeName();

	// Print the message type and size
	printf( "---- %s (%d bytes) -----------------\n", TypeName.c_str(), size );

	va_start( vlist, fmt);
	vprintf( fmt, vlist );
	va_end( vlist );
}

template < class T, int msgType >
void PrintUserMessage( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	T msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		Demo.MsgPrintf( msg, BufferSize, "%s", msg.DebugString().c_str() );
	}
}

void CDemoFileDump::DumpUserMessage( const void *parseBuffer, int BufferSize )
{
	CSVCMsg_UserMessage userMessage;

	if( userMessage.ParseFromArray( parseBuffer, BufferSize ) )
	{
		int Cmd = userMessage.msg_type();
		int SizeUM = userMessage.msg_data().size();
		const void *parseBufferUM = &userMessage.msg_data()[ 0 ];
		switch( Cmd )
		{
#define HANDLE_UserMsg( _x )			case CS_UM_ ## _x: PrintUserMessage< CCSUsrMsg_ ## _x, CS_UM_ ## _x >( *this, parseBufferUM, SizeUM ); break

		default:
			// unknown user message
			break;
			HANDLE_UserMsg( VGUIMenu );
			HANDLE_UserMsg( Geiger );
			HANDLE_UserMsg( Train );
			HANDLE_UserMsg( HudText );
			HANDLE_UserMsg( SayText );
			HANDLE_UserMsg( SayText2 );
			HANDLE_UserMsg( TextMsg );
			HANDLE_UserMsg( HudMsg );
			HANDLE_UserMsg( ResetHud );
			HANDLE_UserMsg( GameTitle );
			HANDLE_UserMsg( Shake );
			HANDLE_UserMsg( Fade );
			HANDLE_UserMsg( Rumble );
			HANDLE_UserMsg( CloseCaption );
			HANDLE_UserMsg( CloseCaptionDirect );
			HANDLE_UserMsg( SendAudio );
			HANDLE_UserMsg( RawAudio );
			HANDLE_UserMsg( VoiceMask );
			HANDLE_UserMsg( RequestState );
			HANDLE_UserMsg( Damage );
			HANDLE_UserMsg( RadioText );
			HANDLE_UserMsg( HintText );
			HANDLE_UserMsg( KeyHintText );
			HANDLE_UserMsg( ProcessSpottedEntityUpdate );
			HANDLE_UserMsg( ReloadEffect );
			HANDLE_UserMsg( AdjustMoney );
			HANDLE_UserMsg( StopSpectatorMode );
			HANDLE_UserMsg( KillCam );
			HANDLE_UserMsg( DesiredTimescale );
			HANDLE_UserMsg( CurrentTimescale );
			HANDLE_UserMsg( AchievementEvent );
			HANDLE_UserMsg( MatchEndConditions );
			HANDLE_UserMsg( DisconnectToLobby );
			HANDLE_UserMsg( DisplayInventory );
			HANDLE_UserMsg( WarmupHasEnded );
			HANDLE_UserMsg( ClientInfo );
			HANDLE_UserMsg( CallVoteFailed );
			HANDLE_UserMsg( VoteStart );
			HANDLE_UserMsg( VotePass );
			HANDLE_UserMsg( VoteFailed );
			HANDLE_UserMsg( VoteSetup );
			HANDLE_UserMsg( SendLastKillerDamageToClient );
			HANDLE_UserMsg( ItemPickup );
			HANDLE_UserMsg( ShowMenu );
			HANDLE_UserMsg( BarTime );
			HANDLE_UserMsg( AmmoDenied );
			HANDLE_UserMsg( MarkAchievement );
			HANDLE_UserMsg( ItemDrop );
			HANDLE_UserMsg( GlowPropTurnOff );

#undef HANDLE_UserMsg
		}

	}
}

template < class T, int msgType >
void PrintNetMessage( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	T msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		if( msgType == svc_GameEventList )
		{
			Demo.m_GameEventList.CopyFrom( msg );
		}

		Demo.MsgPrintf( msg, BufferSize, "%s", msg.DebugString().c_str() );
	}
}

template <>
void PrintNetMessage< CSVCMsg_UserMessage, svc_UserMessage >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	Demo.DumpUserMessage( parseBuffer, BufferSize );
}

template <>
void PrintNetMessage< CSVCMsg_GameEvent, svc_GameEvent >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CSVCMsg_GameEvent msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		int iDescriptor;

		for( iDescriptor = 0; iDescriptor < Demo.m_GameEventList.descriptors().size(); iDescriptor++ )
		{
			const CSVCMsg_GameEventList::descriptor_t& Descriptor = Demo.m_GameEventList.descriptors( iDescriptor );

			if( Descriptor.eventid() == msg.eventid() )
				break;
		}

		if( iDescriptor == Demo.m_GameEventList.descriptors().size() )
		{
			printf( "%s", msg.DebugString().c_str() );
		}
		else
		{
			int numKeys = msg.keys().size();
			const CSVCMsg_GameEventList::descriptor_t& Descriptor = Demo.m_GameEventList.descriptors( iDescriptor );

			printf( "%s eventid:%d %s\n", Descriptor.name().c_str(), msg.eventid(),
				msg.has_event_name() ? msg.event_name().c_str() : "" );

			for( int i = 0; i < numKeys; i++ )
			{
				const CSVCMsg_GameEventList::key_t& Key = Descriptor.keys( i );
				const CSVCMsg_GameEvent::key_t& KeyValue = msg.keys( i );

				printf(" %s: ", Key.name().c_str() );

				if( KeyValue.has_val_string() )
					printf( "%s ", KeyValue.val_string().c_str() );
				if( KeyValue.has_val_float() )
					printf( "%f ", KeyValue.val_float() );
				if( KeyValue.has_val_long() )
					printf( "%d ", KeyValue.val_long() );
				if( KeyValue.has_val_short() )
					printf( "%d ", KeyValue.val_short() );
				if( KeyValue.has_val_byte() )
					printf( "%d ", KeyValue.val_byte() );
				if( KeyValue.has_val_bool() )
					printf( "%d ", KeyValue.val_bool() );
				if( KeyValue.has_val_uint64() )
					printf( "%lld ", KeyValue.val_uint64() );

				printf( "\n" );
			}
		}
	}
}

static std::string GetNetMsgName( int Cmd )
{
	if( NET_Messages_IsValid( Cmd ) )
	{
		return NET_Messages_Name( ( NET_Messages )Cmd );
	}
	else if( SVC_Messages_IsValid( Cmd ) )
	{
		return SVC_Messages_Name( ( SVC_Messages )Cmd );
	}

	assert( 0 );
	return "NETMSG_???";
}

void CDemoFileDump::DumpDemoPacket( const char* buf, size_t length )
{
	size_t index = 0;

	while( index < length )
	{
		int Cmd = ReadVarInt32( buf, length, index );
		uint32 Size = ReadVarInt32( buf, length, index );

		if( index + Size > length )
		{
			const std::string& strName = GetNetMsgName( Cmd );

			fatal_errorf( "DumpDemoPacket()::failed parsing packet. Cmd:%d '%s' \n", Cmd, strName.c_str() );
		}

		switch( Cmd )
		{
#define HANDLE_NetMsg( _x )		case net_ ## _x: PrintNetMessage< CNETMsg_ ## _x, net_ ## _x >( *this, &buf[ index ], Size ); break
#define HANDLE_SvcMsg( _x )		case svc_ ## _x: PrintNetMessage< CSVCMsg_ ## _x, svc_ ## _x >( *this, &buf[ index ], Size ); break

		default:
			// unknown net message
			break;

		HANDLE_NetMsg( NOP );            	// 0
		HANDLE_NetMsg( Disconnect );        // 1
		HANDLE_NetMsg( File );              // 2
		HANDLE_NetMsg( Tick );              // 4
		HANDLE_NetMsg( StringCmd );         // 5
		HANDLE_NetMsg( SetConVar );         // 6
		HANDLE_NetMsg( SignonState );       // 7
		HANDLE_SvcMsg( ServerInfo );        // 8
		HANDLE_SvcMsg( SendTable );         // 9
		HANDLE_SvcMsg( ClassInfo );         // 10
		HANDLE_SvcMsg( SetPause );          // 11
		HANDLE_SvcMsg( CreateStringTable ); // 12
		HANDLE_SvcMsg( UpdateStringTable ); // 13
		HANDLE_SvcMsg( VoiceInit );         // 14
		HANDLE_SvcMsg( VoiceData );         // 15
		HANDLE_SvcMsg( Print );             // 16
		HANDLE_SvcMsg( Sounds );            // 17
		HANDLE_SvcMsg( SetView );           // 18
		HANDLE_SvcMsg( FixAngle );          // 19
		HANDLE_SvcMsg( CrosshairAngle );    // 20
		HANDLE_SvcMsg( BSPDecal );          // 21
		HANDLE_SvcMsg( UserMessage );       // 23
		HANDLE_SvcMsg( GameEvent );         // 25
		HANDLE_SvcMsg( PacketEntities );    // 26
		HANDLE_SvcMsg( TempEntities );      // 27
		HANDLE_SvcMsg( Prefetch );          // 28
		HANDLE_SvcMsg( Menu );              // 29
		HANDLE_SvcMsg( GameEventList );     // 30
		HANDLE_SvcMsg( GetCvarValue );      // 31

#undef HANDLE_SvcMsg
#undef HANDLE_NetMsg
		}

		index += Size;
	}
}

#define NET_MAX_PAYLOAD ( 262144 - 4 )	// largest message we can send in bytes
void CDemoFileDump::HandleDemoPacket()
{
	democmdinfo_t	info;
	int				dummy;
	char			buf[ NET_MAX_PAYLOAD ];

	m_demofile.ReadCmdInfo( info );
	m_demofile.ReadSequenceInfo( dummy, dummy ); 

	size_t length = m_demofile.ReadRawData( buf,  NET_MAX_PAYLOAD );

	if ( length > 0 )
	{
		DumpDemoPacket( buf, length );
	}
}

void CDemoFileDump::DoDump()
{
	bool demofinished = false;
	while ( !demofinished )
	{
		int				tick = 0;
		unsigned char	cmd;
		unsigned char	playerSlot;
		m_demofile.ReadCmdHeader( cmd, tick, playerSlot );

		// COMMAND HANDLERS
		switch ( cmd )
		{
		case dem_synctick:
			break;
		case dem_stop:
			{
				demofinished = true;
			}
			break;
		case dem_consolecmd:
			{
				m_demofile.ReadRawData( NULL, 0 );
			}
			break;
		case dem_datatables:
			{
				m_demofile.ReadRawData( NULL, 0 );
			}
			break;
		case dem_stringtables:
			{
				m_demofile.ReadRawData( NULL, 0 );
			}
			break;
		case dem_usercmd:
			{
				int	dummy;
				m_demofile.ReadUserCmd( NULL, dummy );
			}
			break;
			
		case dem_signon:
		case dem_packet:
			{
				HandleDemoPacket();
			}
			break;

		default:
			{
			}
			break;
		}
	}
}

