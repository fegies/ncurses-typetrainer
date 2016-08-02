#pragma once

#include <string>
#include <fstream>

class Wordtree;
#include "wordtree/Wordtree.hpp"


/*
	A node in a wordtree
*/

class Word
{
public:
	//if insrtioncounts is false it sets number to 0 not 1
	//used when a word is mistyped that was never typed correctly before.
	Word(std::string& name,bool insertioncounts);
	~Word();

	//These should only be used in the context of tree Construction from a file
	Word(Word* newleft, std::string& line);
	Word(Word* newleft, std::string& name, int newnum);
	void assignRight(Word * newright);

	void insert(std::string& insertword);

	void insertError(std::string& correctword, std::string& variation);

	void serializetostream(std::ofstream& s);

	std::string serializetostring();

private:

	int number;

	Word* left;
	Word* right;

	std::string* content;

	Wordtree* wrongvariants;
};