////////////////////////////////////////////////////////////////////////
// GraphClusterAlg.h
//
// GraphClusterAlg class
//
// Andrzej Szelc (andrzej.szelc@yale.edu)
//
////////////////////////////////////////////////////////////////////////
#ifndef GRAPHCLUSTERALG_H
#define GRAPHCLUSTERALG_H

#include <vector>

#include "art/Framework/Principal/fwd.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "fhiclcpp/fwd.h"

namespace util {
  class PxLine;
}

namespace recob {
  class Hit;
}

namespace evd {

  class GraphClusterAlg {
  public:
    GraphClusterAlg(fhicl::ParameterSet const&);

    void GetStartEndHits(unsigned int plane, util::PxLine& startendpoints);

    //void GetHitList(unsigned int plane,std::vector< art::Ptr <recob::Hit> > ptrhitlist);
    void GetHitList(unsigned int plane, art::PtrVector<recob::Hit>& ptrhitlist);

    void GetHitListAndEndPoints(unsigned int plane,
                                art::PtrVector<recob::Hit>& ptrhitlist,
                                util::PxLine& startendpoints);

    int CheckValidity(art::Event& evt);

  private:
    std::vector<util::PxLine> GetSeedLines();

    unsigned int fNPlanes;

    int TestFlag;
    int fRun;
    int fSubRun;
    int fEvent;
  }; //class GraphClusterAlg

} //namespace evd

#endif
