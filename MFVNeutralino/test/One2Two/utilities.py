#!/usr/bin/env python

from JMTucker.MFVNeutralino.UtilitiesBase import *

####

_version = 'v23m'

def cmd_merge_bquarks_nobquarks():
    for year in '2017', '2018':
        weights = '0.79,0.21'
        for ntracks in 3,4,5,7:
            files = ['2v_from_jets_%s_%dtrack_bquarks_%s.root' % (year, ntracks, _version), '2v_from_jets_%s_%dtrack_nobquarks_%s.root' % (year, ntracks, _version)]
            for fn in files:
                if not os.path.isfile(fn):
                    raise RuntimeError('%s not found' % fn)
            cmd = 'mergeTFileServiceHistograms -w %s -i %s -o 2v_from_jets_%s_%dtrack_bquark_corrected_%s.root' % (weights, ' '.join(files), year, ntracks, _version)
            print cmd
            os.system(cmd)

def cmd_merge_btags_nobtags():
    for year in '2017',:
        for ntracks,weights in (3,'0.84,0.16'), (4,'0.87,0.13'), (5,'0.85,0.15'), (7,'0.84,0.16'):
            files = ['2v_from_jets_%s_%dtrack_btags_%s.root' % (year, ntracks, _version), '2v_from_jets_%s_%dtrack_nobtags_%s.root' % (year, ntracks, _version)]
            for fn in files:
                if not os.path.isfile(fn):
                    raise RuntimeError('%s not found' % fn)
            cmd = 'mergeTFileServiceHistograms -w %s -i %s -o 2v_from_jets_%s_%dtrack_btag_corrected_%s.root' % (weights, ' '.join(files), year, ntracks, _version)
            print cmd
            os.system(cmd)

####

if __name__ == '__main__':
    main(locals())
