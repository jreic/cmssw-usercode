from JMTucker.Tools.CMSSWTools import *
from JMTucker.Tools.MiniAOD_cfg import *
from JMTucker.Tools.PileupWeights import pileup_weights
from JMTucker.MFVNeutralino.Year import year

is_mc = True
H = False

process = pat_tuple_process(None, is_mc, year, H)
jets_only(process)

process.source.fileNames = ['/store/user/wsun/croncopyeos/qcdht1000/RunIISummer16DR80-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6/170606_165644/0000/reco_%i.root' % i for i in xrange(1,11)]
if not is_mc:
    process.source.fileNames = ['/store/data/Run2016E/ZeroBias/AOD/23Sep2016-v1/100000/18151C77-4486-E611-84FD-0CC47A7C357A.root']

process.load('JMTucker.Tools.FirstGoodPrimaryVertex_cfi')
process.firstGoodPrimaryVertex.cut = True

process.load('JMTucker.MFVNeutralino.SkimmedTracks_cfi')
process.mfvSkimmedTracks.min_pt = 0.9
process.mfvSkimmedTracks.min_nsigmadxybs = 3
process.mfvSkimmedTracks.cut = True

process.load('JMTucker.MFVNeutralino.V0Vertexer_cff')
process.mfvV0Vertices.cut = True

process.load('JMTucker.MFVNeutralino.TriggerFloats_cff')

process.p = cms.Path(process.firstGoodPrimaryVertex * process.mfvSkimmedTracks * process.mfvV0Vertices * process.mfvTriggerFloats)

if is_mc:
    process.load('PhysicsTools.PatAlgos.slimming.slimmedAddPileupInfo_cfi')
    process.p *= process.slimmedAddPileupInfo

output_commands = [
    'drop *',
    'keep *_mcStat_*_*',
    'keep *_mfvSkimmedTracks_*_*',
    'keep *_mfvV0Vertices_*_*',
    'keep *_mfvTriggerFloats_*_*',
    'keep *_offlineBeamSpot_*_*',
    'keep *_slimmedAddPileupInfo_*_*',
    'keep *_firstGoodPrimaryVertex_*_*',
    'keep *_TriggerResults_*_HLT', # for ZeroBias since I don't wanna mess with MFVTriggerFloats for it
    ]
output_file(process, 'v0ntuple.root', output_commands)

import JMTucker.MFVNeutralino.EventFilter as ef
ef.setup_event_filter(process, path_name='p')
process.triggerFilter.HLTPaths.append('HLT_ZeroBias_v*') # what are the ZeroBias_part* paths?

process.maxEvents.input = -1
#report_every(process, 1)
want_summary(process)

if __name__ == '__main__' and hasattr(sys, 'argv') and 'submit' in sys.argv:
    import JMTucker.Tools.Samples as Samples 
    samples = Samples.data_samples + [s for s in Samples.auxiliary_data_samples if s.name.startswith('ZeroBias')]
    samples += Samples.qcd_samples + Samples.qcd_samples_ext + Samples.qcd_hip_samples[-2:]

    for s in samples:
        s.files_per = 20

    from JMTucker.Tools.MetaSubmitter import *
    ms = MetaSubmitter('V0NtupleV1')
    ms.common.ex = year
    ms.common.pset_modifier = chain_modifiers(is_mc_modifier, H_modifier)
    ms.crab.job_control_from_sample = True
    ms.submit(samples)