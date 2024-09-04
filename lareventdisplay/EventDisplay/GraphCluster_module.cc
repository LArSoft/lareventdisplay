////////////////////////////////////////////////////////////////////////
/// \file  GraphCluster_module.cc
/// \brief
///
///
/// \author  andrzej.szelc@yale.edu
/// \author  ellen.klein@yale.edu
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/ModuleMacros.h"
#include <vector>

#ifdef __ROOTCLING__
namespace art {
  class EDProducer;
  class Event;
  class PtrVector;
  class Ptr;
  class ServiceHandle;
}

namespace fhicl {
  class ParameterSet;
}

namespace recob {
  class Hit;
}
#else
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "larcore/Geometry/Geometry.h"
#include "lardataalg/Utilities/StatCollector.h"
#endif

////////////////////////////////////////////////////////////////////////
//
// GraphCluster class
//
// andrzej.szelc@yale.edu
// ellen.klein@yale.edu
//
//  This dummy producer is designed to create a hitlist and maybe cluster from EVD input
////////////////////////////////////////////////////////////////////////

// Framework includes
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"

// LArSoft Includes
#include "larcorealg/Geometry/PlaneGeo.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "lardata/Utilities/AssociationUtil.h"
#include "lardata/Utilities/PxUtils.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lareventdisplay/EventDisplay/GraphClusterAlg.h"

namespace util {
  class PxPoint;
}

namespace geo {
  class Geometry;
}

namespace evd {

  class InfoTransfer;

  class GraphCluster : public art::EDProducer {

  public:
    explicit GraphCluster(fhicl::ParameterSet const&);
    void produce(art::Event& evt);

  private:
    GraphClusterAlg fGClAlg;

    void GetStartEndHits(unsigned int plane, recob::Hit* starthit, recob::Hit* endhit);
    void GetStartEndHits(unsigned int plane);

    void GetHitList(unsigned int plane, art::PtrVector<recob::Hit>& ptrhitlist);

    std::vector<util::PxLine> GetSeedLines();

    unsigned int fNPlanes;

    int TestFlag;
    int fRun;
    int fSubRun;
    int fEvent;

    std::vector<recob::Hit*> starthit;
    std::vector<recob::Hit*> endhit;

    std::vector<util::PxLine> startendpoints;
  }; // class GraphCluster

  //-------------------------------------------------
  GraphCluster::GraphCluster(fhicl::ParameterSet const& pset)
    : EDProducer{pset}, fGClAlg(pset.get<fhicl::ParameterSet>("GraphClusterAlg"))
  {
    art::ServiceHandle<geo::Geometry const> geo;

    produces<std::vector<recob::Cluster>>();
    produces<art::Assns<recob::Cluster, recob::Hit>>();
    produces<std::vector<art::PtrVector<recob::Cluster>>>();

    fNPlanes = geo->Nplanes();
    starthit.resize(fNPlanes);
    endhit.resize(fNPlanes);

    startendpoints.resize(fNPlanes);
  }

  //
  //-------------------------------------------------
  /// \todo This method appears to produce a recob::Cluster really as it is
  /// \todo a collection of 2D clusters from single planes
  void GraphCluster::produce(art::Event& evt)
  {

    std::unique_ptr<std::vector<recob::Cluster>> Graphcol(new std::vector<recob::Cluster>);
    std::unique_ptr<art::Assns<recob::Cluster, recob::Hit>> hassn(
      new art::Assns<recob::Cluster, recob::Hit>);
    std::unique_ptr<std::vector<art::PtrVector<recob::Cluster>>> classn(
      new std::vector<art::PtrVector<recob::Cluster>>);

    art::ServiceHandle<geo::Geometry const> geo;

    // check if evt and run numbers check out, etc...
    if (fGClAlg.CheckValidity(evt) == -1) { return; }

    for (unsigned int ip = 0; ip < fNPlanes; ip++) {
      startendpoints[ip] = util::PxLine(); //assign empty PxLine
    }

    std::vector<art::PtrVector<recob::Hit>> hitlist;
    hitlist.resize(fNPlanes);

    for (unsigned int ip = 0; ip < fNPlanes; ip++) {

      fGClAlg.GetHitListAndEndPoints(ip, hitlist[ip], startendpoints[ip]);

      if (hitlist[ip].size() == 0) continue;

      if (hitlist[ip].size() > 0 && !(TestFlag == -1)) //old event or transfer not ready
      {
        double swterror = 0., ewterror = 0.;

        if (startendpoints[ip].w0 == 0) swterror = 999;

        if (startendpoints[ip].t1 == 0) ewterror = 999;

        std::cout << " clustering @ " << startendpoints[ip].w0 << " +/- " << swterror << " "
                  << startendpoints[ip].t0 << " +/- " << swterror << " " << startendpoints[ip].w1
                  << " +/- " << ewterror << " " << startendpoints[ip].t1 << " +/- " << ewterror
                  << std::endl;

        // this is all we can do easily without getting the full-blown
        // ClusterParamsAlg (that means lareventdisplay has to depend on larreco!)
        lar::util::StatCollector<float> integral, summedADC;
        for (art::Ptr<recob::Hit> const& hit : hitlist[ip]) {
          integral.add(hit->Integral());
          summedADC.add(hit->ROISummedADC());
        } // for

        // get the plane ID from the first hit
        geo::PlaneID planeID = hitlist[ip].front()->WireID().planeID();
        Graphcol->emplace_back(startendpoints[ip].w0,
                               swterror,
                               startendpoints[ip].t0,
                               swterror,
                               0., // start_charge
                               0., // start_angle
                               0., // start_opening
                               startendpoints[ip].w1,
                               ewterror,
                               startendpoints[ip].t1,
                               ewterror,
                               0.,                 // end_charge
                               0.,                 // end_angle
                               0.,                 // end_opening
                               integral.Sum(),     // integral
                               integral.RMS(),     // integral_stddev
                               summedADC.Sum(),    // summedADC
                               summedADC.RMS(),    // summedADC_stddev
                               hitlist[ip].size(), // n_hits
                               0.,                 // multiple_hit_density
                               0.,                 // width
                               ip,
                               geo->Plane({planeID.asTPCID(), ip}).View(),
                               planeID,
                               recob::Cluster::Sentry);

        // associate the hits to this cluster
        util::CreateAssn(evt, *Graphcol, hitlist[ip], *hassn);
      }

    } // end of loop on planes

    art::PtrVector<recob::Cluster> cvec;
    cvec.reserve(fNPlanes);

    for (unsigned int ip = 0; ip < fNPlanes; ip++) {
      art::ProductID aid = evt.getProductID<std::vector<recob::Cluster>>();
      art::Ptr<recob::Cluster> aptr(aid, ip, evt.productGetter(aid));
      cvec.push_back(aptr);
    }

    classn->push_back(cvec);

    evt.put(std::move(Graphcol));
    evt.put(std::move(hassn));
    evt.put(std::move(classn));

    return;
  } // end of produce

  DEFINE_ART_MODULE(GraphCluster)

} //end of evd namespace
