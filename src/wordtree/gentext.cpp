#include "wordtree/gentext.h"

#include "wordtree/Wordtree.hpp"

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
}

}