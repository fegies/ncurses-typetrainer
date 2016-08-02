#pragma once

#include "felocale/Intstring.hpp"
#include <string>

namespace felocale
{

//returns the number of bytes in utf-8 encoding.
//fills the bytes with an equvalent representation
//bytes must be an array with 4 entries
char to_utf8(int widechar, char* bytes);

//Converts a UTF-32 Intstring to a utf-8 std::string
std::string convertstring(Intstring& input);

}