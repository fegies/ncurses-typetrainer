#pragma once
#include <string>

/*
	Functions related to Word manipulation
*/

namespace wordtree
{

//strips all punctuation from the string.
void stripPunctuation(std::string& tostrip);

//removes spaces inside the String
void stripSpaces(std::string& tostrip);

}