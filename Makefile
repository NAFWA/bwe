CXX=g++
CC=gcc

OBJDIR=objs
SRCDIR=src
INCDIR=$(SRCDIR)/inc

BOTHFLAGS+=-I$(INCDIR) -Wall -c
CFLAGS+=$(BOTHFLAGS)
CXXFLAGS+=$(BOTHFLAGS)
LDFLAGS+=-lm -lswc -lwld -lwayland-client -lwayland-server -lpthread -lv8

CSRCS=$(wildcard $(SRCDIR)/*.c)
COBJS=$(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(CSRCS))

CXXSRCS=$(wildcard $(SRCDIR)/*.cpp)
CXXOBJS=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CXXSRCS))

all: bwe

bwe: $(COBJS) $(CXXOBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(COBJS): | $(OBJDIR)
$(CXXOBJS): | $(OBJDIR)
$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(wildcard $(INCDIR)/*.h) Makefile
	$(CC) $(CFLAGS) $< -c -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(wildcard $(INCDIR)/*.h) Makefile
	$(CXX) $(CXXFLAGS) $< -c -o $@

run:
	swc-launch -t /dev/tty3 ~/code/swc/example/wm &> log.out
