#include "wordtree/Wordtree.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

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

	serialTreeWalk (root, del);
}

void Wordtree::insert(std::string& word)
{
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
	Word* w = find(correctword);
	if ( w == 0)
	{
		w = new Word {new std::string(correctword),0,0,0,0,0};
		insertNode(w);
	}
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

int Wordtree::serialTreeWalk(Word* w, std::function<int (Word*)> action)
{
	if( w == 0 )
		return 0;

	int count = 0;

	count += serialTreeWalk(w -> left, action);
	count += serialTreeWalk(w -> right, action);
	count += action(w);

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

	if( (node -> left) != 0 )
		node -> left -> parent = node;
	if( (node -> right) != 0 )
		node -> right -> parent = node;

	return node;
}

void Wordtree::unlinkNode(Word* victim, bool deleteNode)
{
	if(victim == 0)
		return;

	if( (victim -> left) == 0 )
	{
		if( (victim -> parent) == 0 )
			root = (victim -> right);
		else if( (victim -> parent -> left) == victim )
			(victim -> parent -> left)  = (victim -> right);
		else
			(victim -> parent -> right) = (victim -> right);
	}
	else if( (victim -> right) == 0 )
	{
		if ( (victim -> parent) == 0 )
			root = (victim -> left);
		else if( (victim -> parent -> left) == victim )
			(victim -> parent -> left)  = (victim -> left);
		else 
			(victim -> parent -> right) = (victim -> left);
	}
	else{
		Word* w = victim -> right;
		while( (w -> left) != 0 )
			w = (w -> left);

		Word* tmp;
		tmp                = (w -> parent);
		(w -> parent)      = (victim -> parent);
		(victim -> parent) = tmp;

		tmp                = (w -> left);
		(w -> left)        = (victim -> left);
		(victim -> left)   = tmp;

		tmp                = (w -> right);
		(w -> right)       = (victim -> right);
		(victim -> right)  = tmp;

		unlinkNode(victim,false);
	}
	if(deleteNode)
		delete victim;

}

std::string Wordtree::serializetostring()
{
	std::string s="";

	std::function<int (Word*)> addtoString = [&](Word* w)
	{
		s.append( " " );
		s.append( *(w -> content) );
		s.append( std::to_string( w -> number ) );
		return 0;
	};

	serialTreeWalk(root, addtoString);

	return s;
}