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
