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

	//Intended for the Variations Subtree
	std::string serializetostring();

	//removes all words without variations from the tree
	void trimToErrors();

	//merges all Words that differ only by Punctuation
	//returns a NEW Wordtree
	Wordtree* trimPunctuation();

	Word* find(std::string& word);

	//returns the number of entries in the tree
	int size();

	//incorporates the Wordnumbers saved in the tree
	//if IncludeErrors is set, also adds the number of Words in the variations Subtree
	//could be smallet than size() if a lot of words have the number 0
	int countWords(bool includeErrors);

	Wordtree* copy();

	//merges tree into the tree executing the method
	void merge(Wordtree* tree);

private:
	void minsert(Word* toinsert);

	//if parent = 0 it is a root deletion
	void deleteNode(Word* victim, Word* parent);


	Word* root;
};