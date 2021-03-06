#include "JMTucker/MFVNeutralinoFormats/interface/Event.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/TrackReco/interface/Track.h"

namespace mfv {
  // be sure these end in _v
  const char* hlt_paths[mfv::n_hlt_paths] = {
    "HLT_PFHT1050_v",
    "HLT_Ele35_WPTight_Gsf_v",
    "HLT_Ele115_CaloIdVT_GsfTrkIdT_v",
    "HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165_v",
    "HLT_IsoMu27_v",
    "HLT_Mu50_v",
    "HLT_Ele15_IsoVVVL_PFHT450_v",
    "HLT_Mu15_IsoVVVL_PFHT450_v"
  };

  const char* l1_paths[mfv::n_l1_paths] = {
    "L1_HTT120er",
    "L1_HTT160er",
    "L1_HTT200er",
    "L1_HTT220er",
    "L1_HTT240er",
    "L1_HTT255er",
    "L1_HTT270er",
    "L1_HTT280er",
    "L1_HTT300er",
    "L1_HTT320er",
    "L1_HTT340er",
    "L1_HTT380er",
    "L1_HTT400er",
    "L1_HTT450er",
    "L1_HTT500er",
    "L1_HTT250er_QuadJet_70_55_40_35_er2p5",
    "L1_HTT280er_QuadJet_70_55_40_35_er2p5",
    "L1_HTT300er_QuadJet_70_55_40_35_er2p5",
    "L1_SingleEG24",
    "L1_SingleEG26",
    "L1_SingleEG30",
    "L1_SingleEG32",
    "L1_SingleEG34",
    "L1_SingleEG34er2p1",
    "L1_SingleEG36",
    "L1_SingleEG36er2p1",
    "L1_SingleEG38",
    "L1_SingleEG38er2p1",
    "L1_SingleEG40",
    "L1_SingleEG42",
    "L1_SingleEG45",
    "L1_SingleEG50",
    "L1_SingleIsoEG24",
    "L1_SingleIsoEG24er2p1",
    "L1_SingleIsoEG26",
    "L1_SingleIsoEG26er2p1",
    "L1_SingleIsoEG28",
    "L1_SingleIsoEG28er2p1",
    "L1_SingleIsoEG30",
    "L1_SingleIsoEG30er2p1",
    "L1_SingleIsoEG32",
    "L1_SingleIsoEG32er2p1",
    "L1_SingleIsoEG34",
    "L1_SingleIsoEG34er2p1",
    "L1_SingleIsoEG36",
    "L1_SingleIsoEG36er2p1",
    "L1_SingleIsoEG38",
    "L1_SingleJet170",
    "L1_SingleJet180",
    "L1_SingleJet200",
    "L1_SingleTau100er2p1",
    "L1_SingleTau120er2p1",
    "L1_SingleMu22",
    "L1_SingleMu25"
  };

  const char* clean_paths[mfv::n_clean_paths] = {
    "Flag_HBHENoiseFilter",
    "Flag_HBHENoiseIsoFilter",
    "Flag_EcalDeadCellTriggerPrimitiveFilter",
    "Flag_goodVertices",
    "Flag_eeBadScFilter",
    "Flag_globalTightHalo2016Filter",
    "Flag_CSCTightHalo2015Filter"
  };
}

void MFVEvent::lep_push_back(MFVEvent::lep_id_t id,
                             const reco::Candidate& lep,
                             const reco::Track& trk,
                             const double iso,
                             const std::vector<TLorentzVector>& hltleps,
                             const math::XYZPoint& beamspot,
                             const math::XYZPoint& primary_vertex) {

  lep_id_.push_back(id); // expects already el/mu encoded
  lep_qpt.push_back(lep.charge() * lep.pt());
  lep_eta.push_back(lep.eta());
  lep_phi.push_back(lep.phi());
  lep_dxy.push_back(trk.dxy(primary_vertex));
  lep_dxybs.push_back(trk.dxy(beamspot));
  lep_dz.push_back(trk.dz(primary_vertex));

  lep_pt_err.push_back(trk.ptError());
  lep_eta_err.push_back(trk.etaError());
  lep_phi_err.push_back(trk.phiError());
  lep_dxy_err.push_back(trk.dxyError());
  lep_dz_err.push_back(trk.dzError());

  lep_chi2dof.push_back(trk.normalizedChi2());
  lep_hp_push_back(trk.hitPattern().numberOfValidPixelHits(),
                   trk.hitPattern().numberOfValidStripHits(),
                   trk.hitPattern().pixelLayersWithMeasurement(),
                   trk.hitPattern().stripLayersWithMeasurement());

  lep_iso.push_back(iso);

  double hltmatchdist2 = 0.1*0.1;
  TLorentzVector hltmatch;
  for (auto hlt : hltleps) {
    const double dist2 = reco::deltaR2(lep.eta(), lep.phi(), hlt.Eta(), hlt.Phi());
    if (dist2 < hltmatchdist2) {
      hltmatchdist2 = dist2;
      hltmatch = hlt;
    }
  }
  lep_hlt_pt.push_back(hltmatch.Pt());
  lep_hlt_eta.push_back(hltmatch.Eta());
  lep_hlt_phi.push_back(hltmatch.Phi());
}
