#
# $Header$
#
all:
	touch Doxyfile
	cat ${GLASTPOLICYROOT}/doc/Doxyfile Doxyfile > doxyfile.tmp
	doxygen doxyfile.tmp
	rm -f doxyfile.tmp
