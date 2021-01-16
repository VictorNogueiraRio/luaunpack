#include <lua.h>
#include <lauxlib.h>
#include <luaunpack.h>

int lunpack_newpacket(lua_State *L, void *data, lua_Integer length) {
	packet_t *packet = lua_newuserdata(L, sizeof(packet_t));

	if (packet == NULL)
		return -ENOMEM;

	packet->length = length;
	packet->data = data;

	return 0;
}

static packet_t *verify_mem_access(lua_State *L, lua_Integer *offset, lua_Integer *size) {
	packet_t *packet = lua_touserdata(L, 1);

	if (packet == NULL)
		return NULL;

	*offset = lua_tointeger(L, 2);
	*size = lua_tointeger(L, 3);
	if(*offset + *size > packet->length)
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
		return 0;

	ptr = packet->data + offset;
	switch (size) {
		case 1: lua_pushinteger(L, ptr[0]);        break;
		case 2: lua_pushinteger(L, (ptr[0] << 8) | ptr[1]); break;
		default: return 0;
	}

	return 1;
}

static int unpackstring(lua_State *L) {
	packet_t *packet;
	lua_Integer offset;
	lua_Integer size;

	packet = verify_mem_access(L, &offset, &size);
	if (packet == NULL)
		return 0;

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
