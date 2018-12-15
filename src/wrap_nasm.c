#include "wrap_nasm.h"

#include <stdio.h>
#include <stdlib.h>

#include <disasm/disasm.h>

void DisasmExecutableSection( unsigned char *object_data, uint64_t size, DisasmCallback cb_fn, void *cb_data )
{
    iflag_t prefer;
    int outbuf_size = 2000000;
    char *outbuf = malloc( outbuf_size );

    for ( uint64_t offset = 0; offset < size; )
    {
        int32_t insn_size = disasm( object_data + offset, INSN_MAX, outbuf, outbuf_size, 64, offset, false, &prefer);

        cb_fn( offset, insn_size, outbuf, cb_data );

        offset += insn_size;
    }

    free( outbuf );
}
