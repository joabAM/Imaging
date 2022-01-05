#Implicit rules:
#Compiling C++ programs
#x.o:
#	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) x.cpp -o x.o

#Linking a single object file
#n:
#	$(CC) $(LDFLAGS) n.o $(LOADLIBES) $(LDLIBS)

UNAME := $(shell uname)

OPT			:=	-O2
CXX			:=	h5c++ #--pthreads
H5CXX		:=	h5c++
CXXFLAGS	+=	-g3 -Wall -fmessage-length=0 -I$(INCMOD) -I$(INCDIR) $(shell pkg-config --cflags opencv4)
PLPLOTFLAGS	:=	`PKG_CONFIG_PATH=/usr/lib/pkgconfig pkg-config --cflags --libs plplotd-c++`
INCMOD		+=	./include
INCDIR		+=	../../include
SRCDIR		+=	./modules
LIBDIR		:=	../../lib
LIBS		:=	-llapack -lblas -lminpack -lgfortran -lhdf5 -lhdf5_hl $(shell pkg-config --libs opencv4)
OBJ			:=	procImaging.o
TARGET		:=	procImaging
OBJ2		:=	calibrateImaging.o
TARGET2		:=  calibrateImaging

PREFIX = /usr/local
# Prefix for each installed program, 
# normally empty or `g'.

# The directory to install program in.
BINDIR = $(PREFIX)/bin

ifeq ($(UNAME), Darwin)
	INCDIR		+=	-I/opt/local/include
	LIBDIR      +=	-L/opt/local/lib -L/opt/local/lib/gcc5
	#LIBS		:=	-framework accelerate -lminpack -lhdf5 -lhdf5_hl -pthread
endif



-include $(SRCDIR)/subdir.mk

have_LIBDIR := $(wildcard $(LIBDIR))

##################################################

$(TARGET):	$(OBJS) $(OBJ) Makefile
	@echo ''
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(CXX) -L$(LIBDIR) -o "$(TARGET)"  $(OPT) $(OBJS) $(OBJ) $(LIBS) 
####$(PLPLOTFLAGS)
	@echo 'Finished building target: $@'
	@echo ''

$(TARGET2):	$(OBJS) $(OBJ2) Makefile
	@echo ''
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	$(CXX) -L$(LIBDIR) -o "$(TARGET2)"  $(OPT) $(OBJS) $(OBJ2) $(LIBS) 
####$(PLPLOTFLAGS)
	@echo 'Finished building target: $@'
	@echo ''
	
%.o	: $(@:%.o=%.cpp)
		@echo 'Building file: $(@:%.o=%.cpp)'
		@echo 'Invoking: GCC C++ Compiler'
		$(CXX) $(CPPFLAGS)  $(OPT) $(CXXFLAGS) -c -o $@ $(@:%.o=%.cpp) 
		@echo 'Finished building: $<'
		@echo ' '
		
.PHONY:	all
all:	$(TARGET) $(TARGET2)

.PHONY:	clean
clean:
	@echo ''
	rm -f $(OBJS) $(OBJ) $(OBJ2)
	@echo ''

.PHONY:	cleantarget
cleantarget:
	@echo ''
	rm -f $(TARGET) $(TARGET2)
	@echo ''

.PHONY:	cleanall
cleanall: clean cleantarget

.PHONY:	install
install: all
	@echo ''
	cp $(TARGET) ../../bin/$(TARGET)
	cp $(TARGET2) ../../bin/$(TARGET2)
	cp $(TARGET) $(BINDIR)/$(TARGET)
	cp $(TARGET2) $(BINDIR)/$(TARGET2)
	@echo ''
