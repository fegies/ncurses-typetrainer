#include "wordtree/gentext.h"

#include "wordtree/Wordtree.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <math.h>

namespace wordtree
{

void gentext(std::string& filename)
{
	int wordsperline = 10;
	int lengthGoal = 100;

	std::string tfname("wordstat.stat");
	std::string ofname1("cuttree1.ttree");
	std::string ofname2("cuttree2.ttree");
	Wordtree wt;
	wt.restorefromFile(tfname);
	wt.trimToErrors();
	wt.storeinFile(ofname1);
	std::cout<<std::endl<<"Finished Trimming Errors"<<std::endl<<std::endl;
	wt.trimPunctuation();
	wt.storeinFile(ofname2);
	std::cout<<std::endl<<"Finished Trimming Punctuation"<<std::endl<<std::endl;

	int errorcount = wt.countErrors();
	std::cout<<errorcount<<" Total Errors in the Tree. "<<std::endl;

	std::vector<Wordtree::Word*> words = wt.toList();

	std::vector<std::string*> text;

	Wordtree::Word* w;
	unsigned int s = words.size();
	double share = 0;
	int num = 0;
	for(unsigned int b,a = 0; a < s; ++a)
	{
		w = words[a];
		share = (double)(w -> variationTree -> countWords()) / (double)errorcount;
		num = round( share * lengthGoal );
		std::cout<<*(w -> content)<<" "<<num<<std::endl;
		for( b = 0; b < num; ++b )
			text.push_back( (w -> content) );
	}

	std::random_shuffle(text.begin(), text.end());
	std::cout<<"Text length: "<<text.size()<<" Words"<<std::endl;

	std::ofstream fout(filename);

	s = text.size();
	int linePos = -1;
	for(int a = 0; a < s; ++a)
	{
		if(++linePos == wordsperline)
		{
			linePos = 0;
			fout << std::endl;
		}
		fout << " " << *text[a];
	}

	fout.close();
	std::string pfilename(filename);
	pfilename += ".prog";
	fout.open(pfilename);
	fout << 0;
	fout.close();

}

}