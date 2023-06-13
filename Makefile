SOURCES=$(wildcard *.c)
HEADERS=$(SOURCES:.c=.h)

SOURCES_DWARVES = $(SOURCES) $(wildcard dwarves/*.c)
SOURCES_TAVERNS = $(SOURCES) $(wildcard taverns/*.c)

HEADERS_DWARVES = $(SOURCES_DWARVES:.c=.h)
HEADERS_TAVERNS = $(SOURCES_TAVERNS:.c=.h)

NUM_DWARVES = 3
NUM_TAVERNS = 2
NUM_PORTALS = 2

# FLAGS=-DDEBUG -g
FLAGS=-g

all: main tags

main: $(SOURCES_DWARVES) $(HEADERS_DWARVES) $(SOURCES_TAVERNS) $(HEADERS_TAVERNS) Makefile
	mpicc -Wall $(SOURCES_DWARVES) $(FLAGS) -DNUM_DWARVES=${NUM_DWARVES} -DNUM_TAVERNS=$(NUM_TAVERNS) -DNUM_PORTALS=$(NUM_PORTALS) -o dwarves.out
	mpicc -Wall $(SOURCES_TAVERNS) $(FLAGS) -DNUM_DWARVES=${NUM_DWARVES} -DNUM_TAVERNS=$(NUM_TAVERNS) -DNUM_PORTALS=$(NUM_PORTALS) -o taverns.out

clear: clean

clean:
	rm dwarves.out taverns.out

tags: $(SOURCES_DWARVES) $(HEADERS_DWARVES) $(SOURCES_TAVERNS) $(HEADERS_TAVERNS)
	ctags -R .

run: main Makefile tags
	mpirun -np $(NUM_DWARVES) ./dwarves.out : -np $(NUM_TAVERNS) ./taverns.out
