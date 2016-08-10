#include "wordtree/analysis.h"

#include <fstream>
#include <iostream>

#include "felocale/Intstring.hpp"
#include "felocale/encodeconvert.h"

namespace wordtree
{

void doAllAnalyses()
{
	std::string gnuplotdatafilename = "letterMistypes.stat";
	characterAnalysisToGnuplotFile(gnuplotdatafilename);
}

Wordtree* characterAnalysis()
{
	Wordtree mainTree;
	std::string tfname = "wordstat.stat";
	std::cout<<"Loading Tree..."<<std::endl;
	mainTree.restorefromFile(tfname);
	std::cout<<"Trimming to Errors..."<<std::endl;
	mainTree.trimToErrors();
	std::cout<<"Analyzing Errors... ";

	Wordtree* chartree = new Wordtree;
	Intstring mainWordString,subWordString;

	std::vector<Wordtree::Word*> subWordList,mainWordList = mainTree.toList();

	int subListSize,mainListSize = mainWordList.size();

	Wordtree::Word* currentMainWord;
	Wordtree::Word* currentSubWord;

	int smallerLength;

	char bytes[5];
	bytes[4] = 0;
	std::string sdtstr1,stdstr2;

	for(int mc,sc,i,b,a = 0; a < mainListSize; ++a )
	{
		currentMainWord = mainWordList[a];
		mainWordString = *(currentMainWord -> content);
		subWordList = currentMainWord -> variationTree -> toList();
		subListSize = subWordList.size();

		for( b = 0 ; b < subListSize; ++b )
		{
			currentSubWord = subWordList[b];
			subWordString = *(currentSubWord -> content);

			smallerLength = mainWordString.length();
			if( subWordString.length() < smallerLength)
				smallerLength = subWordString.length();

			for( i = 0 ; i < smallerLength; ++i )
			{
				mc = mainWordString.at(i);
				sc = subWordString.at(i);
				if( mc != sc )
				{
					felocale::to_utf8(mc,bytes);
					sdtstr1 = bytes;
					felocale::to_utf8(sc,bytes);
					stdstr2 = bytes;
					chartree -> insertError(sdtstr1,stdstr2);
				}
			}
		}
	}

	return chartree;
}

void characterAnalysisToGnuplotFile(std::string& filename)
{
	Wordtree* wt = characterAnalysis();

	long double totalerr = (double)(wt -> countErrors());
	std::cout<<totalerr<<" Total differences"<<std::endl;

	std::cout<<"Converting to Plottable format..."<<std::endl;
	std::vector<Wordtree::Word*> subWordList,mainWordList = wt -> toList();

	int subSize,mainSize = mainWordList.size();
	Wordtree::Word* mw;
	Wordtree::Word* sw;

	long double share;

	std::ofstream subfiles;

	std::string sfname;

	std::string command = "mkdir ";
	command += "\"";
	command += filename;
	command += "\"";
	system(command.c_str());
	command = filename;
	command += "/";
	command += filename;

	std::ofstream outf(command);

	for(int b,a = 0; a < mainSize; ++a)
	{
		mw = mainWordList[a];
		subWordList = mw -> variationTree -> toList();
		subSize = subWordList.size();

		
		long double letterErrors = (long double)(mw -> variationTree -> countWords());

		share = letterErrors / totalerr * 100;
		outf << a << " " << *(mw -> content) << " " << std::to_string(share) << std::endl;

		sfname = filename+"/"+*(mw -> content)+".stat";
		subfiles.open(sfname);

		for( b = 0; b < subSize; ++b )
		{
			sw = subWordList[b];
			share = (sw -> number) / letterErrors * 100;
			subfiles << b << " " << *(sw -> content) << " " << std::to_string(share) << std::endl;
		}

		subfiles.close();
	}
	delete wt;
	outf.flush();
}

}