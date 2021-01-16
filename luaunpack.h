#include <lua.h>

typedef struct {
	lua_Integer length;
	void *data;
} packet_t;

extern int luaopen_unpack(lua_State *L);
extern int lunpack_newpacket(lua_State *L, void *packet, lua_Integer length);
