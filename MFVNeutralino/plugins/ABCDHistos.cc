#include "TH2F.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "JMTucker/MFVNeutralinoFormats/interface/Event.h"
#include "JMTucker/MFVNeutralinoFormats/interface/VertexAux.h"
#include "JMTucker/Tools/interface/Utilities.h"

class ABCDHistos : public edm::EDAnalyzer {
 public:
  explicit ABCDHistos(const edm::ParameterSet&);
  void analyze(const edm::Event&, const edm::EventSetup&);

 private:
  const edm::InputTag mfv_event_src;
  const edm::InputTag vertex_src;
  const int which_mom;

  TH1F* h_nsv;
  TH2F* h_ntracks01_maxtrackpt01;

  TH2F* h_bs2derr_drmax;
  TH2F* h_bs2derr_drmax_trackdxyerrmax0p1;
  TH2F* h_bs2derr_drmax_trackdzerrmax0p2;
  TH2F* h_trackdxyerrmax_trackdzerrmax;

  TH2F* h_bs2ddist0_bs2ddist1;
  TH2F* h_mass0_mass1;

  TH2F* h_bs2ddist01_mass01;
  TH2F* h_bs2ddist0_mass0;
  TH2F* h_bs2ddist1_mass1;

  TH2F* h_bs2ddist1_mass0;
  TH2F* h_bs2ddist0_mass1;

  TH2F* h_pv2ddist01_mass01;
  TH2F* h_pv2ddist0_mass0;

  TH2F* h_pv3ddist01_mass01;
  TH2F* h_pv3ddist0_mass0;

  TH2F* h_pv3dctau01_mass01;
  TH2F* h_pv3dctau0_mass0;

  TH2F* h_svdist2d_mass01;
  TH2F* h_svdist3d_mass01;

  TH1F* h_absdeltaphibs01;
  TH1F* h_absdeltaphipv01;

  TH1F* h_pz01lab;
  TH1F* h_pz01cmz;

  TH1F* h_cosanglemom01lab;
  TH1F* h_cosanglemom01cmz;

  TH2F* h_betagamma0lab_betagamma1lab;
  TH2F* h_betagamma0cmz_betagamma1cmz;

  TH2F* h_svdist2dcmz_mass01;
  TH2F* h_svdist3dcmz_mass01;

  TH2F* h_svctau2dcmz_mass01;
  TH2F* h_svctau3dcmz_mass01;
};

ABCDHistos::ABCDHistos(const edm::ParameterSet& cfg)
  : mfv_event_src(cfg.getParameter<edm::InputTag>("mfv_event_src")),
    vertex_src(cfg.getParameter<edm::InputTag>("vertex_src")),
    which_mom(cfg.getParameter<int>("which_mom"))
{
  die_if_not(which_mom >= 0 && which_mom < mfv::NMomenta, "invalid which_mom");

  edm::Service<TFileService> fs;
  h_nsv = fs->make<TH1F>("h_nsv", ";number of secondary vertices;events", 15, 0, 15);
  h_ntracks01_maxtrackpt01 = fs->make<TH2F>("h_ntracks01_maxtrackpt01", ";sum of maxtrackpt for the two SV's with the highest ntracks;sum of ntracks for the two SV's with the highest ntracks", 300, 0, 300, 80, 0, 80);

  h_bs2derr_drmax = fs->make<TH2F>("h_bs2derr_drmax", ";drmax;bs2derr", 150, 0, 7, 100, 0, 0.05);
  h_bs2derr_drmax_trackdxyerrmax0p1 = fs->make<TH2F>("h_bs2derr_drmax_trackdxyerrmax0p1", "trackdxyerrmax <= 0.1;drmax;bs2derr", 150, 0, 7, 100, 0, 0.05);
  h_bs2derr_drmax_trackdzerrmax0p2 = fs->make<TH2F>("h_bs2derr_drmax_trackdzerrmax0p2", "trackdzerrmax <= 0.2;drmax;bs2derr", 150, 0, 7, 100, 0, 0.05);
  h_trackdxyerrmax_trackdzerrmax = fs->make<TH2F>("h_trackdxyerrmax_trackdzerrmax", ";trackdzerrmax;trackdxyerrmax", 50, 0, 2, 50, 0, 0.5);

  h_bs2ddist0_bs2ddist1 = fs->make<TH2F>("h_bs2ddist0_bs2ddist1", ";bs2ddist1;bs2ddist0", 50, 0, 0.5, 50, 0, 0.5);
  h_mass0_mass1 = fs->make<TH2F>("h_mass0_mass1", ";mass1;mass0", 1000, 0, 1000, 1000, 0, 1000);

  h_bs2ddist01_mass01 = fs->make<TH2F>("h_bs2ddist01_mass01", ";mass01;bs2ddist01", 1000, 0, 1000, 100, 0, 1);
  h_bs2ddist0_mass0 = fs->make<TH2F>("h_bs2ddist0_mass0", ";mass0;bs2ddist0", 500, 0, 500, 50, 0, 0.5);
  h_bs2ddist1_mass1 = fs->make<TH2F>("h_bs2ddist1_mass1", ";mass1;bs2ddist1", 500, 0, 500, 50, 0, 0.5);

  h_bs2ddist1_mass0 = fs->make<TH2F>("h_bs2ddist1_mass0", ";mass0;bs2ddist1", 500, 0, 500, 50, 0, 0.5);
  h_bs2ddist0_mass1 = fs->make<TH2F>("h_bs2ddist0_mass1", ";mass1;bs2ddist0", 500, 0, 500, 50, 0, 0.5);

  h_pv2ddist01_mass01 = fs->make<TH2F>("h_pv2ddist01_mass01", ";mass01;pv2ddist01", 1000, 0, 1000, 100, 0, 1);
  h_pv2ddist0_mass0 = fs->make<TH2F>("h_pv2ddist0_mass0", ";mass0;pv2ddist0", 500, 0, 500, 50, 0, 0.5);

  h_pv3ddist01_mass01 = fs->make<TH2F>("h_pv3ddist01_mass01", ";mass01;pv3ddist01", 1000, 0, 1000, 100, 0, 1);
  h_pv3ddist0_mass0 = fs->make<TH2F>("h_pv3ddist0_mass0", ";mass0;pv3ddist0", 500, 0, 500, 50, 0, 0.5);

  h_pv3dctau01_mass01 = fs->make<TH2F>("h_pv3dctau01_mass01", ";mass01;pv3dctau01", 1000, 0, 1000, 100, 0, 1);
  h_pv3dctau0_mass0 = fs->make<TH2F>("h_pv3dctau0_mass0", ";mass0;pv3dctau0", 500, 0, 500, 50, 0, 0.5);

  h_svdist2d_mass01 = fs->make<TH2F>("h_svdist2d_mass01", ";mass01;svdist2d", 1000, 0, 1000, 100, 0, 1);
  h_svdist3d_mass01 = fs->make<TH2F>("h_svdist3d_mass01", ";mass01;svdist3d", 1000, 0, 1000, 100, 0, 1);

  h_absdeltaphibs01 = fs->make<TH1F>("h_absdeltaphibs01", ";absdeltaphibs01;events", 315, 0, 3.15);
  h_absdeltaphipv01 = fs->make<TH1F>("h_absdeltaphipv01", ";absdeltaphipv01;events", 315, 0, 3.15);

  h_pz01lab = fs->make<TH1F>("h_pz01lab", ";pz01lab;events", 100, -300, 300);
  h_pz01cmz = fs->make<TH1F>("h_pz01cmz", ";pz01cmz;events", 100, -300, 300);

  h_cosanglemom01lab = fs->make<TH1F>("h_cosanglemom01lab", ";cosanglemom01lab;events", 100, -1, 1);
  h_cosanglemom01cmz = fs->make<TH1F>("h_cosanglemom01cmz", ";cosanglemom01cmz;events", 100, -1, 1);

  h_betagamma0lab_betagamma1lab = fs->make<TH2F>("h_betagamma0lab_betagamma1lab", ";betagamma1lab;betagamma0lab", 100, 0, 10, 100, 0, 10);
  h_betagamma0cmz_betagamma1cmz = fs->make<TH2F>("h_betagamma0cmz_betagamma1cmz", ";betagamma1cmz;betagamma0cmz", 100, 0, 10, 100, 0, 10);

  h_svdist2dcmz_mass01 = fs->make<TH2F>("h_svdist2dcmz_mass01", ";mass01;svdist2dcmz", 1000, 0, 1000, 100, 0, 1);
  h_svdist3dcmz_mass01 = fs->make<TH2F>("h_svdist3dcmz_mass01", ";mass01;svdist3dcmz", 1000, 0, 1000, 100, 0, 1);

  h_svctau2dcmz_mass01 = fs->make<TH2F>("h_svctau2dcmz_mass01", ";mass01;svctau2dcmz", 1000, 0, 1000, 100, 0, 1);
  h_svctau3dcmz_mass01 = fs->make<TH2F>("h_svctau3dcmz_mass01", ";mass01;svctau3dcmz", 1000, 0, 1000, 100, 0, 1);
}

void ABCDHistos::analyze(const edm::Event& event, const edm::EventSetup&) {
  edm::Handle<MFVEvent> mevent;
  event.getByLabel(mfv_event_src, mevent);

  const float pvx = mevent->pvx;
  const float pvy = mevent->pvy;
  const float pvz = mevent->pvz;

  edm::Handle<MFVVertexAuxCollection> vertices;
  event.getByLabel(vertex_src, vertices);

  const int nsv = int(vertices->size());
  h_nsv->Fill(nsv);

  for (int isv = 0; isv < nsv; ++isv) {
    const MFVVertexAux& vtx = vertices->at(isv);
    h_bs2derr_drmax->Fill(vtx.drmax, vtx.bs2derr);
    h_trackdxyerrmax_trackdzerrmax->Fill(vtx.trackdzerrmax, vtx.trackdxyerrmax);
    if (vtx.trackdxyerrmax < 0.1) {
      h_bs2derr_drmax_trackdxyerrmax0p1->Fill(vtx.drmax, vtx.bs2derr);
    }
    if (vtx.trackdzerrmax < 0.2) {
      h_bs2derr_drmax_trackdzerrmax0p2->Fill(vtx.drmax, vtx.bs2derr);
    }
  }

  if (nsv >= 2) {
    const MFVVertexAux& v0 = vertices->at(0);
    const MFVVertexAux& v1 = vertices->at(1);
    h_ntracks01_maxtrackpt01->Fill(v0.maxtrackpt + v1.maxtrackpt, v0.ntracks + v1.ntracks);

    h_bs2ddist0_bs2ddist1->Fill(v1.bs2ddist, v0.bs2ddist);
    h_mass0_mass1->Fill(v1.mass[which_mom], v0.mass[which_mom]);

    h_bs2ddist01_mass01->Fill(v0.mass[which_mom] + v1.mass[which_mom], v0.bs2ddist + v1.bs2ddist);
    h_bs2ddist0_mass0->Fill(v0.mass[which_mom], v0.bs2ddist);
    h_bs2ddist1_mass1->Fill(v1.mass[which_mom], v1.bs2ddist);

    h_bs2ddist1_mass0->Fill(v0.mass[which_mom], v1.bs2ddist);
    h_bs2ddist0_mass1->Fill(v1.mass[which_mom], v0.bs2ddist);

    h_pv2ddist01_mass01->Fill(v0.mass[which_mom] + v1.mass[which_mom], v0.pv2ddist + v1.pv2ddist);
    h_pv2ddist0_mass0->Fill(v0.mass[which_mom], v0.pv2ddist);

    h_pv3ddist01_mass01->Fill(v0.mass[which_mom] + v1.mass[which_mom], v0.pv3ddist + v1.pv3ddist);
    h_pv3ddist0_mass0->Fill(v0.mass[which_mom], v0.pv3ddist);

    double pv3dctau0 = v0.pv3ddist / (v0.p4(which_mom).Beta() * v0.p4(which_mom).Gamma());
    double pv3dctau1 = v1.pv3ddist / (v1.p4(which_mom).Beta() * v1.p4(which_mom).Gamma());
    h_pv3dctau01_mass01->Fill(v0.mass[which_mom] + v1.mass[which_mom], pv3dctau0 + pv3dctau1);
    h_pv3dctau0_mass0->Fill(v0.mass[which_mom], pv3dctau0);

    double svdist2d = mag(v0.x - v1.x, v0.y - v1.y);
    double svdist3d = mag(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
    h_svdist2d_mass01->Fill(v0.mass[which_mom] + v1.mass[which_mom], svdist2d);
    h_svdist3d_mass01->Fill(v0.mass[which_mom] + v1.mass[which_mom], svdist3d);

    double phibs0 = atan2(v0.y - mevent->bsy, v0.x - mevent->bsx);
    double phibs1 = atan2(v1.y - mevent->bsy, v1.x - mevent->bsx);
    h_absdeltaphibs01->Fill(fabs(reco::deltaPhi(phibs0, phibs1)));
    double phipv0 = atan2(v0.y - mevent->pvy, v0.x - mevent->pvx);
    double phipv1 = atan2(v1.y - mevent->pvy, v1.x - mevent->pvx);
    h_absdeltaphipv01->Fill(fabs(reco::deltaPhi(phipv0, phipv1)));

    TLorentzVector x0 = TLorentzVector(v0.x - pvx, v0.y - pvy, v0.z - pvz, v0.pv3ddist / v0.p4().Beta());
    TLorentzVector x1 = TLorentzVector(v1.x - pvx, v1.y - pvy, v1.z - pvz, v1.pv3ddist / v1.p4().Beta());
    TLorentzVector p0 = v0.p4(which_mom);
    TLorentzVector p1 = v1.p4(which_mom);
    h_pz01lab->Fill(p0.Pz() + p1.Pz());
    h_cosanglemom01lab->Fill(cos(p0.Angle(p1.Vect())));
    h_betagamma0lab_betagamma1lab->Fill(p1.Beta() * p1.Gamma(), p0.Beta() * p0.Gamma());

    TVector3 betacmz = TVector3(0, 0, -(p0.Pz() + p1.Pz()) / (p0.E() + p1.E()));
    x0.Boost(betacmz);
    x1.Boost(betacmz);
    p0.Boost(betacmz);
    p1.Boost(betacmz);
    h_pz01cmz->Fill(p0.Pz() + p1.Pz());
    h_cosanglemom01cmz->Fill(cos(p0.Angle(p1.Vect())));
    h_betagamma0cmz_betagamma1cmz->Fill(p1.Beta() * p1.Gamma(), p0.Beta() * p0.Gamma());

    double svdist2dcmz = mag(x0.X() - x1.X(), x0.Y() - x1.Y());
    double svdist3dcmz = mag(x0.X() - x1.X(), x0.Y() - x1.Y(), x0.Z() - x1.Z());
    h_svdist2dcmz_mass01->Fill(v0.mass[which_mom] + v1.mass[which_mom], svdist2dcmz);
    h_svdist3dcmz_mass01->Fill(v0.mass[which_mom] + v1.mass[which_mom], svdist3dcmz);

    double svctau2dcmz = 2 * svdist2dcmz / (p0.Beta()*p0.Gamma() + p1.Beta()*p1.Gamma());
    double svctau3dcmz = 2 * svdist3dcmz / (p0.Beta()*p0.Gamma() + p1.Beta()*p1.Gamma());
    h_svctau2dcmz_mass01->Fill(v0.mass[which_mom] + v1.mass[which_mom], svctau2dcmz);
    h_svctau3dcmz_mass01->Fill(v0.mass[which_mom] + v1.mass[which_mom], svctau3dcmz);
  }
}

DEFINE_FWK_MODULE(ABCDHistos);
