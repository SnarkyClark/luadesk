#include <stdint.h>
#include <string.h>
#include <iup.h>
#include "hton.h"
#include "libluadesk.h"
#include "crc32.h"
#include "md5.h"

#include <unistd.h>
#define msleep(msec) (usleep((msec) * 1000))

int L_htons(lua_State *L) {
    uint16_t n = (uint16_t)luaL_checkint(L, 1);
    n = htons(n);
    lua_pushlstring(L, (const char*)&n, 2);
    return 1;
}

int L_htonl(lua_State *L) {
    uint32_t n = (uint32_t)luaL_checklong(L, 1);
    n = htonl(n);
    lua_pushlstring(L, (const char*)&n, 4);
    return 1;
}

int L_ntohs(lua_State *L) {
    uint16_t n = *(uint16_t*)luaL_checkstring(L, 1);
    n = htons(n);
    lua_pushnumber(L, n);
    return 1;
}

int L_ntohl(lua_State *L) {
    uint32_t n = *(uint32_t*)luaL_checkstring(L, 1);
    n = htonl(n);
    lua_pushnumber(L, n);
    return 1;
}

int L_msleep(lua_State *L) {
	uint32_t i;
	uint32_t n = (uint32_t)luaL_checklong(L, 1);
	for (i = 0; i < n; i++) {
		usleep(1000);
		if (IupLoopStep() == IUP_CLOSE) break;
	}
	return 0;
}

int L_sleep(lua_State *L) {
	uint32_t i;
	uint32_t n = (uint32_t)luaL_checklong(L, 1);
	for (i = 0; i < n * 100; i++) {
		usleep(1000);
		if (IupLoopStep() == IUP_CLOSE) break;
	}
	return 0;
}

int L_crc32(lua_State* L) {
	size_t l = 0; uint32_t n = 0;
	const unsigned char* s = (const unsigned char*)lua_tolstring(L, 1, &l);
	if (s && l); n = (uint32_t)crc32(s, (unsigned int)l);
	lua_pushlstring(L, (const char*)&n, sizeof(n));
	return 1;
}

int L_md5(lua_State* L) {
	struct ldesk_MD5Context context;
	uint8_t checksum[16];
	size_t l = 0;
	memset(checksum, 0, 16);
	const uint8_t* s = (uint8_t*)lua_tolstring(L, 1, &l);
	if (s && l) {
		ldesk_MD5Init(&context);
		ldesk_MD5Update(&context, s, l);
		ldesk_MD5Final(checksum, &context);
	}
	lua_pushlstring(L, (const char*)checksum, 16);
	return 1;
}

const luaL_Reg R_ld_functions[] = {
	{"htons", L_htons},
	{"htonl", L_htonl},
	{"ntohs", L_htons},
	{"ntohl", L_htonl},
	{"msleep", L_msleep},
	{"sleep", L_sleep},
	{"crc32", L_crc32},
	{"md5", L_md5},
	{NULL, NULL}
};

