#include "typescreen.h"
#include <iostream>
#include <ncurses.h>
#include <locale.h>
#include <chrono>
#include <vector>
#include "Textstream.hpp"
#include "felocale/Intstring.hpp"
#include "statsave.h"
#include "dirlist.h"

namespace typescreen
{

void init()
{
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
	while(true)
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
}

void start(std::string filename, bool saveprog, bool savestats)
{
	Textstream ts;
	ts.open_file(filename);


	//Variables
	bool timerunning = false;
	bool progrunning = true;


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

	int yval = 0;

	//Lambdas
	auto add_widech = [&](int c){

		//ASCII, no multibyte trickery required
		if(c <= 0x7f)
			addch(c);
		//convert Utf32 back to utf8
		else
		{
			char bytes [4];
			for(char i=2;i < 4; ++i)
				bytes[i] = 0;

			//handles the encoding of surrogate bytes
			auto encodebytes = [&](char num){
				for(;num > 0;--num){
					bytes[num] = 0x80;
					bytes[num] |= (c & 0x3f);
					c >>= 6;
				}
			};

			//two byte encoding
			if(c <= 0x7ff)
			{
				encodebytes(1);
				bytes[0] = 0xc0;
				bytes[0] |= (c & 0x1f);
			}
			//three byte encoding
			else if(c <= 0xffff)
			{
				encodebytes(2);
				bytes[0] = 0xe0;
				bytes[0] |= (c & 0xf);
			}
			//four byte encoding
			else{
				encodebytes(3);
				bytes[0] = 0xf0;
				bytes[0] |= (c & 0x4);
			}
			printw(bytes);
		}
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
	};

	auto doscreen = [&](){
		printstatus();
		printline();
		refresh();
	};

	auto comparechar = [&](int c){
		if(tprog == line.length())
			return;

		if(c == line.at(tprog))
			correctness[tprog] = true;
		else{
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

	nextline();
	doscreen();
	int c = get_widech();
	timerunning = true;
	starttime = std::chrono::high_resolution_clock::now();

	while(progrunning)
	{
		++keystrokes;
		switch(c)
		{
			case 27:
				progrunning=false;
				break;
			case 127:
			case 8:
			case KEY_DC:
			case KEY_BACKSPACE:
				if(tprog > 0)
					--tprog;
				break;
			case '\n':
			case KEY_ENTER:
				if(tprog == line.length())
					nextline();
				break;
			default:
				comparechar(c);
		}
		doscreen();
		c = get_widech();
	}

	curs_set(1);
	endwin();

	if(saveprog)
		ts.saveposition();

	if(savestats)
		statsave::savestats(errorpercent,keysperminute);
}

}