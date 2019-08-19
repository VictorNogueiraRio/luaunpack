typedef struct {
	size_t offset;
	size_t length;
	void *data;
} packet_t;

extern int luaopen_unpack(lua_State *L);
extern int lunpack_newpacket(lua_State *L, void *packet, int length);
