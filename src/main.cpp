#include <string>
#include "typescreen.h"


int main(int argc, char** argv)
{

	std::string filename = "";

	bool saveprog = true;
	bool savestats = true;

	for(int i = 1; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
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
					}
					break;
			}
		}
	}

	typescreen::init();

	if(filename.empty())
		filename = typescreen::selectionscreen();

	typescreen::start(filename,saveprog,savestats);
	return 0;
}