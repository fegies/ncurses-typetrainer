#The program Name (Name of the resulting executable)

PROG      = typetrainer
VPATH     = src include
ODIR      = ./bin
SHAREFLAGS= -g -pipe -Wall
CFLAGS    = $(SHAREFLAGS) -std=c++11
LINKFLAGS = $(SHAREFLAGS) -lpthread -lncursesw
COMPILER  = $(CXX)

#Name of subpaths inside Odir (Must be the same in ./include and ./src as well)
SUBPATHS  = . felocale wordtree

#The Objects that are compiled
OBJS      = $(BASEOBS) $(addprefix felocale/, $(FELOCALOBS)) $(addprefix wordtree/, $(WORDTREEOBS))

BASEOBS    = main.o typescreen.o Textstream.o statsave.o dirlist.o
FELOCALOBS = Intstring.o encodeconvert.o
WORDTREEOBS= Wordtree.o gentext.o stripstring.o analysis.o

OPROG = $(addprefix $(ODIR)/, $(PROG))
RUNFLAGS =


run : all
	$(OPROG) $(RUNFLAGS)

flagless: all
	$(OPROG)

analysis: all
	$(OPROG) -a

text: all
	$(OPROG) -g customtext.txt

all : buildbin $(OPROG)

debug: all
	valgrind $(OPROG) $(RUNFLAGS) -g customtext.txt

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
