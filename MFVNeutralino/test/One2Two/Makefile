ROOTFLAGS=$(shell root-config --cflags --libs)

all: 2v_from_jets.exe statmodel.exe

2v_from_jets.exe: 2v_from_jets.cc ${CMSSW_BASE}/src/JMTucker/MFVNeutralino/src/MiniNtuple.cc ${CMSSW_BASE}/src/JMTucker/MFVNeutralino/interface/MiniNtuple.h
	g++ -g -Wall -I${CMSSW_BASE}/src -I${CMSSW_RELEASE_BASE}/src -std=c++14 $< -o $@ $(ROOTFLAGS) ${CMSSW_BASE}/src/JMTucker/MFVNeutralino/src/MiniNtuple.cc

statmodel.exe: statmodel.cc
	g++ -g -Wall -std=c++17 $^ -o $@ -lstdc++fs $(ROOTFLAGS) -I${CMSSW_BASE}/src -L${CMSSW_BASE}/lib/${SCRAM_ARCH} -lJMTuckerTools

clean:
	rm -f 2v_from_jets.exe statmodel.exe
