#pragma once

class Word;
#include "wordtree/Word.hpp"

/*
	A binary Search Tree containing all words in a text and the number of times they were typed correctly,
	the errors typed for a particular word and their numbers
*/

class Wordtree
{
public:

	//true if it is the Tree for the correct words, false otherwise.
	Wordtree();
	Wordtree(Word* root);
	~Wordtree();

	void insert(std::string& word);

	void insertError(std::string& correctword, std::string& variation);

	void storeinFile(std::string& filename);

	void restorefromFile(std::string& filename);

	std::string serializetostring();

private:
	Word* root;
};