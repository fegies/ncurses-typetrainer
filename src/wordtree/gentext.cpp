#include "wordtree/gentext.h"

#include "wordtree/Wordtree.hpp"

namespace wordtree
{

void gentext(std::string& filename)
{
	std::string tfname("wordstat.stat");
	std::string ofname1("cuttree1.ttree");
	std::string ofname2("cuttree2.ttree");
	Wordtree * wt = new Wordtree;
	wt->restorefromFile(tfname);
	wt->trimToErrors();
	wt->storeinFile(ofname1);
	Wordtree* trimmt = wt->trimPunctuation();
	trimmt -> storeinFile(ofname2);
	delete trimmt;
	delete wt;
}

}