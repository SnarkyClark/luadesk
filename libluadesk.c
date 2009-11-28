#include <stdint.h>

#include <iup.h>
#include "hton.h"
#include "libluadesk.h"

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

const luaL_Reg R_ld_functions[] = {
	{"htons", L_htons},
	{"htonl", L_htonl},
	{"ntohs", L_htons},
	{"ntohl", L_htonl},
	{"msleep", L_msleep},
	{"sleep", L_sleep},
	{NULL, NULL}
};

