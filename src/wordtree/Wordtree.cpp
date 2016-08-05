#include "wordtree/Wordtree.hpp"
#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>
#include "wordtree/stripstring.h"

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
	//std::cerr<<"Inserting word: "<<word<<std::endl;
	if( root == 0 )
		root = new Word(word, true);
	else
		root -> insert(word);
}

void Wordtree::insertError(std::string& correctword, std::string& variation)
{
	//std::cerr<<"Inserting error: "<<variation<<" of word: "<<correctword<<std::endl;
	if( root == 0 )
		root = new Word(correctword,false);
	root -> insertError(correctword,variation);
}

void Wordtree::storeinFile(std::string& filename)
{
	std::ofstream outfile(filename);

	//std::cerr<<"Writing Wordtree to file: "<<filename<<std::endl;

	if(root != 0)
		root -> serializetostream(outfile);

	outfile.flush();

	//std::cerr<<"Finished writing Wordtree."<<std::endl;
}

void Wordtree::restorefromFile(std::string& filename)
{
	std::ifstream infile(filename);

	if(!infile.is_open())
		return;

	int lcount = std::count(std::istreambuf_iterator<char>(infile),std::istreambuf_iterator<char>(),'\n');

	//std::cerr<<"loading Wordtree"<<std::endl<<"Wordstatistics file line count is: "<<lcount<<std::endl;

	infile.seekg(0,std::ios_base::beg);

	std::string line;

	//Bottom up Tree building lambda
	std::function<Word* (int,int)> buildtree = [&](int left,int right){

		if (left > right)
			return (Word*)0;

		int mid = left + ((right -left) / 2);

		Word * leftchild = buildtree(left, mid-1);

		std::getline(infile,line);
		Word * node = new Word(leftchild,line);

		node -> assignRight(buildtree(mid+1,right));

		return node;
	};

	root = buildtree(0, lcount-1);

	//std::cerr<<"finished loading Wordtree"<<std::endl;
}

std::string Wordtree::serializetostring()
{
	if (root != 0)
		return root -> serializetostring();
	else
		return "";
}

void Wordtree::trimToErrors()
{
	//returns true if the Node shall be deleted
	//bottom up approach
	std::function<bool (Word*)> traverseTree = [&](Word* w){
		if( w == 0 )
			return false;

		Word* child = (w -> getLeft());
		if( traverseTree(child) )
			deleteNode(child,w);
		child = (w -> getRight());
		if( traverseTree(child) )
			deleteNode(child,w);

		return !(w -> hasVariations());
	};

	if(traverseTree(root))
		deleteNode(root,0);
}

Wordtree* Wordtree::trimPunctuation()
{
	Wordtree* newt = new Wordtree;

	std::function<void (Word*)> traverseTree = [&](Word* w){
		if( w == 0 )
			return;

		std::string strippedword = w -> getContent();
		wordtree::stripPunctuation(strippedword);
		Word* ntw = (newt -> find(strippedword));
		if( ntw == 0 )
		{
			Wordtree* vart = w -> getVariationTree();
			if (vart != 0)
				vart = (vart -> trimPunctuation());
			ntw = new Word(strippedword, vart, w -> getNumber());
			newt -> minsert(ntw);
		}
		else
		{
			ntw -> merge( w );
		}
		traverseTree( w -> getLeft() );
		traverseTree( w -> getRight() );
	};

	traverseTree(root);

	return newt;
}

Word* Wordtree::find(std::string& fword)
{
	int c;
	Word* w = root;
	while (w != 0)
	{
		c = fword.compare((w -> getContent()));
		if(c == 0)
			break;
		else if( c < 0 )
			w = (w -> getLeft());
		else
			w = (w -> getRight());
	}
	return w;
}

int Wordtree::size()
{
	std::function<int (Word*)> subtreeSize = [&](Word* w){
		if( w == 0 )
			return 0;
		int count = 1;
		count += subtreeSize(w -> getLeft());
		count += subtreeSize(w -> getRight());
		return count;
	};

	return subtreeSize(root);
}

Wordtree* Wordtree::copy()
{
	Wordtree* newt = new Wordtree;
	std::function<void (Word*)> traverseTree = [&](Word* w){
		if( w == 0 )
			return;
		traverseTree( ( w-> getLeft()) );
		traverseTree( ( w-> getRight()) );
		newt -> minsert( new Word(*w) );
	};
}

void Wordtree::merge(Wordtree* tomerge)
{
	if(tomerge == 0)
		return;

	std::function<void (Word*)> traverseForeignTree = [&](Word* w){
		if( w == 0 )
			return;

		traverseForeignTree(w -> getLeft());
		traverseForeignTree(w -> getRight());

		Word* p = find( w -> getContent() );
		if( p == 0 )
		{
			if( (w -> getLeft()) != 0)
				delete ( w -> getLeft());
			w -> assignLeft(0);

			if( (w -> getRight()) != 0)
				delete (w -> getRight());
			w -> assignRight(0);
			minsert( w );
		}
		else
			p -> merge(w);
	};

	traverseForeignTree((tomerge -> root));
}

void Wordtree::deleteNode(Word* w, Word* parent)
{
	if( w == 0 )
		return;

	if((w -> getLeft()) == 0)
	{
		if(parent == 0)
			root = (w -> getRight());
		else if((parent -> getLeft()) == w)
			parent -> assignLeft((w -> getRight()));
		else if((parent -> getRight()) == w)
			parent -> assignRight((w -> getRight()));
	}
	else if((w -> getRight()) == 0)
	{
		if(parent == 0)
			root = (w -> getLeft());
		else if((parent -> getLeft()) == w)
			parent -> assignLeft((w -> getLeft()));
		else if((parent -> getRight()) == w)
			parent -> assignRight((w -> getLeft()));
	}
	else{
		parent = w;
		Word* p = (w -> getRight());
		while ((p -> getLeft()) != 0)
		{
			parent = p;
			p = (p -> getLeft());
		}
		*w = *p;
		deleteNode(p,parent);
		return;
	}

	//We don't want the deconstructor to remove the subtrees
	w -> assignLeft(0);
	w -> assignRight(0);
	delete w;
}

void Wordtree::minsert(Word* toinsert)
{
	if( root == 0 )
	{
		root = toinsert;
		return;
	}

	int c;
	Word* wp = root;
	while(wp != 0)
	{
		c = (toinsert -> getContent()).compare((wp-> getContent()));
		if( c == 0 )
		{
			return;
			std::cerr<<"Tree Corruption with "<<toinsert->getContent()<<" already in the tree:"<<std::endl;
			exit(1);
		}
		else if( c < 0 )
		{
			if( (wp -> getLeft()) == 0 )
				wp -> assignLeft(toinsert);
			else
				wp = (wp -> getLeft());
		}
		else{
			if( (wp -> getRight()) == 0 )
				wp -> assignRight(toinsert);
			else
				wp = (wp -> getRight());
		}
	}
}