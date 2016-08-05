#pragma once
#include <string>

namespace typescreen
{
	//initializes curses mode
	void init();

	//displays a selection screen and returs the selected entry
	std::string selectionscreen();

	//if filename is empty, display file selection menu
	void start(std::string filename,bool saveprog,bool savestats,bool usetree);
}