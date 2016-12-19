import os, sys
from JMTucker.Tools.BasicAnalyzer_cfg import cms, process

use_weights = True
do_nominal = True
do_distances = False
do_clusters = False

process.source.fileNames = ['/store/user/tucker/QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/ntuplev9/161019_211934/0000/ntuple_1.root']
process.TFileService.fileName = 'cutplay.root'

process.load('JMTucker.MFVNeutralino.VertexSelector_cfi')
process.load('JMTucker.MFVNeutralino.AnalysisCuts_cfi')
vtx_sel = process.mfvSelectedVerticesTight
ana_sel = process.mfvAnalysisCuts

def pize(f,sz):
    fmt = '%.' + str(sz) + 'f'
    return (fmt % f).replace('.','p').replace('-','n')

changes = []
changes.append(('nm1', '', ''))

if do_nominal:
    for i in xrange(0,10):
        changes.append(('njetsX%i'%i, '', 'min_njets = %i'%i))
    for i in xrange(250,1001,50):
        changes.append(('htX%i'%i, '', 'min_ht = %i'%i))
    for i in xrange(0,15):
        changes.append(('ntracksX%i'%i, 'min_ntracks = %i, min_njetsntks = 0'%i, ''))
    for i in xrange(0,10):
        changes.append(('ntracksptgt3X%i'%i, 'min_ntracksptgt3 = %i'%i, ''))
    for i in xrange(0,6):
        changes.append(('njetsntksX%i'%i, 'min_njetsntks = %i'%i, ''))
    for i in xrange(0,20):
        changes.append(('mintrackpairdphimaxX%s'%pize(0.1*i,1), 'min_trackpairdphimax = %f'%(0.1*i), ''))
    for i in xrange(0,20):
        changes.append(('drminX%s'%pize(0.05*i,2), 'max_drmin = %f'%(0.05*i), ''))
    for i in xrange(0,20):
        changes.append(('mindrmaxX%s'%pize(0.1*i,1), 'min_drmax = %f'%(0.1*i),''))
    for i in xrange(4,28):
        changes.append(('maxdrmaxX%s'%pize(0.25*i,2), 'max_drmax = %f'%(0.25*i), ''))
    for i in xrange(15,41):
        changes.append(('geo2ddistX%s'%pize(0.1*i,1), 'max_geo2ddist = %f'%(0.1*i), ''))
    for i in xrange(0,50):
        changes.append(('bs2derrX%s'%pize(0.0005*i,4), 'max_bs2derr = %f'%(0.0005*i), ''))

if do_distances:
    for i in xrange(20):
        changes.append(('bs2ddist01X%s'%pize(0.005*i,3), '', 'min_bs2ddist01 = %f'%(0.005*i)))
    for i in xrange(20):
        changes.append(('pv2ddist01X%s'%pize(0.005*i,3), '', 'min_pv2ddist01 = %f'%(0.005*i)))
    for i in xrange(20):
        changes.append(('pv3ddist01X%s'%pize(0.005*i,3), '', 'min_pv3ddist01 = %f'%(0.005*i)))
    for i in xrange(20):
        changes.append(('svdist2dX%s'%pize(0.005*i,3), '', 'min_svdist2d = %f'%(0.005*i)))
    for i in xrange(20):
        changes.append(('svdist3dX%s'%pize(0.005*i,3), '', 'min_svdist3d = %f'%(0.005*i)))
    for i in xrange(20):
        changes.append(('bs2dsig01X%s'%pize(1.5*i,1), '', 'min_bs2dsig01 = %f'%(1.5*i)))
    for i in xrange(20):
        changes.append(('pv2dsig01X%s'%pize(1.5*i,1), '', 'min_pv2dsig01 = %f'%(1.5*i)))
    for i in xrange(20):
        changes.append(('pv3dsig01X%s'%pize(1.5*i,1), '', 'min_pv3dsig01 = %f'%(1.5*i)))

if do_clusters:
    ana_sel.min_nvertex = 1
    #vtx_sel.min_bsbs2ddist = 0.05
    vtx_sel.use_cluster_cuts = True
    for i in xrange(7):
        changes.append(('nclustersX%i' % i, 'min_nclusters = %i'%i, ''))
    for i in xrange(7):
        changes.append(('nsingleclustersX%i' % i, 'max_nsingleclusters = %i'%i, ''))
    for i in xrange(20):
        changes.append(('fsingleclustersX%s'%pize(0.05*i,2), 'max_fsingleclusters = %f'%(0.05*i), ''))
    for i in xrange(20):
        changes.append(('nclusterspertkX%s'%pize(0.05*i,2), 'min_nclusterspertk = %f'%(0.05*i), ''))
    for i in xrange(20):
        changes.append(('nsingleclusterspertkX%s'%pize(0.05*i,2), 'max_nsingleclusterspertk = %f'%(0.05*i), ''))
    for i in xrange(7):
        changes.append(('nsingleclusterspb025X%i' % i, 'max_nsingleclusterspb025 = %i'%i, ''))
    for i in xrange(7):
        changes.append(('nsingleclusterspb050X%i' % i, 'max_nsingleclusterspb050 = %i'%i, ''))
    for i in xrange(20):
        changes.append(('avgnconstituentsX%s'%pize(0.25*i,2), 'min_avgnconstituents = %f'%(0.25*i), ''))

for name, vtx_change, ana_change in changes:
    vtx_name = 'Sel' + name
    ana_name = 'Ana' + name
    path_name = name

    vtx_obj = eval('vtx_sel.clone(%s)' % vtx_change)
    ana_obj = eval('ana_sel.clone(%s)' % ana_change)

    ana_obj.vertex_src = vtx_name
    
    setattr(process, vtx_name, vtx_obj)
    setattr(process, ana_name, ana_obj)

    setattr(process, path_name, cms.Path(vtx_obj * ana_obj))


if use_weights:
    process.load('JMTucker.MFVNeutralino.WeightProducer_cfi')

import JMTucker.Tools.SimpleTriggerEfficiency_cfi as SimpleTriggerEfficiency
SimpleTriggerEfficiency.setup_endpath(process, weight_src='mfvWeight' if use_weights else '')


if __name__ == '__main__' and hasattr(sys, 'argv') and 'submit' in sys.argv:
    from JMTucker.Tools.CondorSubmitter import CondorSubmitter
    import JMTucker.Tools.Samples as Samples

    samples = Samples.registry.from_argv(
        Samples.data_samples + \
        Samples.ttbar_samples + Samples.qcd_samples + Samples.qcd_samples_ext + \
        [Samples.mfv_neu_tau00100um_M0800, Samples.mfv_neu_tau00300um_M0800, Samples.mfv_neu_tau01000um_M0800, Samples.mfv_neu_tau10000um_M0800] + \
        [Samples.xx4j_tau00001mm_M0300, Samples.xx4j_tau00010mm_M0300, Samples.xx4j_tau00001mm_M0700, Samples.xx4j_tau00010mm_M0700]
        )

    for sample in samples:
        sample.files_per = 50
        if not sample.is_mc:
            sample.json = 'ana_10pc.json'

    cs = CondorSubmitter('CutPlayV9', dataset='ntuplev9')
    cs.submit_all(samples)
