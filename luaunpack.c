#include <lua.h>
#include <lauxlib.h>
#include <luaunpack.h>

int lunpack_newpacket(lua_State *L, void *data, lua_Integer length) {
	packet_t *packet = lua_newuserdata(L, sizeof(packet_t));

	if (packet == NULL)
		return luaL_error(L, "not enough memory");

	packet->offset = 0;
	packet->length = length;
	packet->data = data;

	return 0;
}

static packet_t *verify_mem_access(lua_State *L, lua_Integer *offset, lua_Integer *size) {
	packet_t *packet = lua_touserdata(L, 1);

	*offset = lua_tointeger(L, 2);
	*size = lua_tointeger(L, 3);
	if(packet->offset + *offset + *size > packet->length)
		return NULL;

	return packet;
}

static int unpackint(lua_State *L) {
	lua_Integer offset;
	lua_Integer size;
	packet_t *packet;
	unsigned char *ptr;

	packet = verify_mem_access(L, &offset, &size);
	if (packet == NULL)
		return luaL_error(L, "trying to access out of bounds memory");

	ptr = packet->data + offset;
	switch (size) {
		case 1: lua_pushinteger(L, ptr[0]);        break;
		case 2: lua_pushinteger(L, (ptr[0] << 8) | ptr[1]); break;
		default: luaL_error(L, "invalid size");
	}

	return 1;
}

static int unpackstring(lua_State *L) {
	packet_t *packet;
	lua_Integer offset;
	lua_Integer size;

	packet = verify_mem_access(L, &offset, &size);
	if (packet == NULL)
		return luaL_error(L, "trying to access out of bounds memory");

	lua_pushlstring(L, packet->data + offset, size);

	return 1;
}

static const struct luaL_Reg unpack [] = {
	{"int", unpackint},
	{"string", unpackstring},
	{NULL,      NULL}
};

int luaopen_unpack(lua_State *L) {
	luaL_newlib(L, unpack);
	return 1;
}
