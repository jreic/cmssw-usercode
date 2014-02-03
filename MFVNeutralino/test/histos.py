import sys
from JMTucker.Tools.BasicAnalyzer_cfg import cms, process
from JMTucker.Tools import SampleFiles

SampleFiles.setup(process, 'MFVNtupleV15', 'qcdht1000', 200000)
process.TFileService.fileName = 'histos.root'

process.load('JMTucker.MFVNeutralino.VertexSelector_cfi')
process.load('JMTucker.MFVNeutralino.Histos_cff')

process.mfvWeight.pileup_weights = [5.9155848575272045e-06, 5e-5, 0.00012931685331215459, 0.00044123388604649363, 0.0027445061796001497, 0.045714390456045341, 0.33297055138077847, 1.0687959511595915, 2.2248301271354189, 3.1783614029761402, 3.0363744139908628, 2.7828582290213664, 2.6427930135541562, 2.2839244951373034, 1.9461708826385724, 1.6599930857052969, 1.4482086675136461, 1.3560629268511939, 1.246238561461098, 1.1848253136030058, 1.0745281811491763, 0.97183584255352606, 0.8452247891562028, 0.68975147365027623, 0.57041787038438851, 0.44312745510946938, 0.31705381634685437, 0.21564450615683173, 0.13328876112001972, 0.074680554767719731, 0.038342230855994433, 0.018714883769292093, 0.0088805169242695565, 0.0040055704822645147, 0.0017827838709893382, 0.00083407350863771025, 0.00036097760387329963, 0.00015322211128503103, 5.8875766614189109e-05, 2.5014893330793753e-05, 8.7645262941351809e-06, 3.3574927765258077e-06, 9.9291122170813691e-07, 3.3477050765109126e-07, 1.0086262167984007e-07, 2.8775989916943902e-08, 7.7832473269228632e-09, 1.7295099942782696e-09, 4.4196332248786806e-10, 8.3356207236738366e-11, 2.3719490813270349e-11, 4.110092003659434e-12, 8.8327381521246206e-13, 1.0466256674348868e-13, 2.5639623829256339e-14, 4.1401151183287217e-15, 6.5314644577352518e-16]

process.p = cms.Path(process.mfvSelectedVerticesSeq * process.mfvHistos)

if __name__ == '__main__' and hasattr(sys, 'argv') and 'submit' in sys.argv:
    import JMTucker.Tools.Samples as Samples
    samples = Samples.ttbar_samples + Samples.qcd_samples + Samples.leptonic_background_samples
    samples += [Samples.mfv_neutralino_tau0100um_M0400, Samples.mfv_neutralino_tau1000um_M0400, Samples.mfv_neutralino_tau9900um_M0400]

    from JMTucker.Tools.CRABSubmitter import CRABSubmitter
    from JMTucker.Tools.SampleFiles import SampleFiles

    cs = CRABSubmitter('MFVHistosV15',
                       total_number_of_events = 1,
                       events_per_job = 1,
                       manual_datasets = SampleFiles['MFVNtupleV15'],
                       )
    cs.submit_all([Samples.MultiJetPk2012B])
