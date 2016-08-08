#include "wordtree/Wordtree.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "wordtree/stripstring.h"

template <typename t>
void swapValues(t& a1, t& a2)
{
	t tmp = a1;
	a1 = a2;
	a2 = tmp;
}

Wordtree::Wordtree()
{
	root = 0;
}

Wordtree::~Wordtree()
{
	std::function<int (Word*)> del = [&](Word* w){
		if( (w-> content) != 0 )
			delete (w-> content);
		if( (w-> variationTree) != 0 )
			delete (w -> variationTree);
		delete w;
		return 0;
	};

	bottomUpTreeWalk (root, del);
}

void Wordtree::insert(std::string& word)
{
	wordtree::stripSpaces(word);

	if( word.empty() )
		return;

	Word* w = find(word);
	if ( w == 0 )
	{
		w = new Word {new std::string(word),1,0,0,0,0};
		insertNode( w );
	}
	else
		++( w -> number );
}

void Wordtree::insertError(std::string& correctword, std::string& variation)
{
	wordtree::stripSpaces(correctword);
	wordtree::stripSpaces(variation);

	if( correctword.empty() || variation.empty() )
		return;

	Word* w = find(correctword);
	if ( w == 0)
	{
		w = new Word {new std::string(correctword),0,0,0,0,0};
		insertNode(w);
	}
	if ( (w -> variationTree) == 0 )
		(w -> variationTree) = new Wordtree;
	w -> variationTree-> insert(variation);
}

void Wordtree::storeinFile(std::string& filename)
{
	std::ofstream os (filename);

	std::function<int (Word*)> addLine = [&](Word* w){
		os << *(w -> content) << " " << w -> number;
		if( (w-> variationTree) != 0)
			os << w -> variationTree -> serializetostring();
		os << std::endl;
		return 0;
	};

	serialTreeWalk(root,addLine);

	os.flush();
}

void Wordtree::restorefromFile(std::string& filename)
{
	std::ifstream infile(filename);
	if( !infile.is_open())
		return;

	int count = std::count(std::istreambuf_iterator<char>(infile),std::istreambuf_iterator<char>(),'\n');

	infile.seekg(0,std::ios_base::beg);

	std::function<Word* ()> nextMainTreeNode = [&](){
		Word* w = new Word{0,0,0,0,0,0};

		std::string line;
		std::getline(infile,line);
		std::stringstream ss(line);

		w -> content = new std::string;
		ss >> *(w -> content);
		ss >> (w -> number);

		if(!ss.eof())
		{
			int spos = ss.tellg();
			int count = std::count(std::istreambuf_iterator<char>(ss),std::istreambuf_iterator<char>(),' ');
			ss.seekg(spos);

			std::function<Word* ()> nextSubtreeNode = [&](){
				Word* w = new Word{0,0,0,0,0,0};
				w -> content = new std::string;
				ss >> *(w -> content);
				ss >> (w -> number);
				return w;
			};

			Wordtree* subtree = new Wordtree;
			subtree -> root = buildtree(0,(count/2)-1,nextSubtreeNode);
			w -> variationTree = subtree;
		}
		return w;
	};

	root = buildtree (0,count-1,nextMainTreeNode);
}

void Wordtree::trimToErrors()
{
	std::function<int (Word*)> examineNode = [&](Word* w){
		if( (w -> variationTree) == 0 )
		{
			unlinkNode(w,true);
			return 1;
		}
		else
			return 0;
	};

	//Ignored return Value is the Number of deleted Nodes
	bottomUpTreeWalk(root,examineNode);
}

void Wordtree::trimPunctuation()
{
	Wordtree alttree;

	std::function<int (Word*)> inspectNode = [&](Word* w){

		std::string stripstring(*(w -> content));
		std::cout<<stripstring<<" : ";
		wordtree::stripPunctuation(stripstring);
		std::cout<<stripstring<<std::endl;
		int c = w -> content -> compare(stripstring);

		if (c != 0)
		{
			Word* alttreeword = alttree.find(stripstring);
			if( alttreeword == 0 )
			{
				alttreeword = new Word{new std::string(stripstring),(w -> number),0,0,0,0};
				(alttreeword -> variationTree) = (w -> variationTree);
				( w -> variationTree ) = 0;
				alttree.insertNode(alttreeword);
			}
			else
			{
				(alttreeword -> number) += (w -> number);
				if( (alttreeword -> variationTree) == 0 )
					swapValues((alttreeword-> variationTree),(w -> variationTree));
				else
				{
					alttreeword -> variationTree -> mergeTree( w -> variationTree );
					alttreeword -> variationTree -> trimPunctuation();
				}
			}
		}
		else
		{
			Word* n=new Word{new std::string(stripstring),(w -> number),0,0,0,(w->variationTree)};
			(w -> variationTree) = 0;
			alttree.insertNode(n);
		}
		return 0;
	};

	topDownTreeWalk(root,inspectNode);

	this -> ~Wordtree();
	root = 0;
	swapValues(root, alttree.root);

}

Wordtree::Word* Wordtree::find(std::string& searchword)
{
	Word* w = root;
	int c;
	while( w != 0 )
	{
		c = searchword.compare( *(w -> content) );
		if ( c == 0 )
			break;
		else if ( c < 0 )
			w = ( w -> left );
		else
			w = ( w -> right );
	}
	return w;
}

int Wordtree::size()
{
	std::function<int (Word*)> onePerNode = [&](Word* w){
		return 1;
	};

	return serialTreeWalk(root,onePerNode);
}

int Wordtree::countWords()
{
	std::function <int (Word*)> getnum = [&](Word* w){
		return ( w -> number );
	};

	return serialTreeWalk(root,getnum);
}

int Wordtree::countErrors()
{
	std::function<int (Word*)> countSubtreeWords = [&](Word* w){
		if( (w -> variationTree) == 0 )
			return 0;
		else
			return ( w -> variationTree -> countWords() );
	};

	return serialTreeWalk(root,countSubtreeWords);
}

void Wordtree::insertNode(Word* toinsert)
{
	if( root == 0 )
	{
		root = toinsert;
		return;
	}

	Word* w = root;
	int c;
	while ( w != 0 )
	{
		c = toinsert-> content-> compare( *(w-> content) );

		//this means the Word is already in the tree. It SHOULD never happen.
		if ( c == 0 )
		{
			std::cerr<<"Tree Corruption with "<<toinsert -> content<<" already in the tree";
			return;
		}
		else if ( c < 0 )
		{
			if ( (w -> left) == 0 )
			{
				w -> left = toinsert;
				toinsert -> parent = w;
				return;
			}
			else
				w = w -> left;
		}
		else
		{
			if ( (w -> right) == 0 )
			{
				w -> right = toinsert;
				toinsert -> parent = w;
				return;
			}
			else
				w = w -> right;
		}
	}
}

void Wordtree::mergeTree(Wordtree* tomerge)
{
	if ( tomerge == 0 )
		return;

	std::function<int (Word*)> inspectNode = [&](Word* w){
		Word* mtw = find(*(w -> content));
		if( mtw == 0 )
		{
			mtw = new Word{new std::string(*(w-> content)),(w-> number),0,0,0,0};
			inspectNode(mtw);
		}
		else
			(mtw -> number) += (w -> number);

		return 0;
	};

	topDownTreeWalk(root, inspectNode);
}

int Wordtree::serialTreeWalk(Word* w, std::function<int (Word*)> action)
{
	if( w == 0 )
		return 0;

	int count = 0;

	count += serialTreeWalk(w -> left, action);
	count += action(w);
	count += serialTreeWalk(w -> right, action);

	return count;
}

int Wordtree::bottomUpTreeWalk(Word* w, std::function<int (Word*)> action)
{
	if( w == 0 )
		return 0;

	int count = 0;

	count += bottomUpTreeWalk(w -> left, action);
	count += bottomUpTreeWalk(w -> right, action);
	count += action(w);

	return count;
}

int Wordtree::topDownTreeWalk(Word* w, std::function<int (Word*)> action)
{
	if( w == 0 )
		return 0;

	int count = 0;

	count += action(w);
	count += topDownTreeWalk(w -> left, action);
	count += topDownTreeWalk(w -> right, action);

	return count;
}

Wordtree::Word* Wordtree::buildtree(int left, int right, std::function<Word* ()> nextNode)
{
	if (left > right)
		return (Word*) 0;

	int mid = left + ((right -left) / 2);

	Word* leftchild = buildtree(left, mid-1,nextNode);

	Word* node = nextNode();

	(node -> left) = leftchild;
	(node -> right) = buildtree(mid+1, right,nextNode);

	if( (node -> left)  != 0 )
		(node -> left  -> parent) = node;
	if( (node -> right) != 0 )
		(node -> right -> parent) = node;

	return node;
}

void Wordtree::unlinkNode(Word*& victim, bool deleteNode)
{
	if(victim == 0)
		return;

	std::cout<<"unlinking: "<<*(victim -> content)<<((deleteNode)?" kill" : " live")<<std::endl;

	//changes the reference from the victims parent to the new child
	//changes nothing else
	auto parentRelink = [&](Word* victim, Word* newchild){
		if( (victim -> parent) == 0 )
		{
			(newchild -> parent) = 0;
			root = newchild;
		}
		else if( (victim -> parent -> left) == victim)
			(victim -> parent -> left) = newchild;
		else
			(victim -> parent -> right) = newchild;
	};

	//The node can be unlinked directly.
	if( (victim -> left) == 0 || (victim -> right) == 0 )
	{
		//Not neccessarily nonempty
		Word* nonemptychild;

		if ( (victim -> left) == 0)
			nonemptychild = (victim -> right);
		else
			nonemptychild = (victim -> left);

		if( nonemptychild != 0 )
			(nonemptychild -> parent) = (victim -> parent);

		parentRelink(victim, nonemptychild);
	}
	//the node has two children and may not be unlinked directly.
	else
	{
		//Find a node with at least a single free child that can replace the victim
		Word* replacement = (victim -> right);
		while ( (replacement -> left) != 0 )
			replacement = (replacement -> left);

		//swap the Tree references of victim and replacement
		//(replace victim with replacament)
		swapValues( (replacement-> content),(victim -> content));
		swapValues( (replacement-> variationTree),(victim -> variationTree));
		swapValues( (replacement-> number),(victim-> number));

		//unlink the victim again (it moved down the tree, has a new position)
		unlinkNode(replacement,false);
		victim = replacement;
	}

	if(deleteNode)
	{
		if( (victim -> content) != 0 )
			delete (victim -> content);
		if( (victim -> variationTree) != 0 )
			delete (victim -> variationTree);
		delete victim;
	}
	//if victim is to be used in another Tree the children must be set to 0
	else
	{
		(victim -> left)  = 0;
		(victim -> right) = 0;
	}
}

std::string Wordtree::serializetostring()
{
	std::string s="";

	std::function<int (Word*)> addtoString = [&](Word* w)
	{
		s.append( " " );
		s.append( *(w -> content) );
		s.append( " ");
		s.append( std::to_string( w -> number ) );
		return 0;
	};

	serialTreeWalk(root, addtoString);

	return s;
}