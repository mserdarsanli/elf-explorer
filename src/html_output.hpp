#pragma once

#include <iostream>
#include <string>

std::string escape( const std::string &s );
void RenderAsStringTable( std::ostream &html_out, std::string_view s );
