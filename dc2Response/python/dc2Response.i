// -*- mode: c++ -*-
%module dc2Response
%{
#include "irfInterface/IAeff.h"
#include "irfInterface/IPsf.h"
#include "irfInterface/IEdisp.h"
#include "irfInterface/Irfs.h"
#include "irfInterface/IrfsFactory.h"
#include "irfInterface/AcceptanceCone.h"
#include "dc2Response/loadIrfs.h"
#include <map>
#include <string>
#include <vector>
%}
%include stl.i
%include ../../../irfInterface/v0/irfInterface/IAeff.h
%include ../../../irfInterface/v0/irfInterface/IPsf.h
%include ../../../irfInterface/v0/irfInterface/IEdisp.h
%include ../../../irfInterface/v0/irfInterface/Irfs.h
%include ../../../irfInterface/v0/irfInterface/IrfsFactory.h
%include ../../../irfInterface/v0/irfInterface/AcceptanceCone.h
%include ../../../dc2Response/v0/dc2Response/loadIrfs.h
%template(DoubleVector) std::vector<double>;
%template(StringVector) std::vector<std::string>;
%template(IrfVector) std::vector<irfInterface::Irfs>;
%template(IrfMap) std::map<unsigned int, irfInterface::Irfs *>;
