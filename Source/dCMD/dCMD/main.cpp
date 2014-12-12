/*
 * Copyright (C) 2014 Inn0va and Tracker1
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

#include "main.h"

std::unordered_map<int, std::pair <int, AMX*>> cmd;
std::unordered_map<unsigned int, char *> Aliases;

PLUGIN_FUNCTION COMMAND (AMX* amx, cell* params) {
	char *cmdtext = NULL;
	char *tokens = NULL;

	amx_StrParam(amx, params[2], cmdtext);
	strtok_s(cmdtext, " ", &tokens);
	int cmdlen = strlen (cmdtext);
	for (int i = 1; i < cmdlen; cmdtext[i] = tolower(cmdtext[i++]));
	unsigned int hash = SuperFastHash(cmdtext, strlen(cmdtext));
	size_t tmp = cmd.count(hash);

	if (Aliases.count(hash)) {
		char Command[32];
		strcpy_s(Command, 32, Aliases.find(hash)->second);
		Command[0] = '_';
		cmdtext = Command;
	}
	
	cmdtext[0] = '_';
	if (*tokens) {
		deblank(tokens);
		int idx;
		cell amx_Address;
		if (tmp) {
			std::pair<int, AMX *> temp = cmd.find(hash)->second;
			amx_PushString(temp.second, &amx_Address, NULL, tokens, NULL, NULL);
			amx_Push(temp.second, params[1]);
			amx_Exec(temp.second, NULL, temp.first);
			amx_Release(temp.second, amx_Address);
			return 1;
		} else {
			amx_FindPublic(amx, cmdtext, &idx);
			amx_PushString(amx, &amx_Address, NULL, tokens, NULL, NULL);
			amx_Push(amx, params [1]);
			amx_Exec(amx, NULL, idx);
			amx_Release(amx, amx_Address);
			cmd[hash] = std::pair<int, AMX*>(idx, amx);
			return 1;
		}
	} else {
		int idx;
		if (tmp) {
			std::pair<int, AMX *> temp = cmd.find(hash)->second;
			amx_Push(temp.second, params[1]);
			amx_Exec(temp.second, NULL, temp.first);
			return 1;
		} else {
			amx_FindPublic(amx, cmdtext, &idx);
			amx_Push(amx, params [1]);
			amx_Exec(amx, NULL, idx);
			cmd[hash] = std::pair<int, AMX*>(idx, amx);
			return 1;
		}
	}
	return 0;
}

PLUGIN_FUNCTION RegisterAliases(AMX* amx, cell* params) {
	char *command, *orig;
	int i = 1;

	amx_StrParam(amx, params[i++], orig);
	amx_StrParam(amx, params[i++], command);

	while (command != NULL) {
		char *temp = new char[32];
		strcpy_s(temp, 32, orig);

		for (size_t x = 0; x < strlen(temp); temp[x] = tolower(temp[x++]));
		for (size_t x = 0; x < strlen(command); command[x] = tolower(command[x++]));
		Aliases.insert(std::make_pair(SuperFastHash(command, strlen(command)), temp));

		delete[] temp;
		command = NULL;
		temp = NULL;

		amx_StrParam(amx, params[i++], command);
	}
	return 0;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()  {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)  {
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
	logprintf("######################################");
	logprintf("#   @Debug Command Loaded            #");
	logprintf("#                                    #");
	logprintf("#   @Author: Inn0va and Tracker1   #");
	logprintf("#   @Version: 0.3                    #");
	logprintf("#   @Released: 10/12/2014            #");
	logprintf("#                                    #");
	logprintf("######################################\n");

	return 1;
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
	return amx_Register( amx, PluginNatives, -1 ) ;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
	return AMX_ERR_NONE ;
}
