# plotter makefile

CXX      := g++
CXXFLAGS := -g -Wall

ROOTLIBS  := $(shell root-config --glibs)
ROOTFLAGS := $(shell root-config --cflags)

OBJDIR    := obj
SRCDIR    := src

_OBJ      := main.o plotter.o item.o itemsbox.o plot.o plotobj.o Dic.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

_HEADER   := plotter.h itemsbox.h
HEADER = $(patsubst %,$(SRCDIR)/%,$(_HEADER))

DIC       := Dic.cxx
TARGET    := plotter

#------------------------------------------------------------------------------

all: $(TARGET)

$(TARGET): $(OBJDIR) $(DIC) $(OBJ)
	@echo "Linking $@"
	@$(CXX)  $(CXXFLAGS) $(OBJ) $(ROOTLIBS) $(ROOTFLAGS) -o $@
	@echo "$@ done."
	@echo "Now you can install it in /usr/bin using \"make install\""

$(OBJDIR)/main.o: $(SRCDIR)/main.cxx
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) $(ROOTFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cxx $(SRCDIR)/%.h
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) $(ROOTFLAGS) -c -o $@ $<

$(OBJDIR)/Dic.o: Dic.cxx
	@echo "Compiling Dic.cxx ..."
	@$(CXX) $(CXXFLAGS) -c Dic.cxx $(ROOTFLAGS) -o $(OBJDIR)/Dic.o

$(DIC): $(HEADER) $(SRCDIR)/LinkDef.h
	@echo "Generating dictionary ..."
	@rootcint -f $@ -c $(ROOTFLAGS) $^

$(OBJDIR):
	@mkdir -p $(OBJDIR)

first: all

install: first FORCE
	@test -d $(INSTALLDIR)/usr/bin/ || mkdir -p $(INSTALLDIR)/usr/bin/
	@install -m 755 -p "$(TARGET)" "$(INSTALLDIR)/usr/bin/$(TARGET)"
	@strip "$(INSTALLDIR)/usr/bin/$(TARGET)"

uninstall: FORCE
	@rm -f "$(INSTALLDIR)/usr/bin/$(TARGET)"

FORCE:

clean:
	@rm -f $(TARGET)
	@rm -f $(OBJS)
	@rm -f $(DIC) Dic.h
	@rm -rf $(OBJDIR)

.PHONY: clean install uninstall
