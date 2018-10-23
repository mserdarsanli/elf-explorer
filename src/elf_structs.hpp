#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

#define ASSERT( expr ) \
    if ( !( expr ) ) \
    { \
        throw std::runtime_error( "Assertion failed: " + std::string( #expr ) ); \
    }


enum class SectionType : uint32_t
{
    ProgramData = 1,
    SymbolTable = 2,
    StringTable = 3,
    RelocationEntries = 4,
    Nobits = 8,
    Constructors = 14,
    Group = 17,
};

enum class SectionFlags : uint64_t
{
    Writable   = 1 << 0,
    Alloc      = 1 << 1,
    Executable = 1 << 2,

    Merge      = 1 << 4,
    Strings    = 1 << 5,
    InfoLink   = 1 << 6,

    Group      = 1 << 9,
};

struct SectionFlagsBitfield
{
    SectionFlagsBitfield() = default;
    SectionFlagsBitfield( uint64_t val )
        : m_val( val )
    {
    }

    uint64_t m_val = 0;
};

inline // TODO
std::string_view to_string( SectionType t )
{
    switch ( t )
    {
    case SectionType::ProgramData: return "ProgramData";
    case SectionType::SymbolTable: return "SymbolTable";
    case SectionType::StringTable: return "StringTable";
    case SectionType::RelocationEntries: return "RelocationEntries";
    case SectionType::Constructors: return "Constructors";
    case SectionType::Group: return "Group";
    case SectionType::Nobits: return "Nobits";
    }
    return "\033[31mUNKNOWN\033[0m";
}

enum class SymbolBinding : uint8_t
{
    Local = 0,
    Global = 1,
    Weak = 2,
};

inline std::string to_string( SymbolBinding b )
{
    switch ( b )
    {
    case SymbolBinding::Local:
        return "Local";
    case SymbolBinding::Global:
        return "Global";
    case SymbolBinding::Weak:
        return "Weak";
    }

    std::stringstream out;
    out << "\033[31mUnknown( " << static_cast< int >( b ) << " )\033[0m";
    return out.str();
}

enum class SymbolType : uint8_t
{
    NotSpecified = 0,
    Object = 1,
    Function = 2,
    Section = 3,
    File = 4,
};

inline std::string to_string( SymbolType t )
{
    switch ( t )
    {
    case SymbolType::NotSpecified:
        return "NotSpecified";
    case SymbolType::Object:
        return "Object";
    case SymbolType::Function:
        return "Function";
    case SymbolType::Section:
        return "Section";
    case SymbolType::File:
        return "File";
    }

    std::stringstream out;
    out << "\033[31mUnknown( " << static_cast< int >( t ) << " )\033[0m";
    return out.str();
}

enum class SymbolVisibility : uint8_t
{
    Default = 0,
    Internal = 1,
    Hidden = 2,
    Protected = 3,
};

inline std::string to_string( SymbolVisibility v )
{
    switch ( v )
    {
    case SymbolVisibility::Default:
        return "Default";
    case SymbolVisibility::Internal:
        return "Internal";
    case SymbolVisibility::Hidden:
        return "Hidden";
    case SymbolVisibility::Protected:
        return "Protected";
    }

    std::stringstream out;
    out << "\033[31mUnknown( " << static_cast< int >( v ) << " )\033[0m";
    return out.str();
}

inline // TODO
std::string to_string( SectionFlagsBitfield f )
{
    uint64_t val = f.m_val;

    std::stringstream out;

    #define SR_PROC_BIT( name ) \
        if ( val & static_cast< uint64_t >( SectionFlags::name ) ) \
        { \
            out << #name; \
            val -= static_cast< uint64_t >( SectionFlags::name ); \
            if ( val ) \
            { \
                out << " | "; \
            } \
        }

    SR_PROC_BIT( Writable   );
    SR_PROC_BIT( Alloc      );
    SR_PROC_BIT( Executable );

    SR_PROC_BIT( Merge      );
    SR_PROC_BIT( Strings    );
    SR_PROC_BIT( InfoLink   );

    SR_PROC_BIT( Group      );

    #undef SR_PROC_BIT

    if ( val )
    {
        out << "\033[31mUnknown( " << val << " )\033[0m";
    }

    return out.str();
}


struct ELF_File;


struct StringTable
{
    StringTable() = default;

    StringTable( const ELF_File &ctx, uint64_t section_offset );
    std::string_view StringAtOffset( uint64_t string_offset ) const;

    std::string m_str;
};

struct Symbol
{
    Symbol( const ELF_File &ctx, uint64_t offset );
    void Dump() const;

    std::string m_name;
    SymbolBinding m_binding;
    SymbolType m_type;
    SymbolVisibility m_visibility;
    uint16_t m_section_idx;
    uint64_t m_value;
    uint64_t m_size;
};

struct SectionHeader
{
    SectionHeader( const ELF_File &ctx, uint64_t header_offset );
    void Dump() const;

    std::string m_name;
    SectionType m_type;
    SectionFlagsBitfield m_attrs;
    uint64_t m_address;
    uint64_t m_offset;
    uint32_t m_asso_idx;
    uint32_t m_info;
    uint64_t m_addr_align;
    uint64_t m_ent_size;
};


struct ELF_File
{
    ELF_File( std::vector< unsigned char > &&contents_ );

    uint8_t U8At( uint64_t offset ) const;
    uint16_t U16At( uint64_t offset ) const;
    uint32_t U32At( uint64_t offset ) const;
    uint64_t U64At( uint64_t offset ) const;

    uint64_t GetSectionSize( uint64_t section_offset ) const;
    void DumpGroupSection( uint64_t offset, uint64_t size ) const;

    std::vector< unsigned char > contents;
    mutable std::vector< bool > m_read; // Mark all the read bytes

    uint64_t section_header_offset;
    uint16_t section_header_entry_size;
    uint16_t section_header_num_entries;
    uint16_t section_names_header_index;

    std::optional< StringTable > strtab;
    std::optional< StringTable > shstrtab;

    std::vector< uint64_t > section_offsets;
};
