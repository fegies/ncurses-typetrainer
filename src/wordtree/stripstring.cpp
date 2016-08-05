#include "wordtree/stripstring.h"

#include "felocale/Intstring.hpp"
#include "felocale/encodeconvert.h"

namespace wordtree
{

void stripPunctuation(std::string& tostrip)
{
	Intstring puncts = "\"';,.?-!*„“”—";
	Intstring word = tostrip;

	bool containspunct = false;
	bool * removechar = new bool [word.length()];

	for(int a,i = 0; i < word.length(); ++i)
	{
		removechar[i] = false;
		for( a = 0; a < puncts.length(); ++a )
		{
			if(word.at(i) == puncts.at(a))
			{
				containspunct = true;
				removechar[i] = true;
				break;
			}
		}
	}

	if( containspunct )
	{
		tostrip.clear();
		char a,bytes[5];
		bytes[4] = 0;

		for(int i = 0; i < word.length(); ++i)
		{
			for( a = 0; a < 4 ; ++a )
				bytes[a] = 0;
			felocale::to_utf8(word.at(i),bytes);
			tostrip.append(bytes);
		}
	}

	delete[] removechar;
}

}