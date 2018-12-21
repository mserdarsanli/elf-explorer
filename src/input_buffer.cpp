#include "input_buffer.hpp"

namespace elfexplorer {

InputBuffer::InputBuffer( std::string file_name_, std::vector< unsigned char > &&contents_ )
    : contents( std::move( contents_ ) )
    , m_read( contents.size(), false )
    , file_name( std::move( file_name_ ) )
{
}

void InputBuffer::SetRead( uint64_t offset ) const
{
    m_read[ offset ] = true;
}

std::string_view InputBuffer::StringViewAt( uint64_t offset, uint64_t size ) const
{
    for ( uint64_t i = 0; i < size; ++i )
    {
        SetRead( offset + i );
    }
    return std::string_view( (const char*)contents.data() + offset, size );
}

uint8_t InputBuffer::U8At( uint64_t offset ) const
{
    ASSERT( offset + 1 <= contents.size() );
    SetRead( offset );
    uint8_t res = contents[ offset ];
    return res;
}

uint16_t InputBuffer::U16At( uint64_t offset ) const
{
    ASSERT( offset + 2 <= contents.size() );
    uint16_t res = 0;
    res <<= 8; res += U8At( offset + 1 );
    res <<= 8; res += U8At( offset + 0 );
    return res;
}

uint32_t InputBuffer::U32At( uint64_t offset ) const
{
    ASSERT( offset + 4 <= contents.size() );
    uint32_t res = 0;
    res <<= 8; res += U8At( offset + 3 );
    res <<= 8; res += U8At( offset + 2 );
    res <<= 8; res += U8At( offset + 1 );
    res <<= 8; res += U8At( offset + 0 );
    return res;
}

uint64_t InputBuffer::U64At( uint64_t offset ) const
{
    ASSERT( offset + 8 <= contents.size() );
    uint64_t res = 0;
    res <<= 8; res += U8At( offset + 7 );
    res <<= 8; res += U8At( offset + 6 );
    res <<= 8; res += U8At( offset + 5 );
    res <<= 8; res += U8At( offset + 4 );
    res <<= 8; res += U8At( offset + 3 );
    res <<= 8; res += U8At( offset + 2 );
    res <<= 8; res += U8At( offset + 1 );
    res <<= 8; res += U8At( offset + 0 );
    return res;
}

} // namespace elfexplorer
