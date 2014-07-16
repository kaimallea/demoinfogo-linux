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

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdarg.h>
#include <rapidjson/document.h>
#include <rapidjson/allocators.h>
#include <rapidjson/filestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include "demofile.h"
#include "demofiledump.h"

#include "google/protobuf/descriptor.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/descriptor.pb.h"

#include "generated_proto/cstrike15_usermessages_public.pb.h"
#include "generated_proto/netmessages_public.pb.h"

rapidjson::FileStream g_JSONFileStream(stdout);
rapidjson::PrettyWriter<rapidjson::FileStream> g_JSONWriter(g_JSONFileStream);

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

	g_JSONWriter.StartObject();
	// Print the message type and size
	//printf( "---- %s (%d bytes) -----------------\n", TypeName.c_str(), size );

	g_JSONWriter.String("name");
	g_JSONWriter.String(TypeName.c_str());
	g_JSONWriter.String("data");

	va_start( vlist, fmt);
	vprintf( fmt, vlist );
	va_end( vlist );

    g_JSONWriter.EndObject();
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

/*
 * Print Net Messages
 *
 * The net_ message enums from netmessages_public.proto:
 *
 * net_NOP
 * net_Disconnect
 * net_File
 * net_Tick
 * net_StringCmd
 * net_SetConvar
 * net_SignonState
 */

/* net_NOP */
template <>
void PrintNetMessage< CNETMsg_NOP, net_NOP>( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
    CNETMsg_NOP msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();

        g_JSONWriter.String("name");
        g_JSONWriter.String("CNETMsg_NOP");

        g_JSONWriter.EndObject();
    }
}

/* net_Disconnect */
template <>
void PrintNetMessage< CNETMsg_Disconnect, net_Disconnect>( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
    CNETMsg_Disconnect msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();

        g_JSONWriter.String("name");
        g_JSONWriter.String("CNETMsg_Disconnect");
        g_JSONWriter.String("data");

        g_JSONWriter.StartObject();

        g_JSONWriter.String("text");
        g_JSONWriter.String(msg.text().c_str());

        g_JSONWriter.EndObject();

        g_JSONWriter.EndObject();
    }
}

/* net_File */
template <>
void PrintNetMessage< CNETMsg_File, net_File>( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
    CNETMsg_File msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();

        g_JSONWriter.String("name");
        g_JSONWriter.String("CNETMsg_File");
        g_JSONWriter.String("data");

        g_JSONWriter.StartObject();

        g_JSONWriter.String("transfer_id");
        g_JSONWriter.Int(msg.transfer_id());
        g_JSONWriter.String("file_name");
        g_JSONWriter.String(msg.file_name().c_str());
        g_JSONWriter.String("is_replay_demo_file");
        g_JSONWriter.Bool(msg.is_replay_demo_file());
        g_JSONWriter.String("deny");
        g_JSONWriter.Bool(msg.deny());

        g_JSONWriter.EndObject();

        g_JSONWriter.EndObject();
    }
}

/* net_Tick */
template <>
void PrintNetMessage< CNETMsg_Tick, net_Tick >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CNETMsg_Tick msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();
		g_JSONWriter.String("name");
		g_JSONWriter.String("CNETMsg_Tick");
		g_JSONWriter.String("data");

		g_JSONWriter.StartObject();

		g_JSONWriter.String("tick");
		g_JSONWriter.Uint(msg.tick());

		g_JSONWriter.String("host_computationtime");
		g_JSONWriter.Uint(msg.host_computationtime());

		g_JSONWriter.String("host_computationtime_std_deviation");
		g_JSONWriter.Uint(msg.host_computationtime_std_deviation());

		g_JSONWriter.String("host_framestarttime_std_deviation");
		g_JSONWriter.Uint(msg.host_framestarttime_std_deviation());

		g_JSONWriter.EndObject();

		g_JSONWriter.EndObject();
	}
}

/* net_StringCmd */
template <>
void PrintNetMessage< CNETMsg_StringCmd, net_StringCmd >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
    CNETMsg_StringCmd msg;

    if( msg.ParseFromArray( parseBuffer, BufferSize ) )
    {
        g_JSONWriter.StartObject();
        g_JSONWriter.String("name");
        g_JSONWriter.String("CNETMsg_StringCmd");
        g_JSONWriter.String("data");

        g_JSONWriter.StartObject();

        g_JSONWriter.String("command");
        g_JSONWriter.String(msg.command().c_str());

        g_JSONWriter.EndObject();

        g_JSONWriter.EndObject();
    }
}

/* net_SetConvar */
template <>
void PrintNetMessage< CNETMsg_SetConVar, net_SetConVar >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CNETMsg_SetConVar msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();
		g_JSONWriter.String("name");
		g_JSONWriter.String("CNETMsg_SetConVar");
		g_JSONWriter.String("data");
		g_JSONWriter.StartObject();
		g_JSONWriter.String("convars");
		g_JSONWriter.StartArray();

		int iConVar;
		CMsg_CVars CVars = msg.convars();
		for( iConVar = 0; iConVar < CVars.cvars().size(); iConVar++ )
		{
			const CMsg_CVars::CVar& CVar = CVars.cvars( iConVar );
			g_JSONWriter.StartObject();
			g_JSONWriter.String("name");
			g_JSONWriter.String(CVar.name().c_str());
			g_JSONWriter.String("value");
			g_JSONWriter.String(CVar.value().c_str());
			g_JSONWriter.EndObject();
		}

		g_JSONWriter.EndArray();
		g_JSONWriter.EndObject();
		g_JSONWriter.EndObject();
	}
}

/* net_SignonState */
template <>
void PrintNetMessage< CNETMsg_SignonState, net_SignonState >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CNETMsg_SignonState msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();
		g_JSONWriter.String("name");
		g_JSONWriter.String("CNETMsg_SignonState");
		g_JSONWriter.String("data");

		g_JSONWriter.StartObject();

		g_JSONWriter.String("signon_state");
		g_JSONWriter.Uint(msg.signon_state());

		g_JSONWriter.String("spawn_count");
		g_JSONWriter.Uint(msg.spawn_count());

		g_JSONWriter.String("num_server_players");
		g_JSONWriter.Uint(msg.num_server_players());

		g_JSONWriter.String("players_networkids");
		g_JSONWriter.StartArray();

		int iNetworkId;
		for( iNetworkId = 0; iNetworkId < msg.players_networkids().size(); iNetworkId++)
		{
			g_JSONWriter.String(msg.players_networkids( iNetworkId ).c_str());
		}

		g_JSONWriter.EndArray();

		g_JSONWriter.String("map_name");
		g_JSONWriter.String(msg.map_name().c_str());

		g_JSONWriter.EndObject();

		g_JSONWriter.EndObject();
	}
}


/*
 * Print Server Messages
 *
 * The svc_ message enums from netmessages_public.proto:
 *
 * svc_ServerInfo
 * svc_SendTable
 * svc_ClassInfo
 * svc_SetPause
 * svc_CreateStringTable
 * svc_UpdateStringTable
 * svc_VoiceInit
 * svc_VoiceData
 * svc_Print
 * svc_Sounds
 * svc_SetView
 * svc_FixAngle
 * svc_Crosshairangle
 * svc_BSPDecal
 * svc_UserMessage
 * svc_GameEvent
 * svc_PacketEntities
 * svc_TempEntities
 * svc_Prefetch
 * svc_Menu
 * svc_GameEventList
 * svc_GetCvarValue
 */

 /* svc_ServerInfo */
template <>
void PrintNetMessage< CSVCMsg_ServerInfo, svc_ServerInfo >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CSVCMsg_ServerInfo msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();
		g_JSONWriter.String("name");
		g_JSONWriter.String("CSVCMsg_ServerInfo");
		g_JSONWriter.String("data");

		g_JSONWriter.StartObject();

		g_JSONWriter.String("protocol");
		g_JSONWriter.Int(msg.protocol());

		g_JSONWriter.String("server_count");
		g_JSONWriter.Int(msg.server_count());

		g_JSONWriter.String("is_dedicated");
		g_JSONWriter.Bool(msg.is_dedicated());

		g_JSONWriter.String("is_official_valuve_server");
		g_JSONWriter.Bool(msg.is_official_valve_server());

		g_JSONWriter.String("is_hltv");
		g_JSONWriter.Bool(msg.is_hltv());

		g_JSONWriter.String("is_replay");
		g_JSONWriter.Bool(msg.is_replay());

		g_JSONWriter.String("is_redirecting_to_proxy_relay");
		g_JSONWriter.Bool(msg.is_redirecting_to_proxy_relay());

		g_JSONWriter.String("c_os");
		g_JSONWriter.Int(msg.c_os());

		g_JSONWriter.String("map_crc");
		g_JSONWriter.Double(double(msg.map_crc()));

		g_JSONWriter.String("client_crc");
		g_JSONWriter.Double(double(msg.client_crc()));

		g_JSONWriter.String("string_table_crc");
		g_JSONWriter.Double(double(msg.string_table_crc()));

		g_JSONWriter.String("max_clients");
		g_JSONWriter.Int(msg.max_clients());

		g_JSONWriter.String("max_classes");
		g_JSONWriter.Int(msg.max_classes());

		g_JSONWriter.String("player_slot");
		g_JSONWriter.Int(msg.player_slot());

		g_JSONWriter.String("tick_interval");
		g_JSONWriter.Double(double(msg.tick_interval()));

		g_JSONWriter.String("game_dir");
		g_JSONWriter.String(msg.game_dir().c_str());

		g_JSONWriter.String("map_name");
		g_JSONWriter.String(msg.map_name().c_str());

		g_JSONWriter.String("map_group_name");
		g_JSONWriter.String(msg.map_group_name().c_str());

		g_JSONWriter.String("sky_name");
		g_JSONWriter.String(msg.sky_name().c_str());

		g_JSONWriter.String("host_name");
		g_JSONWriter.String(msg.host_name().c_str());

		g_JSONWriter.String("ugc_map_id");
		g_JSONWriter.Uint64(msg.ugc_map_id());

		g_JSONWriter.EndObject();

		g_JSONWriter.EndObject(); // server_info
	}
}

 /* svc_SendTable */
template <>
void PrintNetMessage< CSVCMsg_SendTable, svc_SendTable >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CSVCMsg_SendTable msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();
		g_JSONWriter.String("name");
		g_JSONWriter.String("CSVCMsg_SendTable");
		g_JSONWriter.String("data");

		g_JSONWriter.StartObject();
        

        g_JSONWriter.String("is_end");
        g_JSONWriter.Bool(msg.is_end());
        g_JSONWriter.String("net_table_name");
        g_JSONWriter.String(msg.net_table_name().c_str());
        g_JSONWriter.String("needs_decoder");
        g_JSONWriter.StartArray();

        int iProps;
        for( iProps = 0; iProps < msg.props().size(); iProps++ )
        {
            g_JSONWriter.StartObject();

            g_JSONWriter.String("type");
            g_JSONWriter.Int(msg.props( iProps ).type());
            g_JSONWriter.String("var_name");
            g_JSONWriter.String(msg.props( iProps ).var_name().c_str());
            g_JSONWriter.String("flags");
            g_JSONWriter.Int(msg.props( iProps ).flags());
            g_JSONWriter.String("priority");
            g_JSONWriter.Int(msg.props( iProps ).priority());
            g_JSONWriter.String("dt_name");
            g_JSONWriter.String(msg.props( iProps ).dt_name().c_str());
            g_JSONWriter.String("num_elements");
            g_JSONWriter.Int(msg.props( iProps ).num_elements());
            g_JSONWriter.String("low_value");
            g_JSONWriter.Double(msg.props( iProps ).low_value());
            g_JSONWriter.String("high_value");
            g_JSONWriter.Double(msg.props( iProps ).high_value());
            g_JSONWriter.String("num_bits");
            g_JSONWriter.Int(msg.props( iProps ).num_bits());

            g_JSONWriter.EndObject();
            
        }

        g_JSONWriter.EndArray();

		g_JSONWriter.EndObject();
		g_JSONWriter.EndObject();
	}
}

 /* svc_ClassInfo */
template <>
void PrintNetMessage< CSVCMsg_ClassInfo, svc_ClassInfo >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CSVCMsg_ClassInfo msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();
		g_JSONWriter.String("name");
		g_JSONWriter.String("CSVCMsg_ClassInfo");
		g_JSONWriter.String("data");
		g_JSONWriter.StartObject();
		g_JSONWriter.String("classes");
		g_JSONWriter.StartArray();

		int iClasses;
		for( iClasses = 0; iClasses < msg.classes().size(); iClasses++ )
		{
			g_JSONWriter.StartObject();
			g_JSONWriter.String("class_id");
			g_JSONWriter.Int(msg.classes( iClasses ).class_id());
			g_JSONWriter.String("data_table_name");
			g_JSONWriter.String(msg.classes( iClasses ).data_table_name().c_str());
			g_JSONWriter.String("class_name");
			g_JSONWriter.String(msg.classes( iClasses ).class_name().c_str());
			g_JSONWriter.EndObject();
		}

		g_JSONWriter.EndArray();

		g_JSONWriter.String("create_on_client");
		g_JSONWriter.Bool(msg.create_on_client());

		g_JSONWriter.EndObject();
		g_JSONWriter.EndObject();
	}
}

 /* svc_SetPause */
template <>
void PrintNetMessage< CSVCMsg_SetPause, svc_SetPause >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CSVCMsg_SetPause msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();

		g_JSONWriter.String("name");
		g_JSONWriter.String("CSVCMsg_SetPause");
		g_JSONWriter.String("data");

		g_JSONWriter.StartObject();

		g_JSONWriter.String("paused");
		g_JSONWriter.Bool(msg.paused());

		g_JSONWriter.EndObject();

		g_JSONWriter.EndObject();
	}
}

 /* svc_CreateStringTable */
template <>
void PrintNetMessage< CSVCMsg_CreateStringTable, svc_CreateStringTable >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CSVCMsg_CreateStringTable msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();

		g_JSONWriter.String("name");
		g_JSONWriter.String("CSVCMsg_CreateStringTable");
		g_JSONWriter.String("data");
        
		g_JSONWriter.StartObject();

		g_JSONWriter.String("name");
		g_JSONWriter.String(msg.name().c_str());
		g_JSONWriter.String("max_entries");
		g_JSONWriter.Int(msg.max_entries());
		g_JSONWriter.String("num_entries");
		g_JSONWriter.Int(msg.num_entries());
		g_JSONWriter.String("user_data_fixed_size");
		g_JSONWriter.Bool(msg.user_data_fixed_size());
		g_JSONWriter.String("user_data_size");
		g_JSONWriter.Int(msg.user_data_size());
		g_JSONWriter.String("user_data_size_bits");
		g_JSONWriter.Int(msg.user_data_size_bits());
		g_JSONWriter.String("flags");
		g_JSONWriter.Int(msg.flags());
		g_JSONWriter.String("string_data");
		g_JSONWriter.String(msg.string_data().c_str());

		g_JSONWriter.EndObject();

		g_JSONWriter.EndObject();
	}
}

 /* svc_UpdateStringTable */
template <>
void PrintNetMessage< CSVCMsg_UpdateStringTable, svc_UpdateStringTable >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CSVCMsg_UpdateStringTable msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();

		g_JSONWriter.String("name");
		g_JSONWriter.String("CSVCMsg_UpdateStringTable");
		g_JSONWriter.String("data");

		g_JSONWriter.StartObject();

		g_JSONWriter.String("table_id");
		g_JSONWriter.Int(msg.table_id());
		g_JSONWriter.String("num_changed_entries");
		g_JSONWriter.Int(msg.num_changed_entries());
		g_JSONWriter.String("string_data");
		g_JSONWriter.String(msg.string_data().c_str());

		g_JSONWriter.EndObject();

		g_JSONWriter.EndObject();
	}
}

 /* svc_VoiceInit */
template <>
void PrintNetMessage< CSVCMsg_VoiceInit, svc_VoiceInit >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CSVCMsg_VoiceInit msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();

		g_JSONWriter.String("name");
		g_JSONWriter.String("CSVCMsg_VoiceInit");
		g_JSONWriter.String("data");

		g_JSONWriter.StartObject();

		g_JSONWriter.String("quality");
		g_JSONWriter.Int(msg.quality());
		g_JSONWriter.String("codec");
		g_JSONWriter.String(msg.codec().c_str());

		g_JSONWriter.EndObject();

		g_JSONWriter.EndObject();
	}
}

 /* svc_VoiceData */
template <>
void PrintNetMessage< CSVCMsg_VoiceData, svc_VoiceData >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CSVCMsg_VoiceData msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();

		g_JSONWriter.String("name");
		g_JSONWriter.String("CSVCMsg_VoiceData");
		g_JSONWriter.String("data");

		g_JSONWriter.StartObject();

		g_JSONWriter.String("client");
		g_JSONWriter.Int(msg.client());
		g_JSONWriter.String("proximity");
		g_JSONWriter.Bool(msg.proximity());
		g_JSONWriter.String("xuid");
		g_JSONWriter.Uint64(msg.xuid());
		g_JSONWriter.String("audible_mask");
		g_JSONWriter.Int(msg.audible_mask());
		g_JSONWriter.String("voice_data");
		g_JSONWriter.String(msg.voice_data().c_str());

		g_JSONWriter.EndObject();

		g_JSONWriter.EndObject();
	}
}

 /* svc_Print */
template <>
void PrintNetMessage< CSVCMsg_Print, svc_Print >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CSVCMsg_Print msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();

		g_JSONWriter.String("name");
		g_JSONWriter.String("CSVCMsg_Print");
		g_JSONWriter.String("data");

		g_JSONWriter.StartObject();

		g_JSONWriter.String("text");
		g_JSONWriter.String(msg.text().c_str());

		g_JSONWriter.EndObject();

		g_JSONWriter.EndObject();
	}
}
 /* svc_Sounds */
template <>
void PrintNetMessage< CSVCMsg_Sounds, svc_Sounds >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	CSVCMsg_Sounds msg;

	if( msg.ParseFromArray( parseBuffer, BufferSize ) )
	{
		g_JSONWriter.StartObject();

		g_JSONWriter.String("name");
		g_JSONWriter.String("CSVCMsg_Sounds");
		g_JSONWriter.String("data");

		g_JSONWriter.StartObject();

		g_JSONWriter.String("reliable_sound");
		g_JSONWriter.Bool(msg.reliable_sound());

        //g_JSONWriter.StartArray();

        //int iSounds;
        //for( iSounds = 0; iSounds < msg.sounds().size(); iSounds++ )
        //{
        //    g_JSONWriter.StartObject();
        //        //g_JSONWriter.String("origin_x");
        //        //g_JSONWriter.Int(msg.sounds( iSounds ).origin_x());
        //        //g_JSONWriter.String("origin_y");
        //        //g_JSONWriter.Int(msg.sounds( iSounds ).origin_y());
        //        //g_JSONWriter.String("origin_z");
        //        //g_JSONWriter.Int(msg.sounds( iSounds ).origin_z());
        //        //g_JSONWriter.String("volume");
        //        //g_JSONWriter.Uint(msg.sounds( iSounds ).volume());
        //        //g_JSONWriter.String("delay_value");
        //        //g_JSONWriter.Double(msg.sounds( iSounds ).delay_value());
        //        //g_JSONWriter.String("sequence_number");
        //        //g_JSONWriter.Int(msg.sounds( iSounds ).sequence_number());
        //        //g_JSONWriter.String("entity_index");
        //        //g_JSONWriter.Int(msg.sounds( iSounds ).entity_index());
        //        //g_JSONWriter.String("channel");
        //        //g_JSONWriter.Int(msg.sounds( iSounds ).channel());
        //        //g_JSONWriter.String("pitch");
        //        //g_JSONWriter.Int(msg.sounds( iSounds ).pitch());
        //        //g_JSONWriter.String("flags");
        //        //g_JSONWriter.Int(msg.sounds( iSounds ).flags());
        //        //g_JSONWriter.String("sound_num");
        //        //g_JSONWriter.Uint(msg.sounds( iSounds ).sound_num());
        //        //g_JSONWriter.String("sound_num_handle");
        //        //g_JSONWriter.Int(msg.sounds( iSounds ).sound_num_handle());
        //        //g_JSONWriter.String("speaker_entity");
        //        //g_JSONWriter.Int(msg.sounds( iSounds ).speaker_entity());
        //        //g_JSONWriter.String("random_seed");
        //        //g_JSONWriter.Int(msg.sounds( iSounds ).random_seed());
        //        //g_JSONWriter.String("sound_level");
        //        //g_JSONWriter.Int(msg.sounds( iSounds ).sound_level());
        //        //g_JSONWriter.String("is_sentence");
        //        //g_JSONWriter.Bool(msg.sounds( iSounds ).is_sentence());
        //        //g_JSONWriter.String("is_ambient");
        //        //g_JSONWriter.Bool(msg.sounds( iSounds ).is_ambient());
        //    g_JSONWriter.EndObject();
        //}
        //g_JSONWriter.EndArray();

		g_JSONWriter.EndObject();

		g_JSONWriter.EndObject();
	}
}
 /* svc_SetView */
 /* svc_FixAngle */
 /* svc_Crosshairangle */
 /* svc_BSPDecal */
 /* svc_UserMessage */
template <>
void PrintNetMessage< CSVCMsg_UserMessage, svc_UserMessage >( CDemoFileDump& Demo, const void *parseBuffer, int BufferSize )
{
	Demo.DumpUserMessage( parseBuffer, BufferSize );
}

 /* svc_GameEvent */
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

			g_JSONWriter.StartObject();
			g_JSONWriter.String("event_name");
			g_JSONWriter.String(msg.has_event_name() ? msg.event_name().c_str() : Descriptor.name().c_str());
			g_JSONWriter.String("event_id");
			g_JSONWriter.Uint(msg.eventid());

			//printf( "%s eventid:%d %s\n", Descriptor.name().c_str(), msg.eventid(),
			//	msg.has_event_name() ? msg.event_name().c_str() : "" );

			for( int i = 0; i < numKeys; i++ )
			{
				const CSVCMsg_GameEventList::key_t& Key = Descriptor.keys( i );
				const CSVCMsg_GameEvent::key_t& KeyValue = msg.keys( i );

				g_JSONWriter.String(Key.name().c_str());
				//printf(" %s: ", Key.name().c_str() );

				if( KeyValue.has_val_string() )
					g_JSONWriter.String(KeyValue.val_string().c_str());
					//printf( "%s ", KeyValue.val_string().c_str() );
				if( KeyValue.has_val_float() )
					g_JSONWriter.Double(double(KeyValue.val_float()));
					//printf( "%f ", KeyValue.val_float() );
				if( KeyValue.has_val_long() )
					g_JSONWriter.Int(KeyValue.val_long());
					//printf( "%d ", KeyValue.val_long() );
				if( KeyValue.has_val_short() )
					g_JSONWriter.Int(KeyValue.val_short());
					//printf( "%d ", KeyValue.val_short() );
				if( KeyValue.has_val_byte() )
					g_JSONWriter.Int(KeyValue.val_byte());
					//printf( "%d ", KeyValue.val_byte() );
				if( KeyValue.has_val_bool() )
					g_JSONWriter.Bool(KeyValue.val_bool());
					//printf( "%d ", KeyValue.val_bool() );
				if( KeyValue.has_val_uint64() )
					g_JSONWriter.Uint64(KeyValue.val_uint64());
					//printf( "%"PRIu64, KeyValue.val_uint64() );

				//printf( "\n" );
			}

			g_JSONWriter.EndObject();
		}
	}
}

 /* svc_PacketEntities */
 /* svc_TempEntities */
 /* svc_Prefetch */
 /* svc_Menu */
 /* svc_GameEventList */
 /* svc_GetCvarValue */



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

	printf("{[\n");

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

	printf("\n]}");
}

