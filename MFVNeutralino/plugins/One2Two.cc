#include "Math/QuantFuncMathCore.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TH2D.h"
#include "TRandom3.h"
#include "TTree.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "JMTucker/MFVNeutralinoFormats/interface/VertexAux.h"
#include "JMTucker/MFVNeutralino/interface/MiniNtuple.h"

namespace {
  template <typename T>
  T mag(T x, T y) {
    return sqrt(x*x + y*y);
  }

  double svdist2d(const MFVVertexAux& v0, const MFVVertexAux& v1) {
    return mag(v0.x - v1.x, v0.y - v1.y);
  }

  double dphi(const MFVVertexAux& v0, const MFVVertexAux& v1) {
    return reco::deltaPhi(atan2(v0.y, v0.x),
			  atan2(v1.y, v1.x));
  }

  double dz(const MFVVertexAux& v0, const MFVVertexAux& v1) {
    return v0.z - v1.z;
  }

  double accept_prob(const double f, const double g, const double M) {
    return f/(M*g);
  }

  bool accept(TRandom* rand, const double f, const double g, const double M) {
    return rand->Rndm() < accept_prob(f, g, M);
  }

  double clopper_pearson(const double n_on, const double n_tot, double& lower, double& upper) {
    const double alpha=1-0.6827;
    const bool equal_tailed=true;
    const double alpha_min = equal_tailed ? alpha/2 : alpha;
    lower = 0;
    upper = 1;

    if (n_on > 0)
      lower = ROOT::Math::beta_quantile(alpha_min, n_on, n_tot - n_on + 1);
    if (n_tot - n_on > 0)
      upper = ROOT::Math::beta_quantile_c(alpha_min, n_on + 1, n_tot - n_on);

    if (n_on == 0 and n_tot == 0)
      return 0;
    else
      return n_on/n_tot;
  }

  double clopper_pearson_poisson_means(const double x, const double y, double& lower, double& upper) {
    double rl, rh;
    clopper_pearson(x, x+y, rl, rh);

    lower = rl/(1-rl);

    if (y == 0 or fabs(rh - 1) < 1e-9) {
      upper = 0;
      return 0;
    }

    upper = rh/(1-rh);
    return x/y;
  }
}

class MFVOne2Two : public edm::EDAnalyzer {
public:
  explicit MFVOne2Two(const edm::ParameterSet&);
  ~MFVOne2Two();

  MFVVertexAux xform_vertex(const MFVVertexAux&) const;
  bool sel_vertex(const MFVVertexAux&) const;
  bool is_sideband(const MFVVertexAux&, const MFVVertexAux&) const;

  typedef std::vector<std::pair<MFVVertexAux, MFVVertexAux> > MFVVertexPairCollection;

  void read_file(const std::string& filename, MFVVertexAuxCollection&, MFVVertexPairCollection&) const;
  void fill_2d(const int ih, const double weight, const MFVVertexAux&, const MFVVertexAux&) const;
  void fill_1d(              const double weight, const MFVVertexAux&, const MFVVertexAux&) const;

  double prob_1v_pair(const MFVVertexAux&, const MFVVertexAux&) const;
  bool accept_1v_pair(const MFVVertexAux&, const MFVVertexAux&) const;
 
  void analyze(const edm::Event&, const edm::EventSetup&) {}
  void endJob();

  const int min_ntracks;
  const double svdist2d_cut;

  const std::string tree_path;
  const std::vector<std::string> filenames;
  const size_t nfiles;
  const std::vector<int> n1vs;
  const std::vector<double> weights;
  const bool just_print;

  const int seed;
  const bool toy_mode;
  const bool poisson_n1vs;
  const int sampling_type; // 0 = sample random pairs with replacement, 1 = sample all unique pairs and accept/reject, 2 = sample all unique pairs and fill 1v dists with weight according to accept/reject prob
  const int npairs;

  const bool find_g_dz;
  const std::string form_g_dz;
  const bool find_f_dphi;
  const std::string form_f_dphi;
  const bool find_f_dz;
  const std::string form_f_dz;

  double gdpmax;
  double fdpmax;
  double Mdp;
  double gdzmax;
  double fdzmax;
  double Mdz;

  const bool use_f_dz;
  const double max_1v_dz;
  const int max_1v_ntracks;

  const std::vector<std::string> signal_files;
  const size_t nsignals;
  const std::vector<int> signal_n1vs;
  const std::vector<double> signal_weights;
  const int signal_contamination;

  TF1* f_dphi;
  TF1* f_dz;
  TF1* g_dz;
  TH1D* h_1v_dphi_env;
  TH1D* h_1v_absdphi_env;
  TH1D* h_1v_dz_env;
  TH1D* h_fcn_dphi;
  TH1D* h_fcn_abs_dphi;
  TH1D* h_fcn_dz;
  TH1D* h_fcn_g_dz;

  TH1D* h_dphi_env_mean;
  TH1D* h_dphi_env_mean_err;
  TH1D* h_dphi_env_rms;
  TH1D* h_dphi_env_rms_err;
  TH1D* h_dphi_env_fit_p0;
  TH1D* h_dphi_env_fit_p0_err;
  TH1D* h_dphi_env_fit_p0_pull;
  TH1D* h_dphi_env_fit_chi2;
  TH1D* h_dphi_env_fit_chi2prob;
  TH1D* h_dz_env_mean;
  TH1D* h_dz_env_mean_err;
  TH1D* h_dz_env_rms;
  TH1D* h_dz_env_rms_err;
  TH1D* h_dz_env_fit_sig;
  TH1D* h_dz_env_fit_sig_err;
  TH1D* h_dz_env_fit_sig_pull;
  TH1D* h_dz_env_fit_chi2;
  TH1D* h_dz_env_fit_chi2prob;
  TH1D* h_dphi_mean;
  TH1D* h_dphi_mean_err;
  TH1D* h_dphi_rms;
  TH1D* h_dphi_rms_err;
  TH1D* h_dphi_asym;
  TH1D* h_dphi_asym_err;
  TH1D* h_dphi_fit_exp;
  TH1D* h_dphi_fit_exp_err;
  TH1D* h_dphi_fit_exp_pull;
  TH1D* h_dphi_fit_chi2;
  TH1D* h_dphi_fit_chi2prob;
  TH1D* h_dz_mean;
  TH1D* h_dz_mean_err;
  TH1D* h_dz_rms;
  TH1D* h_dz_rms_err;
  TH1D* h_dz_fit_sig;
  TH1D* h_dz_fit_sig_err;
  TH1D* h_dz_fit_sig_pull;
  TH1D* h_dz_fit_chi2;
  TH1D* h_dz_fit_chi2prob;

  TRandom3* rand;

  enum { t_2v, t_2vsideband, t_1v, t_sig, n_t };
  static const char* t_names[n_t];

  TH2D* h_xy[n_t];
  TH1D* h_bs2ddist[n_t];
  TH2D* h_bs2ddist_v_bsdz[n_t];
  TH1D* h_bsdz[n_t];
  TH1D* h_bs2ddist_0[n_t];
  TH2D* h_bs2ddist_v_bsdz_0[n_t];
  TH1D* h_bsdz_0[n_t];
  TH1D* h_bs2ddist_1[n_t];
  TH2D* h_bs2ddist_v_bsdz_1[n_t];
  TH1D* h_bsdz_1[n_t];
  TH2D* h_ntracks[n_t];
  TH1D* h_ntracks01[n_t];
  TH1D* h_svdist2d[n_t];
  TH1D* h_svdist2d_all[n_t];
  TH1D* h_svdz[n_t];
  TH1D* h_svdz_all[n_t];
  TH1D* h_dphi[n_t];
  TH1D* h_abs_dphi[n_t];
  TH2D* h_svdz_v_dphi[n_t];

  TH1D* h_1v_svdist2d_fit_2v;

  TH1D* h_meandiff;
  TH1D* h_shift;
  TH1D* h_ksdist;
  TH1D* h_ksprob;
  TH1D* h_ksdistX;
  TH1D* h_ksprobX;

  TH2D* h_pred_v_true;
  TH1D* h_pred_m_true;
};

const char* MFVOne2Two::t_names[MFVOne2Two::n_t] = { "2v", "2vsideband", "1v", "2vsig" };

MFVOne2Two::MFVOne2Two(const edm::ParameterSet& cfg)
  : min_ntracks(cfg.getParameter<int>("min_ntracks")),
    svdist2d_cut(cfg.getParameter<double>("svdist2d_cut")),

    tree_path(cfg.getParameter<std::string>("tree_path")),
    filenames(cfg.getParameter<std::vector<std::string> >("filenames")),
    nfiles(filenames.size()),
    n1vs(cfg.getParameter<std::vector<int> >("n1vs")),
    weights(cfg.getParameter<std::vector<double> >("weights")),
    just_print(cfg.getParameter<bool>("just_print")),

    seed(cfg.getParameter<int>("seed")),
    toy_mode(cfg.getParameter<bool>("toy_mode")),
    poisson_n1vs(cfg.getParameter<bool>("poisson_n1vs")),
    sampling_type(cfg.getParameter<int>("sampling_type")),
    npairs(cfg.getParameter<int>("npairs")),

    find_g_dz(cfg.getParameter<bool>("find_g_dz")),
    form_g_dz(cfg.getParameter<std::string>("form_g_dz")),

    find_f_dphi(cfg.getParameter<bool>("find_f_dphi")),
    form_f_dphi(cfg.getParameter<std::string>("form_f_dphi")),

    find_f_dz(cfg.getParameter<bool>("find_f_dz")),
    form_f_dz(cfg.getParameter<std::string>("form_f_dz")),

    use_f_dz(cfg.getParameter<bool>("use_f_dz")),
    max_1v_dz(cfg.getParameter<double>("max_1v_dz")),
    max_1v_ntracks(cfg.getParameter<int>("max_1v_ntracks")),

    signal_files(cfg.getParameter<std::vector<std::string> >("signal_files")),
    nsignals(signal_files.size()),
    signal_n1vs(cfg.getParameter<std::vector<int> >("signal_n1vs")),
    signal_weights(cfg.getParameter<std::vector<double> >("signal_weights")),
    signal_contamination(cfg.getParameter<int>("signal_contamination")),

    f_dphi(0),
    f_dz(0),
    g_dz(0),
    rand(0)
{
  if (n1vs.size() != weights.size() || (toy_mode && nfiles != n1vs.size()))
    throw cms::Exception("VectorMismatch", "inconsistent sample info");

  if (nsignals != signal_n1vs.size() || nsignals != signal_weights.size())
    throw cms::Exception("VectorMismatch", "inconsistent signal sample info");

  if (signal_contamination >= 0 && !toy_mode)
    throw cms::Exception("Misconfiguration", "no signal contamination when not in toy mode");

  if (sampling_type < 0 || sampling_type > 2)
    throw cms::Exception("Misconfiguration", "sampling_type must be one of 0,1,2");

  edm::Service<TFileService> fs;
  TH1::SetDefaultSumw2();

  f_dphi = new TF1("f_dphi", form_f_dphi.c_str(), 0, M_PI);
  f_dz = new TF1("f_dz", form_f_dz.c_str(), -40, 40);
  g_dz = new TF1("g_dz", form_g_dz.c_str(), -40, 40);
  
  h_1v_dphi_env = fs->make<TH1D>("h_1v_dphi_env", "", 8, -M_PI, M_PI);
  h_1v_absdphi_env = fs->make<TH1D>("h_1v_absdphi_env", "", 8, 0, M_PI);
  h_1v_dz_env = fs->make<TH1D>("h_1v_dz_env", "", 200, -40, 40);
  h_fcn_dphi = fs->make<TH1D>("h_fcn_dphi", "", 8, -M_PI, M_PI);
  h_fcn_abs_dphi = fs->make<TH1D>("h_fcn_abs_dphi", "", 8, 0, M_PI);
  h_fcn_dz = fs->make<TH1D>("h_fcn_dz", "", 20, -0.1, 0.1);
  h_fcn_g_dz = fs->make<TH1D>("h_fcn_g_dz", "", 200, -40, 40);

  h_dphi_env_mean         = fs->make<TH1D>("h_dphi_env_mean"         , "", 100, 1.54, 1.6);
  h_dphi_env_mean_err     = fs->make<TH1D>("h_dphi_env_mean_err"     , "", 100, 0, 4e-4);
  h_dphi_env_rms          = fs->make<TH1D>("h_dphi_env_rms"          , "", 100, .85, .97);
  h_dphi_env_rms_err      = fs->make<TH1D>("h_dphi_env_rms_err"      , "", 100, 0, 4e-4);
  h_dphi_env_fit_p0       = fs->make<TH1D>("h_dphi_env_fit_p0"       , "", 100, 0.1245, 0.1255);
  h_dphi_env_fit_p0_err   = fs->make<TH1D>("h_dphi_env_fit_p0_err"   , "", 100, 0, 4e-4);
  h_dphi_env_fit_p0_pull  = fs->make<TH1D>("h_dphi_env_fit_p0_pull"  , "", 100, -5, 5);
  h_dphi_env_fit_chi2     = fs->make<TH1D>("h_dphi_env_fit_chi2"     , "", 100, 0, 20);
  h_dphi_env_fit_chi2prob = fs->make<TH1D>("h_dphi_env_fit_chi2prob" , "", 100, 0, 1);
  h_dz_env_mean           = fs->make<TH1D>("h_dz_env_mean"           , "", 100, -0.1, 0.1);
  h_dz_env_mean_err       = fs->make<TH1D>("h_dz_env_mean_err"       , "", 100, 0, 0.004);
  h_dz_env_rms            = fs->make<TH1D>("h_dz_env_rms"            , "", 100, 8.8, 9.8);
  h_dz_env_rms_err        = fs->make<TH1D>("h_dz_env_rms_err"        , "", 100, 0, 0.004);
  h_dz_env_fit_sig        = fs->make<TH1D>("h_dz_env_fit_sig"        , "", 100, 8.8, 9.8);
  h_dz_env_fit_sig_err    = fs->make<TH1D>("h_dz_env_fit_sig_err"    , "", 100, 0, 0.004);
  h_dz_env_fit_sig_pull   = fs->make<TH1D>("h_dz_env_fit_sig_pull"   , "", 100, -5, 5);
  h_dz_env_fit_chi2       = fs->make<TH1D>("h_dz_env_fit_chi2"       , "", 100, 0, 20);
  h_dz_env_fit_chi2prob   = fs->make<TH1D>("h_dz_env_fit_chi2prob"   , "", 100, 0, 1);
  h_dphi_mean             = fs->make<TH1D>("h_dphi_mean"             , "", 100, 1.5, 3);
  h_dphi_mean_err         = fs->make<TH1D>("h_dphi_mean_err"         , "", 100, 0, 0.2);
  h_dphi_rms              = fs->make<TH1D>("h_dphi_rms"              , "", 100, 0, 2);
  h_dphi_rms_err          = fs->make<TH1D>("h_dphi_rms_err"          , "", 100, 0, 0.2);
  h_dphi_asym             = fs->make<TH1D>("h_dphi_asym"             , "", 100, 0, 2);
  h_dphi_asym_err         = fs->make<TH1D>("h_dphi_asym_err"         , "", 100, 0, 2);
  h_dphi_fit_exp          = fs->make<TH1D>("h_dphi_fit_exp"          , "", 100, 0, 8);
  h_dphi_fit_exp_err      = fs->make<TH1D>("h_dphi_fit_exp_err"      , "", 100, 0, 4);
  h_dphi_fit_exp_pull     = fs->make<TH1D>("h_dphi_fit_exp_pull"     , "", 100, -5, 5);
  h_dphi_fit_chi2         = fs->make<TH1D>("h_dphi_fit_chi2"         , "", 100, 0, 20);
  h_dphi_fit_chi2prob     = fs->make<TH1D>("h_dphi_fit_chi2prob"     , "", 100, 0, 1);
  h_dz_mean           = fs->make<TH1D>("h_dz_mean"           , "", 100, -0.1, 0.1);
  h_dz_mean_err       = fs->make<TH1D>("h_dz_mean_err"       , "", 100, 0, 0.004);
  h_dz_rms            = fs->make<TH1D>("h_dz_rms"            , "", 100, 0, 0.04);
  h_dz_rms_err        = fs->make<TH1D>("h_dz_rms_err"        , "", 100, 0, 0.004);
  h_dz_fit_sig        = fs->make<TH1D>("h_dz_fit_sig"        , "", 100, 0, 0.04);
  h_dz_fit_sig_err    = fs->make<TH1D>("h_dz_fit_sig_err"    , "", 100, 0, 0.004);
  h_dz_fit_sig_pull   = fs->make<TH1D>("h_dz_fit_sig_pull"   , "", 100, -5, 5);
  h_dz_fit_chi2       = fs->make<TH1D>("h_dz_fit_chi2"       , "", 100, 0, 20);
  h_dz_fit_chi2prob   = fs->make<TH1D>("h_dz_fit_chi2prob"   , "", 100, 0, 1);

  for (int i = 0; i < n_t; ++i) {
    const char* iv = t_names[i];

    h_xy                [i] = fs->make<TH2D>(TString::Format("h_%s_xy"                , iv), "", 100, -0.05, 0.05, 100, 0.05, 0.05);
    h_bs2ddist          [i] = fs->make<TH1D>(TString::Format("h_%s_bs2ddist"          , iv), "", 100, 0, 0.1);
    h_bs2ddist_v_bsdz   [i] = fs->make<TH2D>(TString::Format("h_%s_bs2ddist_v_bsdz"   , iv), "", 200, -20, 20, 100, 0, 0.1);
    h_bsdz              [i] = fs->make<TH1D>(TString::Format("h_%s_bsdz"              , iv), "", 200, -20, 20);
    h_bs2ddist_0        [i] = fs->make<TH1D>(TString::Format("h_%s_bs2ddist_0"        , iv), "", 100, 0, 0.1);
    h_bs2ddist_v_bsdz_0 [i] = fs->make<TH2D>(TString::Format("h_%s_bs2ddist_v_bsdz_0" , iv), "", 200, -20, 20, 100, 0, 0.1);
    h_bsdz_0            [i] = fs->make<TH1D>(TString::Format("h_%s_bsdz_0"            , iv), "", 200, -20, 20);
    h_bs2ddist_1        [i] = fs->make<TH1D>(TString::Format("h_%s_bs2ddist_1"        , iv), "", 100, 0, 0.1);
    h_bs2ddist_v_bsdz_1 [i] = fs->make<TH2D>(TString::Format("h_%s_bs2ddist_v_bsdz_1" , iv), "", 200, -20, 20, 100, 0, 0.1);
    h_bsdz_1            [i] = fs->make<TH1D>(TString::Format("h_%s_bsdz_1"            , iv), "", 200, -20, 20);

    h_ntracks           [i] = fs->make<TH2D>(TString::Format("h_%s_ntracks"           , iv), "", 20, 0, 20, 20, 0, 20);
    h_ntracks01         [i] = fs->make<TH1D>(TString::Format("h_%s_ntracks01"         , iv), "", 30, 0, 30);
    h_svdist2d          [i] = fs->make<TH1D>(TString::Format("h_%s_svdist2d"          , iv), "", 100, 0, 0.1);
    h_svdist2d_all      [i] = fs->make<TH1D>(TString::Format("h_%s_svdist2d_all"      , iv), "", 1000, 0, 1);
    h_svdz              [i] = fs->make<TH1D>(TString::Format("h_%s_svdz"              , iv), "", 20, -0.1, 0.1);
    h_svdz_all          [i] = fs->make<TH1D>(TString::Format("h_%s_svdz_all"          , iv), "", 400, -20, 20);
    h_dphi              [i] = fs->make<TH1D>(TString::Format("h_%s_dphi"              , iv), "", 8, -M_PI, M_PI);
    h_abs_dphi          [i] = fs->make<TH1D>(TString::Format("h_%s_abs_dphi"          , iv), "", 8, 0, M_PI);
    h_svdz_v_dphi       [i] = fs->make<TH2D>(TString::Format("h_%s_svdz_v_dphi"       , iv), "", 8, -M_PI, M_PI, 50, -0.1, 0.1);
  }

  h_1v_svdist2d_fit_2v = fs->make<TH1D>("h_1v_svdist2d_fit_2v", "", 100, 0, 0.1);

  h_meandiff = fs->make<TH1D>("h_meandiff", "", 100, 0, 0.05);
  h_shift = fs->make<TH1D>("h_shift", "", 100, 0, 100);
  h_ksdist  = fs->make<TH1D>("h_ksdist",  "", 101, 0, 1.01);
  h_ksprob  = fs->make<TH1D>("h_ksprob",  "", 101, 0, 1.01);
  h_ksdistX = fs->make<TH1D>("h_ksdistX", "", 101, 0, 1.01);
  h_ksprobX = fs->make<TH1D>("h_ksprobX", "", 101, 0, 1.01);

  h_pred_v_true = fs->make<TH2D>("h_pred_v_true", "", 100, 0, 20, 100, 0, 20);
  h_pred_m_true = fs->make<TH1D>("h_pred_m_true", "", 100, -20, 20);
}

MFVOne2Two::~MFVOne2Two() {
  delete f_dphi;
  delete f_dz;
  delete g_dz;
  delete rand;
}

MFVVertexAux MFVOne2Two::xform_vertex(const MFVVertexAux& v) const {
  return v;
}

bool MFVOne2Two::sel_vertex(const MFVVertexAux& v) const {
  return v.ntracks() >= min_ntracks;
}

bool MFVOne2Two::is_sideband(const MFVVertexAux& v0, const MFVVertexAux& v1) const {
  return svdist2d(v0, v1) < svdist2d_cut;
}

void MFVOne2Two::read_file(const std::string& filename, MFVVertexAuxCollection& one_vertices, MFVVertexPairCollection& two_vertices) const {
  TFile* f = new TFile(filename.c_str());
  if (!f)
    throw cms::Exception("One2Two") << "could not read file " << filename;

  TTree* tree = (TTree*)f->Get(tree_path.c_str());
  if (!tree)
    throw cms::Exception("One2Two") << "could not get " << tree_path << " from file " << filename;

  mfv::MiniNtuple nt;
  mfv::read_from_tree(tree, nt);

  for (int j = 0, je = tree->GetEntries(); j < je; ++j) {
    if (tree->LoadTree(j) < 0) break;
    if (tree->GetEntry(j) <= 0) continue;

    if (nt.nvtx == 1) {
      MFVVertexAux v0;
      v0.x = nt.x0; v0.y = nt.y0; v0.z = nt.z0; v0.bs2ddist = mag(nt.x0, nt.y0);
      for (int i = 0; i < nt.ntk0; ++i)
	v0.insert_track();
      if (sel_vertex(v0))
	one_vertices.push_back(v0);
    }
    else if (nt.nvtx == 2) {
      MFVVertexAux v0, v1;
      v0.x = nt.x0; v0.y = nt.y0; v0.z = nt.z0; v0.bs2ddist = mag(nt.x0, nt.y0);
      v1.x = nt.x1; v1.y = nt.y1; v1.z = nt.z1; v1.bs2ddist = mag(nt.x1, nt.y1);
      for (int i = 0; i < nt.ntk0; ++i) v0.insert_track();
      for (int i = 0; i < nt.ntk1; ++i) v1.insert_track();
      bool sel0 = sel_vertex(v0);
      bool sel1 = sel_vertex(v1);
      if (sel0 && sel1)
	two_vertices.push_back(std::make_pair(v0, v1));
      else if (sel0)
	one_vertices.push_back(v0);
      else if (sel1)
	one_vertices.push_back(v1);
    }
  }

  f->Close();
  delete f;

  printf("# 1v: %i  # 2v: %i\n", int(one_vertices.size()), int(two_vertices.size())); fflush(stdout);
}

void MFVOne2Two::fill_2d(const int ih, const double w, const MFVVertexAux& v0, const MFVVertexAux& v1) const {
  if (ih == t_2vsideband && !is_sideband(v0, v1))
    return;

  h_xy[ih]->Fill(v0.x, v0.y, w);
  h_xy[ih]->Fill(v1.x, v1.y, w);
  h_bs2ddist[ih]->Fill(v0.bs2ddist, w);
  h_bs2ddist[ih]->Fill(v1.bs2ddist, w);
  h_bs2ddist_0[ih]->Fill(v0.bs2ddist, w);
  h_bs2ddist_1[ih]->Fill(v1.bs2ddist, w);
  h_bs2ddist_v_bsdz[ih]->Fill(v0.z, v0.bs2ddist, w);
  h_bs2ddist_v_bsdz[ih]->Fill(v1.z, v1.bs2ddist, w);
  h_bs2ddist_v_bsdz_0[ih]->Fill(v0.z, v0.bs2ddist, w);
  h_bs2ddist_v_bsdz_1[ih]->Fill(v1.z, v1.bs2ddist, w);
  h_bsdz[ih]->Fill(v0.z, w);
  h_bsdz[ih]->Fill(v1.z, w);
  h_bsdz_0[ih]->Fill(v0.z, w);
  h_bsdz_1[ih]->Fill(v1.z, w);

  h_ntracks[ih]->Fill(v0.ntracks(), v1.ntracks(), w);
  h_ntracks01[ih]->Fill(v0.ntracks() + v1.ntracks(), w);
  h_svdist2d[ih]->Fill(svdist2d(v0, v1), w);
  h_svdist2d_all[ih]->Fill(svdist2d(v0, v1), w);
  h_svdz[ih]->Fill(dz(v0, v1), w);
  h_svdz_all[ih]->Fill(dz(v0, v1), w);
  h_dphi[ih]->Fill(dphi(v0, v1), w);
  h_abs_dphi[ih]->Fill(fabs(dphi(v0, v1)), w);
  h_svdz_v_dphi[ih]->Fill(dphi(v0, v1), dz(v0, v1), w);
}

void MFVOne2Two::fill_1d(const double w, const MFVVertexAux& v0, const MFVVertexAux& v1) const {
  // The 2v pairs are ordered with ntk0 > ntk1, so fill here the same way.
  const int ntk0 = v0.ntracks(); 
  const int ntk1 = v1.ntracks();
  if (ntk1 > ntk0)
    fill_2d(t_1v, w, v1, v0);
  else
    fill_2d(t_1v, w, v0, v1);
}

double MFVOne2Two::prob_1v_pair(const MFVVertexAux& v0, const MFVVertexAux& v1) const {
  const double dp = fabs(dphi(v0, v1));
  const double dz = v0.z - v1.z;

  return
    accept_prob(f_dphi->Eval(dp), gdpmax,         Mdp) *
    accept_prob(f_dz  ->Eval(dz), g_dz->Eval(dz), Mdz);
}

bool MFVOne2Two::accept_1v_pair(const MFVVertexAux& v0, const MFVVertexAux& v1) const {
  const double dp = fabs(dphi(v0, v1));
  const double dz = v0.z - v1.z;

  return
    v0.ntracks() + v1.ntracks() < max_1v_ntracks &&
    accept(rand, f_dphi->Eval(dp), gdpmax, Mdp) &&
    (use_f_dz ? accept(rand, f_dz->Eval(dz), g_dz->Eval(dz), Mdz) : fabs(dz) < max_1v_dz);
}

void MFVOne2Two::endJob() {
  edm::Service<TFileService> fs;
  rand = new TRandom3(121982 + seed);

  printf("\n\n==================================================================\n\nconfig: ntracks >= %i  svdist2d sideband <= %f\n", min_ntracks, svdist2d_cut);

  // When in all-sample mode (i.e. when we're scaling numbers to data
  // luminosity), read all signal samples to print signal
  // contamination in sideband and signal strength in signal region.
  std::vector<MFVVertexAuxCollection> signal_one_vertices(nsignals);
  std::vector<MFVVertexPairCollection> signal_two_vertices(nsignals);
  if (nfiles > 1) {
    printf("reading %lu signals\n", nsignals);
    for (size_t isig = 0; isig < nsignals; ++isig) {
      printf("%s ", signal_files[isig].c_str());
      read_file(signal_files[isig], signal_one_vertices[isig], signal_two_vertices[isig]);

      int nside = 0, nsig = 0;
      for (const auto& pair : signal_two_vertices[isig])
        if (is_sideband(pair.first, pair.second))
          ++nside;
        else
          ++nsig;

      const double w = signal_weights[isig];
      printf("  scaled: # 1v: %f  # 2v: %f = (%f sideband + %f signal region)\n", signal_one_vertices[isig].size()*w, signal_two_vertices[isig].size()*w, nside*w, nsig*w);
    }
  }


  // Read all vertices from the input files. Two modes: unweighted,
  // single sample, or combine multiple samples, reading a random
  // subset of events.

  MFVVertexAuxCollection one_vertices;
  std::vector<MFVVertexPairCollection> two_vertices(nfiles);

  if (!toy_mode) {
    // In regular mode, take all events from the file (with weight 1).
    printf("\n==============================\n\nsingle sample mode, filename: %s\n", filenames[0].c_str());
    read_file(filenames[0], one_vertices, two_vertices[0]);
  }
  else {
    // Config file specifies how many to take (or the Poisson-mean
    // number) from each sample. Directly keep the 2vs; they will be
    // histogrammed with config-specified weights below.
    printf("\n==============================\n\nmultiple sample mode, #filenames: %i\n", int(filenames.size()));

    TTree* t_sample_use = fs->make<TTree>("t_sample_use", "");
    unsigned char b;
    unsigned short s;
    t_sample_use->Branch("ifile", &b, "ifile/b");
    t_sample_use->Branch("evuse", &s, "evuse/s");

    for (size_t ifile = 0; ifile < nfiles; ++ifile) {
      printf("file: %s\n", filenames[ifile].c_str());
      MFVVertexAuxCollection v1v;
      read_file(filenames[ifile], v1v, two_vertices[ifile]);

      const int n1v = poisson_n1vs ? rand->Poisson(n1vs[ifile]) : n1vs[ifile];
      const int N1v = int(v1v.size());
      if (n1v > N1v)
        throw cms::Exception("NotEnough") << "not enough v1vs (" << N1v << " to sample " << n1v << " of them";
      
      printf("sampling %i/%i events from %s\n", n1v, N1v, filenames[ifile].c_str()); fflush(stdout);

      // Knuth sample-without-replacement.
      int t = 0, m = 0;
      while (m < n1v) {
        if ((N1v - t) * rand->Rndm() >= n1v - m)
          ++t;
        else {
          ++m;
          b = ifile;
          s = t;
          t_sample_use->Fill();
          one_vertices.push_back(v1v[t++]);
        }
      }
    }

    if (signal_contamination >= 0) {
      const size_t isig(signal_contamination);
      const MFVVertexAuxCollection& v1v = signal_one_vertices[isig];
      const double w = signal_weights[isig];
      const double n1v_d = v1v.size() * w;
      const int n1v = poisson_n1vs ? rand->Poisson(n1v_d) : int(n1v_d) + 1;
      const int N1v = int(v1v.size());

      printf("including signal contamination from %s: %i/%i 1v events", signal_files[isig].c_str(), n1v, N1v);
      int t = 0, m = 0;
      while (m < n1v) {
        if ((N1v - t) * rand->Rndm() >= n1v - m)
          ++t;
        else {
          ++m;
          b = 100 + isig;
          s = t;
          t_sample_use->Fill();
          one_vertices.push_back(v1v[t++]);
        }
      }

      const int N2v(signal_two_vertices[isig].size());
      printf(" and %f (%i unweighted) 2v events\n", w*N2v, N2v);
    }
  }

  if (just_print)
    return;


  const int N1v = int(one_vertices.size());

  // Find the envelope functions for dphi (just check that it is flat)
  // and dz.
  printf("\n==============================\n\nfitting envelopes\n"); fflush(stdout);

  for (int iv = 0; iv < N1v; ++iv) {
    const MFVVertexAux& v0 = one_vertices[iv];
    for (int jv = iv+1; jv < N1v; ++jv) {
      const MFVVertexAux& v1 = one_vertices[jv];
      h_1v_dphi_env->Fill(dphi(v0, v1));
      h_1v_absdphi_env->Fill(fabs(dphi(v0, v1)));
      h_1v_dz_env->Fill(dz(v0, v1));
    }
  }

  if (1) { // no find_g_dphi, just assuming it's flat and checking that assumption here.
    h_1v_absdphi_env->Scale(1./h_1v_absdphi_env->Integral());
    TFitResultPtr res = h_1v_absdphi_env->Fit("pol0", "QS");
    printf("h_dphi_env mean %.3f +- %.3f  rms %.3f +- %.3f   g_dphi fit to pol0 chi2/ndf = %6.3f/%i = %6.3f   prob: %g\n", h_1v_absdphi_env->GetMean(), h_1v_absdphi_env->GetMeanError(), h_1v_absdphi_env->GetRMS(), h_1v_absdphi_env->GetRMSError(), res->Chi2(), res->Ndf(), res->Chi2()/res->Ndf(), res->Prob());
    h_dphi_env_mean    ->Fill(h_1v_absdphi_env->GetMean());
    h_dphi_env_mean_err->Fill(h_1v_absdphi_env->GetMeanError());
    h_dphi_env_rms     ->Fill(h_1v_absdphi_env->GetRMS());
    h_dphi_env_rms_err ->Fill(h_1v_absdphi_env->GetRMSError());
    h_dphi_env_fit_p0      ->Fill(res->Parameter(0));
    h_dphi_env_fit_p0_err  ->Fill(res->ParError(0));
    h_dphi_env_fit_p0_pull ->Fill((res->Parameter(0) - 0.125) / res->ParError(0));
    h_dphi_env_fit_chi2    ->Fill(res->Chi2() / res->Ndf());
    h_dphi_env_fit_chi2prob->Fill(res->Prob());
  }

  if (find_g_dz) {
    h_1v_dz_env->Scale(1./h_1v_dz_env->Integral());
    TF1* g_dz_temp = new TF1("g_dz_temp", TString::Format("%f*(%s)", h_1v_dz_env->Integral()*h_1v_dz_env->GetXaxis()->GetBinWidth(1), form_g_dz.c_str()), g_dz->GetXmin(), g_dz->GetXmax());
    g_dz_temp->SetParameter(0, 10);
    TFitResultPtr res = h_1v_dz_env->Fit(g_dz_temp, "RQS");
    printf("g_dz fit to gaus sigma %6.3f +- %6.3f   chi2/ndf = %6.3f/%i = %6.3f   prob: %g\n", res->Parameter(0), res->ParError(0), res->Chi2(), res->Ndf(), res->Chi2()/res->Ndf(), res->Prob());
    g_dz->FixParameter(0, res->Parameter(0));

    h_dz_env_mean    ->Fill(h_1v_dz_env->GetMean());
    h_dz_env_mean_err->Fill(h_1v_dz_env->GetMeanError());
    h_dz_env_rms     ->Fill(h_1v_dz_env->GetRMS());
    h_dz_env_rms_err ->Fill(h_1v_dz_env->GetRMSError());
    h_dz_env_fit_sig     ->Fill(res->Parameter(0));
    h_dz_env_fit_sig_err ->Fill(res->Parameter(0));
    h_dz_env_fit_sig_pull->Fill((res->Parameter(0) - h_1v_dz_env->GetRMS()) / res->ParError(0));
    h_dz_env_fit_chi2    ->Fill(res->Chi2() / res->Ndf());
    h_dz_env_fit_chi2prob->Fill(res->Prob());

    delete g_dz_temp;
  }

  h_fcn_g_dz->FillRandom("g_dz", 100000);

  // Fill all the 2v histograms. In toy_mode we add together many
  // samples with appropriate weights. Also fit f_dphi and f_dz from
  // the 2v events in the sideband.

  for (size_t ifile = 0; ifile < nfiles; ++ifile) {
    const double w = toy_mode ? weights[ifile] : 1;

    for (const auto& pair : two_vertices[ifile])
      for (int ih = 0; ih < 2; ++ih)
        fill_2d(ih, w, pair.first, pair.second);
  }

  if (signal_contamination >= 0) {
    const size_t isig = signal_contamination;
    for (const auto& pair : signal_two_vertices[isig])
      for (int ih = 0; ih < n_t; ++ih)
        if (ih != t_1v)
          fill_2d(ih, signal_weights[isig], pair.first, pair.second);
  }


  printf("\n==============================\n\nfitting fs\n"); fflush(stdout);

  TString opt = "LIRQS";
  if (toy_mode)
    opt = "W" + opt;

  if (find_f_dphi) {
    TF1* f_dphi_temp = new TF1("f_dphi_temp", TString::Format("%f*(%s)", h_abs_dphi[t_2vsideband]->Integral()*h_abs_dphi[t_2vsideband]->GetXaxis()->GetBinWidth(1), form_f_dphi.c_str()), 0, M_PI);
    TFitResultPtr res = h_abs_dphi[t_2vsideband]->Fit(f_dphi_temp, opt);
    printf("f_dphi fit exp = %6.3f +- %6.3f   chi2/ndf = %6.3f/%i = %6.3f   prob: %g\n", res->Parameter(0), res->ParError(0), res->Chi2(), res->Ndf(), res->Chi2()/res->Ndf(), res->Prob());
    f_dphi->FixParameter(0, res->Parameter(0));

    h_dphi_mean    ->Fill(h_abs_dphi[t_2vsideband]->GetMean());
    h_dphi_mean_err->Fill(h_abs_dphi[t_2vsideband]->GetMeanError());
    h_dphi_rms     ->Fill(h_abs_dphi[t_2vsideband]->GetRMS());
    h_dphi_rms_err ->Fill(h_abs_dphi[t_2vsideband]->GetRMSError());
    double integ1, err1, integ2, err2;
    integ1 = h_abs_dphi[t_2vsideband]->IntegralAndError(1,5, err1);
    integ2 = h_abs_dphi[t_2vsideband]->IntegralAndError(6,8, err2);
    double N1 = pow(integ1/err1, 2);
    double N2 = pow(integ2/err2, 2);
    double asym, asymlo, asymhi;
    asym = clopper_pearson_poisson_means(N1, N2, asymlo, asymhi);
    h_dphi_asym      ->Fill(asym);
    h_dphi_asym_err  ->Fill((asymhi - asymlo)/2);
    h_dphi_fit_exp     ->Fill(res->Parameter(0));
    h_dphi_fit_exp_err ->Fill(res->ParError(0));
    h_dphi_fit_exp_pull->Fill((res->Parameter(0) - 2) / res->ParError(0)); // JMTBAD
    h_dphi_fit_chi2    ->Fill(res->Chi2() / res->Ndf());
    h_dphi_fit_chi2prob->Fill(res->Prob());

    delete f_dphi_temp;
  }

  if (find_f_dz) {
    TF1* f_dz_temp = new TF1("f_dz_temp", TString::Format("%f*(%s)", h_svdz[t_2vsideband]->Integral()*h_svdz[t_2vsideband]->GetXaxis()->GetBinWidth(1), form_f_dz.c_str()), f_dz->GetXmin(), f_dz->GetXmax());
    TFitResultPtr res = h_svdz[t_2vsideband]->Fit(f_dz_temp, opt);
    printf("f_dz fit gaus sigma = %6.3f +- %6.3f   chi2/ndf = %6.3f/%i = %6.3f   prob: %g\n", res->Parameter(0), res->ParError(0), res->Chi2(), res->Ndf(), res->Chi2()/res->Ndf(), res->Prob());
    f_dz->FixParameter(0, res->Parameter(0));

    h_dz_mean    ->Fill(h_svdz[t_2vsideband]->GetMean());
    h_dz_mean_err->Fill(h_svdz[t_2vsideband]->GetMeanError());
    h_dz_rms     ->Fill(h_svdz[t_2vsideband]->GetRMS());
    h_dz_rms_err ->Fill(h_svdz[t_2vsideband]->GetRMSError());
    h_dz_fit_sig     ->Fill(res->Parameter(0));
    h_dz_fit_sig_err ->Fill(res->ParError(0));
    h_dz_fit_sig_pull->Fill((res->Parameter(0) - h_svdz[t_2vsideband]->GetRMS()) / res->ParError(0));
    h_dz_fit_chi2    ->Fill(res->Chi2() / res->Ndf());
    h_dz_fit_chi2prob->Fill(res->Prob());

    delete f_dz_temp;
  }

  h_fcn_dphi->FillRandom("f_dphi", 100000);
  h_fcn_abs_dphi->FillRandom("f_dphi", 100000);
  h_fcn_dz->FillRandom("f_dz", 100000);


  gdpmax = 1./2/M_PI;
  fdpmax = f_dphi->GetMaximum();
  Mdp = fdpmax/gdpmax;

  gdzmax = g_dz->GetMaximum();
  fdzmax = f_dz->GetMaximum();
  Mdz = fdzmax/gdzmax;


  // Now sample npairs from the one_vertices sample.
  // - sampling_type = 0: sample random pairs with replacement,
  // accepting according to the f_dphi/dz functions.
  // - sampling_type = 1: for every unique pair, accept according to
  // f_dphi/dz.
  // - sampling_type = 2: for every unique pair, use pair with weight
  // = prob according to f_dphi/dz.

  printf("\n==============================\n\nsampling 1v pairs\n"); fflush(stdout);

  if (sampling_type == 0) {
    std::vector<int> used(N1v, 0);
    const int giveup = 10*N1v; // After choosing one vertex, may be so far out in e.g. dz tail that you can't find another one. Give up after trying this many times.
    const int npairsuse = npairs > 0 ? npairs : N1v/2;

    for (int ipair = 0; ipair < npairsuse; ++ipair) {
      int iv = rand->Integer(N1v);
      ++used[iv];
      const MFVVertexAux& v0 = one_vertices[iv];
      int tries = 0;
      int jv = -1;
      while (jv == -1) {
        int x = rand->Integer(N1v);
        if (x != iv) {
          const MFVVertexAux& v1 = one_vertices[x];
          if (accept_1v_pair(v0, v1)) {
            jv = x;
            ++used[jv];
            fill_1d(1, v0, v1);
            break;
          }

          if (++tries % 50000 == 0)
            ; //printf("\ripair %10i try %10i with v0 = %2i (%12f, %12f, %12f) and v1 = %2i (%12f, %12f, %12f)", ipair, tries, v0.ntracks(), v0.x, v0.y, v0.z, vx.ntracks(), vx.x, vx.y, vx.z); fflush(stdout);

          if (tries == giveup)
            break;
        }
      }

      if (jv == -1) {
        assert(tries == giveup);
        --ipair;
      }
    }

    // Output 1v usage counts (versus z).
    TTree* t_use = fs->make<TTree>("t_use", "");
    unsigned char b;
    unsigned short s;
    float z;
    bool use_short = false;
    for (int i = 0; i < N1v; ++i) {
      if (used[i] >= 65536)
        throw cms::Exception("problemo");
      else if (used[i] >= 256) {
        use_short = true;
        break;
      }
    }
    t_use->Branch("z", &z, "z/F");
    if (use_short)
      t_use->Branch("nuse", &s, "nuse/s");
    else
      t_use->Branch("nuse", &b, "nuse/b");
    for (int i = 0; i < N1v; ++i) {
      z = one_vertices[i].z;
      if (use_short)
        s = used[i];
      else
        b = used[i];
      t_use->Fill();
    }
  }
  else if (sampling_type == 1) {
    for (int iv = 0; iv < N1v; ++iv) {
      for (int jv = iv+1; jv < N1v; ++jv) {
        const MFVVertexAux& v0 = one_vertices[iv];
        const MFVVertexAux& v1 = one_vertices[jv];
        if (accept_1v_pair(v0, v1))
          fill_1d(1, v0, v1);
      }
    }
  }
  else if (sampling_type == 2) {
    for (int iv = 0; iv < N1v; ++iv) {
      for (int jv = iv+1; jv < N1v; ++jv) {
        const MFVVertexAux& v0 = one_vertices[iv];
        const MFVVertexAux& v1 = one_vertices[jv];
        const double w = prob_1v_pair(v0, v1);
        fill_1d(w, v0, v1);
      }
    }
  }


  // Fit the 1v distribution to the 2v one by shifting it over and
  // scaling in the sideband.
  printf("\n==============================\n\nfitting 1v shape to 2v dist\n"); fflush(stdout);

  const double meandiff = h_svdist2d[t_2v]->GetMean() - h_svdist2d[t_1v]->GetMean();
  const int nbins = h_svdist2d[t_1v]->GetNbinsX();
  assert(h_svdist2d[t_2v]->GetNbinsX() == nbins);
  assert(fabs(h_svdist2d[t_2v]->GetBinWidth(1) - h_svdist2d[t_1v]->GetBinWidth(1)) < 1e-5);

  const int shift = int(round(meandiff/h_svdist2d[t_1v]->GetBinWidth(1)));
  printf("shift by %i bins (mean diff %f)\n", shift, meandiff); fflush(stdout);

  h_meandiff->Fill(meandiff);
  h_shift->Fill(shift);

  for (int ibin = 1; ibin <= nbins+1; ++ibin) {
    const int ifrom = ibin - shift;
    double val = 0, err = 0;
    if (ifrom >= 0) {
      val = h_svdist2d[t_1v]->GetBinContent(ifrom);
      err = h_svdist2d[t_1v]->GetBinError  (ifrom);
    }
    if (ibin == nbins+1) {
      double var = err*err;
      for (int irest = ifrom+1; irest <= nbins+1; ++irest) {
        val += h_svdist2d[t_1v]->GetBinContent(irest);
        var += pow(h_svdist2d[t_1v]->GetBinError(irest), 2);
      }
      err = sqrt(var);
    }
    h_1v_svdist2d_fit_2v->SetBinContent(ibin, val);
    h_1v_svdist2d_fit_2v->SetBinError  (ibin, err);
  }

  const int last_sideband_bin = h_1v_svdist2d_fit_2v->FindBin(svdist2d_cut) - 1;
  h_1v_svdist2d_fit_2v->Scale(h_svdist2d[t_2v]    ->Integral(1, last_sideband_bin) / 
                              h_1v_svdist2d_fit_2v->Integral(1, last_sideband_bin));

  printf("KStest(h_1v_svdist2d_fit_2v, h_2v_svdist2d): "); fflush(stdout);
  const double ksdist = h_1v_svdist2d_fit_2v->KolmogorovTest(h_svdist2d[t_2v], "MO");
  const double ksprob = h_1v_svdist2d_fit_2v->KolmogorovTest(h_svdist2d[t_2v], "O");
  const double ksdistX = h_1v_svdist2d_fit_2v->KolmogorovTest(h_svdist2d[t_2v], "MOX");
  const double ksprobX = h_1v_svdist2d_fit_2v->KolmogorovTest(h_svdist2d[t_2v], "OX");
  printf(" dist = %f (X: %f)  pval = %g (X: %g)\n", ksdist, ksprob, ksdistX, ksprobX); fflush(stdout);

  h_ksdist->Fill(ksdist);
  h_ksprob->Fill(ksprob);
  h_ksdistX->Fill(ksdistX);
  h_ksprobX->Fill(ksprobX);
  
  const double pred_sidereg_bkg = h_1v_svdist2d_fit_2v->Integral(1, last_sideband_bin);
  const double true_sidereg     = h_svdist2d[t_2v]    ->Integral(1, last_sideband_bin);
  const double true_sidereg_sig = h_svdist2d[t_sig]   ->Integral(1, last_sideband_bin);
  const double pred_signreg_bkg = h_1v_svdist2d_fit_2v->Integral(last_sideband_bin+1, nbins+1);
  const double true_signreg     = h_svdist2d[t_2v]    ->Integral(last_sideband_bin+1, nbins+1);
  const double true_signreg_sig = h_svdist2d[t_sig]   ->Integral(last_sideband_bin+1, nbins+1);

  const double true_sidereg_bkg = true_sidereg - true_sidereg_sig;
  const double true_signreg_bkg = true_signreg - true_signreg_sig;

  printf("h_1v_svdist2d_fit_2v integral in sideband    : %f\n", pred_sidereg_bkg);
  printf("h_1v_svdist2d_fit_2v integral in signal      : %f\n", pred_signreg_bkg);
  printf("h_2v_svdist2d integral in sideband           : %f\n", true_sidereg);
  printf("h_2v_svdist2d integral in sideband, sig cont.: %f\n", true_sidereg_sig);
  printf("h_2v_svdist2d integral in sideband, bkg only : %f\n", true_sidereg_bkg);
  printf("h_2v_svdist2d integral in sig.reg.           : %f\n", true_signreg);
  printf("h_2v_svdist2d integral in sig.reg., signal   : %f\n", true_signreg_sig);
  printf("h_2v_svdist2d integral in sig.reg., bkg only : %f\n", true_signreg_bkg);

  h_pred_v_true->Fill(true_signreg_bkg, pred_signreg_bkg);
  h_pred_m_true->Fill(pred_signreg_bkg - true_signreg_bkg);
}

DEFINE_FWK_MODULE(MFVOne2Two);
