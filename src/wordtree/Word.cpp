#include "wordtree/Word.hpp"

#include <sstream>
#include <functional>
#include <algorithm>

Word::Word(std::string& newstring, bool insertioncounts)
{
	number = (insertioncounts)? 1 : 0;
	left = 0;
	right = 0;
	content = new std::string(newstring);
	wrongvariants = 0;
}

Word::~Word()
{
	if(left != 0)
		delete left;
	if(right != 0)
		delete right;
	if(wrongvariants != 0)
		delete wrongvariants;
}

Word::Word(Word* newleft, std::string& line)
{
	left = newleft;
	std::stringstream ss(line);
	ss >> *content;
	ss >> number;
	
	//the word contains mispelled components
	if(!ss.eof())
	{
		int spos = ss.tellg();

		int tcount = std::count(std::istreambuf_iterator<char>(ss),std::istreambuf_iterator<char>(),' ');

		ss.seekg(spos);

		int newnum;
		std::string newname;

		std::function<Word* (int,int)> buildtree = [&](int l, int r){
			if(l > r)
				return (Word*)0;

			int mid = l + (r - l) / 2;
	
			Word * lc = buildtree(l, mid-1);
	
			ss >> newname;
			ss >> newnum;
			Word * node = new Word(lc, newname, newnum);
	
			node -> assignRight(buildtree(mid+1,r));
	
			return node;
		};

		wrongvariants = new Wordtree(buildtree(0,(tcount/2)-1));
	}
}

Word::Word(Word* newleft, std::string& name, int newnum)
{
	number = newnum;
	left = newleft;
	wrongvariants = 0;
	content = new std::string(name);
}

void Word::assignRight(Word* newright)
{
	right = newright;
}

void Word::insert(std::string& insertword)
{
	int i = insertword.compare(*content);
	if(i == 0)
		++number;
	else if(i < 0)
	{
		if (left == 0)
			left = new Word(insertword,true);
		else
			left -> insert(insertword);
	}
	else{
		if (right == 0)
			right = new Word(insertword,true);
		else
			right -> insert(insertword);
	}
}

void Word::insertError(std::string& correctword, std::string& variation)
{
	int i = correctword.compare(*content);
	if(i == 0)
	{
		if(wrongvariants == 0)
		{
			wrongvariants = new Wordtree();
		}
		wrongvariants->insert(variation);
	}
	else if(i < 0)
	{
		if (left == 0)
			left = new Word(correctword,false);
		left->insertError(correctword,variation);
	}
	else
	{
		if (right == 0)
			right = new Word(correctword,false);
		right->insertError(correctword,variation);
	}
}

void Word::serializetostream(std::ofstream& s)
{
	if(left != 0)
		left -> serializetostream(s);

	s << *content << " " << number;
	if(wrongvariants != 0)
		s << " " << wrongvariants -> serializetostring();
	s << std::endl;

	if(right != 0)
		right -> serializetostream(s);
}

std::string Word::serializetostring()
{
	std::string ss;
	if(left != 0)
		ss = left -> serializetostring();

	if(!ss.empty())
		ss.append(" ");

	ss.append(*content + " " + std::to_string(number));

	if(right != 0)
		ss.append(right -> serializetostring());

	return ss;
}