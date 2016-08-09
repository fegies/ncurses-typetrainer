#include <string>
#include <iostream>
#include "typescreen.h"
#include "wordtree/gentext.h"

void printhelp()
{
	using namespace std;
	cout << "A ncurses based touchtyping trainer"<<endl;
	cout << "Accepted arguments:"<<endl<<endl;
	cout << "-? -h     : print this message"<<endl;
	cout << "-f [file] : Skip the text selection dialog and and load the specified file instead"<<endl;
	cout << "-g [file] : generate a custom pactice text and save it in [file]"<<endl;
	cout << "--s       : do not save statistics about errors in percent and keystrokes per minute"<<endl;
	cout << "--p       : do not load and save typing progress"<<endl;
	cout << "--t       : do not use the errorstatistics tree"<<endl;
}

int main(int argc, char** argv)
{

	std::string filename = "";

	bool saveprog = true;
	bool savestats = true;
	bool usetree = true;

	bool gentextmode = false;

	for(int i = 1; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case '?':
				case 'h':
					printhelp();
					return 0;
				case 'g':
					gentextmode = true;
				case 'f':
					if(i < argc-1)
						filename = argv[++i];
					break;
				case '-':
					switch(argv[i][2])
					{
						case 's':
							savestats = false;
							break;
						case 'p':
							saveprog = false;
							break;
						case 't':
							usetree =false;
							break;
					}
					break;
			}
		}
	}

	if(gentextmode)
	{
		if(filename.empty())
			filename = "texts/customtext.txt";
		wordtree::gentext(filename);
		return 0;
	}

	typescreen::init();

	if(filename.empty())
		filename = typescreen::selectionscreen();

	if(filename == "texts/Generate a Text based on the Error Tree" )
	{
		filename = "texts/.customtext.txt";
		wordtree::gentext(filename);
	}

	typescreen::start(filename,saveprog,savestats,usetree);
	return 0;
}