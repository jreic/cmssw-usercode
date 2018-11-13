from JMTucker.MFVNeutralino.NtupleCommon import *

settings = NtupleSettings()
settings.is_mc = True
settings.is_miniaod = True
settings.cross = '' # 2017to2018' # 2017to2017p8'

process = ntuple_process(settings)
tfileservice(process, 'presel.root')
del process.out
del process.outp
del process.p

max_events(process, 10000)
report_every(process, 1000000)
#want_summary(process)
dataset = 'miniaod' if settings.is_miniaod else 'main'
sample_files(process, 'qcdht2000_2017', dataset, 1)
file_event_from_argv(process)

process.load('JMTucker.MFVNeutralino.WeightProducer_cfi')
process.load('JMTucker.MFVNeutralino.EventHistos_cfi')
process.load('JMTucker.MFVNeutralino.AnalysisCuts_cfi')

process.mfvEvent.vertex_seed_tracks_src = cms.InputTag('mfvVertexTracks', 'seed')
process.mfvWeight.throw_if_no_mcstat = False

process.mfvAnalysisCutsForJetHistos    = process.mfvAnalysisCuts.clone(apply_vertex_cuts = False)
process.mfvAnalysisCutsForLeptonHistos = process.mfvAnalysisCuts.clone(apply_vertex_cuts = False, apply_presel = 2)

process.mfvEventHistosJetTriggered    = process.mfvEventHistos.clone()
process.mfvEventHistosLeptonTriggered = process.mfvEventHistos.clone()
process.mfvEventHistosJetPreSel       = process.mfvEventHistos.clone()
process.mfvEventHistosLeptonPreSel    = process.mfvEventHistos.clone()

process.eventHistosPreSeq = cms.Sequence(process.goodOfflinePrimaryVertices *
                                         process.selectedPatJets *
                                         process.selectedPatMuons *
                                         process.selectedPatElectrons *
                                         process.mfvTriggerFloats *
                                         process.mfvGenParticles *
                                         process.mfvUnpackedCandidateTracks *
                                         process.mfvVertexTracks *
                                         process.mfvEvent *
                                         process.mfvWeight)

process.pEventHistosJetTriggered = cms.Path(process.mfvTriggerFilterJetsOnly    * process.eventHistosPreSeq                                          * process.mfvEventHistosJetTriggered)
process.pEventHistosJetPreSel    = cms.Path(process.mfvTriggerFilterJetsOnly    * process.eventHistosPreSeq * process.mfvAnalysisCutsForJetHistos    * process.mfvEventHistosJetPreSel)
process.pEventHistosLepTriggered = cms.Path(process.mfvTriggerFilterLeptonsOnly * process.eventHistosPreSeq                                          * process.mfvEventHistosLeptonTriggered)
process.pEventHistosLepPreSel    = cms.Path(process.mfvTriggerFilterLeptonsOnly * process.eventHistosPreSeq * process.mfvAnalysisCutsForLeptonHistos * process.mfvEventHistosLeptonPreSel)


if __name__ == '__main__' and hasattr(sys, 'argv') and 'submit' in sys.argv:
    from JMTucker.Tools.MetaSubmitter import *
    from JMTucker.Tools import Samples

    if year == 2017:
        samples  = Samples.ttbar_samples_2017 + Samples.qcd_samples_2017 + Samples.all_signal_samples_2017
        samples += Samples.data_samples_2017
        #samples += Samples.leptonic_samples_2017
    elif year == 2018:
        samples = Samples.data_samples_2018 + [s for s in Samples.auxiliary_data_samples_2018 if s.name.startswith('ReRecoJetHT')]

    samples = [s for s in samples if s.has_dataset(dataset) and (s.is_mc or not settings.cross)]
    set_splitting(samples, dataset, 'ntuple', data_json=json_path('ana_2017p8_1pc.json'))

    ms = MetaSubmitter('PreselHistos%s%s' % (settings.version.capitalize(), '_' + settings.cross if settings.cross else ''), dataset=dataset)
    ms.common.pset_modifier = chain_modifiers(is_mc_modifier, per_sample_pileup_weights_modifier(cross=settings.cross))
    ms.condor.stageout_files = 'all'
    ms.submit(samples)
