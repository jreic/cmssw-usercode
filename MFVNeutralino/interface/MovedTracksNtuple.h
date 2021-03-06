#ifndef JMTucker_MFVNeutralino_interface_MovedTracksNtuple_h
#define JMTucker_MFVNeutralino_interface_MovedTracksNtuple_h

#include <vector>
#include "TLorentzVector.h"
#include "JMTucker/MFVNeutralinoFormats/interface/HitPattern.h"

class TTree;

namespace mfv {
  struct MovedTracksNtuple {
    typedef unsigned char uchar;
    typedef unsigned short ushort;

    unsigned run;
    unsigned lumi;
    unsigned long long event;

    float weight;

    bool gen_valid;
    float gen_lsp_pt[2];
    float gen_lsp_eta[2];
    float gen_lsp_phi[2];
    float gen_lsp_mass[2];
    float gen_lsp_decay[2*3];
    uchar gen_decay_type[2];
    std::vector<float> gen_daughter_pt;
    std::vector<float> gen_daughter_eta;
    std::vector<float> gen_daughter_phi;
    std::vector<float> gen_daughter_mass;
    TLorentzVector gen_daughter_p4(int i) const;
    std::vector<int> gen_daughter_id;

    uchar pass_hlt;
    float bsx;
    float bsy;
    float bsz;
    float bsdxdz;
    float bsdydz;
    float bsx_at_z(float z) const { return bsx + bsdxdz * (z - bsz); }
    float bsy_at_z(float z) const { return bsy + bsdydz * (z - bsz); }
    uchar npu;
    uchar npv;
    float pvx;
    float pvy;
    float pvz;
    ushort pvntracks;
    float pvscore;
    float jetht;
    ushort ntracks;
    uchar nmovedtracks;

    std::vector<float> alljets_pt;
    std::vector<float> alljets_eta;
    std::vector<float> alljets_phi;
    std::vector<float> alljets_energy;
    TLorentzVector alljets_p4(size_t i) const;
    std::vector<uchar> alljets_ntracks;
    std::vector<float> alljets_bdisc;
    std::vector<uchar> alljets_hadronflavor;
    std::vector<bool>  alljets_moved;
    size_t nalljets() const { return p_alljets_pt ? p_alljets_pt->size() : alljets_pt.size(); }
    std::vector<int> alljets_tracks(int i, double dRmax=0.4) const;

    // JMTBAD "presel" on these two really doesn't mean anything other than they have pt > 20 and pass the jet id
    uchar npreseljets; // JMTBAD this is actually # of jets with bdisc < veto
    uchar npreselbjets;

    float move_x;
    float move_y;
    float move_z;
    TVector3 move_vector() const;
    double move_tau() const;

    std::vector<float> vtxs_x;
    std::vector<float> vtxs_y;
    std::vector<float> vtxs_z;
    std::vector<float> vtxs_pt; // this and next three are from tracksplusjets momentum
    std::vector<float> vtxs_theta;
    std::vector<float> vtxs_phi;
    std::vector<float> vtxs_mass;
    std::vector<float> vtxs_tkonlymass;
    std::vector<uchar> vtxs_ntracks;
    std::vector<float> vtxs_bs2derr;
    std::vector<int> vtxs_tracks(int i) const;
    size_t nvtxs() const { return p_vtxs_x ? p_vtxs_x->size() : vtxs_x.size(); }

    std::vector<float> tks_qpt;
    std::vector<float> tks_eta;
    std::vector<float> tks_phi;
    std::vector<float> tks_dxy;
    std::vector<float> tks_dz;
    std::vector<float> tks_err_pt;
    std::vector<float> tks_err_eta;
    std::vector<float> tks_err_phi;
    std::vector<float> tks_err_dxy;
    std::vector<float> tks_err_dz;
    std::vector<mfv::HitPattern::value_t> tks_hp_;
    std::vector<bool> tks_moved;
    std::vector<uchar> tks_vtx;
    size_t ntks() const { return p_tks_qpt ? p_tks_qpt->size() : tks_qpt.size(); }
    float tks_pt(int i) const { return fabs(p_tks_qpt ? (*p_tks_qpt)[i] : tks_qpt[i]); }
    TVector3 tks_p(int i) const { TVector3 p; p.SetPtEtaPhi(tks_pt(i), p_tks_eta ? (*p_tks_eta)[i] : tks_eta[i], p_tks_phi ? (*p_tks_phi)[i] : tks_phi[i]); return p; }
    void tks_hp_push_back(int npxh, int nsth, int npxl, int nstl) { tks_hp_.push_back(mfv::HitPattern(npxh, nsth, npxl, nstl).value); }
    mfv::HitPattern tks_hp(int i) const { return mfv::HitPattern(p_tks_hp_ ? (*p_tks_hp_)[i] : tks_hp_[i]); }
    int tks_npxhits(int i) const { return tks_hp(i).npxhits(); }
    int tks_nsthits(int i) const { return tks_hp(i).nsthits(); }
    int tks_nhits(int i) const { return tks_hp(i).nhits(); }
    int tks_npxlayers(int i) const { return tks_hp(i).npxlayers(); }
    int tks_nstlayers(int i) const { return tks_hp(i).nstlayers(); }
    int tks_nlayers(int i) const { return tks_hp(i).nlayers(); }
    float tks_nsigmadxy(int i) const { return fabs(p_tks_dxy ? (*p_tks_dxy)[i] : tks_dxy[i]) / (p_tks_err_dxy ? (*p_tks_err_dxy)[i] : tks_err_dxy[i]); }
    bool tks_sel(int i) const;

    ////

    MovedTracksNtuple();
    void clear();
    void write_to_tree(TTree* tree);
    void read_from_tree(TTree* tree);

    // ugh
    std::vector<float>* p_gen_daughter_pt;
    std::vector<float>* p_gen_daughter_eta;
    std::vector<float>* p_gen_daughter_phi;
    std::vector<float>* p_gen_daughter_mass;
    std::vector<int>* p_gen_daughter_id;
    std::vector<float>* p_alljets_pt;
    std::vector<float>* p_alljets_eta;
    std::vector<float>* p_alljets_phi;
    std::vector<float>* p_alljets_energy;
    std::vector<uchar>* p_alljets_ntracks;
    std::vector<float>* p_alljets_bdisc;
    std::vector<uchar>* p_alljets_hadronflavor;
    std::vector<bool>* p_alljets_moved;
    std::vector<float>* p_vtxs_x;
    std::vector<float>* p_vtxs_y;
    std::vector<float>* p_vtxs_z;
    std::vector<float>* p_vtxs_pt;
    std::vector<float>* p_vtxs_theta;
    std::vector<float>* p_vtxs_phi;
    std::vector<float>* p_vtxs_mass;
    std::vector<float>* p_vtxs_tkonlymass;
    std::vector<uchar>* p_vtxs_ntracks;
    std::vector<float>* p_vtxs_bs2derr;
    std::vector<float>* p_tks_qpt;
    std::vector<float>* p_tks_eta;
    std::vector<float>* p_tks_phi;
    std::vector<float>* p_tks_dxy;
    std::vector<float>* p_tks_dz;
    std::vector<float>* p_tks_err_pt;
    std::vector<float>* p_tks_err_eta;
    std::vector<float>* p_tks_err_phi;
    std::vector<float>* p_tks_err_dxy;
    std::vector<float>* p_tks_err_dz;
    std::vector<mfv::HitPattern::value_t>* p_tks_hp_;
    std::vector<bool>* p_tks_moved;
    std::vector<uchar>* p_tks_vtx;
  };
}

#endif
