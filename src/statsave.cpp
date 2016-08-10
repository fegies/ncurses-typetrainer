#include "statsave.h"

#include <fstream>
#include <algorithm>
#include <math.h>

namespace statsave
{

void savestats(float errorpercent, float keysperminute)
{
	std::fstream fs;

	fs.open("errorpercent.stat",std::fstream::in | std::fstream::out | std::fstream::app);

	if(!isnan(errorpercent))
	{
		int lcount = std::count(std::istreambuf_iterator<char>(fs),std::istreambuf_iterator<char>(),'\n');
		fs << (lcount) << " " << errorpercent << std::endl;
	}

	fs.close();

	fs.open("keysperminute.stat",std::fstream::in | std::fstream::out | std::fstream::app);

	if( !isnan(keysperminute) && keysperminute > 0.0 )
	{
		int lcount = std::count(std::istreambuf_iterator<char>(fs),std::istreambuf_iterator<char>(),'\n');
		fs << (lcount) << " " << keysperminute << std::endl;
	}

	fs.close();
}

}