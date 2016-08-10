#pragma once

#include <string>
#include <functional>
#include <vector>

/*
	A binary Search Tree containing all words in a text and the number of times they were typed correctly,
	the errors typed for a particular word and their numbers
*/

class Wordtree
{
public:

	struct Word
	{
		std::string* content;
		int number;
		Word* parent;
		Word* left;
		Word* right;
		Wordtree* variationTree;
	};

	//true if it is the Tree for the correct words, false otherwise.
	Wordtree();
	~Wordtree();

	void insert(std::string& word);
	void insertError(std::string& correctword, std::string& variation);

	void storeinFile(std::string& filename);

	void restorefromFile(std::string& filename);

	//removes all words without variations from the tree
	void trimToErrors();

	//merges all Words that differ only by Punctuation
	//returns a NEW Wordtree
	void trimPunctuation();

	Word* find(std::string& word);

	//returns the number of entries in the tree
	int size();

	//incorporates the Wordnumbers saved in the tree
	//Returns the sum of the number entries of the entire Tree
	//Ignores Subtree Values
	int countWords();

	//returns the Sum of countWords() of all subtrees
	int countErrors();

	//returns a vector sorted by the number of errors in descending order
	std::vector<Word*> toList();

private:

	Word* root;

	void insertNode(Word* toinsert);

	//does not unlink, only copy.
	//does not copy Subtrees
	void mergeTree(Wordtree* tomerge);

	//takes a function to execute for every Word in the tree
	//Descends into the first subtree, executes the action and then descends into
	//the second subtree
	int serialTreeWalk(Word* w,std::function<int (Word*)> action);
	//Descends into both Subtrees before executing the action
	int bottomUpTreeWalk(Word* w, std::function<int (Word*)> action);
	//executes the action before descending into the subtree
	int topDownTreeWalk(Word* w, std::function<int (Word*)> action);

	//rebuild the Wordtree
	Word* buildtree(int left,int right,std::function<Word* ()> nextNode);

	//if parent = 0 it is a root deletion
	//if delete is true, Word victim is deleted. Otherwise it is only unlinked and can be
	//used in another Tree.
	void unlinkNode(Word*& victim,bool deleteNode);

	//Intended for the Variations Subtree
	std::string serializetostring();

};