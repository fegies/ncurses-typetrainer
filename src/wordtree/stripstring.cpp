#include "wordtree/stripstring.h"

#include "felocale/Intstring.hpp"
#include "felocale/encodeconvert.h"

namespace wordtree
{

void stripPunctuation(std::string& tostrip)
{
	Intstring puncts = "\"';`:,.?()~-!*„“”—";
	Intstring word = tostrip;

	bool needsstripping = false;
	bool ignorethis[word.length()];

	for(unsigned int b,a = 0 ; a < word.length(); ++a )
	{
		ignorethis[a] = false;
		for(b = 0; b < puncts.length(); ++b)
		{
			if(word.at(a) == puncts.at(b))
			{
				needsstripping = true;
				ignorethis[a] = true;
				break;
			}
		}
	}

	if(! needsstripping)
		return;

	tostrip.clear();
	char bytes[5];
	bytes[4] = 0;
	for( int i = 0; i < word.length(); ++i )
	{
		if(ignorethis[i])
			continue;
		for(unsigned char a = 0; a < 4; ++a)
			bytes[a] = 0;
		felocale::to_utf8(word.at(i),bytes);
		tostrip.append(bytes);
	}
}

void stripSpaces(std::string& tostrip)
{
	std::string t(tostrip);
	tostrip.clear();

	for(unsigned int i = 0; i < t.length(); ++i)
		if( t.at(i) != ' ' )
			tostrip.push_back(t.at(i));
}

}