#ifndef ELFEXPLORER__ELF_STRUCTS_HPP__
#define ELFEXPLORER__ELF_STRUCTS_HPP__

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>
#include <variant>
#include <vector>

#include "enums.hpp"
#include "input_buffer.hpp"

namespace elfexplorer {

struct StringTable
{
    std::string_view StringAtOffset( uint64_t string_offset ) const;

    std::string m_str;
};

struct Symbol
{
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
    std::string m_name;
    SectionType m_type;
    SectionFlags m_attrs;
    uint64_t m_address;
    uint64_t m_offset;
    uint64_t m_size;
    uint32_t m_asso_idx;
    uint32_t m_info;
    uint64_t m_addr_align;
    uint64_t m_ent_size;
};

struct SymbolTable
{
    std::vector< Symbol > m_symbols;
};

struct RelocationEntry
{
    uint64_t m_offset;
    X64RelocationType m_type;
    uint32_t m_symbol;
    int64_t m_addend;
};

struct RelocationEntries
{
    std::vector< RelocationEntry > m_entries;
};

struct GroupSection
{
    uint32_t m_flags;
    std::vector< uint32_t > m_section_indices;
};

struct NoBitsSection
{
    std::string m_data;
};

struct InitArraySection
{
    std::string m_data;
};

struct ProgBitsSection
{
    std::string m_data;
    bool m_is_executable = false; // TODO this can be used from section header
};

struct Section
{
    SectionHeader m_header;

    std::variant< std::monostate
                , GroupSection
                , StringTable
                , SymbolTable
                , RelocationEntries
                , NoBitsSection
                , InitArraySection
                , ProgBitsSection
    > m_var;
};

struct ELF_File
{
    static ELF_File LoadFrom( InputBuffer & );

    std::vector< Section > m_sections;
};

} // namspace elfexplorer

#endif // ELFEXPLORER__ELF_STRUCTS_HPP__
