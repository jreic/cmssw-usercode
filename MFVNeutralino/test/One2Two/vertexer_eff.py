from JMTucker.Tools.ROOTTools import *

is_mc = True
year = '2016'
ntkseeds = False

set_style()
ps = plot_saver('../plots/bkgest/v15/vertexer_pair_effs%s_%s%s' % ('' if is_mc else '_data', year, '_ntkseeds' if ntkseeds else ''), size=(700,700), log=False, root=False)

if is_mc:
    f = ROOT.TFile('/uscms_data/d1/jchu/crab_dirs/mfv_8025/VertexerPairEffsV15/background%s.root' % ('' if year=='2016' else '_%s'%year))
else:
    f = ROOT.TFile('/uscms_data/d1/jchu/crab_dirs/mfv_8025/VertexerPairEffsV15/JetHT%s.root' % year)

fh = ROOT.TFile('vpeffs%s_%s_v15%s.root' % ('' if is_mc else '_data', year, '_ntkseeds' if ntkseeds else ''), 'recreate')

for itk in [3,4,5]:
    h_merge = f.Get('mfvVertexerPairEffs%s/h_merge_d2d_mintk0_maxtk%i' % ('%iTkSeed' % itk if ntkseeds else '', itk))
    h_pairs = f.Get('mfvVertexerPairEffs%s/h_pairs_d2d_mintk0_maxtk%i' % ('%iTkSeed' % itk if ntkseeds else '', itk))
    h_erase = f.Get('mfvVertexerPairEffs%s/h_erase_d2d_mintk0_maxtk%i' % ('%iTkSeed' % itk if ntkseeds else '', itk))

    h_merge.Rebin(10)
    h_pairs.Rebin(10)
    h_erase.Rebin(10)

    h_add_eff = h_merge.Clone('maxtk%i' % itk)
    h_add_eff.Add(h_erase)
    h_add_eff.Divide(h_add_eff, h_pairs, 1,1,'B')
    for i in range(1, h_add_eff.GetNbinsX()+2):
        h_add_eff.SetBinContent(i, 1-h_add_eff.GetBinContent(i))
    h_add_eff.Scale(1./h_add_eff.GetBinContent(h_add_eff.GetNbinsX()))
    h_add_eff.SetTitle('maxtk%i;d_{VV} (cm);efficiency' % itk)
    h_add_eff.GetYaxis().SetRangeUser(0,1.05)
    h_add_eff.SetLineColor(ROOT.kGreen+2)
    h_add_eff.SetStats(0)
    h_add_eff.Draw()

    h_merge_eff = h_merge.Clone('maxtk%i_merge' % itk)
    h_merge_eff.Divide(h_merge_eff, h_pairs, 1,1,'B')
    for i in range(1, h_merge_eff.GetNbinsX()+2):
        h_merge_eff.SetBinContent(i, 1-h_merge_eff.GetBinContent(i))
    h_merge_eff.Scale(1./h_merge_eff.GetBinContent(h_merge_eff.GetNbinsX()))
    h_merge_eff.SetTitle('maxtk%i merge;d_{VV} (cm);efficiency' % itk)
    h_merge_eff.GetYaxis().SetRangeUser(0,1.05)
    h_merge_eff.SetLineColor(ROOT.kBlue)
    h_merge_eff.SetStats(0)
    h_merge_eff.Draw('sames')

    l = ROOT.TLegend(0.50,0.55,0.85,0.70)
    l.AddEntry(h_merge_eff, '1 - merge / pairs')
    l.AddEntry(h_add_eff, '1 - (merge + erase) / pairs')
    l.SetFillColor(0)
    l.Draw()

    ps.save('h_eff_maxtk%i' % itk)
    fh.cd()
    h_add_eff.Write()
    h_merge_eff.Write()

fh.Close()

f = ROOT.TFile('vpeffs%s_%s_v15%s.root' % ('' if is_mc else '_data', year, '_ntkseeds' if ntkseeds else ''))

colors = [0, 0, 0, ROOT.kRed, ROOT.kBlue, ROOT.kGreen+2]
l = ROOT.TLegend(0.50,0.15,0.85,0.30)
for i in [3,4,5]:
    h = f.Get('maxtk%i'%i)
    h.SetStats(0)
    h.SetLineWidth(3)
    h.SetLineColor(colors[i])
    if i == 3:
        h.SetTitle(';d_{VV} (cm);Efficiency')
        h.GetXaxis().SetRangeUser(0,0.4)
        h.Draw('hist')
    else:
        h.Draw('hist sames')
    l.AddEntry(h, '%i-track'%i)
l.SetFillColor(0)
l.Draw()
ps.save('efficiency')

if not ntkseeds:
    h = f.Get('maxtk3')
    h.Draw('hist')
    ps.save('efficiency3')

    ROOT.TH1.AddDirectory(0)

    h2 = ROOT.TFile('vpeffs%s_%s_v15_ntkseeds.root' % ('' if is_mc else '_data', year)).Get('maxtk5')
    h2.SetLineWidth(3)
    h.Draw('hist')
    h2.Draw('hist sames')
    l = ROOT.TLegend(0.50,0.15,0.85,0.30)
    l.AddEntry(h, 'default')
    l.AddEntry(h2, 'variation')
    l.Draw()
    ps.save('compare_efficiency')

if is_mc and not ntkseeds:
    fn1 = ['2v_from_jets_%s_3track_default_v15.root' % year, '2v_from_jets_%s_3track_noclearing_v15.root' % year]
    fn2 = ['2v_from_jets_%s_4track_default_v15.root' % year, '2v_from_jets_%s_4track_noclearing_v15.root' % year]
    fn3 = ['2v_from_jets_%s_5track_default_v15.root' % year, '2v_from_jets_%s_5track_noclearing_v15.root' % year]

    fns = [fn1, fn2, fn3]
    ntk = ['3-track', '4-track', '5-track']

    n2v = [939., 7., 1.]
    if year == '2015':
        n2v = [45., 1., 1.]
    if year == '2015p6':
        n2v = [984., 7., 1.]

    for i in range(3):
        h0 = ROOT.TFile(fns[i][0]).Get('h_c1v_dvv')
        h0.SetTitle(';d_{VV}^{C} (cm);Events')
        h0.SetStats(0)
        h0.SetLineColor(ROOT.kRed)
        h0.SetLineWidth(3)
        h0.Scale(n2v[i]/h0.Integral())
        if i == 2:
            h0.GetYaxis().SetRangeUser(0,0.4)
        h0.Draw('hist e')

        h1 = ROOT.TFile(fns[i][1]).Get('h_c1v_dvv')
        h1.SetStats(0)
        h1.SetLineColor(ROOT.kBlack)
        h1.SetLineWidth(3)
        h1.Scale(n2v[i]/h1.Integral())
        h1.Draw('hist e sames')

        l = ROOT.TLegend(0.35,0.75,0.85,0.85)
        l.AddEntry(h1, 'without efficiency correction')
        l.AddEntry(h0, 'with efficiency correction')
        l.SetFillColor(0)
        l.Draw()
        ps.save(ntk[i])
