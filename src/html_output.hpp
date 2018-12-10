#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "elf_structs.hpp"

std::string escape( const std::string &s );
void RenderAsStringTable( std::ostream &html_out, std::string_view s );
void RenderSymbolTable( std::ostream &html_out, const std::vector< Symbol > &symbols );

void RenderSectionHeaders( std::ostream &html_out,
                           const std::vector< SectionHeader > &section_headers );
void RenderSectionTitle( std::ostream &html_out, size_t i, const SectionHeader &sh );


void RenderBinaryData( std::ostream &html_out, std::string_view s );
