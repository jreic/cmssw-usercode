# https://twiki.cern.ch/twiki/bin/view/CMS/PdmVMCcampaignRunIIFall17GS
# https://cms-pdmv.cern.ch/mcm/public/restapi/requests/get_test/EXO-RunIIFall17GS-00073
# 9_3_9_patch1 cmsDriver.py Configuration/GenProduction/python/EXO-RunIIFall17GS-00073-fragment.py --fileout file:EXO-RunIIFall17GS-00073.root --mc --eventcontent RAWSIM --datatier GEN-SIM --conditions 93X_mc2017_realistic_v3 --beamspot Realistic25ns13TeVEarly2017Collision --step GEN,SIM --nThreads 8 --geometry DB:Extended --era Run2_2017 --python_filename EXO-RunIIFall17GS-00073_1_cfg.py --no_exec --customise Configuration/DataProcessing/Utils.addMonitoring -n 724 

import sys, FWCore.ParameterSet.Config as cms, dynamicconf
from Configuration.StandardSequences.Eras import eras

genonly = 'genonly' in sys.argv
debug = 'debug' in sys.argv
randomize = 'norandomize' not in sys.argv
salt = ''
maxevents = 1
jobnum = 1
scanpack = None

for arg in sys.argv:
    if arg.startswith('scanpack='):
        print 'scanpack: wiping out todos'
        todos = []
        scanpack = arg.replace('scanpack=','').split(',')
    elif arg.startswith('salt='):
        salt = arg.replace('salt=','')
    elif arg.startswith('maxevents='):
        maxevents = int(arg.replace('maxevents=',''))
    elif arg.startswith('jobnum='):
        jobnum = int(arg.replace('jobnum=',''))

################################################################################

process = cms.Process('SIM', eras.Run2_2017)

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.GeometrySimDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedRealistic25ns13TeVEarly2017Collision_cfi')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.MessageLogger.cerr.FwkReport.reportEvery = 10000
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(False))
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(maxevents))
process.source = cms.Source('EmptySource', firstLuminosityBlock = cms.untracked.uint32(jobnum))

process.XMLFromDBSource.label = cms.string("Extended")
process.genstepfilter.triggerConditions = cms.vstring('generation_step')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, dynamicconf.globaltag, '')

from Configuration.Generator.Pythia8CommonSettings_cfi import pythia8CommonSettingsBlock
from Configuration.Generator.MCTunes2017.PythiaCP2Settings_cfi import pythia8CP2SettingsBlock

process.generator = cms.EDFilter('Pythia8GeneratorFilter',
    comEnergy = cms.double(13000.0),
    filterEfficiency = cms.untracked.double(1.0),
    maxEventsToPrint = cms.untracked.int32(0),
    pythiaHepMCVerbosity = cms.untracked.bool(False),
    pythiaPylistVerbosity = cms.untracked.int32(0),
    PythiaParameters = cms.PSet(
        pythia8CommonSettingsBlock,
        pythia8CP2SettingsBlock,
        parameterSets = cms.vstring(
            'pythia8CommonSettings',
            'pythia8CP2Settings',
            'processParameters'
            ),
        processParameters = cms.vstring(),
        ),
    )

process.generation_step = cms.Path(process.pgen)
process.simulation_step = cms.Path(process.psim)
process.genfiltersummary_step = cms.EndPath(process.genFilterSummary)

process.RAWSIMoutput = cms.OutputModule('PoolOutputModule',
    SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('generation_step')),
    fileName = cms.untracked.string('gensim.root'),
    outputCommands = process.RAWSIMEventContent.outputCommands,
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(9),
    eventAutoFlushCompressedSize = cms.untracked.int32(20971520),
    splitLevel = cms.untracked.int32(0),
)

process.RAWSIMoutput_step = cms.EndPath(process.RAWSIMoutput)

if genonly:
    sched = [process.generation_step, process.genfiltersummary_step, process.RAWSIMoutput_step]
else:
    sched = [process.generation_step, process.genfiltersummary_step, process.simulation_step, process.RAWSIMoutput_step]

if debug:
    process.options.wantSummary = True
    process.MessageLogger.cerr.FwkReport.reportEvery = 1
    process.generator.maxEventsToPrint = 2
    process.generator.pythiaPylistVerbosity = 13
    process.generator.pythiaHepMCVerbosity = True
    process.p = cms.EDAnalyzer('JMTParticleListDrawer',
                               src = cms.InputTag('genParticles'),
                               printVertex = cms.untracked.bool(True),
                               )
    process.pp = cms.Path(process.p)
    sched.insert(-1, process.pp)

process.schedule = cms.Schedule(*sched)
#from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
#associatePatAlgosToolsTask(process)

process.ProductionFilterSequence = cms.Sequence(process.generator)
for path in process.paths:
    getattr(process,path)._seq = process.ProductionFilterSequence * getattr(process,path)._seq

if randomize:
    from modify import deterministic_seeds
    deterministic_seeds(process, 8675309, salt, jobnum)

if scanpack:
    from scanpack import do_scanpack
    scanpack_x, scanpack_batch = scanpack
    do_scanpack(process, scanpack_x, int(scanpack_batch), jobnum-1)

from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
