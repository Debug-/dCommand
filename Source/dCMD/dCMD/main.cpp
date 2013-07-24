/*
 * Copyright (C) 2013 Пельмень and Tracker1
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "main.h"

std::list<AMX*> d_Command ;

std::map<unsigned int,char *> Aliases;

PLUGIN_FUNCTION COMMAND( AMX* amx, cell* params ) {
	
	char	*cmdtext	;
	int		cmdlen		;

	amx_StrParam( amx, params[ 2 ], cmdtext,cmdlen ) ;

	char   *dcommand_str 	=  strstr( cmdtext, " " ) 	;

	if ( dcommand_str ) {
		cmdtext[ dcommand_str++ - cmdtext ] = '\0' ;

		#ifdef _DEBUG
			logprintf("dCommand_STR: %s",dcommand_str);
		#endif	

		deblank( dcommand_str ) ;

		for ( int i = 0, j = strlen( cmdtext ); i < j; ++i ) { cmdtext[ i ] = tolower( cmdtext[ i ] ) ; }

		unsigned int Hash = SuperFastHash( cmdtext, strlen( cmdtext ) ) ;

		if ( Aliases.count( Hash ) ) {

			#ifdef _DEBUG
				logprintf("Alias found %s -> %s| %d", cmdtext, Aliases.find( Hash )->second, Hash ) ;
			#endif
			char Command[ 32 ];

			strcpy_s( Command, 32, Aliases.find( Hash ) -> second ) ;

			Command[ 0 ] = '_' ;
			cmdtext = Command ;
		}

		cmdtext[ 0 ] = '_' ;

		int amx_Idx;
		#ifdef _DEBUG
			logprintf("Got Params: %s | Command: %s",dcommand_str,cmdtext);
		#endif
		for( std::list<AMX*>::iterator amx = d_Command.begin(); amx != d_Command.end(); ++amx ) {
			if( amx_FindPublic( *amx, cmdtext, &amx_Idx ) == AMX_ERR_NONE ) {

				#ifdef _DEBUG
					logprintf("Found public!!");
				#endif

				cell amx_Address, amx_Ret, *phys_addr ;

				amx_PushString( *amx, &amx_Address, &phys_addr, dcommand_str, 0, 0 ) ;
				amx_Push( *amx, params[ 1 ] ) ;

				amx_Exec( *amx, &amx_Ret, amx_Idx ) ;
				amx_Release( *amx, amx_Address ) ;

				return 1 ;
			}
		}
	} else {
		int amx_Idx;

		for ( int i = 0, j = strlen( cmdtext ) ; i < j; ++i, cmdtext[ i ] = tolower( cmdtext[ i ] ) ) { }

		unsigned int Hash = SuperFastHash( cmdtext, strlen( cmdtext ) ) ;
		if ( Aliases.count( Hash ) ) {

			#ifdef _DEBUG
				logprintf( "Alias found %s -> %s| %d", cmdtext, Aliases.find( Hash ) -> second, Hash ) ;
			#endif

			char Command[ 32 ] ;
			strcpy_s( Command, 32, Aliases.find( Hash ) -> second ) ;
			Command[ 0 ] = '_' ;
			cmdtext = Command;
		}

		cmdtext[ 0 ] = '_' ;

		#ifdef _DEBUG		
			logprintf( "Got Params: NULL | Command: %s", cmdtext ) ;
		#endif

		for( std::list<AMX*>::iterator amx = d_Command.begin(); amx != d_Command.end(); ++amx )  {
			if( amx_FindPublic( *amx, cmdtext, &amx_Idx ) == AMX_ERR_NONE ) {
				cell amx_Ret ;

				#ifdef _DEBUG
					logprintf( "Found public!!" ) ;
				#endif

				amx_Push( *amx, 0 ) ;
				amx_Push( *amx, params[ 1 ] ) ;

				amx_Exec( *amx, &amx_Ret, amx_Idx ) ;

				return 1 ;
			}
		}
	}

	return 0 ;
}



PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()  {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)  {
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];
	logprintf("######################################");
	logprintf("#   @Debug Command Loaded            #");
	logprintf("#                                    #");
	logprintf("#   @Author: Пельмень and Tracker1   #");
	logprintf("#   @Version: 0.2                    #");
	logprintf("#   @Released: 28/04/2013            #");
	logprintf("#                                    #");
	logprintf("######################################\n");

    return 1;
}

PLUGIN_FUNCTION RegisterAliases( AMX* amx, cell* params ) {
	char *command, *orig;
	int length,
		i = 1 ;

	amx_StrParam( amx, params[ i++ ], orig, length ) ;
	amx_StrParam( amx, params[ i++ ], command, length ) ; 

	while( command != NULL ) {
		char *temp = new char[ 32 ] ;

		strcpy_s( temp, 32, orig ) ;

		for ( int x = 0, j = strlen( temp ) ; x < j; ++x, temp[ x ] = tolower( temp[ x ] ) ) { }
		for ( int x = 0, j = strlen( command ) ; x < j; ++x, command[ x ] = tolower( command[ x ] ) ) { }

		Aliases.insert( std::make_pair( SuperFastHash( command, length ),temp ) ) ;

		#ifdef _DEBUG
			logprintf( "Hash Alias: %d,Alias: %s, Cmd: %s", SuperFastHash( command, length ), command, temp ) ;
		#endif

		command = NULL ;
		temp = NULL ;

		amx_StrParam( amx, params[ i++ ], command, length ) ;
	}
	#ifdef _DEBUG
		logprintf( "Aliases:" ) ;
		for ( std::map< unsigned int, char * >::iterator it=Aliases.begin(); it != Aliases.end(); ++it )
			std::cout << it->first << " => " << it->second << '\n';
	#endif

	return 0;
}
PLUGIN_EXPORT void PLUGIN_CALL Unload() {
    logprintf( "@Debug Command Unloaded" ) ;
}
AMX_NATIVE_INFO PluginNatives[] = {
    {"dCOMMAND", COMMAND},
	{"dAliasReg", RegisterAliases},
    {0, 0}
};
PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
	d_Command.push_back( amx ) ;
	return amx_Register( amx, PluginNatives, -1 ) ;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
	for( std::list<AMX*>::iterator i = d_Command.begin() ; i != d_Command.end() ; ++i ) {
		if( *i == amx ) {
			d_Command.erase( i ) ;
			break ;
		}
	}

	return AMX_ERR_NONE ;
}
