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


#ifndef ELFEXPLORER__INPUT_BUFFER_HPP__
#define ELFEXPLORER__INPUT_BUFFER_HPP__

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

// TODO move this
#define ASSERT( expr ) \
    if ( !( expr ) ) \
    { \
        throw std::runtime_error( "Assertion failed: " + std::string( #expr ) ); \
    }


namespace elfexplorer {

class InputBuffer
{
public:
    InputBuffer( std::string file_name_, std::vector< unsigned char > &&contents_ );

    // Reading data in little endian
    uint8_t U8At( uint64_t offset ) const;
    uint16_t U16At( uint64_t offset ) const;
    uint32_t U32At( uint64_t offset ) const;
    uint64_t U64At( uint64_t offset ) const;

    std::string_view StringViewAt( uint64_t offset, uint64_t size ) const;

private:
    void SetRead( uint64_t offset ) const;

public: // TODO make private
    std::vector< unsigned char > contents;
    mutable std::vector< bool > m_read; // Mark all the read bytes
    std::string file_name;
};

} // namespace elfexplorer

#endif // ELFEXPLORER__INPUT_BUFFER_HPP__
