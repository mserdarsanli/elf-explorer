#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void DisasmExecutableSection( const unsigned char *data, uint64_t size, void (*output_callback)( const char *, void * ), void *cb_data );

#ifdef __cplusplus
}
#endif
