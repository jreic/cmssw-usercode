#include "TH2F.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "JMTucker/Tools/interface/PairwiseHistos.h"
#include "JMTucker/Tools/interface/Utilities.h"
#include "JMTucker/MFVNeutralinoFormats/interface/Event.h"
#include "JMTucker/MFVNeutralinoFormats/interface/VertexAux.h"
#include "JMTucker/MFVNeutralino/interface/VertexTools.h"

class MFVVertexHistos : public edm::EDAnalyzer {
 public:
  explicit MFVVertexHistos(const edm::ParameterSet&);
  void analyze(const edm::Event&, const edm::EventSetup&);

 private:
  const edm::EDGetTokenT<MFVEvent> mevent_token;
  const edm::EDGetTokenT<double> weight_token;
  const edm::EDGetTokenT<MFVVertexAuxCollection> vertex_token;
  const std::vector<double> force_bs;
  const bool do_trackplots;
  const bool do_scatterplots;

  VertexDistanceXY distcalc_2d;
  VertexDistance3D distcalc_3d;

  TH1F* h_w;

  TH1F* h_nsv;

  // indices for h_sv below:
  enum sv_index { sv_all, sv_num_indices };
  static const char* sv_index_names[sv_num_indices];

  // max number of extra track-related plots to make
  static const int max_ntracks;

  void fill_multi(TH1F** hs, const int isv, const double val, const double weight) const;
  void fill_multi(TH2F** hs, const int isv, const double val, const double val2, const double weight) const;
  void fill_multi(PairwiseHistos* hs, const int isv, const PairwiseHistos::ValueMap& val, const double weight) const;

  TH1F* h_sv_pos_1d[3];
  TH2F* h_sv_pos_2d[3];
  TH2F* h_sv_pos_rz;
  TH1F* h_sv_pos_phi;
  TH1F* h_sv_pos_phi_pv;
  TH1F* h_sv_pos_bs1d[3];
  TH2F* h_sv_pos_bs2d[3];
  TH2F* h_sv_pos_bsrz;
  TH1F* h_sv_pos_bsphi;

  PairwiseHistos h_sv[sv_num_indices];

  TH1F* h_sv_jets_deltaphi[4][sv_num_indices];

  TH2F* h_sv_bs2derr_bsbs2ddist[sv_num_indices];
  TH2F* h_pvrho_bsbs2ddist[sv_num_indices];

  TH1F* h_svdist2d;
  TH1F* h_svdist3d;
  TH2F* h_sv0pvdz_v_sv1pvdz;
  TH2F* h_sv0pvdzsig_v_sv1pvdzsig;
  TH1F* h_absdeltaphi01;
  TH2F* h_pvmosttracksshared;
  TH1F* h_fractrackssharedwpv01;
  TH1F* h_fractrackssharedwpvs01;

  TH1F* h_sv_track_weight[sv_num_indices];
  TH1F* h_sv_track_q[sv_num_indices];
  TH1F* h_sv_track_pt[sv_num_indices];
  TH1F* h_sv_track_eta[sv_num_indices];
  TH1F* h_sv_track_phi[sv_num_indices];
  TH1F* h_sv_track_dxy[sv_num_indices];
  TH1F* h_sv_track_dz[sv_num_indices];
  TH1F* h_sv_track_pt_err[sv_num_indices];
  TH1F* h_sv_track_eta_err[sv_num_indices];
  TH1F* h_sv_track_phi_err[sv_num_indices];
  TH1F* h_sv_track_dxy_err[sv_num_indices];
  TH1F* h_sv_track_dz_err[sv_num_indices];
  TH1F* h_sv_track_chi2dof[sv_num_indices];
  TH1F* h_sv_track_npxhits[sv_num_indices];
  TH1F* h_sv_track_nsthits[sv_num_indices];
  TH1F* h_sv_track_nhitsbehind[sv_num_indices];
  TH1F* h_sv_track_nhitslost[sv_num_indices];
  TH1F* h_sv_track_nhits[sv_num_indices];
  TH1F* h_sv_track_injet[sv_num_indices];
  TH1F* h_sv_track_inpv[sv_num_indices];

  TH2F* h_sv_track_nsthits_track_eta[sv_num_indices];
  TH2F* h_sv_bs2derr_track_pt[sv_num_indices];
  TH2F* h_sv_bs2derr_track_eta[sv_num_indices];
  TH2F* h_sv_bs2derr_track_phi[sv_num_indices];
  TH2F* h_sv_bs2derr_track_dxy[sv_num_indices];
  TH2F* h_sv_bs2derr_track_dz[sv_num_indices];
  TH2F* h_sv_bs2derr_track_pt_err[sv_num_indices];
  TH2F* h_sv_bs2derr_track_eta_err[sv_num_indices];
  TH2F* h_sv_bs2derr_track_phi_err[sv_num_indices];
  TH2F* h_sv_bs2derr_track_dxy_err[sv_num_indices];
  TH2F* h_sv_bs2derr_track_dz_err[sv_num_indices];
  TH2F* h_sv_bs2derr_track_npxhits[sv_num_indices];
  TH2F* h_sv_bs2derr_track_nsthits[sv_num_indices];
};

const char* MFVVertexHistos::sv_index_names[MFVVertexHistos::sv_num_indices] = { "all" };
const int MFVVertexHistos::max_ntracks = 5;

MFVVertexHistos::MFVVertexHistos(const edm::ParameterSet& cfg)
  : mevent_token(consumes<MFVEvent>(cfg.getParameter<edm::InputTag>("mevent_src"))),
    weight_token(consumes<double>(cfg.getParameter<edm::InputTag>("weight_src"))),
    vertex_token(consumes<MFVVertexAuxCollection>(cfg.getParameter<edm::InputTag>("vertex_src"))),
    force_bs(cfg.getParameter<std::vector<double> >("force_bs")),
    do_trackplots(cfg.getParameter<bool>("do_trackplots")),
    do_scatterplots(cfg.getParameter<bool>("do_scatterplots"))
{
  if (force_bs.size() && force_bs.size() != 3)
    throw cms::Exception("Misconfiguration", "force_bs must be empty or size 3");

  edm::Service<TFileService> fs;

  h_w = fs->make<TH1F>("h_w", ";event weight;events/0.1", 100, 0, 10);

  h_nsv = fs->make<TH1F>("h_nsv", ";# of secondary vertices;arb. units", 15, 0, 15);

  PairwiseHistos::HistoDefs hs;

  if (do_trackplots) {
    for (int i = 0; i < 11; ++i) {
      hs.add(TString::Format("ntracksstgt%i", i).Data(), TString::Format("# of tracks/SV w/ number of strip hits >= %i", i).Data(), 10, 0, 10);
    }
    for (int i = 0; i < max_ntracks; ++i) {
      hs.add(TString::Format("track%i_weight",        i).Data(), TString::Format("track%i weight",                      i).Data(),  21,  0,      1.05);
      hs.add(TString::Format("track%i_q",             i).Data(), TString::Format("track%i charge",                      i).Data(),   4, -2,      2);
      hs.add(TString::Format("track%i_pt",            i).Data(), TString::Format("track%i p_{T} (GeV)",                 i).Data(), 200,  0,    200);
      hs.add(TString::Format("track%i_eta",           i).Data(), TString::Format("track%i #eta",                        i).Data(),  50, -4,      4);
      hs.add(TString::Format("track%i_phi",           i).Data(), TString::Format("track%i #phi",                        i).Data(),  50, -3.15,   3.15);
      hs.add(TString::Format("track%i_dxy",           i).Data(), TString::Format("track%i dxy (cm)",                    i).Data(), 100,  0,      1);
      hs.add(TString::Format("track%i_dz",            i).Data(), TString::Format("track%i dz (cm)",                     i).Data(), 100,  0,      1);
      hs.add(TString::Format("track%i_pt_err",        i).Data(), TString::Format("track%i #sigma(p_{T})/p_{T}",         i).Data(), 200,  0,      2);
      hs.add(TString::Format("track%i_eta_err",       i).Data(), TString::Format("track%i #sigma(#eta)",                i).Data(), 200,  0,      0.02);
      hs.add(TString::Format("track%i_phi_err",       i).Data(), TString::Format("track%i #sigma(#phi)",                i).Data(), 200,  0,      0.02);
      hs.add(TString::Format("track%i_dxy_err",       i).Data(), TString::Format("track%i #sigma(dxy) (cm)",            i).Data(), 100,  0,      0.1);
      hs.add(TString::Format("track%i_dz_err",        i).Data(), TString::Format("track%i #sigma(dz) (cm)",             i).Data(), 100,  0,      0.1);
      hs.add(TString::Format("track%i_chi2dof",       i).Data(), TString::Format("track%i #chi^{2}/dof",                i).Data(), 100,  0,     10);
      hs.add(TString::Format("track%i_npxhits",       i).Data(), TString::Format("track%i number of pixel hits",        i).Data(),  12,  0,     12);
      hs.add(TString::Format("track%i_nsthits",       i).Data(), TString::Format("track%i number of strip hits",        i).Data(),  28,  0,     28);
      hs.add(TString::Format("track%i_nhitsbehind",   i).Data(), TString::Format("track%i number of hits behind",       i).Data(),  10,  0,     10);
      hs.add(TString::Format("track%i_nhitslost",     i).Data(), TString::Format("track%i number of hits lost",         i).Data(),  10,  0,     10);
      hs.add(TString::Format("track%i_nhits",         i).Data(), TString::Format("track%i number of hits",              i).Data(),  40,  0,     40);
      hs.add(TString::Format("track%i_injet",         i).Data(), TString::Format("track%i in-jet?",                     i).Data(),   2,  0,      2);
      hs.add(TString::Format("track%i_inpv",          i).Data(), TString::Format("track%i in-PV?",                      i).Data(),  10, -1,      9);
      hs.add(TString::Format("track%i_jet_deltaphi0", i).Data(), TString::Format("track%i |#Delta#phi| to closest jet", i).Data(),  25,  0,      3.15);
    }
  }

  hs.add("nlep", "# leptons", 10, 0, 10);

  hs.add("ntracks",                       "# of tracks/SV",                                                               40,    0,      40);
  hs.add("ntracksptgt3",                  "# of tracks/SV w/ p_{T} > 3 GeV",                                              40,    0,      40);
  hs.add("ntracksptgt10",                 "# of tracks/SV w/ p_{T} > 10 GeV",                                             40,    0,      40);
  hs.add("trackminnhits",                 "min number of hits on track per SV",                                           40,    0,      40);
  hs.add("trackmaxnhits",                 "max number of hits on track per SV",                                           40,    0,      40);
  hs.add("njetsntks",                     "# of jets assoc. by tracks to SV",                                             10,    0,      10);
  hs.add("chi2dof",                       "SV #chi^2/dof",                                                                50,    0,       7);
  hs.add("chi2dofprob",                   "SV p(#chi^2, dof)",                                                            50,    0,       1.2);

  hs.add("tkonlyp",                       "SV tracks-only p (GeV)",                                                       50,    0,     500);
  hs.add("tkonlypt",                      "SV tracks-only p_{T} (GeV)",                                                   50,    0,     400);
  hs.add("tkonlyeta",                     "SV tracks-only #eta",                                                          50,   -4,       4);
  hs.add("tkonlyrapidity",                "SV tracks-only rapidity",                                                      50,   -4,       4);
  hs.add("tkonlyphi",                     "SV tracks-only #phi",                                                          50,   -3.15,    3.15);
  hs.add("tkonlymass",                    "SV tracks-only mass (GeV)",                                                   100,    0,    1000);

  hs.add("jetsntkp",                      "SV jets-by-ntracks -only p (GeV)",                                             50,    0,    1000);
  hs.add("jetsntkpt",                     "SV jets-by-ntracks -only p_{T} (GeV)",                                         50,    0,    1000);
  hs.add("jetsntketa",                    "SV jets-by-ntracks -only #eta",                                                50,   -4,       4);
  hs.add("jetsntkrapidity",               "SV jets-by-ntracks -only rapidity",                                            50,   -4,       4);
  hs.add("jetsntkphi",                    "SV jets-by-ntracks -only #phi",                                                50,   -3.15,    3.15);
  hs.add("jetsntkmass",                   "SV jets-by-ntracks -only mass (GeV)",                                          50,    0,    2000);

  hs.add("tksjetsntkp",                   "SV tracks-plus-jets-by-ntracks p (GeV)",                                       50,    0,    1000);
  hs.add("tksjetsntkpt",                  "SV tracks-plus-jets-by-ntracks p_{T} (GeV)",                                   50,    0,    1000);
  hs.add("tksjetsntketa",                 "SV tracks-plus-jets-by-ntracks #eta",                                          50,   -4,       4);
  hs.add("tksjetsntkrapidity",            "SV tracks-plus-jets-by-ntracks rapidity",                                      50,   -4,       4);
  hs.add("tksjetsntkphi",                 "SV tracks-plus-jets-by-ntracks #phi",                                          50,   -3.15,    3.15);
  hs.add("tksjetsntkmass",                "SV tracks-plus-jets-by-ntracks mass (GeV)",                                   100,    0,    5000);
				        
  hs.add("costhtkonlymombs",              "cos(angle(2-momentum (tracks-only), 2-dist to BS))",                           21,   -1,       1.1);
  hs.add("costhtkonlymompv2d",            "cos(angle(2-momentum (tracks-only), 2-dist to PV))",                           21,   -1,       1.1);
  hs.add("costhtkonlymompv3d",            "cos(angle(3-momentum (tracks-only), 3-dist to PV))",                           21,   -1,       1.1);

  hs.add("costhjetsntkmombs",             "cos(angle(2-momentum (jets-by-ntracks -only), 2-dist to BS))",                21,   -1,       1.1);
  hs.add("costhjetsntkmompv2d",           "cos(angle(2-momentum (jets-by-ntracks -only), 2-dist to PV))",                21,   -1,       1.1);
  hs.add("costhjetsntkmompv3d",           "cos(angle(3-momentum (jets-by-ntracks -only), 3-dist to PV))",                21,   -1,       1.1);

  hs.add("costhtksjetsntkmombs",          "cos(angle(2-momentum (tracks-plus-jets-by-ntracks), 2-dist to BS))",          21,   -1,       1.1);
  hs.add("costhtksjetsntkmompv2d",        "cos(angle(2-momentum (tracks-plus-jets-by-ntracks), 2-dist to PV))",          21,   -1,       1.1);
  hs.add("costhtksjetsntkmompv3d",        "cos(angle(3-momentum (tracks-plus-jets-by-ntracks), 3-dist to PV))",          21,   -1,       1.1);

  hs.add("missdisttkonlypv",              "miss dist. (tracks-only) of SV to PV (cm)",                                   100,    0,       2);
  hs.add("missdisttkonlypverr",           "#sigma(miss dist. (tracks-only) of SV to PV) (cm)",                           100,    0,       0.05);
  hs.add("missdisttkonlypvsig",           "N#sigma(miss dist. (tracks-only) of SV to PV) (cm)",                          100,    0,     100);

  hs.add("missdistjetsntkpv",             "miss dist. (jets-by-ntracks -only) of SV to PV (cm)",                         100,    0,       2);
  hs.add("missdistjetsntkpverr",          "#sigma(miss dist. (jets-by-ntracks -only) of SV to PV) (cm)",                 100,    0,       0.05);
  hs.add("missdistjetsntkpvsig",          "N#sigma(miss dist. (jets-by-ntracks -only) of SV to PV) (cm)",                100,    0,     100);

  hs.add("missdisttksjetsntkpv",          "miss dist. (tracks-plus-jets-by-ntracks) of SV to PV (cm)",                   100,    0,       2);
  hs.add("missdisttksjetsntkpverr",       "#sigma(miss dist. (tracks-plus-jets-by-ntracks) of SV to PV) (cm)",           100,    0,       0.05);
  hs.add("missdisttksjetsntkpvsig",       "N#sigma(miss dist. (tracks-plus-jets-by-ntracks) of SV to PV) (cm)",          100,    0,     100);
					  
  hs.add("sumpt2",                        "SV #Sigma p_{T}^{2} (GeV^2)",                                                  50,    0,    10000);

  hs.add("ntrackssharedwpv",  "number of tracks shared with the PV", 30, 0, 30);
  hs.add("ntrackssharedwpvs", "number of tracks shared with any PV", 30, 0, 30);
  hs.add("fractrackssharedwpv",  "fraction of tracks shared with the PV", 41, 0, 1.025);
  hs.add("fractrackssharedwpvs", "fraction of tracks shared with any PV", 41, 0, 1.025);
  hs.add("npvswtracksshared", "number of PVs having tracks shared",  30, 0, 30);
  
  hs.add("trackdxymin", "SV min{trk_{i} dxy(BS)} (cm)", 50, 0, 0.2);
  hs.add("trackdxymax", "SV max{trk_{i} dxy(BS)} (cm)", 50, 0, 2);
  hs.add("trackdxyavg", "SV avg{trk_{i} dxy(BS)} (cm)", 50, 0, 0.5);
  hs.add("trackdxyrms", "SV rms{trk_{i} dxy(BS)} (cm)", 50, 0, 0.5);

  hs.add("trackdzmin", "SV min{trk_{i} dz(PV)} (cm)", 50, 0, 0.5);
  hs.add("trackdzmax", "SV max{trk_{i} dz(PV)} (cm)", 50, 0, 2);
  hs.add("trackdzavg", "SV avg{trk_{i} dz(PV)} (cm)", 50, 0, 1);
  hs.add("trackdzrms", "SV rms{trk_{i} dz(PV)} (cm)", 50, 0, 0.5);

  hs.add("trackpterrmin", "SV min{frac. #sigma trk_{i} p_{T}}", 32, 0, 2);
  hs.add("trackpterrmax", "SV max{frac. #sigma trk_{i} p_{T}}", 32, 0, 2);
  hs.add("trackpterravg", "SV avg{frac. #sigma trk_{i} p_{T}}", 32, 0, 2);
  hs.add("trackpterrrms", "SV rms{frac. #sigma trk_{i} p_{T}}", 32, 0, 2);

  hs.add("tracketaerrmin", "SV min{frac. #sigma trk_{i} #eta}", 32, 0, 0.002);
  hs.add("tracketaerrmax", "SV max{frac. #sigma trk_{i} #eta}", 32, 0, 0.005);
  hs.add("tracketaerravg", "SV avg{frac. #sigma trk_{i} #eta}", 32, 0, 0.002);
  hs.add("tracketaerrrms", "SV rms{frac. #sigma trk_{i} #eta}", 32, 0, 0.002);

  hs.add("trackphierrmin", "SV min{frac. #sigma trk_{i} #phi}", 32, 0, 0.002);
  hs.add("trackphierrmax", "SV max{frac. #sigma trk_{i} #phi}", 32, 0, 0.005);
  hs.add("trackphierravg", "SV avg{frac. #sigma trk_{i} #phi}", 32, 0, 0.002);
  hs.add("trackphierrrms", "SV rms{frac. #sigma trk_{i} #phi}", 32, 0, 0.002);

  hs.add("trackdxyerrmin", "SV min{#sigma trk_{i} dxy(BS)} (cm)", 32, 0, 0.004);
  hs.add("trackdxyerrmax", "SV max{#sigma trk_{i} dxy(BS)} (cm)", 32, 0, 0.1);
  hs.add("trackdxyerravg", "SV avg{#sigma trk_{i} dxy(BS)} (cm)", 32, 0, 0.1);
  hs.add("trackdxyerrrms", "SV rms{#sigma trk_{i} dxy(BS)} (cm)", 32, 0, 0.1);

  hs.add("trackdzerrmin", "SV min{#sigma trk_{i} dz(PV)} (cm)", 32, 0, 0.01);
  hs.add("trackdzerrmax", "SV max{#sigma trk_{i} dz(PV)} (cm)", 32, 0, 0.1);
  hs.add("trackdzerravg", "SV avg{#sigma trk_{i} dz(PV)} (cm)", 32, 0, 0.1);
  hs.add("trackdzerrrms", "SV rms{#sigma trk_{i} dz(PV)} (cm)", 32, 0, 0.1);

  hs.add("trackpairdetamin", "SV min{#Delta #eta(i,j)}", 150,    0,       1.5);
  hs.add("trackpairdetamax", "SV max{#Delta #eta(i,j)}", 150,    0,       7);
  hs.add("trackpairdetaavg", "SV avg{#Delta #eta(i,j)}", 150,    0,       5);
  hs.add("trackpairdetarms", "SV rms{#Delta #eta(i,j)}", 150,    0,       3);

  hs.add("trackpairdphimax",   "SV max{|#Delta #phi(i,j)|}",   100, 0, 3.15);
  hs.add("trackpairdphimaxm1", "SV max-1{|#Delta #phi(i,j)|}", 100, 0, 3.15);
  hs.add("trackpairdphimaxm2", "SV max-2{|#Delta #phi(i,j)|}", 100, 0, 3.15);

  hs.add("drmin",                         "SV min{#Delta R(i,j)}",                                                       150,    0,       1.5);
  hs.add("drmax",                         "SV max{#Delta R(i,j)}",                                                       150,    0,       7);
  hs.add("dravg",                         "SV avg{#Delta R(i,j)}",                                                       150,    0,       5);
  hs.add("drrms",                         "SV rms{#Delta R(i,j)}",                                                       150,    0,       3);

  hs.add("costhtkmomvtxdispmin", "SV min{cos(angle(trk_{i}, SV-PV))}", 50, -1, 1);
  hs.add("costhtkmomvtxdispmax", "SV max{cos(angle(trk_{i}, SV-PV))}", 50, -1, 1);
  hs.add("costhtkmomvtxdispavg", "SV avg{cos(angle(trk_{i}, SV-PV))}", 50, -1, 1);
  hs.add("costhtkmomvtxdisprms", "SV rms{cos(angle(trk_{i}, SV-PV))}", 50,  0, 1);

  hs.add("costhjetmomvtxdispmin", "SV min{cos(angle(jet_{i}, SV-PV))}", 50, -1, 1);
  hs.add("costhjetmomvtxdispmax", "SV max{cos(angle(jet_{i}, SV-PV))}", 50, -1, 1);
  hs.add("costhjetmomvtxdispavg", "SV avg{cos(angle(jet_{i}, SV-PV))}", 50, -1, 1);
  hs.add("costhjetmomvtxdisprms", "SV rms{cos(angle(jet_{i}, SV-PV))}", 50,  0, 1);

  hs.add("gen2ddist",                     "dist2d(SV, closest gen vtx) (cm)",                                            200,    0,       0.2);
  hs.add("gen2derr",                      "#sigma(dist2d(SV, closest gen vtx)) (cm)",                                    200,    0,       0.2);
  hs.add("gen2dsig",                      "N#sigma(dist2d(SV, closest gen vtx)) (cm)",                                   200,    0,     100);
  hs.add("gen3ddist",                     "dist3d(SV, closest gen vtx) (cm)",                                            200,    0,       0.2);
  hs.add("gen3derr",                      "#sigma(dist3d(SV, closest gen vtx)) (cm)",                                    200,    0,       0.2);
  hs.add("gen3dsig",                      "N#sigma(dist3d(SV, closest gen vtx)) (cm)",                                   200,    0,     100);
  hs.add("bs2ddist",                      "dist2d(SV, beamspot) (cm)",                                                   500,    0,      2.5);
  hs.add("bsbs2ddist",                    "dist2d(SV, beamspot) (cm)",                                                   500,    0,      2.5);
  hs.add("bs2derr",                       "#sigma(dist2d(SV, beamspot)) (cm)",                                           100,    0,       0.05);
  hs.add("bs2dsig",                       "N#sigma(dist2d(SV, beamspot))",                                               100,    0,     100);
  hs.add("pv2ddist",                      "dist2d(SV, PV) (cm)",                                                         100,    0,       0.5);
  hs.add("pv2derr",                       "#sigma(dist2d(SV, PV)) (cm)",                                                 100,    0,       0.05);
  hs.add("pv2dsig",                       "N#sigma(dist2d(SV, PV))",                                                     100,    0,     100);
  hs.add("pv3ddist",                      "dist3d(SV, PV) (cm)",                                                         100,    0,       0.5);
  hs.add("pv3derr",                       "#sigma(dist3d(SV, PV)) (cm)",                                                 100,    0,       0.1);
  hs.add("pv3dsig",                       "N#sigma(dist3d(SV, PV))",                                                     100,    0,     100);
  hs.add("pvdz",                          "dz(SV, PV) (cm)",                                                             100,    0,       0.5);
  hs.add("pvdzerr",                       "#sigma(dz(SV, PV)) (cm)",                                                     100,    0,       0.1);
  hs.add("pvdzsig",                       "N#sigma(dz(SV, PV))",                                                         100,    0,     100);

  const char* lmt_ex[4] = {"", "loose_b", "medium_b", "tight_b"};
  for (int i = 0; i < 4; ++i) {
    hs.add(TString::Format("jet%d_deltaphi0", i).Data(), TString::Format("|#Delta#phi| to closest %sjet", lmt_ex[i]).Data(),               25, 0,   3.15);
    hs.add(TString::Format("jet%d_deltaphi1", i).Data(), TString::Format("|#Delta#phi| to next closest %sjet", lmt_ex[i]).Data(),          25, 0,   3.15);
  }

  for (int i = 0; i < 3; ++i) {
    float l = i == 2 ? 25 : 4;
    h_sv_pos_1d[i] = fs->make<TH1F>(TString::Format("h_sv_pos_1d_%i", i), TString::Format(";SV pos[%i] (cm);arb. units", i), 100, -l, l);
    h_sv_pos_bs1d[i] = fs->make<TH1F>(TString::Format("h_sv_pos_bs1d_%i", i), TString::Format(";SV pos[%i] (cm);arb. units", i), 100, -l, l);
  }
  h_sv_pos_2d[0] = fs->make<TH2F>("h_sv_pos_2d_xy", ";SV x (cm);SV y (cm)", 100, -4, 4, 100, -4, 4);
  h_sv_pos_2d[1] = fs->make<TH2F>("h_sv_pos_2d_xz", ";SV x (cm);SV z (cm)", 100, -4, 4, 100, -25, 25);
  h_sv_pos_2d[2] = fs->make<TH2F>("h_sv_pos_2d_yz", ";SV y (cm);SV z (cm)", 100, -4, 4, 100, -25, 25);
  h_sv_pos_rz    = fs->make<TH2F>("h_sv_pos_rz", ";SV r (cm);SV z (cm)", 100, -4, 4, 100, -25, 25);
  h_sv_pos_phi   = fs->make<TH1F>("h_sv_pos_phi", ";SV phi;arb. units", 25, -3.15, 3.15);
  h_sv_pos_phi_pv = fs->make<TH1F>("h_sv_pos_phi_pv", ";SV phi w.r.t. PV;arb. units", 25, -3.15, 3.15);
  h_sv_pos_bs2d[0] = fs->make<TH2F>("h_sv_pos_bs2dxy", ";SV x (cm);SV y (cm)", 100, -4, 4, 100, -4, 4);
  h_sv_pos_bs2d[1] = fs->make<TH2F>("h_sv_pos_bs2dxz", ";SV x (cm);SV z (cm)", 100, -4, 4, 100, -25, 25);
  h_sv_pos_bs2d[2] = fs->make<TH2F>("h_sv_pos_bs2dyz", ";SV y (cm);SV z (cm)", 100, -4, 4, 100, -25, 25);
  h_sv_pos_bsrz    = fs->make<TH2F>("h_sv_pos_bsrz", ";SV r (cm);SV z (cm)", 100, -4, 4, 100, -25, 25);
  h_sv_pos_bsphi   = fs->make<TH1F>("h_sv_pos_bsphi", ";SV phi;arb. units", 25, -3.15, 3.15);

  for (int j = 0; j < sv_num_indices; ++j) {
    const char* exc = sv_index_names[j];

    h_sv[j].Init("h_sv_" + std::string(exc), hs, true, do_scatterplots);

    for (int i = 0; i < 4; ++i)
      h_sv_jets_deltaphi[i][j] = fs->make<TH1F>(TString::Format("h_sv_%s_%sjets_deltaphi", exc, lmt_ex[i]), TString::Format(";%s SV #Delta#phi to %sjets;arb. units", exc, lmt_ex[i]), 50, -3.15, 3.15);

    h_sv_bs2derr_bsbs2ddist[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_bsbs2ddist", exc), TString::Format("%s SV;dist2d(SV, beamspot) (cm);#sigma(dist2d(SV, beamspot)) (cm)", exc), 500, 0, 2.5, 100, 0, 0.05);
    h_pvrho_bsbs2ddist[j] = fs->make<TH2F>(TString::Format("h_pvrho_sv_%s_bsbs2ddist", exc), TString::Format("%s SV;dist2d(SV, beamspot) (cm);dist2d(PV, beamspot)) (cm)", exc), 5000, 0, 2.5, 200, 0, 0.1);

    h_sv_track_weight[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_weight", exc), TString::Format(";%s SV tracks weight;arb. units", exc), 21, 0, 1.05);
    h_sv_track_q[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_q", exc), TString::Format(";%s SV tracks charge;arb. units.", exc), 4, -2, 2);
    h_sv_track_pt[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_pt", exc), TString::Format(";%s SV tracks p_{T} (GeV);arb. units", exc), 200, 0, 200);
    h_sv_track_eta[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_eta", exc), TString::Format(";%s SV tracks #eta;arb. units", exc), 50, -4, 4);
    h_sv_track_phi[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_phi", exc), TString::Format(";%s SV tracks #phi;arb. units", exc), 50, -3.15, 3.15);
    h_sv_track_dxy[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_dxy", exc), TString::Format(";%s SV tracks dxy (cm);arb. units", exc), 100, 0, 1);
    h_sv_track_dz[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_dz", exc), TString::Format(";%s SV tracks dz (cm);arb. units", exc), 100, 0, 1);
    h_sv_track_pt_err[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_pt_err", exc), TString::Format(";%s SV tracks #sigma(p_{T})/p_{T};arb. units", exc), 200, 0, 2);
    h_sv_track_eta_err[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_eta_err", exc), TString::Format(";%s SV tracks #sigma(#eta);arb. units", exc), 200, 0, 0.02);
    h_sv_track_phi_err[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_phi_err", exc), TString::Format(";%s SV tracks #sigma(#phi);arb. units", exc), 200, 0, 0.02);
    h_sv_track_dxy_err[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_dxy_err", exc), TString::Format(";%s SV tracks #sigma(dxy) (cm);arb. units", exc), 100, 0, 0.1);
    h_sv_track_dz_err[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_dz_err", exc), TString::Format(";%s SV tracks #sigma(dz) (cm);arb. units", exc), 100, 0, 0.1);
    h_sv_track_chi2dof[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_chi2dof", exc), TString::Format(";%s SV tracks #chi^{2}/dof;arb. units", exc), 100, 0, 10);
    h_sv_track_npxhits[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_npxhits", exc), TString::Format(";%s SV tracks number of pixel hits;arb. units", exc), 12, 0, 12);
    h_sv_track_nsthits[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_nsthits", exc), TString::Format(";%s SV tracks number of strip hits;arb. units", exc), 28, 0, 28);
    h_sv_track_nhitsbehind[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_nhitsbehind", exc), TString::Format(";%s SV tracks number of hits behind;arb. units", exc), 10, 0, 10);
    h_sv_track_nhitslost[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_nhitslost", exc), TString::Format(";%s SV tracks number of hits lost;arb. units", exc), 10, 0, 10);
    h_sv_track_nhits[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_nhits", exc), TString::Format(";%s SV tracks number of hits", exc), 40, 0, 40);
    h_sv_track_injet[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_injet", exc), TString::Format(";%s SV tracks in-jet?", exc), 2, 0, 2);
    h_sv_track_inpv[j] = fs->make<TH1F>(TString::Format("h_sv_%s_track_inpv", exc), TString::Format(";%s SV tracks in-PV?", exc), 10, -1, 9);

    h_sv_track_nsthits_track_eta[j] = fs->make<TH2F>(TString::Format("h_sv_%s_track_nsthits_track_eta", exc), TString::Format("%s SV;tracks #eta;tracks number of strip hits;", exc), 50, -4, 4, 28, 0, 28);
    h_sv_bs2derr_track_pt[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_track_pt", exc), TString::Format("%s SV;tracks p_{T} (GeV);#sigma(dist2d(SV, beamspot)) (cm)", exc), 200, 0, 200, 100, 0, 0.05);
    h_sv_bs2derr_track_eta[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_track_eta", exc), TString::Format("%s SV;tracks #eta;#sigma(dist2d(SV, beamspot)) (cm)", exc), 50, -4, 4, 100, 0, 0.05);
    h_sv_bs2derr_track_phi[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_track_phi", exc), TString::Format("%s SV;tracks #phi;#sigma(dist2d(SV, beamspot)) (cm)", exc), 50, -3.15, 3.15, 100, 0, 0.05);
    h_sv_bs2derr_track_dxy[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_track_dxy", exc), TString::Format("%s SV;tracks dxy (cm);#sigma(dist2d(SV, beamspot)) (cm)", exc), 100, 0, 1, 100, 0, 0.05);
    h_sv_bs2derr_track_dz[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_track_dz", exc), TString::Format("%s SV;tracks dz (cm);#sigma(dist2d(SV, beamspot)) (cm)", exc), 100, 0, 1, 100, 0, 0.05);
    h_sv_bs2derr_track_pt_err[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_track_pt_err", exc), TString::Format("%s SV;tracks #sigma(p_{T})/p_{T};#sigma(dist2d(SV, beamspot)) (cm)", exc), 200, 0, 2, 100, 0, 0.05);
    h_sv_bs2derr_track_eta_err[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_track_eta_err", exc), TString::Format("%s SV;tracks #sigma(#eta);#sigma(dist2d(SV, beamspot)) (cm)", exc), 200, 0, 0.02, 100, 0, 0.05);
    h_sv_bs2derr_track_phi_err[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_track_phi_err", exc), TString::Format("%s SV;tracks #sigma(#phi);#sigma(dist2d(SV, beamspot)) (cm)", exc), 200, 0, 0.02, 100, 0, 0.05);
    h_sv_bs2derr_track_dxy_err[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_track_dxy_err", exc), TString::Format("%s SV;tracks #sigma(dxy) (cm);#sigma(dist2d(SV, beamspot)) (cm)", exc), 100, 0, 0.1, 100, 0, 0.05);
    h_sv_bs2derr_track_dz_err[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_track_dz_err", exc), TString::Format("%s SV;tracks #sigma(dz) (cm);#sigma(dist2d(SV, beamspot)) (cm)", exc), 100, 0, 0.1, 100, 0, 0.05);
    h_sv_bs2derr_track_npxhits[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_track_npxhits", exc), TString::Format("%s SV;tracks number of pixel hits;#sigma(dist2d(SV, beamspot)) (cm)", exc), 12, 0, 12, 100, 0, 0.05);
    h_sv_bs2derr_track_nsthits[j] = fs->make<TH2F>(TString::Format("h_sv_%s_bs2derr_track_nsthits", exc), TString::Format("%s SV;tracks number of strip hits;#sigma(dist2d(SV, beamspot)) (cm)", exc), 28, 0, 28, 100, 0, 0.05);
  }

  h_svdist2d = fs->make<TH1F>("h_svdist2d", ";dist2d(sv #0, #1) (cm);arb. units", 500, 0, 1);
  h_svdist3d = fs->make<TH1F>("h_svdist3d", ";dist3d(sv #0, #1) (cm);arb. units", 500, 0, 1);
  h_sv0pvdz_v_sv1pvdz = fs->make<TH2F>("h_sv0pvdz_v_sv1pvdz", ";sv #1 dz to PV (cm);sv #0 dz to PV (cm)", 100, 0, 0.5, 100, 0, 0.5);
  h_sv0pvdzsig_v_sv1pvdzsig = fs->make<TH2F>("h_sv0pvdzsig_v_sv1pvdzsig", ";N#sigma(sv #1 dz to PV);sv N#sigma(#0 dz to PV)", 100, 0, 50, 100, 0, 50);
  h_absdeltaphi01 = fs->make<TH1F>("h_absdeltaphi01", ";abs(delta(phi of sv #0, phi of sv #1));arb. units", 315, 0, 3.15);
  h_fractrackssharedwpv01 = fs->make<TH1F>("h_fractrackssharedwpv01", ";fraction of sv #0 and sv #1 tracks shared with the PV;arb. units", 41, 0, 1.025);
  h_fractrackssharedwpvs01 = fs->make<TH1F>("h_fractrackssharedwpvs01", ";fraction of sv #0 and sv #1 tracks shared with any PV;arb. units", 41, 0, 1.025);
  h_pvmosttracksshared = fs->make<TH2F>("h_pvmosttracksshared", ";index of pv most-shared to sv #0; index of pv most-shared to sv #1", 71, -1, 70, 71, -1, 70);
}

// JMTBAD get rid of this crap
void MFVVertexHistos::fill_multi(TH1F** hs, const int, const double val, const double weight) const {
  hs[sv_all]->Fill(val, weight);
}

void MFVVertexHistos::fill_multi(TH2F** hs, const int, const double val, const double val2, const double weight) const {
  hs[sv_all]->Fill(val, val2, weight);
}

void MFVVertexHistos::fill_multi(PairwiseHistos* hs, const int, const PairwiseHistos::ValueMap& val, const double weight) const {
  hs[sv_all].Fill(val, -1, weight);
}

void MFVVertexHistos::analyze(const edm::Event& event, const edm::EventSetup&) {
  edm::Handle<MFVEvent> mevent;
  event.getByToken(mevent_token, mevent);

  edm::Handle<double> weight;
  event.getByToken(weight_token, weight);
  const double w = *weight;
  h_w->Fill(w);

  const double bsx = force_bs.size() ? force_bs[0] : mevent->bsx;
  const double bsy = force_bs.size() ? force_bs[1] : mevent->bsy;
  const double bsz = force_bs.size() ? force_bs[2] : mevent->bsz;
  const math::XYZPoint bs(bsx, bsy, bsz);
  const math::XYZPoint pv(mevent->pvx, mevent->pvy, mevent->pvz);

  edm::Handle<MFVVertexAuxCollection> auxes;
  event.getByToken(vertex_token, auxes);

  const int nsv = int(auxes->size());

  for (int isv = 0; isv < nsv; ++isv) {
    const MFVVertexAux& aux = auxes->at(isv);

    h_sv_pos_1d[0]->Fill(aux.x - bsx, w);
    h_sv_pos_1d[1]->Fill(aux.y - bsy, w);
    h_sv_pos_1d[2]->Fill(aux.z - bsz, w);
    h_sv_pos_2d[0]->Fill(aux.x - bsx, aux.y - bsy, w);
    h_sv_pos_2d[1]->Fill(aux.x - bsx, aux.z - bsz, w);
    h_sv_pos_2d[2]->Fill(aux.y - bsy, aux.z - bsz, w);
    h_sv_pos_rz->Fill(aux.bs2ddist * (aux.y - bsy >= 0 ? 1 : -1), aux.z - bsz, w);
    const double pos_phi = atan2(aux.y - bsy, aux.x - bsx);
    h_sv_pos_phi->Fill(pos_phi, w);
    h_sv_pos_phi_pv->Fill(atan2(aux.y - mevent->pvy, aux.x - mevent->pvx), w);

    h_sv_pos_bs1d[0]->Fill(aux.x - mevent->bsx_at_z(aux.z), w);
    h_sv_pos_bs1d[1]->Fill(aux.y - mevent->bsy_at_z(aux.z), w);
    h_sv_pos_bs1d[2]->Fill(aux.z - bsz, w);
    h_sv_pos_bs2d[0]->Fill(aux.x - mevent->bsx_at_z(aux.z), aux.y - mevent->bsy_at_z(aux.z), w);
    h_sv_pos_bs2d[1]->Fill(aux.x - mevent->bsx_at_z(aux.z), aux.z - bsz, w);
    h_sv_pos_bs2d[2]->Fill(aux.y - mevent->bsy_at_z(aux.z), aux.z - bsz, w);
    h_sv_pos_bsrz->Fill(mevent->bs2ddist(aux) * (aux.y - mevent->bsy_at_z(aux.z) >= 0 ? 1 : -1), aux.z - bsz, w);
    const double pos_bsphi = atan2(aux.y - mevent->bsy_at_z(aux.z), aux.x - mevent->bsx_at_z(aux.z));
    h_sv_pos_bsphi->Fill(pos_bsphi, w);

    MFVVertexAux::stats trackpairdeta_stats(&aux, aux.trackpairdetas());
    MFVVertexAux::stats   trackpairdr_stats(&aux, aux.trackpairdrs());

    PairwiseHistos::ValueMap v = {
        {"nlep",                    aux.which_lep.size()},
        {"ntracks",                 aux.ntracks()},
        {"ntracksptgt3",            aux.ntracksptgt(3)},
        {"ntracksptgt10",           aux.ntracksptgt(10)},
        {"trackminnhits",           aux.trackminnhits()},
        {"trackmaxnhits",           aux.trackmaxnhits()},
        {"njetsntks",               aux.njets[mfv::JByNtracks]},
        {"chi2dof",                 aux.chi2dof()},
        {"chi2dofprob",             TMath::Prob(aux.chi2, aux.ndof())},

        {"tkonlyp",             aux.p4(mfv::PTracksOnly).P()},
        {"tkonlypt",            aux.pt[mfv::PTracksOnly]},
        {"tkonlyeta",           aux.eta[mfv::PTracksOnly]},
        {"tkonlyrapidity",      aux.p4(mfv::PTracksOnly).Rapidity()},
        {"tkonlyphi",           aux.phi[mfv::PTracksOnly]},
        {"tkonlymass",          aux.mass[mfv::PTracksOnly]},

        {"jetsntkp",             aux.p4(mfv::PJetsByNtracks).P()},
        {"jetsntkpt",            aux.pt[mfv::PJetsByNtracks]},
        {"jetsntketa",           aux.eta[mfv::PJetsByNtracks]},
        {"jetsntkrapidity",      aux.p4(mfv::PJetsByNtracks).Rapidity()},
        {"jetsntkphi",           aux.phi[mfv::PJetsByNtracks]},
        {"jetsntkmass",          aux.mass[mfv::PJetsByNtracks]},

        {"tksjetsntkp",             aux.p4(mfv::PTracksPlusJetsByNtracks).P()},
        {"tksjetsntkpt",            aux.pt[mfv::PTracksPlusJetsByNtracks]},
        {"tksjetsntketa",           aux.eta[mfv::PTracksPlusJetsByNtracks]},
        {"tksjetsntkrapidity",      aux.p4(mfv::PTracksPlusJetsByNtracks).Rapidity()},
        {"tksjetsntkphi",           aux.phi[mfv::PTracksPlusJetsByNtracks]},
        {"tksjetsntkmass",          aux.mass[mfv::PTracksPlusJetsByNtracks]},

        {"costhtkonlymombs",         aux.costhmombs  (mfv::PTracksOnly)},
        {"costhtkonlymompv2d",       aux.costhmompv2d(mfv::PTracksOnly)},
        {"costhtkonlymompv3d",       aux.costhmompv3d(mfv::PTracksOnly)},

        {"costhjetsntkmombs",        aux.costhmombs  (mfv::PJetsByNtracks)},
        {"costhjetsntkmompv2d",      aux.costhmompv2d(mfv::PJetsByNtracks)},
        {"costhjetsntkmompv3d",      aux.costhmompv3d(mfv::PJetsByNtracks)},

        {"costhtksjetsntkmombs",     aux.costhmombs  (mfv::PTracksPlusJetsByNtracks)},
        {"costhtksjetsntkmompv2d",   aux.costhmompv2d(mfv::PTracksPlusJetsByNtracks)},
        {"costhtksjetsntkmompv3d",   aux.costhmompv3d(mfv::PTracksPlusJetsByNtracks)},

        {"missdisttkonlypv",        aux.missdistpv   [mfv::PTracksOnly]},
        {"missdisttkonlypverr",     aux.missdistpverr[mfv::PTracksOnly]},
        {"missdisttkonlypvsig",     aux.missdistpvsig(mfv::PTracksOnly)},

        {"missdistjetsntkpv",        aux.missdistpv   [mfv::PJetsByNtracks]},
        {"missdistjetsntkpverr",     aux.missdistpverr[mfv::PJetsByNtracks]},
        {"missdistjetsntkpvsig",     aux.missdistpvsig(mfv::PJetsByNtracks)},

        {"missdisttksjetsntkpv",        aux.missdistpv   [mfv::PTracksPlusJetsByNtracks]},
        {"missdisttksjetsntkpverr",     aux.missdistpverr[mfv::PTracksPlusJetsByNtracks]},
        {"missdisttksjetsntkpvsig",     aux.missdistpvsig(mfv::PTracksPlusJetsByNtracks)},

        {"sumpt2",                  aux.sumpt2()},

        {"ntrackssharedwpv", aux.ntrackssharedwpv()},
        {"ntrackssharedwpvs", aux.ntrackssharedwpvs()},
        {"fractrackssharedwpv", float(aux.ntrackssharedwpv()) / aux.ntracks()},
        {"fractrackssharedwpvs", float(aux.ntrackssharedwpvs()) / aux.ntracks()},
        {"npvswtracksshared", aux.npvswtracksshared()},

        {"trackdxymin", aux.trackdxymin()},
        {"trackdxymax", aux.trackdxymax()},
        {"trackdxyavg", aux.trackdxyavg()},
        {"trackdxyrms", aux.trackdxyrms()},

        {"trackdzmin", aux.trackdzmin()},
        {"trackdzmax", aux.trackdzmax()},
        {"trackdzavg", aux.trackdzavg()},
        {"trackdzrms", aux.trackdzrms()},

        {"trackpterrmin", aux.trackpterrmin()},
        {"trackpterrmax", aux.trackpterrmax()},
        {"trackpterravg", aux.trackpterravg()},
        {"trackpterrrms", aux.trackpterrrms()},

        {"tracketaerrmin", aux.tracketaerrmin()},
        {"tracketaerrmax", aux.tracketaerrmax()},
        {"tracketaerravg", aux.tracketaerravg()},
        {"tracketaerrrms", aux.tracketaerrrms()},

        {"trackphierrmin", aux.trackphierrmin()},
        {"trackphierrmax", aux.trackphierrmax()},
        {"trackphierravg", aux.trackphierravg()},
        {"trackphierrrms", aux.trackphierrrms()},

        {"trackdxyerrmin", aux.trackdxyerrmin()},
        {"trackdxyerrmax", aux.trackdxyerrmax()},
        {"trackdxyerravg", aux.trackdxyerravg()},
        {"trackdxyerrrms", aux.trackdxyerrrms()},

        {"trackdzerrmin", aux.trackdzerrmin()},
        {"trackdzerrmax", aux.trackdzerrmax()},
        {"trackdzerravg", aux.trackdzerravg()},
        {"trackdzerrrms", aux.trackdzerrrms()},

        {"trackpairdetamin", trackpairdeta_stats.min},
        {"trackpairdetamax", trackpairdeta_stats.max},
        {"trackpairdetaavg", trackpairdeta_stats.avg},
        {"trackpairdetarms", trackpairdeta_stats.rms},

        {"drmin",  trackpairdr_stats.min},
        {"drmax",  trackpairdr_stats.max},
        {"dravg",  trackpairdr_stats.avg},
        {"drrms",  trackpairdr_stats.rms},

        {"costhtkmomvtxdispmin", aux.costhtkmomvtxdispmin()},
        {"costhtkmomvtxdispmax", aux.costhtkmomvtxdispmax()},
        {"costhtkmomvtxdispavg", aux.costhtkmomvtxdispavg()},
        {"costhtkmomvtxdisprms", aux.costhtkmomvtxdisprms()},

        {"costhjetmomvtxdispmin", aux.costhjetmomvtxdispmin()},
        {"costhjetmomvtxdispmax", aux.costhjetmomvtxdispmax()},
        {"costhjetmomvtxdispavg", aux.costhjetmomvtxdispavg()},
        {"costhjetmomvtxdisprms", aux.costhjetmomvtxdisprms()},

        {"gen2ddist",               aux.gen2ddist},
        {"gen2derr",                aux.gen2derr},
        {"gen2dsig",                aux.gen2dsig()},
        {"gen3ddist",               aux.gen3ddist},
        {"gen3derr",                aux.gen3derr},
        {"gen3dsig",                aux.gen3dsig()},
        {"bs2ddist",                aux.bs2ddist},
        {"bsbs2ddist",              mevent->bs2ddist(aux)},
        {"bs2derr",                 aux.bs2derr},
        {"bs2dsig",                 aux.bs2dsig()},
        {"pv2ddist",                aux.pv2ddist},
        {"pv2derr",                 aux.pv2derr},
        {"pv2dsig",                 aux.pv2dsig()},
        {"pv3ddist",                aux.pv3ddist},
        {"pv3derr",                 aux.pv3derr},
        {"pv3dsig",                 aux.pv3dsig()},
        {"pvdz",                    aux.pvdz()},
        {"pvdzerr",                 aux.pvdzerr()},
        {"pvdzsig",                 aux.pvdzsig()}
    };

    std::vector<float> trackpairdphis = aux.trackpairdphis();
    int npairs = trackpairdphis.size();
    for (int i = 0; i < npairs; ++i) {
      trackpairdphis[i] = fabs(trackpairdphis[i]);
    }
    std::sort(trackpairdphis.begin(), trackpairdphis.end());
    v["trackpairdphimax"] = 0 > npairs - 1 ? -1 : trackpairdphis[npairs-1-0];
    v["trackpairdphimaxm1"] = 1 > npairs - 1 ? -1 : trackpairdphis[npairs-1-1];
    v["trackpairdphimaxm2"] = 2 > npairs - 1 ? -1 : trackpairdphis[npairs-1-2];

    std::vector<double> jetdeltaphis;
    for (int i = 0; i < 4; ++i) {
      jetdeltaphis.clear();
      for (size_t ijet = 0; ijet < mevent->jet_id.size(); ++ijet) {
        if (mevent->jet_pt[ijet] < mfv::min_jet_pt)
          continue;
        if (((mevent->jet_id[ijet] >> 2) & 3) >= i) {
          fill_multi(h_sv_jets_deltaphi[i], isv, reco::deltaPhi(atan2(aux.y - bsy, aux.x - bsx), mevent->jet_phi[ijet]), w);
          jetdeltaphis.push_back(fabs(reco::deltaPhi(atan2(aux.y - bsy, aux.x - bsx), mevent->jet_phi[ijet])));
        }
      }
      std::sort(jetdeltaphis.begin(), jetdeltaphis.end());
      int njets = jetdeltaphis.size();
      v[TString::Format("jet%d_deltaphi0", i).Data()] = 0 > njets - 1 ? -1 : jetdeltaphis[0];
      v[TString::Format("jet%d_deltaphi1", i).Data()] = 1 > njets - 1 ? -1 : jetdeltaphis[1];
    }

    fill_multi(h_sv_bs2derr_bsbs2ddist, isv, mevent->bs2ddist(aux), aux.bs2derr, w);
    fill_multi(h_pvrho_bsbs2ddist, isv, mevent->bs2ddist(aux), mevent->pv_rho(), w);

    for (int i = 0; i < int(aux.ntracks()); ++i) {
      fill_multi(h_sv_track_weight, isv, aux.track_weight(i), w);
      fill_multi(h_sv_track_q, isv, aux.track_q(i), w);
      fill_multi(h_sv_track_pt, isv, aux.track_pt(i), w);
      fill_multi(h_sv_track_eta, isv, aux.track_eta[i], w);
      fill_multi(h_sv_track_phi, isv, aux.track_phi[i], w);
      fill_multi(h_sv_track_dxy, isv, aux.track_dxy[i], w);
      fill_multi(h_sv_track_dz, isv, aux.track_dz[i], w);
      fill_multi(h_sv_track_pt_err, isv, aux.track_pt_err[i], w);
      fill_multi(h_sv_track_eta_err, isv, aux.track_eta_err(i), w);
      fill_multi(h_sv_track_phi_err, isv, aux.track_phi_err(i), w);
      fill_multi(h_sv_track_dxy_err, isv, aux.track_dxy_err(i), w);
      fill_multi(h_sv_track_dz_err, isv, aux.track_dz_err(i), w);
      fill_multi(h_sv_track_chi2dof, isv, aux.track_chi2dof(i), w);
      fill_multi(h_sv_track_npxhits, isv, aux.track_npxhits(i), w);
      fill_multi(h_sv_track_nsthits, isv, aux.track_nsthits(i), w);
      fill_multi(h_sv_track_nhitsbehind, isv, aux.track_nhitsbehind(i), w);
      fill_multi(h_sv_track_nhitslost, isv, aux.track_nhitslost(i), w);
      fill_multi(h_sv_track_nhits, isv, aux.track_nhits(i), w);
      fill_multi(h_sv_track_injet, isv, aux.track_injet[i], w);
      fill_multi(h_sv_track_inpv, isv, aux.track_inpv[i], w);

      fill_multi(h_sv_track_nsthits_track_eta, isv, aux.track_eta[i], aux.track_nsthits(i), w);
      fill_multi(h_sv_bs2derr_track_pt, isv, aux.track_pt(i), aux.bs2derr, w);
      fill_multi(h_sv_bs2derr_track_eta, isv, aux.track_eta[i], aux.bs2derr, w);
      fill_multi(h_sv_bs2derr_track_phi, isv, aux.track_phi[i], aux.bs2derr, w);
      fill_multi(h_sv_bs2derr_track_dxy, isv, aux.track_dxy[i], aux.bs2derr, w);
      fill_multi(h_sv_bs2derr_track_dz, isv, aux.track_dz[i], aux.bs2derr, w);
      fill_multi(h_sv_bs2derr_track_pt_err, isv, aux.track_pt_err[i], aux.bs2derr, w);
      fill_multi(h_sv_bs2derr_track_eta_err, isv, aux.track_eta_err(i), aux.bs2derr, w);
      fill_multi(h_sv_bs2derr_track_phi_err, isv, aux.track_phi_err(i), aux.bs2derr, w);
      fill_multi(h_sv_bs2derr_track_dxy_err, isv, aux.track_dxy_err(i), aux.bs2derr, w);
      fill_multi(h_sv_bs2derr_track_dz_err, isv, aux.track_dz_err(i), aux.bs2derr, w);
      fill_multi(h_sv_bs2derr_track_npxhits, isv, aux.track_npxhits(i), aux.bs2derr, w);
      fill_multi(h_sv_bs2derr_track_nsthits, isv, aux.track_nsthits(i), aux.bs2derr, w);
    }

    if (do_trackplots) {
      int ntracksstgtn[11] = {0};

      std::vector<std::pair<int,float>> itk_pt;
      for (int i = 0; i < int(aux.ntracks()); ++i) {
        itk_pt.push_back(std::make_pair(i, aux.track_pt(i)));

        for (int j = 0; j < 11; ++j) {
          if (aux.track_nsthits(i) >= j) {
            ++ntracksstgtn[j];
          }
        }
      }

      for (int i = 0; i < 11; ++i) {
        v[TString::Format("ntracksstgt%i", i).Data()] = ntracksstgtn[i];
      }

      std::sort(itk_pt.begin(), itk_pt.end(), [](std::pair<int,float> itk_pt1, std::pair<int,float> itk_pt2) { return itk_pt1.second > itk_pt2.second; } );
      for (int i = 0; i < max_ntracks; ++i) {
        if (i < int(aux.ntracks())) {
          v[TString::Format("track%i_weight",        i).Data()] = aux.track_weight(itk_pt[i].first);
          v[TString::Format("track%i_q",             i).Data()] = aux.track_q(itk_pt[i].first);
          v[TString::Format("track%i_pt",            i).Data()] = aux.track_pt(itk_pt[i].first);
          v[TString::Format("track%i_eta",           i).Data()] = aux.track_eta[itk_pt[i].first];
          v[TString::Format("track%i_phi",           i).Data()] = aux.track_phi[itk_pt[i].first];
          v[TString::Format("track%i_dxy",           i).Data()] = aux.track_dxy[itk_pt[i].first];
          v[TString::Format("track%i_dz",            i).Data()] = aux.track_dz[itk_pt[i].first];
          v[TString::Format("track%i_pt_err",        i).Data()] = aux.track_pt_err[itk_pt[i].first];
          v[TString::Format("track%i_eta_err",       i).Data()] = aux.track_eta_err(itk_pt[i].first);
          v[TString::Format("track%i_phi_err",       i).Data()] = aux.track_phi_err(itk_pt[i].first);
          v[TString::Format("track%i_dxy_err",       i).Data()] = aux.track_dxy_err(itk_pt[i].first);
          v[TString::Format("track%i_dz_err",        i).Data()] = aux.track_dz_err(itk_pt[i].first);
          v[TString::Format("track%i_chi2dof",       i).Data()] = aux.track_chi2dof(itk_pt[i].first);
          v[TString::Format("track%i_npxhits",       i).Data()] = aux.track_npxhits(itk_pt[i].first);
          v[TString::Format("track%i_nsthits",       i).Data()] = aux.track_nsthits(itk_pt[i].first);
          v[TString::Format("track%i_nhitsbehind",   i).Data()] = aux.track_nhitsbehind(itk_pt[i].first);
          v[TString::Format("track%i_nhitslost",     i).Data()] = aux.track_nhitslost(itk_pt[i].first);
          v[TString::Format("track%i_nhits",         i).Data()] = aux.track_nhits(itk_pt[i].first);
          v[TString::Format("track%i_injet",         i).Data()] = aux.track_injet[itk_pt[i].first];
          v[TString::Format("track%i_inpv",          i).Data()] = aux.track_inpv[itk_pt[i].first];

          std::vector<double> jetdeltaphis;
          for (size_t ijet = 0; ijet < mevent->jet_id.size(); ++ijet) {
            if (mevent->jet_pt[ijet] < mfv::min_jet_pt)
              continue;
            jetdeltaphis.push_back(fabs(reco::deltaPhi(aux.track_phi[itk_pt[i].first], mevent->jet_phi[ijet])));
          }
          std::sort(jetdeltaphis.begin(), jetdeltaphis.end());
          int njets = jetdeltaphis.size();
          v[TString::Format("track%i_jet_deltaphi0", i).Data()] = 0 > njets - 1 ? -1 : jetdeltaphis[0];
        } else {
          v[TString::Format("track%i_weight",        i).Data()] = -1e6;
          v[TString::Format("track%i_q",             i).Data()] = -1e6;
          v[TString::Format("track%i_pt",            i).Data()] = -1e6;
          v[TString::Format("track%i_eta",           i).Data()] = -1e6;
          v[TString::Format("track%i_phi",           i).Data()] = -1e6;
          v[TString::Format("track%i_dxy",           i).Data()] = -1e6;
          v[TString::Format("track%i_dz",            i).Data()] = -1e6;
          v[TString::Format("track%i_pt_err",        i).Data()] = -1e6;
          v[TString::Format("track%i_eta_err",       i).Data()] = -1e6;
          v[TString::Format("track%i_phi_err",       i).Data()] = -1e6;
          v[TString::Format("track%i_dxy_err",       i).Data()] = -1e6;
          v[TString::Format("track%i_dz_err",        i).Data()] = -1e6;
          v[TString::Format("track%i_chi2dof",       i).Data()] = -1e6;
          v[TString::Format("track%i_npxhits",       i).Data()] = -1e6;
          v[TString::Format("track%i_nsthits",       i).Data()] = -1e6;
          v[TString::Format("track%i_nhitsbehind",   i).Data()] = -1e6;
          v[TString::Format("track%i_nhitslost",     i).Data()] = -1e6;
          v[TString::Format("track%i_nhits",         i).Data()] = -1e6;
          v[TString::Format("track%i_injet",         i).Data()] = -1e6;
          v[TString::Format("track%i_inpv",          i).Data()] = -1e6;
          v[TString::Format("track%i_jet_deltaphi0", i).Data()] = -1e6;
        }
      }
    }

    fill_multi(h_sv, isv, v, w);
  }

  //////////////////////////////////////////////////////////////////////

  h_nsv->Fill(nsv, w);

  if (nsv >= 2) {
    const MFVVertexAux& sv0 = auxes->at(0);
    const MFVVertexAux& sv1 = auxes->at(1);
    double svdist2d = mag(sv0.x - sv1.x, sv0.y - sv1.y);
    double svdist3d = mag(sv0.x - sv1.x, sv0.y - sv1.y, sv0.z - sv1.z);
    h_svdist2d->Fill(svdist2d, w);
    h_svdist3d->Fill(svdist3d, w);
    h_sv0pvdz_v_sv1pvdz->Fill(sv0.pvdz(), sv1.pvdz(), w);
    h_sv0pvdzsig_v_sv1pvdzsig->Fill(sv0.pvdzsig(), sv1.pvdzsig(), w);
    double phi0 = atan2(sv0.y - bsy, sv0.x - bsx);
    double phi1 = atan2(sv1.y - bsy, sv1.x - bsx);
    h_absdeltaphi01->Fill(fabs(reco::deltaPhi(phi0, phi1)), w);

    h_fractrackssharedwpv01 ->Fill(float(sv0.ntrackssharedwpv () + sv1.ntrackssharedwpv ())/(sv0.ntracks() + sv1.ntracks()), w);
    h_fractrackssharedwpvs01->Fill(float(sv0.ntrackssharedwpvs() + sv1.ntrackssharedwpvs())/(sv0.ntracks() + sv1.ntracks()), w);
    h_pvmosttracksshared->Fill(sv0.ntrackssharedwpvs() ? sv0.pvmosttracksshared() : -1,
                               sv1.ntrackssharedwpvs() ? sv1.pvmosttracksshared() : -1,
                               w);
  }
}

DEFINE_FWK_MODULE(MFVVertexHistos);
