#include "Textstream.hpp"
#include <iostream>

Textstream::Textstream()
{
	openfile = "";
	lastposition = 0;
}

Textstream::~Textstream()
{
	in.close();
}

std::string* Textstream::listfiles(int& count)
{
	return new std::string [1];
}

void Textstream::saveposition()
{
	if(openfile.empty())
		return;
	std::string progfilename = openfile + ".prog";
	std::ofstream out(progfilename);
	if(out.is_open())
	{
		out << lastposition;
		out.flush();
		out.close();
	}
}

void Textstream::open_file(std::string filename)
{
	in = std::ifstream(filename);
	if(in.is_open())
	{
		openfile = filename;
	}
	else{
		std::cerr << "failed to open file: " << filename << std::endl;
		openfile.clear();
	}
}

std::string Textstream::nextline()
{
	if(openfile.empty())
		return "";

	lastposition = in.tellg();

	std::string line;

	while(line.empty() && !in.eof())
	{
		std::getline (in,line);
		stripstring(line);
	}

	return line;
}

bool Textstream::fileisopen()
{
	return !(openfile.empty());
}

void Textstream::loadposition()
{
	std::string progfilename = openfile + ".prog";
	std::ifstream prin(progfilename);
	if(prin.is_open())
	{
		int p;
		prin >> p;
		in.seekg(p);
	}
	prin.close();
}

void Textstream::stripstring(std::string& instring)
{
	std::string tstr = "";
	std::string allowedchars = " 1234567890xvlcwkhgfqßuiaeosnrtdyüöäpzbm,.jXVLCWKHGFQẞUIAEOSNRTDYÜÖÄPZBMJ…_[]^!<>=&ſ\\/{}*?()-:@#$|~`+\%\"';";
	for(unsigned int a,i = 0; i < instring.length(); ++i)
	{
		for(a = 0; a < allowedchars.length(); ++a )
		{
			if(instring.at(i) == allowedchars.at(a))
			{
				tstr.push_back(instring.at(i));
				break;
			}
		}
	}

	instring.clear();

	bool prespace = true;
	char c;
	for(int i=0; i<tstr.length();++i)
	{
		c = tstr.at(i);
		if(c == ' ')
		{
			if(prespace)
				continue;
			else
				prespace = true;
		}
		else
			prespace = false;

		instring.push_back(c);
	}
}