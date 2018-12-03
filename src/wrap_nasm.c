#include "wrap_nasm.h"

#include <stdio.h>
#include <stdlib.h>

#include <disasm/disasm.h>

static void output_ins(uint64_t offset, uint8_t *data, int datalen, char *insn, void (*output_callback)( const char *, void * ), void *cb_data )
{
    char out_buf[ 500 ]; // TODO is this enough?
    char *out = out_buf;
    *out = 0;

    const int bytes_per_line = 15; // Max x64 instruction size??
    int bytes;
    out += sprintf( out, "%08"PRIX64"  ", offset);

    bytes = 0;
    while (datalen > 0 && bytes < bytes_per_line) {
        out += sprintf( out, "%02X ", *data++);
        bytes++;
        datalen--;
    }

    out += sprintf(out, "%*s%s\n", (bytes_per_line + 1 - bytes) * 3, "", insn);

    while (datalen > 0) {
        out += sprintf(out, "         -");
        bytes = 0;
        while (datalen > 0 && bytes < bytes_per_line) {
            out += sprintf(out, "%02X", *data++);
            bytes++;
            datalen--;
        }
        out += sprintf(out, "\n");
    }

    output_callback( out_buf, cb_data );
}

void DisasmExecutableSection( const unsigned char *object_data, uint64_t size, void (*output_callback)( const char *, void * ), void *cb_data )
{
    iflag_t prefer;
    int outbuf_size = 2000000;
    char *outbuf = malloc( outbuf_size );

    for ( uint64_t offset = 0; offset < size; )
    {
        int32_t lendis = disasm( object_data + offset, INSN_MAX, outbuf, outbuf_size, 64, offset, false, &prefer);
        output_ins( offset, object_data + offset, lendis, outbuf, output_callback, cb_data );
        offset += lendis;
    }

    free( outbuf );
}
