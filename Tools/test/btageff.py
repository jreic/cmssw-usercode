#!/usr/bin/env python

from JMTucker.Tools.MiniAOD_cfg import *
from JMTucker.Tools.CMSSWTools import *
from JMTucker.Tools.PileupWeights import pileup_weights
from JMTucker.MFVNeutralino.Year import year

process = pat_tuple_process(None, True, year, False, False)
remove_met_filters(process)
remove_output_module(process)
tfileservice(process, 'btageff.root')

add_analyzer(process, 'JMTBTagEfficiency',
             pileup_info_src = cms.InputTag('addPileupInfo'),
             pileup_weights = cms.vdouble(*pileup_weights[year]),
             jets_src = cms.InputTag('selectedPatJets'),
             jet_pt_min = cms.double(20),
             jet_ht_min = cms.double(1000),
             njets_min = cms.int32(4),
             b_discriminator = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
             b_discriminator_min = cms.vdouble(0.46, 0.935, 0.5426, 0.9535),
             )

process.maxEvents.input = 100
file_event_from_argv(process)

nwp = len(process.JMTBTagEfficiency.b_discriminator_min)

if __name__ == '__main__' and hasattr(sys, 'argv') and 'submit' in sys.argv:
    from JMTucker.Tools.MetaSubmitter import *
    import JMTucker.Tools.Samples as Samples 

    if year == 2015:
        samples = Samples.ttbar_samples_2015 + Samples.qcd_samples_2015 + Samples.qcd_samples_ext_2015
    elif year == 2016:
        samples = Samples.ttbar_samples + Samples.qcd_samples + Samples.qcd_samples_ext + Samples.qcd_hip_samples

    ms = MetaSubmitter('BTagEffV1')
    ms.common.ex = year
    ms.crab.job_control_from_sample = True
    ms.submit(samples)

elif __name__ == '__main__' and hasattr(sys, 'argv') and 'ana' in sys.argv:
    from JMTucker.Tools.ROOTTools import *
    fn = root_fns_from_argv()[0]
    f = ROOT.TFile(fn, 'update')
    d = f.GetDirectory('JMTBTagEfficiency')
    d.cd()
    for kind in 'light', 'charm', 'bottom':
        den = d.Get('den_%s' % kind)
        for wp in xrange(nwp):
            num = d.Get('num_%s_%i' % (kind, wp))
            eff = num.Clone('eff_%s_%i' % (kind, wp))
            for ibin in xrange(1, eff.GetNbinsX()+1):
                for jbin in xrange(1, eff.GetNbinsY()+1):
                    a = (num.GetBinContent(ibin,jbin) / num.GetBinError(ibin,jbin))**2
                    b = (den.GetBinContent(ibin,jbin) / den.GetBinError(ibin,jbin))**2
                    e,l,u = clopper_pearson(a,b)
                    ee = (u-l)/2
                    eff.SetBinContent(ibin,jbin, e)
                    eff.SetBinError  (ibin,jbin, ee)
            eff.Write()