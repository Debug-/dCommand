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

#pragma once

#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
#include <iostream>
#include <unordered_map>

void deblank(char *str) {
	size_t count = 0;
	for (size_t i = 1; i < strlen (str); i++) {
		if (str[count] == ' ' && str[i] == ' ') continue;
		str[++count] = str[i];
	}
	str[++count] = NULL;
}

#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
	|| defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
	+ (uint32_t)(((const uint8_t *)(d))[0]))
#endif

uint32_t SuperFastHash(const char * data, int len) {
	uint32_t hash = len, tmp;
	int rem;

	if (len <= 0 || data == 0) return 0;

	rem = len & 3;
	len >>= 2;

	/* Main loop */
	for (; len > 0; len--) {
		hash += get16bits(data);
		tmp = (get16bits(data + 2) << 11) ^ hash;
		hash = (hash << 16) ^ tmp;
		data += 2 * sizeof (uint16_t);
		hash += hash >> 11;
	}

	/* Handle end cases */
	switch (rem) {
	case 3: hash += get16bits(data);
		hash ^= hash << 16;
		hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
		hash += hash >> 11;
		break;
	case 2: hash += get16bits(data);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1: hash += (signed char)*data;
		hash ^= hash << 10;
		hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

typedef void (*logprintf_t)(char* format, ...);
extern void *pAMXFunctions;
uint32_t SuperFastHash(const char * data, int len);
logprintf_t logprintf;

#define PLUGIN_FUNCTION cell AMX_NATIVE_CALL
