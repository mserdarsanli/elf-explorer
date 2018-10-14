#pragma once

#define ASSERT( expr ) \
    if ( !( expr ) ) \
    { \
        throw std::runtime_error( "Assertion failed: " + std::string( #expr ) ); \
    }

inline
uint8_t LoadU8( const unsigned char *data )
{
    uint8_t res = data[ 0 ];
    return res;
}

inline
uint16_t LoadU16( const unsigned char *data )
{
    uint16_t res = 0;
    res <<= 8; res += data[ 1 ];
    res <<= 8; res += data[ 0 ];
    return res;
}

inline
uint32_t LoadU32( const unsigned char *data )
{
    uint32_t res = 0;
    res <<= 8; res += data[ 3 ];
    res <<= 8; res += data[ 2 ];
    res <<= 8; res += data[ 1 ];
    res <<= 8; res += data[ 0 ];
    return res;
}

inline
uint64_t LoadU64( const unsigned char *data )
{
    uint64_t res = 0;
    res <<= 8; res += data[ 7 ];
    res <<= 8; res += data[ 6 ];
    res <<= 8; res += data[ 5 ];
    res <<= 8; res += data[ 4 ];
    res <<= 8; res += data[ 3 ];
    res <<= 8; res += data[ 2 ];
    res <<= 8; res += data[ 1 ];
    res <<= 8; res += data[ 0 ];
    return res;
}
