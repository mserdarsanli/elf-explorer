#include "wrap_nasm.h"

#include <stdio.h>
#include <stdlib.h>

#include <disasm/disasm.h>

static void output_ins(uint64_t offset, uint8_t *data, int datalen, char *insn)
{
    const int BPL = 8; /* bytes per line of hex dump */
    int bytes;
    fprintf(stderr, "%08"PRIX64"  ", offset);

    bytes = 0;
    while (datalen > 0 && bytes < BPL) {
        fprintf(stderr, "%02X", *data++);
        bytes++;
        datalen--;
    }

    fprintf(stderr, "%*s%s\n", (BPL + 1 - bytes) * 2, "", insn);

    while (datalen > 0) {
        fprintf(stderr, "         -");
        bytes = 0;
        while (datalen > 0 && bytes < BPL) {
            fprintf(stderr, "%02X", *data++);
            bytes++;
            datalen--;
        }
        fprintf(stderr, "\n");
    }
}

void DisasmExecutableSection( const unsigned char *object_data, uint64_t size )
{
    fprintf( stderr, "Disassembling section of size = %" PRIu64 "\n", size );

    iflag_t prefer;
    int outbuf_size = 2000000;
    char *outbuf = malloc( outbuf_size );

    for ( uint64_t offset = 0; offset < size; ++offset )
    {
        int32_t lendis = disasm( object_data + offset, INSN_MAX, outbuf, outbuf_size, 64, offset, false, &prefer);
        output_ins( offset, object_data + offset, lendis, outbuf);
        offset += lendis;
    }

    free( outbuf );
}
