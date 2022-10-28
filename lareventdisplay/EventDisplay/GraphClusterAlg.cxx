////////////////////////////////////////////////////////////////////////
//
// GraphClusterAlg class
//
// andrzej.szelc@yale.edu
// ellen.klein@yale.edu
//
//  Methods to use by a dummy producer
////////////////////////////////////////////////////////////////////////

#include "lareventdisplay/EventDisplay/GraphClusterAlg.h"
#include "larcore/Geometry/WireReadout.h"
#include "lardata/Utilities/PxUtils.h"
#include "lareventdisplay/EventDisplay/InfoTransfer.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

//-------------------------------------------------
evd::GraphClusterAlg::GraphClusterAlg(fhicl::ParameterSet const&)
{
  fNPlanes = art::ServiceHandle<geo::WireReadout>()->Get().Nplanes();
}

//-------------------------------------------------
void evd::GraphClusterAlg::GetHitListAndEndPoints(unsigned int plane,
                                                  art::PtrVector<recob::Hit>& ptrhitlist,
                                                  util::PxLine& startendpoints)
{
  GetHitList(plane, ptrhitlist);
  GetStartEndHits(plane, startendpoints);
}

void evd::GraphClusterAlg::GetStartEndHits(unsigned int plane, util::PxLine& startendpoints)
{
  std::vector<double> starthit;
  std::vector<double> endhit;
  art::ServiceHandle<evd::InfoTransfer const> intr;
  starthit = intr->GetStartHitCoords(plane);
  endhit = intr->GetEndHitCoords(plane);

  startendpoints.w0 = starthit[0];
  startendpoints.t0 = starthit[1];
  startendpoints.w1 = endhit[0];
  startendpoints.t1 = endhit[1];
  startendpoints.plane = plane;
}

//----------------------------------------------------------------------------
void evd::GraphClusterAlg::GetHitList(unsigned int plane, art::PtrVector<recob::Hit>& ptrhitlist)
{
  art::ServiceHandle<evd::InfoTransfer const> intr;

  std::vector<art::Ptr<recob::Hit>> ptlist = intr->GetHitList(plane);

  if (ptlist.size() == 0) {
    mf::LogVerbatim("GraphClusterAlg") << ("hit list of zero size, please select some hits");
    return;
  }

  for (art::PtrVector<recob::Hit>::const_iterator hitIter = ptlist.begin(); hitIter != ptlist.end();
       hitIter++) {
    ptrhitlist.push_back((*hitIter));
  }

  return;
}

//----------------------------------------------------------------------------
std::vector<util::PxLine> evd::GraphClusterAlg::GetSeedLines()
{

  art::ServiceHandle<evd::InfoTransfer const> intr;
  //////////////////////////////////////////////////
  //this is where you could create Bezier Tracks if you wanted to do it inside a producer
  //////////////////////////////////////////////////
  std::vector<util::PxLine> plines = intr->GetSeedList();

  std::cout << " Received Seed List of Size: " << plines.size() << std::endl;

  return plines;
}

int evd::GraphClusterAlg::CheckValidity(art::Event& evt)
{
  art::ServiceHandle<evd::InfoTransfer const> intr;
  TestFlag = intr->GetTestFlag();

  fEvent = intr->GetEvtNumber();
  fRun = intr->GetRunNumber();
  fSubRun = intr->GetSubRunNumber();

  if (TestFlag == -1) return -1;

  if (fEvent != (int)evt.id().event() || fRun != (int)evt.id().run() ||
      fSubRun != (int)evt.id().subRun()) {
    mf::LogVerbatim("GraphClusterAlg") << (" old event ");
    return -1;
  }

  return TestFlag;
}
