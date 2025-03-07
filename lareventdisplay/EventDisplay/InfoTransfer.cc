////////////////////////////////////////////////////////////////////////
//
// Transfer hitlist and run info into a producer module.
// Do not copy this code without contacting Andrzej Szelc and Brian Rebel first.
//
// \author andrzej.szelc@yale.edu
// ellen.klein@yale.edu
////////////////////////////////////////////////////////////////////////

#include "lareventdisplay/EventDisplay/InfoTransfer.h"

#include "larcore/Geometry/WireReadout.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "lardata/Utilities/PxUtils.h"
#include "lardataobj/RecoBase/Hit.h"

#include "nuevdb/EventDisplayBase/NavState.h"

// Framework includes
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace {
  void WriteMsg(const char* fcn)
  {
    mf::LogWarning("InfoTransfer") << "InfoTransfer::" << fcn << " \n";
  }
}

namespace evd {

  //......................................................................
  InfoTransfer::InfoTransfer(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg)
    : evdb::Reconfigurable{pset}
  {
    this->reconfigure(pset);
    testflag = -1;
    fEvt = -1;
    fRun = -1;
    fSubRun = -1;
    reg.sPreProcessEvent.watch(this, &InfoTransfer::Rebuild);
    auto const& wireReadoutGeom = art::ServiceHandle<geo::WireReadout const>()->Get();
    unsigned int nplanes = wireReadoutGeom.Nplanes();

    fSelectedHitlist.resize(nplanes);
    fStartHit.resize(nplanes);
    fRefStartHit.resize(nplanes);
    fEndHit.resize(nplanes);
    fRefEndHit.resize(nplanes);
    starthitout.resize(nplanes);
    endhitout.resize(nplanes);
    refstarthitout.resize(nplanes);
    refendhitout.resize(nplanes);
    for (unsigned int i = 0; i < nplanes; i++) {
      starthitout[i].resize(2);
      endhitout[i].resize(2);
      refstarthitout[i].resize(2);
      refendhitout[i].resize(2);
    }
  }

  //......................................................................
  InfoTransfer::~InfoTransfer() {}

  //......................................................................
  void InfoTransfer::reconfigure(fhicl::ParameterSet const& pset)
  {
    auto const& wireReadoutGeom = art::ServiceHandle<geo::WireReadout const>()->Get();
    unsigned int nplanes = wireReadoutGeom.Nplanes();
    //clear everything
    fRefinedHitlist.resize(nplanes);
    fSelectedHitlist.resize(nplanes);
    for (unsigned int i = 0; i < nplanes; i++) {
      fRefinedHitlist[i].clear();
      fSelectedHitlist[i].clear();
    }
    fHitModuleLabel = pset.get<std::string>("HitModuleLabel", "ffthit");
  }

  //......................................................................
  void InfoTransfer::Rebuild(const art::Event& evt, art::ScheduleContext)
  {
    auto const& wireReadoutGeom = art::ServiceHandle<geo::WireReadout const>()->Get();
    unsigned int nplanes = wireReadoutGeom.Nplanes();
    unsigned int which_call = evdb::NavState::Which();
    if (which_call != 2) {
      //unless we're reloading we want to clear all the selected and refined hits
      fRefinedHitlist.resize(nplanes);
      fSelectedHitlist.resize(nplanes);
      for (unsigned int j = 0; j < nplanes; j++) {
        fRefinedHitlist[j].clear();
        fSelectedHitlist[j].clear();
        starthitout[j].clear();
        endhitout[j].clear();
        starthitout[j].resize(2);
        endhitout[j].resize(2);
        refstarthitout[j].clear();
        refendhitout[j].clear();
        refstarthitout[j].resize(2);
        refendhitout[j].resize(2);
      }
      //also clear start and end points
      fRefStartHit.clear();
      fRefEndHit.clear();
      fFullHitlist.clear();
    }
    art::Handle<std::vector<recob::Hit>> hHandle;

    fEvt = evt.id().event();
    fRun = evt.id().run();
    fSubRun = evt.id().subRun();
    evt.getByLabel(fHitModuleLabel, hHandle);

    if (hHandle.failedToGet()) { return; }

    // Clear out anything remaining from previous calls to Rebuild

    fRefinedHitlist.resize(nplanes);

    for (unsigned int i = 0; i < nplanes; i++) {
      fRefinedHitlist[i].clear(); ///< the refined hitlist after rebuild
    }

    fFullHitlist.clear();
    for (unsigned int i = 0; i < fRefStartHit.size(); i++) {
      fRefStartHit[i] = NULL;
      fRefEndHit[i] = NULL;
    }

    /////Store start and end hits in new lists and clear the old ones:
    for (unsigned int i = 0; i < nplanes; i++) {
      refstarthitout[i].clear();
      refendhitout[i].clear();
      refstarthitout[i].resize(2);
      refendhitout[i].resize(2);

      refstarthitout[i] = starthitout[i];
      refendhitout[i] = endhitout[i];

      starthitout[i].clear();
      endhitout[i].clear();
      starthitout[i].resize(2);
      endhitout[i].resize(2);
    }

    for (size_t p = 0; p < hHandle->size(); ++p) {
      art::Ptr<recob::Hit> hit(hHandle, p);
      fFullHitlist.push_back(hit);
    }

    // fill the selected Hits into the fRefinedHitList from the fSelectedHitList
    char buf[200];
    for (unsigned int j = 0; j < nplanes; j++) {
      sprintf(buf,
              " ++++rebuilding with %lu selected hits in plane %u \n",
              fSelectedHitlist[j].size(),
              j);
      WriteMsg(buf);
    }

    for (size_t t = 0; t < fFullHitlist.size(); ++t) {
      for (unsigned int ip = 0; ip < nplanes; ip++) {
        for (size_t xx = 0; xx < fSelectedHitlist[ip].size(); ++xx) {
          if (fFullHitlist[t] == fSelectedHitlist[ip][xx]) {
            fRefinedHitlist[ip].push_back(fFullHitlist[t]);
            break;
          }
        }

        if (fStartHit[ip] && fFullHitlist[t].get() == fStartHit[ip]) {
          fRefStartHit[ip] = const_cast<recob::Hit*>(fFullHitlist[t].get());
        }

        if (fEndHit[ip] && fFullHitlist[t].get() == fEndHit[ip]) {
          fRefEndHit[ip] = const_cast<recob::Hit*>(fFullHitlist[t].get());
        }
      }
    }

    fSelectedHitlist.clear();
    fSelectedHitlist = fRefinedHitlist;

    return;
  }

  //......................................................................
  void InfoTransfer::SetSeedList(std::vector<util::PxLine> seedlines)
  {
    fSeedList = seedlines;
  }

  //......................................................................
  std::vector<util::PxLine> const& InfoTransfer::GetSeedList() const
  {
    return fSeedList;
  }

  //......................................................................
  void InfoTransfer::FillStartEndHitCoords(unsigned int plane)
  {
    if (fRefStartHit[plane]) {
      starthitout[plane][1] = fRefStartHit[plane]->PeakTime();
      try {
        if (fRefStartHit[plane]->WireID().isValid) {
          starthitout[plane][0] = fRefStartHit[plane]->WireID().Wire;
        }
        else {
          starthitout[plane][0] = 0;
        }
      }
      catch (cet::exception const& e) {
        mf::LogWarning("GraphCluster") << "caught exception \n" << e;
        starthitout[plane][0] = 0;
      }
    }
    else {
      starthitout[plane][1] = 0.;
      starthitout[plane][0] = 0.;
    }

    if (fRefEndHit[plane]) {
      endhitout[plane][1] = fRefEndHit[plane]->PeakTime();
      try {
        if (fRefEndHit[plane]->WireID().isValid) {
          endhitout[plane][0] = fRefEndHit[plane]->WireID().Wire;
        }
        else {
          endhitout[plane][0] = 0;
        }
      }
      catch (cet::exception const& e) {
        mf::LogWarning("GraphCluster") << "caught exception \n" << e;
        endhitout[plane][0] = 0;
      }
    }
    else {
      endhitout[plane][1] = 0.;
      endhitout[plane][0] = 0.;
    }
  }

} //namespace
////////////////////////////////////////////////////////////////////////
