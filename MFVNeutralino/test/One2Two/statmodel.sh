#!/bin/bash

#echo script starting on $(date)

wd=$(pwd)
job=$1

source /cvmfs/cms.cern.ch/cmsset_default.sh
cd /cvmfs/cms.cern.ch/slc6_amd64_gcc630/cms/cmssw-patch/CMSSW_9_4_6_patch1
eval `scramv1 runtime -sh`
cd $wd

export LD_LIBRARY_PATH=$(pwd):${LD_LIBRARY_PATH}

samples_index=$((job/9))
year_index=$(((job%9)/3))
ntracks=$((3+job%3))
echo samples_index $samples_index year_index $year_index ntracks $ntracks

out_fn=sm_samples${samples_index}_year${year_index}_ntk${ntracks}

env sm_out_fn=${out_fn} sm_samples_index=${samples_index} sm_year_index=${year_index} sm_ntracks=${ntracks} ./statmodel.exe 2>&1 > ${out_fn}.txt
