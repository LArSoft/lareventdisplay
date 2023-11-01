///
/// \file    TQPad.cxx
/// \brief   Drawing pad for time or charge histograms
/// \author  messier@indiana.edu
///
#include "lareventdisplay/EventDisplay/TQPad.h"
#include "TH1F.h"
#include "TPad.h"

#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Utilities/make_tool.h"
#include "cetlib_except/exception.h"

#include "larcore/Geometry/WireReadout.h"
#include "lareventdisplay/EventDisplay/ColorDrawingOptions.h"
#include "lareventdisplay/EventDisplay/RawDataDrawer.h"
#include "lareventdisplay/EventDisplay/RawDrawingOptions.h"
#include "lareventdisplay/EventDisplay/RecoDrawingOptions.h"
#include "lareventdisplay/EventDisplay/wfHitDrawers/IWFHitDrawer.h"
#include "lareventdisplay/EventDisplay/wfHitDrawers/IWaveformDrawer.h"
#include "nuevdb/EventDisplayBase/EventHolder.h"
#include "nuevdb/EventDisplayBase/View2D.h"

// C/C++ standard libraries
#include <algorithm> // std::min(), std::max()

namespace evd {

  static const int kRAW = 0;
  static const int kCALIB = 1;
  static const int kQ = 0;
  static const int kTQ = 1;

  //......................................................................

  TQPad::TQPad(const char* nm,
               const char* ti,
               double x1,
               double y1,
               double x2,
               double y2,
               const char* opt,
               unsigned int plane,
               unsigned int wire)
    : DrawingPad(nm, ti, x1, y1, x2, y2), fWire(wire), fPlane(plane), fFrameHist(0)
  {
    unsigned int planes = art::ServiceHandle<geo::WireReadout const>()->Get().Nplanes();

    Pad()->cd();

    Pad()->SetLeftMargin(0.050);
    Pad()->SetRightMargin(0.050);

    Pad()->SetTopMargin(0.005);
    Pad()->SetBottomMargin(0.110);

    // there has to be a better way of doing this that does
    // not have a case for each number of planes in a detector
    if (planes == 2 && fPlane > 0) {
      Pad()->SetTopMargin(0.110);
      Pad()->SetBottomMargin(0.010);
    }
    else if (planes > 2) {
      if (fPlane == 1) {
        Pad()->SetTopMargin(0.005);
        Pad()->SetBottomMargin(0.010);
      }
      else if (fPlane == 2) {
        Pad()->SetTopMargin(0.110);
        Pad()->SetBottomMargin(0.010);
      }
    }

    std::string opts(opt);
    if (opts == "TQ") {
      fTQ = kTQ;
      // BB adjust the vertical spacing
      Pad()->SetTopMargin(0);
      Pad()->SetBottomMargin(0.2);
    }
    if (opts == "Q") { fTQ = kQ; }

    BookHistogram();
    fView = new evdb::View2D();

    art::ServiceHandle<evd::RawDrawingOptions const> rawOptions;
    art::ServiceHandle<evd::RecoDrawingOptions const> recoOptions;

    fHitDrawerTool = art::make_tool<evdb_tool::IWFHitDrawer>(recoOptions->fHitDrawerParams);
    fRawDigitDrawerTool =
      art::make_tool<evdb_tool::IWaveformDrawer>(rawOptions->fRawDigitDrawerParams);
    fWireDrawerTool = art::make_tool<evdb_tool::IWaveformDrawer>(recoOptions->fWireDrawerParams);
  }

  //......................................................................

  TQPad::~TQPad()
  {
    if (fView) {
      delete fView;
      fView = nullptr;
    }
    if (fFrameHist) {
      delete fFrameHist;
      fFrameHist = nullptr;
    }
  }

  //......................................................................
  void TQPad::Draw()
  {
    art::ServiceHandle<evd::RawDrawingOptions const> drawopt;

    //grab the singleton with the event
    const art::Event* evt = evdb::EventHolder::Instance()->GetEvent();
    if (!evt) return;

    fPad->Clear();
    fPad->cd();

    // Note this handles drawing waveforms for both SP and DP where the difference is handled by the tools
    if (fTQ == kTQ) {
      // Recover a channel number from current information
      geo::WireID const wireid{drawopt->fCryostat, drawopt->fTPC, fPlane, fWire};
      raw::ChannelID_t channel =
        art::ServiceHandle<geo::WireReadout const>()->Get().PlaneWireToChannel(wireid);

      // Call the tools to fill the histograms for RawDigits and Wire data
      fRawDigitDrawerTool->Fill(
        *fView, channel, RawDataDraw()->StartTick(), RawDataDraw()->TotalClockTicks());
      fWireDrawerTool->Fill(
        *fView, channel, RawDataDraw()->StartTick(), RawDataDraw()->TotalClockTicks());

      // Vertical limits set for the enclosing histogram, then draw it with axes only
      float maxLowVal = std::min(fRawDigitDrawerTool->getMinimum(), fWireDrawerTool->getMinimum());
      float maxHiVal = std::max(fRawDigitDrawerTool->getMaximum(), fWireDrawerTool->getMaximum());

      if (drawopt->fDrawRawDataOrCalibWires == kCALIB) {
        maxLowVal = fWireDrawerTool->getMinimum();
        maxHiVal = fWireDrawerTool->getMaximum();
      }

      if (maxLowVal < std::numeric_limits<float>::max())
        maxLowVal -= 5.;
      else
        maxLowVal = -10.;
      if (maxHiVal > std::numeric_limits<float>::lowest())
        maxHiVal += 5.;
      else
        maxHiVal = 10.;

      fFrameHist->SetMaximum(maxHiVal);
      fFrameHist->SetMinimum(maxLowVal);
      fFrameHist->Draw("AXIS");

      // draw with histogram style, only (square) lines, no errors
      static const std::string defaultDrawOptions = "HIST same";

      // Draw the desired histograms
      // If its not just the raw hists then we output the wire histograms
      if (drawopt->fDrawRawDataOrCalibWires != kRAW) {
        fWireDrawerTool->Draw(defaultDrawOptions.c_str(), maxLowVal, maxHiVal);

        fHitDrawerTool->Draw(*fView, channel);
      }

      // Likewise, if it is not just the calib hists then we output the raw histogram
      if (drawopt->fDrawRawDataOrCalibWires != kCALIB)
        fRawDigitDrawerTool->Draw(defaultDrawOptions.c_str(), maxLowVal, maxHiVal);

      // This is a remnant from a time long past...
      fFrameHist->SetTitleOffset(0.2, "Y");
    } // end if fTQ == kTQ
  }

  //......................................................................
  void TQPad::BookHistogram()
  {
    if (fFrameHist) {
      delete fFrameHist;
      fFrameHist = 0;
    }

    art::ServiceHandle<evd::ColorDrawingOptions const> cst;
    art::ServiceHandle<evd::RawDrawingOptions const> drawopt;

    // figure out the signal type for this plane, assume that
    // plane n in each TPC/cryostat has the same type
    geo::PlaneID planeid(drawopt->CurrentTPC(), fPlane);
    geo::SigType_t sigType =
      art::ServiceHandle<geo::WireReadout const>()->Get().SignalType(planeid);

    /// \todo decide if ndivraw and ndivreco are useful
    double qxloraw = cst->fRawQLow[(size_t)sigType];
    double qxhiraw = cst->fRawQHigh[(size_t)sigType];
    double tqxlo = 1. * RawDataDraw()->StartTick();
    double tqxhi = 1. * RawDataDraw()->TotalClockTicks();

    switch (fTQ) {
    case kQ:
      fFrameHist = new TH1F("fFrameHist", ";t [ticks];[ADC]", 2, 0., 1.);
      fFrameHist->SetMaximum(qxhiraw);
      fFrameHist->SetMinimum(qxloraw);
      break; // kQ
    case kTQ:
      fFrameHist = new TH1F("fFrameHist", ";t [ticks];q [ADC]", (int)tqxhi, tqxlo, tqxhi + tqxlo);
      break;
    default: throw cet::exception("TQPad") << __func__ << ": unexpected quantity #" << fTQ << "\n";
    } //end if fTQ == kTQ

    // Set the label, title size and offsets
    // Note this is the base histogram so this control these for both the raw and wire histograms
    fFrameHist->GetXaxis()->SetLabelSize(0.10);
    fFrameHist->GetXaxis()->SetLabelOffset(0.00);
    fFrameHist->GetXaxis()->SetTitleSize(0.10);
    fFrameHist->GetXaxis()->SetTitleOffset(0.80);

    fFrameHist->GetYaxis()->SetLabelSize(0.10);
    fFrameHist->GetYaxis()->SetLabelOffset(0.01);
    fFrameHist->GetYaxis()->SetTitleSize(0.10);
    fFrameHist->GetYaxis()->SetTitleOffset(0.80);
  }

}
//////////////////////////////////////////////////////////////////////////
