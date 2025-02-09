#!/bin/bash

INPUTDIR=/dune/data/users/chasnip
OUTPUTDIR=/dune/data/users/chasnip/StateFiles_2022

NDFHCPNFSDIR=/pnfs/dune/persistent/users/chasnip/NDCAF_OnAxisHadd/FHC
NDRHCPNFSDIR=/pnfs/dune/persistent/users/chasnip/NDCAF_OnAxisHadd/RHC

# NDFHCPNFSDIR=/root/work/software/lblpwgtools/CAFAna/inputs/ndcafs/FHC
# NDRHCPNFSDIR=/root/work/software/lblpwgtools/CAFAna/inputs/ndcafs/RHC

#ListOSysts="allsyst"
#ListOSysts="noxsec:nodet"
#ListOSysts="noflux:nodet"
#ListOSysts="noxsec:noflux"
ListOSysts="nosyst"
FakeData="MissingProtonFakeData_pos"
AnaBinning="prism_default"
AvaVar="EVisReco"

#FD
MakePRISMPredInterps -o ${OUTPUTDIR}/PRISMState_EVisReco_3.5YrFull_FDOnly.root \
    -F-nu ${INPUTDIR}/OffAxisCAFs/FD_FHC_nonswap.root \
    -F-nub ${INPUTDIR}/OffAxisCAFs/FD_RHC_nonswap.root \
    -Fe-nu ${INPUTDIR}/OffAxisCAFs/FD_FHC_nueswap.root \
    -Fe-nub ${INPUTDIR}/OffAxisCAFs/FD_RHC_nueswap.root \
    -Ft-nu ${INPUTDIR}/OffAxisCAFs/FD_FHC_tauswap.root \
    -Ft-nub ${INPUTDIR}/OffAxisCAFs/FD_RHC_tauswap.root \
    --bin-descriptor ${AnaBinning} \
    --syst-descriptor ${ListOSysts} \
    --no-fakedata-dials \
    -A ${AvaVar} \
    --PRISM-fake-data ${FakeData} \
    --UseSelection \
    #-n 10000

#ND nu
#CPUPROFILE=MPPI.prof MakePRISMPredInterps -o PRISMState_EVisReco_3.5YrFull_ND_nuOnly.root \
#    -N-nu "${NDFHCPNFSDIR}/CAFv7_*.root" \
#    --bin-descriptor ${AnaBinning} \
#    --syst-descriptor ${ListOSysts} \
#    --no-fakedata-dials \
#    -A ${AvaVar} \
#    --PRISM-fake-data ${FakeData} \
#    --UseSelection \
#    #-n 10000


#ND nub
#MakePRISMPredInterps -o PRISMState_EVisReco_3.5YrFull_ND_nubOnly.root \
#    -N-nub "${NDRHCPNFSDIR}/CAFv7_*.root" \
#    --bin-descriptor ${AnaBinning} \
#    --syst-descriptor ${ListOSysts} \
#    --no-fakedata-dials \
#    -A ${AvaVar} \
#    --PRISM-fake-data ${FakeData} \
#    --UseSelection \
#    -n 10000

