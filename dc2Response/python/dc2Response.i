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
%include /nfs/farm/g/glast/u06/jchiang/ST_new/irfs/irfInterface/v0/irfInterface/IAeff.h
%include /nfs/farm/g/glast/u06/jchiang/ST_new/irfs/irfInterface/v0/irfInterface/IPsf.h
%include /nfs/farm/g/glast/u06/jchiang/ST_new/irfs/irfInterface/v0/irfInterface/IEdisp.h
%include /nfs/farm/g/glast/u06/jchiang/ST_new/irfs/irfInterface/v0/irfInterface/Irfs.h
%include /nfs/farm/g/glast/u06/jchiang/ST_new/irfs/irfInterface/v0/irfInterface/IrfsFactory.h
%include /nfs/farm/g/glast/u06/jchiang/ST_new/irfs/irfInterface/v0/irfInterface/AcceptanceCone.h
%include /nfs/farm/g/glast/u06/jchiang/ST_new/irfs/dc2Response/v0/dc2Response/loadIrfs.h
%template(DoubleVector) std::vector<double>;
%template(StringVector) std::vector<std::string>;
%template(IrfVector) std::vector<irfInterface::Irfs>;
%template(IrfMap) std::map<unsigned int, irfInterface::Irfs *>;