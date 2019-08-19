#include <lua.h>
#include <lauxlib.h>
#include <luaunpack.h>

int lunpack_newpacket(lua_State *L, void *data, int length) {
	packet_t *packet = lua_newuserdata(L, sizeof(packet_t));

	if (packet == NULL)
		return luaL_error(L, "not enough memory");

	packet->offset = 0;
	packet->length = length;
	packet->data = data;

	return 0;
}

static packet_t *verify_mem_access(lua_State *L, unsigned int *offset, unsigned int *size) {
	packet_t *packet = lua_touserdata(L, 1);

	*offset = lua_tointeger(L, 2);
	*size = lua_tointeger(L, 3);
	if(packet->offset + *offset + *size / 8 >= packet->length)
		return NULL;

	return packet;
}

static int unpackint(lua_State *L) {
	unsigned int value;
	unsigned int offset;
	unsigned int size;
	packet_t *packet;
	int *ptr;

	packet = verify_mem_access(L, &offset, &size);
	if (packet == NULL)
		return luaL_error(L, "trying to access out of bounds memory");

	ptr = packet->data + offset;
	switch (size) {
		case 4:
			value = *ptr;
			value &= 0x0F;
			lua_pushinteger(L, value);
			break;
		case 8:
			value = *ptr;
			value &= 0xFF;
			lua_pushinteger(L, value);
			break;
		case 16:
			value = *((unsigned short *) ptr);
			value &= 0xFFFF;
			lua_pushinteger(L, htons(value));
			break;
	}

	return 1;
}

static const struct luaL_Reg unpack [] = {
	{"unpackint", unpackint},
	{NULL,      NULL}
};

int luaopen_unpack(lua_State *L) {
	luaL_newlib(L, unpack);
	return 1;
}
