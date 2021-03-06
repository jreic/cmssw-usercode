import FWCore.ParameterSet.Config as cms

from JMTucker.MFVNeutralino.JetTrackRefGetter_cff import *

mfvVerticesAuxTmp = cms.EDProducer('MFVVertexAuxProducer',
                                   beamspot_src = cms.InputTag('offlineBeamSpot'),
                                   primary_vertex_src = cms.InputTag('goodOfflinePrimaryVertices'),
                                   muons_src = cms.InputTag('selectedPatMuons'),
                                   electrons_src = cms.InputTag('selectedPatElectrons'),
                                   gen_vertices_src = cms.InputTag('mfvGenParticles', 'decays'),
                                   vertex_src = cms.InputTag('mfvVertices'),
                                   sv_to_jets_src = cms.string('dummy'),
                                   jet_track_ref_getter = mfvJetTrackRefGetter,
                                   sort_by = cms.string('ntracks_then_mass'),
                                   verbose = cms.untracked.bool(False),
                                   )

mfvVerticesAux = mfvVerticesAuxTmp.clone(sv_to_jets_src = 'mfvVerticesToJets')
