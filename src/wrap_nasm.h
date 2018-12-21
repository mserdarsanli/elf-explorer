// Copyright 2018 Mustafa Serdar Sanli
//
// This file is part of ELF Explorer.
//
// ELF Explorer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ELF Explorer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ELF Explorer.  If not, see <https://www.gnu.org/licenses/>.


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void ( *DisasmCallback )( int offset, int len, char *instruction_str, void *user_data );

void DisasmExecutableSection( unsigned char *data, uint64_t size, DisasmCallback cb_fn, void *cb_data );

#ifdef __cplusplus
}
#endif
