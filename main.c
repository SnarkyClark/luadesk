#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lua_c

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <iup.h>
#include <iuplua.h>

/*
#include "iupcontrols.h"
#include "iupluacontrols.h"
#include "iup_pplot.h"
#include "iuplua_pplot.h"
#include <cd.h>
#include <cdgdiplus.h>
#include <cdlua.h>
#include <cdluaiup.h>
*/

/*
#include "iupluaim.h"
#include <im.h>
#include <im_image.h>
#include <imlua.h>
*/

//#include <cdluaim.h>


#include <stdint.h>
#include <string.h>

#include <lfs.h>
extern int luaopen_luacurl (lua_State *L);
extern int luaopen_lsqlite3 (lua_State *L);

#include "hton.h"
#include "libluadesk.h"
#include "linker.h"

//static lua_State *globalL = NULL;
//static const char *progname = LUA_PROGNAME;

/*
static void lstop (lua_State *L, lua_Debug *ar) {
	// unused args
    (void)ar;
    lua_sethook(L, NULL, 0, 0);
    luaL_error(L, "interrupted!");
}
*/

/*
static void laction (int i) {
	// if another SIGINT happens before lstop, terminate process (default action)
    signal(i, SIG_DFL);
    lua_sethook(globalL, lstop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
}
*/

struct script_struct {
	uint16_t lname;
	uint32_t ldata;
	char* name;
	unsigned char* data;
} typedef script_t;

struct paquet_struct {
	uint16_t count;
	script_t* script;
} typedef paquet_t;

/*
Summary: validate a LuaDesk Paquet TOC footer
Returns: file offset to TOC header, or 0 on failure
*/
uint32_t toc_footer (FILE* fp) {
	// 4 bytes - TOC file offset
	// 10 bytes - signature ("LuaDesk PQ")
	uint32_t offset = 0;
	char sig[10];

	// read TOC footer
	if (fseek(fp, -14, SEEK_END) == 0) {
		if ((fread(&offset, 4, 1, fp) == 1)
		  && (fread(sig, 10, 1, fp) == 1)
		  && (strncmp(sig, "LuaDesk PQ", 10) == 0)) {
			offset = ntohl(offset);
		} else {
			offset = 0;
		}
	}

	return offset;
}

/*
Summary: validate a LuaDesk Paquet TOC header
Returns: number of linked files, or 0 on failure
*/
uint16_t toc_header (FILE* fp, uint32_t offset) {
	// 10 bytes - signature ("LuaDesk PQ")
	// 2 bytes - number of embedded scripts
	char sig[10];
	uint16_t count = 0;

	// read TOC header
	if (fseek(fp, offset, SEEK_SET) == 0) {
		if ((fread(sig, 10, 1, fp) == 1)
		  && (fread(&count, 2, 1, fp) == 1)
		  && (strncmp(sig, "LuaDesk PQ", 10) == 0)) {
			count = ntohs(count);
		} else {
			count = 0;
		}
	}

	return count;
}

/*
Summary: validate, alloc, and read a linked LuaDesk Paquet
Returns: a paquet_t pointer, or NULL on failure
*/
paquet_t* paquet_open (const char* filename) {
	FILE* fp;
	paquet_t* p = NULL;
	int i;
	uint32_t offset = 0;
	uint16_t count = 0;
	uint32_t soffset = 0;

	fp = fopen(filename, "rb");

	if (fp) {
		// validate footer & get TOC offset
		offset = toc_footer(fp);
		// validate header & get file count
		if (offset > 0) count = toc_header(fp, offset);
		if (count > 0 && count <= 16) {
			// alloc & init paquet holder
			p = malloc(sizeof(paquet_t));
            memset(p, 0, sizeof(paquet_t));
            p->count = count;
            p->script = malloc(sizeof(script_t) * p->count);
			if (p->script) {
				// walk through TOC, loading files in mem
				memset(p->script, 0, sizeof(script_t) * p->count);
				offset += 12;
				for (i = 0; i < count; i++) {
					script_t* s = &p->script[i];
					// validate TOC entry
					if ((fseek(fp, offset, SEEK_SET) == 0)
					  && (fread(&s->lname, 2, 1, fp) == 1)
					  && (fread(&s->ldata, 4, 1, fp) == 1)
					  && (fread(&soffset, 4, 1, fp) == 1)) {
					  	// translate and alloc script holder
						s->lname = ntohs(s->lname);
						s->ldata = ntohl(s->ldata);
						s->name = malloc(s->lname + 1);
						s->data = malloc(s->ldata);
						soffset = ntohl(soffset);
						if (s->name && s->data) {
							// zero buffers and read file & info into them
							memset(s->name, 0, s->lname + 1);
							fread(s->name, s->lname, 1, fp);
							memset(s->data, 0, s->ldata);
							if (fseek(fp, soffset, SEEK_SET) == 0)
								fread(s->data, s->ldata, 1, fp);
						} else {
							// TODO: do something drastic
						}
						offset += 10 + s->lname;
					}
				}
			} else {
				// TODO: do something drastic
			}
		}
		fclose(fp);
	}

	return p;
}

/*
Summary: dealloc a paquet buffer
Returns: nada
*/
void paquet_free (paquet_t* p) {
	uint16_t i;
	if (p) {
		if (p->script) {
			for (i = 0; i < p->count; i--) {
				if (p->script->name) free (p->script->name);
				if (p->script->data) free (p->script->data);
			}
			free(p->script);
		}
		free(p);
	}
}

/*
Summary: report on the status of a Lua VM invocation to the user
Returns: Lua status code (0 = success)
*/
int report (lua_State *L, int status) {
    if (status && !lua_isnil(L, -1)) {
        const char *msg = lua_tostring(L, -1);
        if (msg == NULL) msg = "(error object is not a string)";
        fprintf(stderr, msg);
        IupMessage("Script Error!", msg);
        lua_pop(L, 1);
    }
    return status;
}

int main (int argc, char *argv[]) {
    int i = 0;
    int result = 0;
	paquet_t* p = NULL;

    // init IUP
    IupOpen(&argc, &argv);
    //IupSetLanguage("ENGLISH");

    // create Lua VM
    lua_State *L = lua_open();
    // add default libs
    luaL_openlibs(L);
    // add third-party libs
	luaopen_lfs(L);
    luaopen_luacurl(L);
    luaopen_lsqlite3(L);
    iuplua_open(L);
    // add internal lib
    luaL_register(L, "ldesk", R_ld_functions);

	p = paquet_open(argv[0]);

	if (p) {
		// if found, run paquet scripts in order
		for (i = 0; i < p->count; i++) {
			script_t* s = &p->script[i];
			if (s->name && s->data) {
				result = report(L, luaL_loadbuffer(L, s->data, s->ldata, s->name)
					|| lua_pcall(L, 0, 0, 0));
				// there was a syntax error, stop the car!
				if (result != 0) break;
			}
		}
		paquet_free(p);
	} else {
		// if no linked scripts, then run embedded packaging script
		result = report(L, luaL_loadbuffer(L, linker_bytes, sizeof(linker_bytes), linker_source)
			|| lua_pcall(L, 0, 0, 0));
	}

	if (result == 0) {
		// all done loading scripts & no errors, time to rock & roll
		lua_getglobal(L, "main");
		if (lua_isfunction(L, -1)) {
			// create table with arguments
			lua_createtable(L, argc, 0);
			for (i = 0; i < argc; ++i) {
				lua_pushstring(L, argv[i]);
				lua_rawseti(L, -2, i + 1);
			}
			// call main() function
			report(L, lua_pcall(L, 1, 1, 0));
			// if main() returned an number,
			// cast it to int and return it as process result
			if (lua_isnumber(L, -1)) result = lua_tointeger(L, -1);
		} else {
			IupMessage("LuaDesk", "main() function not found!");
		}
	}

    lua_close(L);
    IupClose();

    return result;
}
