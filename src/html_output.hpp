#ifndef ELFEXPLORER__HTML_OUTPUT_HPP__
#define ELFEXPLORER__HTML_OUTPUT_HPP__

#include <iostream>
#include <string>
#include <vector>

#include "elf_structs.hpp"

namespace elfexplorer {

void RenderAsHTML( std::ostream &html_out, const ELF_File &elf );

std::string escape( const std::string &s );

} // namespace elfexplorer

#endif // ELFEXPLORER__HTML_OUTPUT_HPP__
