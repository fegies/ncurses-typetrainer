#pragma once
#include <string>

namespace typescreen
{
	//initializes curses mode
	void init();

	std::string selectionscreen();

	//if filename is empty, display file selection menu
	void start(std::string filename,bool saveprog,bool savestats);
}