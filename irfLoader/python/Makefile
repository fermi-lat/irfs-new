#
# @file Makefile
# @brief Hand-coded Makefile for building the irfLoader.py module
#
# @author J. Chiang
#
# $Header$
#

SWIG = ${SWIG_DIR}/bin/swig -w401 -w402

CFLAGS = -g -w -fPIC

INCLUDE = -I$(IRFLOADERROOT)\
	-I$(DC1RESPONSEROOT)\
	-I$(G25RESPONSEROOT)\
	-I$(TESTRESPONSEROOT)\
	-I$(DEVRESPONSEROOT)\
	-I$(IRFINTERFACEROOT)\
	-I$(ASTROROOT)\
	-I$(F2CROOT)\
	-I$(GLAST_EXT)/cfitsio/v2470/include\
	-I$(GLAST_EXT)/CLHEP/1.8.0.0/include

LIBS = -L../$(BINDIR)/ -lirfLoader\
	-L$(DC1RESPONSEROOT)/$(BINDIR) -ldc1Response\
	-L$(G25RESPONSEROOT)/$(BINDIR) -lg25Response\
	-L$(TESTRESPONSEROOT)/$(BINDIR) -ltestResponse\
	-L$(DEVRESPONSEROOT)/$(BINDIR) -ldevResponse\
	-L$(IRFINTERFACEROOT)/$(BINDIR) -lirfInterface\
	-L$(IRFUTILROOT)/$(BINDIR) -lirfUtil\
	-L$(ASTROROOT)/$(BINDIR) -lastro\
	-L$(TIPROOT)/$(BINDIR) -ltipLib\
	-L$(ST_FACILITIESROOT)/$(BINDIR) -lst_facilitiesLib\
	-L$(FACILITIESROOT)/$(BINDIR) -lfacilities\
	-L$(GLAST_EXT)/cfitsio/v2470/lib -lcfitsio\
	-L$(GLAST_EXT)/CLHEP/1.8.0.0/lib -lCLHEP\
	-L$(GLAST_EXT)/ROOT/v3.10.02/root/lib\
	-lCore -lCint -lTree -lMatrix -lPhysics\
	-lHist -lGraf -lGraf3d -lGpad -lRint -lPostscript\
	-lg2c -L$(SWIG_DIR)/lib -lswigpy

module_base = irfLoader
module_name = _${module_base}.so

${module_name}: ${module_base}_wrap.o
	${CXX} -shared $< ${LIBS} -o ${module_name}

${module_base}_wrap.o: ${module_base}_wrap.cxx
	${CXX} -c ${INCLUDE} -I${PYINCLUDE} $<

${module_base}_wrap.cxx: ${module_base}.i ${HFILES}
	${SWIG} -python -c++ ${module_base}.i

${module_base}.i: ${module_base}.in
	python setup.py $<

anew:
	make clean
	make

tar_file:
	tar cvf ${MY_PROG}.tar ${SRCS} ${HFILES} ${OTHER_FILES} ${srcs}

clean:
	/bin/rm -f *~
	/bin/rm -f *.trace
	/bin/rm -f *wrap*
	/bin/rm -f *.so
	/bin/rm -f *.o
	/bin/rm -f $(module_base).cxx
	/bin/rm -f $(module_base).py*
	/bin/rm -f $(module_base).i
