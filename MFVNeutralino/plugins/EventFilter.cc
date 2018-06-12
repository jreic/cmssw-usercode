#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

class MFVEventFilter : public edm::EDFilter {
public:
  explicit MFVEventFilter(const edm::ParameterSet&);
private:
  bool filter(edm::Event&, const edm::EventSetup&) override;

  const edm::EDGetTokenT<pat::JetCollection> jets_token;
  const StringCutObjectSelector<pat::Jet> jet_selector;
  const int min_njets;
  const double min_pt_for_ht;
  const double min_ht;
  const edm::EDGetTokenT<pat::MuonCollection> muons_token;
  const StringCutObjectSelector<pat::Muon> muon_selector;
  const double min_muon_pt;
  const edm::EDGetTokenT<pat::ElectronCollection> electrons_token;
  const StringCutObjectSelector<pat::Electron> electron_selector;
  const double min_electron_pt;
  const int min_nleptons;
  const bool debug;
};

MFVEventFilter::MFVEventFilter(const edm::ParameterSet& cfg)
  : jets_token(consumes<pat::JetCollection>(cfg.getParameter<edm::InputTag>("jets_src"))),
    jet_selector(cfg.getParameter<std::string>("jet_cut")),
    min_njets(cfg.getParameter<int>("min_njets")),
    min_pt_for_ht(cfg.getParameter<double>("min_pt_for_ht")),
    min_ht(cfg.getParameter<double>("min_ht")),
    muons_token(consumes<pat::MuonCollection>(cfg.getParameter<edm::InputTag>("muons_src"))),
    muon_selector(cfg.getParameter<std::string>("muon_cut")),
    min_muon_pt(cfg.getParameter<double>("min_muon_pt")),
    electrons_token(consumes<pat::ElectronCollection>(cfg.getParameter<edm::InputTag>("electrons_src"))),
    electron_selector(cfg.getParameter<std::string>("electron_cut")),
    min_electron_pt(cfg.getParameter<double>("min_electron_pt")),
    min_nleptons(cfg.getParameter<int>("min_nleptons")),
    debug(cfg.getUntrackedParameter<bool>("debug", false))
{
}

bool MFVEventFilter::filter(edm::Event& event, const edm::EventSetup&) {
  edm::Handle<pat::JetCollection> jets;
  event.getByToken(jets_token, jets);

  int njets = 0;
  double ht = 0;
  for (const pat::Jet& jet : *jets)
    if (jet_selector(jet)) {
      ++njets;
      if (jet.pt() > min_pt_for_ht)
        ht += jet.pt();
    }

  const bool jets_pass = njets >= min_njets && ht >= min_ht;

  if (debug) printf("MFVEventFilter: njets: %i  ht: %f pass? %i\n", njets, ht, jets_pass);

  if (jets_pass)
    return true;

  edm::Handle<pat::MuonCollection> muons;
  edm::Handle<pat::ElectronCollection> electrons;
  event.getByToken(muons_token, muons);
  event.getByToken(electrons_token, electrons);

  int nmuons = 0, nelectrons = 0;

  for (const pat::Muon& muon : *muons)
    if (muon_selector(muon) && muon.pt() > min_muon_pt)
      ++nmuons;

  for (const pat::Electron& electron : *electrons)
    if (electron_selector(electron) && electron.pt() > min_electron_pt)
      ++nelectrons;

  const bool leptons_pass = nmuons + nelectrons >= min_nleptons;

  if (debug) printf("MFVEventFilter: nmuons: %i nelectrons: %i pass? %i\n", nmuons, nelectrons, leptons_pass);

  return leptons_pass;
}

DEFINE_FWK_MODULE(MFVEventFilter);
