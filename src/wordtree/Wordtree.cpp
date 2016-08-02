#include "wordtree/Wordtree.hpp"
#include <algorithm>
#include <functional>
#include <fstream>

Wordtree::Wordtree()
{
	root = 0;
}

Wordtree::Wordtree(Word* newroot)
{
	root = newroot;
}

Wordtree::~Wordtree()
{
	if(root != 0)
		delete root;
}

void Wordtree::insert(std::string& word)
{
	if( root == 0 )
		root = new Word(word, true);
	else
		root -> insert(word);
}

void Wordtree::insertError(std::string& correctword, std::string& variation)
{
	if( root == 0 )
		root = new Word(correctword,false);
	root -> insertError(correctword,variation);
}

void Wordtree::storeinFile(std::string& filename)
{
	std::ofstream outfile(filename);

	if(root != 0)
		root -> serializetostream(outfile);

	outfile.flush();
}

void Wordtree::restorefromFile(std::string& filename)
{
	std::ifstream infile(filename);
	int lcount = std::count(std::istreambuf_iterator<char>(infile),std::istreambuf_iterator<char>(),'\n');

	infile.seekg(0,std::ios_base::beg);

	std::string line;

	//Bottom up Tree building lambda
	std::function<Word* (int,int)> buildtree = [&](int left,int right){

		if (left > right)
			return (Word*)0;

		int mid = left + (right -left) / 2;

		Word * leftchild = buildtree(left, mid-1);

		std::getline(infile,line);
		Word * node = new Word(leftchild,line);

		node -> assignRight(buildtree(mid+1,right));

		return node;
	};

	root = buildtree(0, lcount-1);
}

std::string Wordtree::serializetostring()
{
	if (root != 0)
		return root -> serializetostring();
	else
		return "";
}