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
	void assignLeft(Word* newleft);

	//Sets values of nw to 0 to avoid their destruction
	Word& operator= (Word& nw);

	void insert(std::string& insertword);

	void insertError(std::string& correctword, std::string& variation);

	void serializetostream(std::ofstream& s);

	std::string serializetostring();

	//merges the numbers and subtree of w into the word executing
	//name and children are unaffected
	void merge(Word* w);

	std::string& getContent();

	Word* getLeft();
	Word* getRight();

	bool hasVariations();

private:

	int number;

	Word* left;
	Word* right;

	std::string* content;

	Wordtree* wrongvariants;
};