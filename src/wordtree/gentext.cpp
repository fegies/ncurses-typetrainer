#include "wordtree/gentext.h"

#include "wordtree/Wordtree.hpp"

#include <iostream>

namespace wordtree
{

void gentext(std::string& filename)
{
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
}

}