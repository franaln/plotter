# plotter makefile
# July 2013 - frani

CXX      := g++
CXXFLAGS := -g -Wall

ROOTLIBS  := $(shell root-config --libs)
ROOTFLAGS := $(shell root-config --cflags) 

OBJDIR    := obj
SRCDIR    := src

OBJS      := $(OBJDIR)/main.o $(OBJDIR)/plotter.o $(OBJDIR)/item.o $(OBJDIR)/filebox.o $(OBJDIR)/Dic.o $(OBJDIR)/macro.o 
HEADERS   := $(SRCDIR)/plotter.h $(SRCDIR)/filebox.h
DIC       := Dic.cxx
TARGET    := plotter

#------------------------------------------------------------------------------

all: $(TARGET)

$(TARGET): $(OBJDIR) $(DIC) $(OBJS) 
	@echo "Linking $@"
	@$(CXX)  $(CXXFLAGS) $(OBJS) $(ROOTLIBS) $(ROOTFLAGS) -lGui -o $@
	@echo "$@ done."
	@echo "Now you can install it in /usr/bin using \"make install\""

$(OBJDIR)/main.o: $(SRCDIR)/main.cxx
	@echo "Compiling main.cxx ..."
	@$(CXX) $(CXXFLAGS) -c $(SRCDIR)/main.cxx $(ROOTFLAGS) -o $(OBJDIR)/main.o

$(OBJDIR)/plotter.o: $(SRCDIR)/plotter.cxx $(SRCDIR)/plotter.h
	@echo "Compiling plotter.cxx ..."
	@$(CXX) $(CXXFLAGS) -c $(SRCDIR)/plotter.cxx $(ROOTFLAGS) -o $(OBJDIR)/plotter.o

$(OBJDIR)/item.o: $(SRCDIR)/item.cxx $(SRCDIR)/item.h
	@echo "Compiling item.cxx ..."
	@$(CXX) $(CXXFLAGS) -c $(SRCDIR)/item.cxx $(ROOTFLAGS) -o $(OBJDIR)/item.o

$(OBJDIR)/filebox.o: $(SRCDIR)/filebox.cxx $(SRCDIR)/filebox.h
	@echo "Compiling filebox.cxx ..."
	@$(CXX) $(CXXFLAGS) -c $(SRCDIR)/filebox.cxx $(ROOTFLAGS) -o $(OBJDIR)/filebox.o

$(OBJDIR)/macro.o:  $(SRCDIR)/macro.cxx $(SRCDIR)/macro.h
	@echo "Compiling macro.cxx ..."
	@$(CXX) $(CXXFLAGS) -c $(SRCDIR)/macro.cxx $(ROOTFLAGS) -o $(OBJDIR)/macro.o

$(OBJDIR)/Dic.o:  Dic.cxx
	@echo "Compiling Dic.cxx ..."
	@$(CXX) $(CXXFLAGS) -c Dic.cxx $(ROOTFLAGS) -o $(OBJDIR)/Dic.o

$(DIC): $(HEADERS) $(SRCDIR)/LinkDef.h
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
