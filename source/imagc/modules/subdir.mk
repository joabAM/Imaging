#Implicit rules:
#Compiling C++ programs
#x.o:
#	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o x.o x.cpp

#Linking a single object file
#n:
#	$(CC) $(LDFLAGS) n.o $(LOADLIBES) $(LDLIBS)

OBJS	+=	$(SRCDIR)/CpdataRead.o $(SRCDIR)/CpdataUtils.o \
			$(SRCDIR)/Cimaging.o $(SRCDIR)/memUtils.o $(SRCDIR)/cnoise.o \
			$(SRCDIR)/complex.o $(SRCDIR)/cmdUtils.o $(SRCDIR)/fileUtils.o \
			$(SRCDIR)/hdf5File.o $(SRCDIR)/calibration.o\
			$(SRCDIR)/configArg.o

HDRS	+=	$(INCDIR)/CpdataRead.h $(INCDIR)/CpdataUtils.h \
			$(INCDIR)/Cimaging.h $(INCDIR)/memUtils.h $(INCDIR)/cnoise.h \
			$(INCDIR)/complex.h $(INCDIR)/cmdUtils.h $(INCDIR)/fileUtils.h \
			$(INCDIR)/hdf5File.h $(INCDIR)/calibration.h \
			$(INCDIR)/configArg.h \
			$(INCDIR)/cminpack.h 

%.o: 	%.cpp $(SRCDIR)/subdir.mk
		@echo 'Building file: $(@:%.o=%.cpp)'
		@echo 'Invoking: GCC C++ Compiler'
		$(CXX) $(CPPFLAGS) $(CXXFLAGS)  $(OPT) -c -o $@ $(@:%.o=%.cpp) 
		@echo 'Finished building: $<'
		@echo ' '

