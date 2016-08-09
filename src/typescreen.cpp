#include "typescreen.h"
#include <iostream>
#include <ncurses.h>
#include <locale.h>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "Textstream.hpp"
#include "felocale/Intstring.hpp"
#include "felocale/encodeconvert.h"
#include "statsave.h"
#include "dirlist.h"
#include "wordtree/Wordtree.hpp"

namespace typescreen
{

volatile sig_atomic_t progrunning = 1;

void term(int sigterm)
{
	progrunning = 0;
	endwin();
	std::cout<<"Press enter to terminate..."<<std::endl;
}

void init()
{
	signal (SIGINT, term);

	setlocale(LC_ALL,"en_US.UTF.8");
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr ,true);
	start_color();

	//Default color
	init_pair(1,COLOR_WHITE,COLOR_BLACK);
	//Cursor color
	init_pair(2,COLOR_BLACK,COLOR_WHITE);
	//Wrong color
	init_pair(3,COLOR_BLACK,COLOR_RED);
	//color for once mistyped text
	init_pair(4,COLOR_RED,COLOR_BLACK);
}

std::string selectionscreen()
{

	std::vector<std::string> filenames;
	dirops::lstexts(filenames);
	filenames.push_back("Generate a Text based on the Error Tree");
	mvprintw(0,0,"Use the arrow keys and enter to select one of the following Files:");
	int selection = 0;

	auto printfiles = [&](){
		for(int y=0;y<filenames.size();++y)
		{
			if(selection == y)
				color_set(2,0);
			mvprintw(y+2,0,filenames[y].c_str());
			color_set(1,0);
			refresh();
		}
	};

	int c;
	while(progrunning)
	{
		printfiles();
		c = getch();
		switch(c)
		{
			case KEY_UP:
				if(selection>0)
					--selection;
				break;
			case KEY_DOWN:
				if(selection < filenames.size()-1)
					++selection;
				break;
			case KEY_ENTER:
			case 10:
				return "texts/"+filenames[selection];
		}
	}
	return "";
}

void start(std::string filename, bool saveprog, bool savestats,bool usetree)
{
	if(!progrunning)
		return;

	clear();

	Textstream ts;
	ts.open_file(filename);
	if(saveprog)
		ts.loadposition();

	Wordtree errortree;
	std::string treefilename="wordstat.stat";
	
	if(usetree)
		errortree.restorefromFile(treefilename);

	//Variables
	bool timerunning = false;

	int keystrokes = 0;
	int wrongstrokes = 0;
	std::chrono::high_resolution_clock::time_point starttime;
	std::chrono::high_resolution_clock::time_point curtime;
	int elapsedseconds = 0;
	float keysperminute = 0.0;
	float errorpercent = 0.0;

	Intstring line;
	int clength = 0;
	int tprog = 0;
	bool * correctness = new bool;
	bool lastwordwrong = false;
	std::string typedword;

	int yval = 0;

	std::mutex printingmutex;

	//Lambdas
	auto add_widech = [&](int c){
		char bytes[5];
		for(unsigned char a = 0; a < 5 ; ++a)
			bytes[a] = 0;

		felocale::to_utf8(c,bytes);
		printw(bytes);
	};

	auto updatestates = [&](){
		if(timerunning)
		{
			curtime = std::chrono::high_resolution_clock::now();
			elapsedseconds = std::chrono::duration_cast <std::chrono::seconds> (curtime - starttime) .count();
		}
		if(elapsedseconds > 0)
			keysperminute = (((double)keystrokes) * 60 / ((double)elapsedseconds));
		errorpercent = (float)(wrongstrokes * 100) / (float)keystrokes;
	};

	auto printstatus = [&](){
		printingmutex.lock();
		if(timerunning)
		{
			updatestates();
			move(0,0);
			clrtoeol();
			std::string s = "Seconds elapsed: "+std::to_string(elapsedseconds)+" Error Ratio:"+std::to_string(errorpercent)+'%'
			+" Keystrokes per Minutes:"+std::to_string(keysperminute);
			mvprintw(0,0,s.c_str());
		}
		else
		{
			mvprintw(0,0,"Begin Typing to start the time measurement....");
		}
		refresh();
		printingmutex.unlock();
	};

	auto nextline = [&](){
		clear();
		std::string tstring = ts.nextline();
		line = tstring;
		if(line.empty())
			timerunning = false;
		else
		{
			if(clength < line.length())
			{
				delete[] correctness;
				correctness = new bool [line.length()];
				clength = line.length();
			}
			tprog = 0;
			yval = 2;
		}
	};

	auto printline = [&](){
		printingmutex.lock();
		move(yval,0);
		int x;
		for(x = 0; x < tprog; ++x)
		{
			color_set(1,0);
			if(!correctness[x])
				color_set(3,0);
			add_widech(line.at(x));
		}
		if(x < line.length())
		{
			color_set(2,0);
			add_widech(line.at(x));
			++x;
		}
		color_set(1,0);
		for(; x < line.length();++x)
			add_widech(line.at(x));

		//Add a pilcrow at the end
		if(tprog == line.length())
			color_set(2,0);

		add_widech(0xb6);

		color_set(1,0);
		printingmutex.unlock();
	};

	auto doscreen = [&](){
		printstatus();
		printline();
		refresh();
	};

	auto feedtree = [&](int c){
		//Do the wordtree magic
		if(usetree)
		{
			if(line.at(tprog) == ' ' || tprog == line.length())
			{
				Intstring* word = line.getWordBefore(tprog);
				std::string uword = felocale::convertstring(*word);
				if(lastwordwrong)
					errortree.insertError(uword,typedword);
				else
					errortree.insert(uword);
				typedword.clear();
				lastwordwrong=false;
				delete word;
			}
			else if(c != 0){
				char bytes[5];
				for(unsigned char d = 0; d < 5; ++d)
					bytes[d] = 0;
				char count = felocale::to_utf8(c,bytes);
				for(unsigned char a = 0; a < count; ++a)
					typedword.append(bytes);
			}
		}
	};

	auto comparechar = [&](int c){
		feedtree(c);
		++keystrokes;
		if(tprog == line.length())
			return;

		if(c == line.at(tprog))
			correctness[tprog] = true;
		else{
			lastwordwrong = true;
			correctness[tprog] = false;
			++wrongstrokes;
		}

		++tprog;
	};

	auto get_widech = [&]() {

		int a = getch();

		//UNICODE char!
		if( (a & 0x80) == 0x80)
		{
			int c = 0;

			char bytecount = 0;
			char marker = 0x40;
			while ((a & marker) == marker)
			{
				marker >>= 1;
				++bytecount;
			}

			//move the bits from the first byte over
			char cm = 1;
			char mask = 0;
			while(cm != marker)
			{
				mask |= cm;
				cm <<= 1; 
			}
			c |= (a & mask);

			//move the remaining bytes in.
			//the length is cut appropiately
			for(int a = 0; a<bytecount; ++a)
			{
				c <<= 6;
				c |= (getch() & 0x3f);
			}
			a = c;
		}
		return a;
	};

	auto statusthreadloop = [&](){
		while(progrunning)
		{
			printstatus();
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	};

	std::thread statusthread (statusthreadloop);
	statusthread.detach();

	nextline();
	doscreen();

	int c = get_widech();
	timerunning = true;
	starttime = std::chrono::high_resolution_clock::now();

	while(progrunning)
	{
		switch(c)
		{
			case 27:
				progrunning=false;
				break;
			case 127:
			case 8:
			case KEY_DC:
			case KEY_BACKSPACE:
				if(!usetree && tprog > 0)
					--tprog;
				break;
			case '\n':
			case KEY_ENTER:
				if(tprog == line.length())
				{
					feedtree(0);
					nextline();
				}
				break;
			default:
				comparechar(c);
		}
		doscreen();
		c = get_widech();
	}

	curs_set(1);
	endwin();

	delete correctness;

	//std::cerr<<"Exiting gracefully..."<<std::endl;

	if(saveprog)
		ts.saveposition();

	if(savestats)
		statsave::savestats(errorpercent,keysperminute);

	if(usetree)
		errortree.storeinFile(treefilename);
}

}