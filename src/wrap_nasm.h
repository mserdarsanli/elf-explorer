#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void ( *DisasmCallback )( int offset, int len, char *instruction_str, void *user_data );

void DisasmExecutableSection( unsigned char *data, uint64_t size, DisasmCallback cb_fn, void *cb_data );

#ifdef __cplusplus
}
#endif
