from JMTucker.Tools.BasicAnalyzer_cfg import *

settings = CMSSWSettings()
settings.is_mc = True

geometry_etc(process, which_global_tag(settings))
tfileservice(process, 'trackingtreer.root')
sample_files(process, 'qcdht2000_2017', 'miniaod')
cmssw_from_argv(process)

process.load('PhysicsTools.PatAlgos.selectionLayer1.jetSelector_cfi')
process.load('JMTucker.MFVNeutralino.UnpackedCandidateTracks_cfi')
process.load('JMTucker.Tools.MCStatProducer_cff')
process.load('JMTucker.Tools.NtupleFiller_cff')
process.load('JMTucker.Tools.PATTupleSelection_cfi')
process.load('JMTucker.Tools.UpdatedJets_cff')
process.load('JMTucker.Tools.WeightProducer_cfi')

process.selectedPatJets.src = 'updatedJetsMiniAOD'
process.selectedPatJets.cut = process.jtupleParams.jetCut

process.tt = cms.EDAnalyzer('TrackingTreer',
                            process.jmtNtupleFillerMiniAOD,
                            track_sel = cms.bool(True),
                            )

process.p = cms.Path(process.tt)

from JMTucker.MFVNeutralino.EventFilter import setup_event_filter
setup_event_filter(process, input_is_miniaod=True, mode='jets only novtx', event_filter_jes_mult=0)

ReferencedTagsTaskAdder(process)('p')


if __name__ == '__main__' and hasattr(sys, 'argv') and 'submit' in sys.argv:
    from JMTucker.Tools.MetaSubmitter import *
    import JMTucker.Tools.Samples as Samples
    from JMTucker.Tools.Year import year

    if year == 2017:
        samples = Samples.qcd_samples_2017 + Samples.data_samples_2017
    elif year == 2018:
        samples = Samples.qcd_samples_2018 + Samples.data_samples_2018

    set_splitting(samples, 'miniaod', 'default', json_path('ana_2017p8_1pc.json'), 16)

    ms = MetaSubmitter('TrackingTreerV3', dataset='miniaod')
    ms.common.pset_modifier = chain_modifiers(is_mc_modifier, era_modifier, per_sample_pileup_weights_modifier())
    ms.condor.stageout_files = 'all'
    ms.submit(samples)
