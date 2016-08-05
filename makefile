#The program Name (Name of the resulting executable)

PROG      = typetrainer
VPATH     = src include
ODIR      = ./bin
SHAREFLAGS= -g -pipe -O2
CFLAGS    = $(SHAREFLAGS) -std=c++11 -Wall
LINKFLAGS = $(SHAREFLAGS) -lpthread -lncursesw
COMPILER  = g++

#Name of subpaths inside Odir (Must be the same in ./include and ./src as well)
SUBPATHS  = . felocale wordtree

#The Objects that are compiled
OBJS      = $(BASEOBS) $(addprefix felocale/, $(FELOCALOBS)) $(addprefix wordtree/, $(WORDTREEOBS))

BASEOBS    = main.o typescreen.o Textstream.o statsave.o dirlist.o
FELOCALOBS = Intstring.o encodeconvert.o
WORDTREEOBS= Wordtree.o Word.o gentext.o stripstring.o

OPROG = $(addprefix $(ODIR)/, $(PROG))
RUNFLAGS = -g customtext.txt


run : all
	$(OPROG) $(RUNFLAGS)

flagless: all
	$(OPROG)

all : buildbin $(OPROG)

debug: all
	valgrind $(OPROG) $(RUNFLAGS)

clean:
	find bin -name '*.o' -delete
	rm -f $(OPROG)

.PHONY: run all clean debug flagless

#linking
$(OPROG): $(addprefix $(ODIR)/, $(OBJS))
	$(COMPILER) -o $@ $^ $(LINKFLAGS)

#compiling
$(ODIR)/%.o : %.cpp
	$(COMPILER) $(CFLAGS) -c -o $@ $< $(addprefix -I./include/,$(SUBPATHS))

#Building the Directories if they don't exist
buildbin: | $(ODIR) $(addprefix $(ODIR)/,$(SUBPATHS))

$(ODIR):
	mkdir $(ODIR)

$(addprefix $(ODIR)/,$(SUBPATHS)):
	mkdir $@
