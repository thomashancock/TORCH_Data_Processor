CC = $(CXX)
CFLAGS = -g3 -O3

WARNINGFLAGS = -Wall -Wpedantic -Wextra -Wmissing-include-dirs -Wshadow

# OSX ROOT Library Linking
ROOTINC = `root-config --cflags`
ROOTLIB = `root-config --libs`

GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)
CFLAGS += -DGITVERSION=\"$(GIT_VERSION)\"

ODIR = obj
SDIR = src
INC = -Iinc

# List Sources Here
SOURCES = main.cpp
SOURCES += GlobalConfig.cpp
SOURCES += Packet.cpp
SOURCES += Event.cpp
SOURCES += Edge.cpp
# SOURCES += RootManager.cpp

_OBJS = $(SOURCES:.cpp=.o)
OBJS = $(patsubst %, $(ODIR)/%, $(_OBJS))
# patsubst take _OBJS and appends the ODIR directory to the front of each object file

OUT = TORCH_Data_Processor

all: $(OUT)

debug: CFLAGS += -DDEBUG
debug: $(OUT)

$(OUT): $(OBJS)
	$(CC) -o $(OUT) $^ $(ROOTLIB)
# $^ = The names of all prerequisites with spaces between them

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) -c $(INC) $(ROOTINC) -o $@ $< $(CFLAGS) $(WARNINGFLAGS)
# $@ = The file name of the target of the rule.
# $< = The name of the first prerequisite

.PHONY : clean
clean:
	rm -f $(ODIR)/*.o $(OUT)
