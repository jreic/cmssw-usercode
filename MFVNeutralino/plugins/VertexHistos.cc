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
  const edm::InputTag mfv_event_src;
  const edm::InputTag vertex_aux_src;
  const edm::InputTag weight_src;
  const bool do_scatterplots;

  VertexDistanceXY distcalc_2d;
  VertexDistance3D distcalc_3d;

  TH1F* h_nsv;
  TH2F* h_nsv_v_minlspdist2d;
  TH2F* h_nsv_v_lspdist2d;
  TH2F* h_nsv_v_lspdist3d;

  // indices for h_sv below:
  enum sv_index { sv_best0, sv_best1, sv_best2, sv_rest, sv_top2, sv_all, sv_num_indices };
  static const char* sv_index_names[sv_num_indices];

  void fill_multi(TH1F** hs, const int isv, const double val, const double weight) const;
  void fill_multi(TH2F** hs, const int isv, const double val, const double val2, const double weight) const;
  void fill_multi(PairwiseHistos* hs, const int isv, const PairwiseHistos::ValueMap& val, const double weight) const;

  TH1F* h_sv_pos_1d[4][3];
  TH2F* h_sv_pos_2d[4][3];

  PairwiseHistos h_sv[sv_num_indices];
  PairwiseHistos h_sv_sums[3]; // top2, top3, all

  TH1F* h_svdist2d;
  TH1F* h_svdist3d;
  TH2F* h_svdist2d_v_lspdist2d;
  TH2F* h_svdist3d_v_lspdist3d;
  TH2F* h_svdist2d_v_minlspdist2d;
  TH2F* h_svdist2d_v_minbsdist2d;
  TH2F* h_sv0pvdz_v_sv1pvdz;
  TH2F* h_sv0pvdzsig_v_sv1pvdzsig;

  TH1F* h_pair2dcompatscss;
  TH1F* h_pair2dcompat;
  TH1F* h_pair2ddist;
  TH1F* h_pair2derr;
  TH1F* h_pair2dsig;
  TH1F* h_pair3dcompatscss;
  TH1F* h_pair3dcompat;
  TH1F* h_pair3ddist;
  TH1F* h_pair3derr;
  TH1F* h_pair3dsig;
};

const char* MFVVertexHistos::sv_index_names[MFVVertexHistos::sv_num_indices] = { "best0", "best1", "best2", "rest", "top2", "all" };

MFVVertexHistos::MFVVertexHistos(const edm::ParameterSet& cfg)
  : mfv_event_src(cfg.getParameter<edm::InputTag>("mfv_event_src")),
    vertex_aux_src(cfg.getParameter<edm::InputTag>("vertex_aux_src")),
    weight_src(cfg.getParameter<edm::InputTag>("weight_src")),
    do_scatterplots(cfg.getParameter<bool>("do_scatterplots"))
{
  edm::Service<TFileService> fs;

  h_nsv = fs->make<TH1F>("h_nsv", ";# of secondary vertices;arb. units", 15, 0, 15);
  h_nsv_v_minlspdist2d = fs->make<TH2F>("h_nsv_v_minlspdist2d", ";min dist2d(gen vtx #0, #1) (cm);# of secondary vertices", 600, 0, 3, 5, 0, 5);
  h_nsv_v_lspdist2d = fs->make<TH2F>("h_nsv_v_lspdist2d", ";dist2d(gen vtx #0, #1) (cm);# of secondary vertices", 600, 0, 3, 5, 0, 5);
  h_nsv_v_lspdist3d = fs->make<TH2F>("h_nsv_v_lspdist3d", ";dist3d(gen vtx #0, #1) (cm);# of secondary vertices", 600, 0, 3, 5, 0, 5);

  PairwiseHistos::HistoDefs hs;
  hs.add("ntracks",                       "# of tracks/SV",                                                               40,    0,      40);
  hs.add("nbadtracks",                    "# of 'bad' tracks/SV",                                                         40,    0,      40);
  hs.add("ntracksptgt3",                  "# of tracks/SV w/ p_{T} > 3 GeV",                                              40,    0,      40);
  hs.add("ntracksptgt5",                  "# of tracks/SV w/ p_{T} > 5 GeV",                                              40,    0,      40);
  hs.add("ntracksptgt10",                 "# of tracks/SV w/ p_{T} > 10 GeV",                                             40,    0,      40);
  hs.add("trackminnhits",                 "min number of hits on track per SV",                                           40,    0,      40);
  hs.add("trackmaxnhits",                 "max number of hits on track per SV",                                           40,    0,      40);
  hs.add("njetsntks",                     "# of jets assoc. by tracks to SV",                                             10,    0,      10);
  hs.add("chi2dof",                       "SV #chi^2/dof",                                                                50,    0,       7);
  hs.add("chi2dofprob",                   "SV p(#chi^2, dof)",                                                            50,    0,       1.2);

  hs.add("tkonlyp",                       "SV tracks-only p (GeV)",                                                      100,    0,     300);
  hs.add("tkonlypt",                      "SV tracks-only p_{T} (GeV)",                                                  100,    0,     300);
  hs.add("tkonlyeta",                     "SV tracks-only #eta",                                                          50,   -4,       4);
  hs.add("tkonlyrapidity",                "SV tracks-only rapidity",                                                      50,   -4,       4);
  hs.add("tkonlyphi",                     "SV tracks-only #phi",                                                          50,   -3.15,    3.15);
  hs.add("tkonlymass",                    "SV tracks-only mass (GeV)",                                                   100,    0,     250);

  hs.add("jetsntkp",                      "SV jets-by-ntracks -only p (GeV)",                                            100,    0,     300);
  hs.add("jetsntkpt",                     "SV jets-by-ntracks -only p_{T} (GeV)",                                        100,    0,     300);
  hs.add("jetsntketa",                    "SV jets-by-ntracks -only #eta",                                                50,   -4,       4);
  hs.add("jetsntkrapidity",               "SV jets-by-ntracks -only rapidity",                                            50,   -4,       4);
  hs.add("jetsntkphi",                    "SV jets-by-ntracks -only #phi",                                                50,   -3.15,    3.15);
  hs.add("jetsntkmass",                   "SV jets-by-ntracks -only mass (GeV)",                                         100,    0,    1500);

  hs.add("tksjetsntkp",                   "SV tracks-plus-jets-by-ntracks p (GeV)",                                      100,    0,     300);
  hs.add("tksjetsntkpt",                  "SV tracks-plus-jets-by-ntracks p_{T} (GeV)",                                  100,    0,     300);
  hs.add("tksjetsntketa",                 "SV tracks-plus-jets-by-ntracks #eta",                                          50,   -4,       4);
  hs.add("tksjetsntkrapidity",            "SV tracks-plus-jets-by-ntracks rapidity",                                      50,   -4,       4);
  hs.add("tksjetsntkphi",                 "SV tracks-plus-jets-by-ntracks #phi",                                          50,   -3.15,    3.15);
  hs.add("tksjetsntkmass",                "SV tracks-plus-jets-by-ntracks mass (GeV)",                                   100,    0,    1500);
				        
  hs.add("costhtkonlymombs",              "cos(angle(2-momentum (tracks-only), 2-dist to BS))",                          101,   -1,       1.02);
  hs.add("costhtkonlymompv2d",            "cos(angle(2-momentum (tracks-only), 2-dist to PV))",                          101,   -1,       1.02);
  hs.add("costhtkonlymompv3d",            "cos(angle(3-momentum (tracks-only), 3-dist to PV))",                          101,   -1,       1.02);

  hs.add("costhjetsntkmombs",             "cos(angle(2-momentum (jets-by-ntracks -only), 2-dist to BS))",                101,   -1,       1.02);
  hs.add("costhjetsntkmompv2d",           "cos(angle(2-momentum (jets-by-ntracks -only), 2-dist to PV))",                101,   -1,       1.02);
  hs.add("costhjetsntkmompv3d",           "cos(angle(3-momentum (jets-by-ntracks -only), 3-dist to PV))",                101,   -1,       1.02);

  hs.add("costhtksjetsntkmombs",          "cos(angle(2-momentum (tracks-plus-jets-by-ntracks), 2-dist to BS))",          101,   -1,       1.02);
  hs.add("costhtksjetsntkmompv2d",        "cos(angle(2-momentum (tracks-plus-jets-by-ntracks), 2-dist to PV))",          101,   -1,       1.02);
  hs.add("costhtksjetsntkmompv3d",        "cos(angle(3-momentum (tracks-plus-jets-by-ntracks), 3-dist to PV))",          101,   -1,       1.02);

  hs.add("missdisttkonlypv",              "miss dist. (tracks-only) of SV to PV (cm)",                                   100,    0,       2);
  hs.add("missdisttkonlypverr",           "#sigma(miss dist. (tracks-only) of SV to PV) (cm)",                           100,    0,       0.05);
  hs.add("missdisttkonlypvsig",           "N#sigma(miss dist. (tracks-only) of SV to PV) (cm)",                          100,    0,     100);

  hs.add("missdistjetsntkpv",             "miss dist. (jets-by-ntracks -only) of SV to PV (cm)",                         100,    0,       2);
  hs.add("missdistjetsntkpverr",          "#sigma(miss dist. (jets-by-ntracks -only) of SV to PV) (cm)",                 100,    0,       0.05);
  hs.add("missdistjetsntkpvsig",          "N#sigma(miss dist. (jets-by-ntracks -only) of SV to PV) (cm)",                100,    0,     100);

  hs.add("missdisttksjetsntkpv",          "miss dist. (tracks-plus-jets-by-ntracks) of SV to PV (cm)",                   100,    0,       2);
  hs.add("missdisttksjetsntkpverr",       "#sigma(miss dist. (tracks-plus-jets-by-ntracks) of SV to PV) (cm)",           100,    0,       0.05);
  hs.add("missdisttksjetsntkpvsig",       "N#sigma(miss dist. (tracks-plus-jets-by-ntracks) of SV to PV) (cm)",          100,    0,     100);
					  
  hs.add("sumpt2",                        "SV #Sigma p_{T}^{2} (GeV^2)",                                                 300,    0,    6000);
  hs.add("maxnhitsbehind",                "max number of hits behind SV",                                                 15,    0,      15);
  hs.add("sumnhitsbehind",                "sum number of hits behind SV",                                                100,    0,     100);

  hs.add("mintrackpt",                    "SV min{trk_{i} p_{T}} (GeV)",                                                  50,    0,      10);
  hs.add("maxtrackpt",                    "SV max{trk_{i} p_{T}} (GeV)",                                                 100,    0,     150);
  hs.add("maxm1trackpt",                  "SV max-1{trk_{i} p_{T}} (GeV)",                                               100,    0,     150);
  hs.add("maxm2trackpt",                  "SV max-2{trk_{i} p_{T}} (GeV)",                                               100,    0,     150);
  hs.add("trackptavg",                    "SV avg{trk_{i} p_{T}} (GeV)",                                                  50,    0,      50);
  hs.add("trackptrms",                    "SV rms{trk_{i} p_{T}} (GeV)",                                                  50,    0,      20);

  hs.add("trackdxymin", "SV min{trk_{i} dxy(BS)} (cm)", 50, 0, 0.2);
  hs.add("trackdxymax", "SV max{trk_{i} dxy(BS)} (cm)", 50, 0, 2);
  hs.add("trackdxyavg", "SV avg{trk_{i} dxy(BS)} (cm)", 50, 0, 0.5);
  hs.add("trackdxyrms", "SV rms{trk_{i} dxy(BS)} (cm)", 50, 0, 0.5);

  hs.add("trackdzmin", "SV min{trk_{i} dz(PV)} (cm)", 50, 0, 0.5);
  hs.add("trackdzmax", "SV max{trk_{i} dz(PV)} (cm)", 50, 0, 2);
  hs.add("trackdzavg", "SV avg{trk_{i} dz(PV)} (cm)", 50, 0, 1);
  hs.add("trackdzrms", "SV rms{trk_{i} dz(PV)} (cm)", 50, 0, 0.5);

  hs.add("trackpterrmin", "SV min{#sigma trk_{i} p_{T}} (GeV)", 50, 0, 0.1);
  hs.add("trackpterrmax", "SV max{#sigma trk_{i} p_{T}} (GeV)", 50, 0, 2);
  hs.add("trackpterravg", "SV avg{#sigma trk_{i} p_{T}} (GeV)", 50, 0, 1);
  hs.add("trackpterrrms", "SV rms{#sigma trk_{i} p_{T}} (GeV)", 50, 0, 2);

  hs.add("trackdxyerrmin", "SV min{#sigma trk_{i} dxy(BS)} (cm)", 50, 0, 0.01);
  hs.add("trackdxyerrmax", "SV max{#sigma trk_{i} dxy(BS)} (cm)", 50, 0, 0.5);
  hs.add("trackdxyerravg", "SV avg{#sigma trk_{i} dxy(BS)} (cm)", 50, 0, 0.1);
  hs.add("trackdxyerrrms", "SV rms{#sigma trk_{i} dxy(BS)} (cm)", 50, 0, 0.1);

  hs.add("trackdzerrmin", "SV min{#sigma trk_{i} dz(PV)} (cm)", 50, 0, 0.01);
  hs.add("trackdzerrmax", "SV max{#sigma trk_{i} dz(PV)} (cm)", 50, 0, 2);
  hs.add("trackdzerravg", "SV avg{#sigma trk_{i} dz(PV)} (cm)", 50, 0, 1);
  hs.add("trackdzerrrms", "SV rms{#sigma trk_{i} dz(PV)} (cm)", 50, 0, 0.4);

  hs.add("trackpairmassmin", "SV min{mass(trk_{i}, trk_{j})} (GeV)", 50, 0, 0.5);
  hs.add("trackpairmassmax", "SV max{mass(trk_{i}, trk_{j})} (GeV)", 50, 0, 50);
  hs.add("trackpairmassavg", "SV avg{mass(trk_{i}, trk_{j})} (GeV)", 50, 0, 20);
  hs.add("trackpairmassrms", "SV rms{mass(trk_{i}, trk_{j})} (GeV)", 50, 0, 20);

  hs.add("tracktripmassmin", "SV min{mass(trk_{i}, trk_{j}, trk_{k})} (GeV)", 50, 0, 4);
  hs.add("tracktripmassmax", "SV max{mass(trk_{i}, trk_{j}, trk_{k})} (GeV)", 50, 0, 100);
  hs.add("tracktripmassavg", "SV avg{mass(trk_{i}, trk_{j}, trk_{k})} (GeV)", 50, 0, 50);
  hs.add("tracktripmassrms", "SV rms{mass(trk_{i}, trk_{j}, trk_{k})} (GeV)", 50, 0, 40);

  hs.add("trackquadmassmin", "SV min{mass(trk_{i}, trk_{j}, trk_{k}, trk_{l})} (GeV)", 50, 0, 10);
  hs.add("trackquadmassmax", "SV max{mass(trk_{i}, trk_{j}, trk_{k}, trk_{l})} (GeV)", 50, 0, 200);
  hs.add("trackquadmassavg", "SV avg{mass(trk_{i}, trk_{j}, trk_{k}, trk_{l})} (GeV)", 50, 0, 50);
  hs.add("trackquadmassrms", "SV rms{mass(trk_{i}, trk_{j}, trk_{k}, trk_{l})} (GeV)", 50, 0, 30);

  hs.add("drmin",                         "SV min{#Delta R(i,j)}",                                                       150,    0,       1.5);
  hs.add("drmax",                         "SV max{#Delta R(i,j)}",                                                       150,    0,       7);
  hs.add("dravg",                         "SV avg{#Delta R(i,j)}",                                                       150,    0,       5);
  hs.add("drrms",                         "SV rms{#Delta R(i,j)}",                                                       150,    0,       3);
  hs.add("dravgw",                        "SV wavg{#Delta R(i,j)}",                                                      150,    0,       5);
  hs.add("drrmsw",                        "SV wrms{#Delta R(i,j)}",                                                      150,    0,       3);

  hs.add("jetpairdrmin", "SV min{#Delta R(jet_{i}, jet_{j})}", 50, 0, 5);
  hs.add("jetpairdrmax", "SV max{#Delta R(jet_{i}, jet_{j})}", 50, 0, 7);
  hs.add("jetpairdravg", "SV avg{#Delta R(jet_{i}, jet_{j})}", 50, 0, 5);
  hs.add("jetpairdrrms", "SV rms{#Delta R(jet_{i}, jet_{j})}", 50, 0, 1.5);

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
  hs.add("bs2dcompatscss",                "compat2d(SV, beamspot) success",                                                2,    0,       2);
  hs.add("bs2dcompat",                    "compat2d(SV, beamspot)",                                                      100,    0,    1000);
  hs.add("bs2ddist",                      "dist2d(SV, beamspot) (cm)",                                                   100,    0,       0.5);
  hs.add("bs2derr",                       "#sigma(dist2d(SV, beamspot)) (cm)",                                           100,    0,       0.05);
  hs.add("bs2dsig",                       "N#sigma(dist2d(SV, beamspot))",                                               100,    0,     100);
  hs.add("bs3ddist",                      "dist2d(SV, beamspot) * sin(SV theta) (cm)",                                   100,    0,       0.5);
  hs.add("pv2dcompatscss",                "compat2d(SV, PV) success",                                                      2,    0,       2);
  hs.add("pv2dcompat",                    "compat2d(SV, PV)",                                                            100,    0,    1000);
  hs.add("pv2ddist",                      "dist2d(SV, PV) (cm)",                                                         100,    0,       0.5);
  hs.add("pv2derr",                       "#sigma(dist2d(SV, PV)) (cm)",                                                 100,    0,       0.05);
  hs.add("pv2dsig",                       "N#sigma(dist2d(SV, PV))",                                                     100,    0,     100);
  hs.add("pv3dcompatscss",                "compat3d(SV, PV) success",                                                      2,    0,       2);
  hs.add("pv3dcompat",                    "compat3d(SV, PV)",                                                            100,    0,    1000);
  hs.add("pv3ddist",                      "dist3d(SV, PV) (cm)",                                                         100,    0,       0.5);
  hs.add("pv3derr",                       "#sigma(dist3d(SV, PV)) (cm)",                                                 100,    0,       0.1);
  hs.add("pv3dsig",                       "N#sigma(dist3d(SV, PV))",                                                     100,    0,     100);
  hs.add("pvdz",                          "dz(SV, PV) (cm)",                                                             100,    0,       0.5);
  hs.add("pvdzerr",                       "#sigma(dz(SV, PV)) (cm)",                                                     100,    0,       0.1);
  hs.add("pvdzsig",                       "N#sigma(dz(SV, PV))",                                                         100,    0,     100);

  for (int j = 0; j < sv_num_indices; ++j) {
    const std::string ex = sv_index_names[j];
    const char* exc = ex.c_str();

    if (j < 4) {
      for (int i = 0; i < 3; ++i) {
        float l = i == 2 ? 25 : 0.8;
        h_sv_pos_1d[j][i] = fs->make<TH1F>(TString::Format("h_sv_pos_1d_%i%i", j, i), TString::Format(";%s SV pos[%i] (cm);arb. units", exc, i), 100, -l, l);
      }
      h_sv_pos_2d[j][0] = fs->make<TH2F>(TString::Format("h_sv_pos_2d_%ixy", j), TString::Format(";%s SV x (cm);%s SV y (cm)", exc, exc), 100, -1, 1, 100, -1, 1);
      h_sv_pos_2d[j][1] = fs->make<TH2F>(TString::Format("h_sv_pos_2d_%ixz", j), TString::Format(";%s SV x (cm);%s SV z (cm)", exc, exc), 100, -1, 1, 100,-25,25);
      h_sv_pos_2d[j][2] = fs->make<TH2F>(TString::Format("h_sv_pos_2d_%iyz", j), TString::Format(";%s SV y (cm);%s SV z (cm)", exc, exc), 100, -1, 1, 100,-25,25);
    }

    h_sv[j].Init("h_sv_" + ex, hs, true, do_scatterplots);
  }

  for (int j = 0; j < 3; ++j) {
    std::string ex;
    if (j == 0)
      ex = "sumtop2";
    else if (j == 1)
      ex = "sumtop3";
    else if (j == 2)
      ex = "sumtop4";

    h_sv_sums[j].Init("h_sv_" + ex, hs, true, do_scatterplots, j+2);
  }    

  h_svdist2d = fs->make<TH1F>("h_svdist2d", ";dist2d(sv #0, #1) (cm);arb. units", 500, 0, 1);
  h_svdist3d = fs->make<TH1F>("h_svdist3d", ";dist3d(sv #0, #1) (cm);arb. units", 500, 0, 1);
  h_svdist2d_v_lspdist2d = fs->make<TH2F>("h_svdist2d_v_lspdist2d", ";dist2d(gen vtx #0, #1) (cm);dist2d(sv #0, #1) (cm)", 600, 0, 3, 600, 0, 3);
  h_svdist3d_v_lspdist3d = fs->make<TH2F>("h_svdist3d_v_lspdist3d", ";dist3d(gen vtx #0, #1) (cm);dist3d(sv #0, #1) (cm)", 600, 0, 3, 600, 0, 3);
  h_svdist2d_v_minlspdist2d = fs->make<TH2F>("h_svdist2d_v_minlspdist2d", ";min dist2d(gen vtx #0, #1) (cm);dist2d(sv #0, #1) (cm)", 600, 0, 3, 600, 0, 3);
  h_svdist2d_v_minbsdist2d = fs->make<TH2F>("h_svdist2d_v_mindist2d", ";min dist2d(sv, bs) (cm);dist2d(sv #0, #1) (cm)", 600, 0, 3, 600, 0, 3);
  h_sv0pvdz_v_sv1pvdz = fs->make<TH2F>("h_sv0pvdz_v_sv1pvdz", ";sv #1 dz to PV (cm);sv #0 dz to PV (cm)", 100, 0, 0.5, 100, 0, 0.5);
  h_sv0pvdzsig_v_sv1pvdzsig = fs->make<TH2F>("h_sv0pvdzsig_v_sv1pvdzsig", ";N#sigma(sv #1 dz to PV);sv N#sigma(#0 dz to PV)", 100, 0, 50, 100, 0, 50);

  h_pair2dcompatscss = fs->make<TH1F>("h_pair2dcompatscss", ";pair compat2d success;arb. units",       2,    0,     2);
  h_pair2dcompat     = fs->make<TH1F>("h_pair2dcompat",     ";pair compat2d;arb. units",             100,    0,  1000);
  h_pair2ddist       = fs->make<TH1F>("h_pair2ddist",       ";pair dist2d (cm);arb. units",          150,    0,     0.3);
  h_pair2derr        = fs->make<TH1F>("h_pair2derr",        ";pair #sigma(dist2d) (cm);arb. units",  100,    0,     0.05);
  h_pair2dsig        = fs->make<TH1F>("h_pair2dsig",        ";pair N#sigma(dist2d);arb. units",      100,    0,   100);
  h_pair3dcompatscss = fs->make<TH1F>("h_pair3dcompatscss", ";pair compat3d success;arb. units",       2,    0,     2);
  h_pair3dcompat     = fs->make<TH1F>("h_pair3dcompat",     ";pair compat3d;arb. units",             100,    0,  1000);
  h_pair3ddist       = fs->make<TH1F>("h_pair3ddist",       ";pair dist3d (cm);arb. units",          100,    0,     0.5);
  h_pair3derr        = fs->make<TH1F>("h_pair3derr",        ";pair #sigma(dist3d) (cm);arb. units",  100,    0,     0.07);
  h_pair3dsig        = fs->make<TH1F>("h_pair3dsig",        ";pair N#sigma(dist3d);arb. units",      100,    0,   100);
}

// JMTBAD ugh
void MFVVertexHistos::fill_multi(TH1F** hs, const int isv, const double val, const double weight) const {
  hs[isv < 3 ? isv : sv_rest]->Fill(val, weight);
  if (isv < 2)
    hs[sv_top2]->Fill(val, weight);
  hs[sv_all]->Fill(val, weight);
}

void MFVVertexHistos::fill_multi(TH2F** hs, const int isv, const double val, const double val2, const double weight) const {
  hs[isv < 3 ? isv : sv_rest]->Fill(val, val2, weight);
  if (isv < 2)
    hs[sv_top2]->Fill(val, val2, weight);
  hs[sv_all]->Fill(val, val2, weight);
}

void MFVVertexHistos::fill_multi(PairwiseHistos* hs, const int isv, const PairwiseHistos::ValueMap& val, const double weight) const {
  hs[isv < 3 ? isv : sv_rest].Fill(val, -1, weight);
  if (isv < 2)
    hs[sv_top2].Fill(val, -1, weight);
  hs[sv_all].Fill(val, -1, weight);
}

void MFVVertexHistos::analyze(const edm::Event& event, const edm::EventSetup& setup) {
  edm::Handle<MFVEvent> mevent;
  event.getByLabel(mfv_event_src, mevent);

  edm::Handle<double> weight;
  event.getByLabel(weight_src, weight);

  const float bsx = mevent->bsx;
  const float bsy = mevent->bsy;
  const float bsz = mevent->bsz;
  const math::XYZPoint bs(bsx, bsy, bsz);

  edm::Handle<MFVVertexAuxCollection> auxes;
  event.getByLabel(vertex_aux_src, auxes);

  const int nsv = int(auxes->size());

  for (int isv = 0; isv < nsv; ++isv) {
    const MFVVertexAux& aux = auxes->at(isv);

    const int svndx = isv >= 3 ? 3 : isv; // don't use fill_multi for the position plots
    h_sv_pos_1d[svndx][0]->Fill(aux.x - bsx, *weight);
    h_sv_pos_1d[svndx][1]->Fill(aux.y - bsy, *weight);
    h_sv_pos_1d[svndx][2]->Fill(aux.z - bsz, *weight);
    h_sv_pos_2d[svndx][0]->Fill(aux.x - bsx, aux.y - bsy, *weight);
    h_sv_pos_2d[svndx][1]->Fill(aux.x - bsx, aux.z - bsz, *weight);
    h_sv_pos_2d[svndx][2]->Fill(aux.y - bsy, aux.z - bsz, *weight);

    PairwiseHistos::ValueMap v = {
        {"ntracks",                 aux.ntracks},
        {"nbadtracks",              aux.nbadtracks},
        {"ntracksptgt3",            aux.ntracksptgt3},
        {"ntracksptgt5",            aux.ntracksptgt5},
        {"ntracksptgt10",           aux.ntracksptgt10},
        {"trackminnhits",           aux.trackminnhits},
        {"trackmaxnhits",           aux.trackmaxnhits},
        {"njetsntks",               aux.njets[mfv::JByNtracks]},
        {"chi2dof",                 aux.chi2/aux.ndof},
        {"chi2dofprob",             TMath::Prob(aux.chi2, aux.ndof)},

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

        {"costhtkonlymombs",         aux.costhmombs  [mfv::PTracksOnly]},
        {"costhtkonlymompv2d",       aux.costhmompv2d[mfv::PTracksOnly]},
        {"costhtkonlymompv3d",       aux.costhmompv3d[mfv::PTracksOnly]},

        {"costhjetsntkmombs",        aux.costhmombs  [mfv::PJetsByNtracks]},
        {"costhjetsntkmompv2d",      aux.costhmompv2d[mfv::PJetsByNtracks]},
        {"costhjetsntkmompv3d",      aux.costhmompv3d[mfv::PJetsByNtracks]},

        {"costhtksjetsntkmombs",     aux.costhmombs  [mfv::PTracksPlusJetsByNtracks]},
        {"costhtksjetsntkmompv2d",   aux.costhmompv2d[mfv::PTracksPlusJetsByNtracks]},
        {"costhtksjetsntkmompv3d",   aux.costhmompv3d[mfv::PTracksPlusJetsByNtracks]},

        {"missdisttkonlypv",        aux.missdistpv   [mfv::PTracksOnly]},
        {"missdisttkonlypverr",     aux.missdistpverr[mfv::PTracksOnly]},
        {"missdisttkonlypvsig",     aux.missdistpvsig(mfv::PTracksOnly)},

        {"missdistjetsntkpv",        aux.missdistpv   [mfv::PJetsByNtracks]},
        {"missdistjetsntkpverr",     aux.missdistpverr[mfv::PJetsByNtracks]},
        {"missdistjetsntkpvsig",     aux.missdistpvsig(mfv::PJetsByNtracks)},

        {"missdisttksjetsntkpv",        aux.missdistpv   [mfv::PTracksPlusJetsByNtracks]},
        {"missdisttksjetsntkpverr",     aux.missdistpverr[mfv::PTracksPlusJetsByNtracks]},
        {"missdisttksjetsntkpvsig",     aux.missdistpvsig(mfv::PTracksPlusJetsByNtracks)},

        {"sumpt2",                  aux.sumpt2},
        {"sumnhitsbehind",          aux.sumnhitsbehind},
        {"maxnhitsbehind",          aux.maxnhitsbehind},
        {"mintrackpt",              aux.mintrackpt},
        {"maxtrackpt",              aux.maxtrackpt},
        {"maxm1trackpt",            aux.maxm1trackpt},
        {"maxm2trackpt",            aux.maxm2trackpt},

        {"trackptavg", aux.trackptavg},
        {"trackptrms", aux.trackptrms},

        {"trackdxymin", aux.trackdxymin},
        {"trackdxymax", aux.trackdxymax},
        {"trackdxyavg", aux.trackdxyavg},
        {"trackdxyrms", aux.trackdxyrms},

        {"trackdzmin", aux.trackdzmin},
        {"trackdzmax", aux.trackdzmax},
        {"trackdzavg", aux.trackdzavg},
        {"trackdzrms", aux.trackdzrms},

        {"trackpterrmin", aux.trackpterrmin},
        {"trackpterrmax", aux.trackpterrmax},
        {"trackpterravg", aux.trackpterravg},
        {"trackpterrrms", aux.trackpterrrms},

        {"trackdxyerrmin", aux.trackdxyerrmin},
        {"trackdxyerrmax", aux.trackdxyerrmax},
        {"trackdxyerravg", aux.trackdxyerravg},
        {"trackdxyerrrms", aux.trackdxyerrrms},

        {"trackdzerrmin", aux.trackdzerrmin},
        {"trackdzerrmax", aux.trackdzerrmax},
        {"trackdzerravg", aux.trackdzerravg},
        {"trackdzerrrms", aux.trackdzerrrms},

        {"trackpairmassmin", aux.trackpairmassmin},
        {"trackpairmassmax", aux.trackpairmassmax},
        {"trackpairmassavg", aux.trackpairmassavg},
        {"trackpairmassrms", aux.trackpairmassrms},

        {"tracktripmassmin", aux.tracktripmassmin},
        {"tracktripmassmax", aux.tracktripmassmax},
        {"tracktripmassavg", aux.tracktripmassavg},
        {"tracktripmassrms", aux.tracktripmassrms},

        {"trackquadmassmin", aux.trackquadmassmin},
        {"trackquadmassmax", aux.trackquadmassmax},
        {"trackquadmassavg", aux.trackquadmassavg},
        {"trackquadmassrms", aux.trackquadmassrms},

        {"drmin",                   aux.drmin},
        {"drmax",                   aux.drmax},
        {"dravg",                   aux.dravg},
        {"drrms",                   aux.drrms},
        {"dravgw",                  aux.dravgw},
        {"drrmsw",                  aux.drrmsw},

        {"jetpairdrmin", aux.jetpairdrmin},
        {"jetpairdrmax", aux.jetpairdrmax},
        {"jetpairdravg", aux.jetpairdravg},
        {"jetpairdrrms", aux.jetpairdrrms},

        {"costhtkmomvtxdispmin", aux.costhtkmomvtxdispmin},
        {"costhtkmomvtxdispmax", aux.costhtkmomvtxdispmax},
        {"costhtkmomvtxdispavg", aux.costhtkmomvtxdispavg},
        {"costhtkmomvtxdisprms", aux.costhtkmomvtxdisprms},

        {"costhjetmomvtxdispmin", aux.costhjetmomvtxdispmin},
        {"costhjetmomvtxdispmax", aux.costhjetmomvtxdispmax},
        {"costhjetmomvtxdispavg", aux.costhjetmomvtxdispavg},
        {"costhjetmomvtxdisprms", aux.costhjetmomvtxdisprms},

        {"gen2ddist",               aux.gen2ddist},
        {"gen2derr",                aux.gen2derr},
        {"gen2dsig",                aux.gen2dsig()},
        {"gen3ddist",               aux.gen3ddist},
        {"gen3derr",                aux.gen3derr},
        {"gen3dsig",                aux.gen3dsig()},
        {"bs2dcompatscss",          aux.bs2dcompatscss},
        {"bs2dcompat",              aux.bs2dcompat},
        {"bs2ddist",                aux.bs2ddist},
        {"bs2derr",                 aux.bs2derr},
        {"bs2dsig",                 aux.bs2dsig()},
        {"bs3ddist",                aux.bs3ddist},
        {"pv2dcompatscss",          aux.pv2dcompatscss},
        {"pv2dcompat",              aux.pv2dcompat},
        {"pv2ddist",                aux.pv2ddist},
        {"pv2derr",                 aux.pv2derr},
        {"pv2dsig",                 aux.pv2dsig()},
        {"pv3dcompatscss",          aux.pv3dcompatscss},
        {"pv3dcompat",              aux.pv3dcompat},
        {"pv3ddist",                aux.pv3ddist},
        {"pv3derr",                 aux.pv3derr},
        {"pv3dsig",                 aux.pv3dsig()},
        {"pvdz",                    aux.pvdz()},
        {"pvdzerr",                 aux.pvdzerr()},
        {"pvdzsig",                 aux.pvdzsig()}
    };

    fill_multi(h_sv, isv, v, *weight);

    for (PairwiseHistos& h : h_sv_sums)
      h.Fill(v, isv, *weight);
  }

  //////////////////////////////////////////////////////////////////////

  h_nsv->Fill(nsv, *weight);
  h_nsv_v_minlspdist2d->Fill(mevent->minlspdist2d(), nsv, *weight);
  h_nsv_v_lspdist2d->Fill(mevent->lspdist2d(), nsv, *weight);
  h_nsv_v_lspdist3d->Fill(mevent->lspdist3d(), nsv, *weight);

  if (nsv >= 2) {
    const MFVVertexAux& sv0 = auxes->at(0);
    const MFVVertexAux& sv1 = auxes->at(1);
    double svdist2d = mag(sv0.x - sv1.x, sv0.y - sv1.y);
    double svdist3d = mag(sv0.x - sv1.x, sv0.y - sv1.y, sv0.z - sv1.z);
    h_svdist2d->Fill(svdist2d, *weight);
    h_svdist3d->Fill(svdist3d, *weight);
    h_svdist2d_v_lspdist2d->Fill(mevent->lspdist2d(), svdist2d, *weight);
    h_svdist3d_v_lspdist3d->Fill(mevent->lspdist3d(), svdist3d, *weight);
    h_svdist2d_v_minlspdist2d->Fill(mevent->minlspdist2d(), svdist2d, *weight);
    h_sv0pvdz_v_sv1pvdz->Fill(sv0.pvdz(), sv1.pvdz());
    h_sv0pvdzsig_v_sv1pvdzsig->Fill(sv0.pvdzsig(), sv1.pvdzsig());
  }

  for (int ivtx = 0; ivtx < nsv; ++ivtx) {
    const MFVVertexAux& auxi = auxes->at(ivtx);
    const reco::Vertex vtxi = mfv::aux_to_reco(auxi);

    for (int jvtx = ivtx + 1; jvtx < nsv; ++jvtx) {
      const MFVVertexAux& auxj = auxes->at(jvtx);
      const reco::Vertex vtxj = mfv::aux_to_reco(auxj);

      Measurement1D pair2ddist = distcalc_2d.distance(vtxi, vtxj);
      Measurement1D pair3ddist = distcalc_3d.distance(vtxi, vtxj);

      std::pair<bool, float> pair2dcompat = mfv::compatibility(vtxi, vtxj, false);
      std::pair<bool, float> pair3dcompat = mfv::compatibility(vtxi, vtxj, true);

      h_pair2dcompatscss->Fill(pair2dcompat.first, *weight);
      h_pair2dcompat->Fill(pair2dcompat.second, *weight);
      h_pair2ddist->Fill(pair2ddist.value(), *weight);
      h_pair2derr->Fill(pair2ddist.error(), *weight);
      h_pair2dsig->Fill(pair2ddist.significance(), *weight);
      h_pair3dcompatscss->Fill(pair3dcompat.first, *weight);
      h_pair3dcompat->Fill(pair3dcompat.second, *weight);
      h_pair3ddist->Fill(pair3ddist.value(), *weight);
      h_pair3derr->Fill(pair3ddist.error(), *weight);
      h_pair3dsig->Fill(pair3ddist.significance(), *weight);
    }
  }
}

DEFINE_FWK_MODULE(MFVVertexHistos);
