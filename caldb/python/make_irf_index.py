import datetime
import pyfits

def read_mappings(infile):
    """
    Read event_class or event_type mappings from a text file.
    Return matched sequences with the name to bit-position mapping.
    """
    event_class = [[], [], []]
    event_type = [[], [], []]
    for line in open(infile):
        # skip comment and whitespace-only lines
        if line[0] == "#" or not line.split():
            continue
        if line.startswith("event_class:"):
            mappings = event_class
            continue
        if line.startswith("event_type:"):
            mappings = event_type
            continue
        tokens = line.split()
        mappings[0].append(tokens[0])
        mappings[1].append(int(tokens[1]))
        try:
            mappings[2].append(int(tokens[2]))
        except ValueError:
            mappings[2].append(tokens[2])
    return event_class, event_type

evclass, evtype = read_mappings("irf_index.txt")

output = pyfits.HDUList()
output.append(pyfits.PrimaryHDU())

evclass_cols = [pyfits.Column('EVENT_CLASS', format='60A', unit=' ',
                              array=evclass[0]),
                pyfits.Column('BITPOSITION', format='1I', unit=' ',
                              array=evclass[1]),
                pyfits.Column('EVENT_TYPES', format='1J', unit='  ',
                              array=evclass[2])]
evclass_hdu = pyfits.new_table(evclass_cols)
evclass_hdu.name = "BITMASK_MAPPING"
output.append(evclass_hdu)

evtype_cols = [pyfits.Column('EVENT_TYPE', format='60A', unit=' ',
                             array=evtype[0]),
               pyfits.Column('BITPOSITION', format='1I', unit=' ',
                             array=evtype[1]),
               pyfits.Column('EVENT_TYPE_PARTITION', format='20A', unit=' ',
                             array=evtype[2])]
evtype_hdu = pyfits.new_table(evtype_cols)
evtype_hdu.name = "EVENT_TYPE_MAPPING"
output.append(evtype_hdu)

output.writeto("irf_index_candidate.fits", clobber=True)
