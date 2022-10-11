///
/// \file    HeaderPad.cxx
/// \brief   Drawing pad for time or charge histograms
/// \author  messier@indiana.edu
///
#include "lareventdisplay/EventDisplay/HeaderPad.h"
#include "lareventdisplay/EventDisplay/HeaderDrawer.h"
#include "nuevdb/EventDisplayBase/View2D.h"

#include "TPad.h"

namespace evd {

  //static const int kTPAD = 0;
  //static const int kQPAD = 1;
  //static const int kRAW   = 0;
  //static const int kCALIB = 1;
  //static const int kPE =  2;
  //static const int kTNS = 3;

  //......................................................................

  HeaderPad::HeaderPad(const char* nm,
                       const char* ti,
                       double x1,
                       double y1,
                       double x2,
                       double y2,
                       const char* /*opt*/)
    : DrawingPad(nm, ti, x1, y1, x2, y2)
  {
    fView = new evdb::View2D();
  }

  //......................................................................

  HeaderPad::~HeaderPad()
  {
    if (fView != 0) {
      delete fView;
      fView = 0;
    }
  }

  //......................................................................

  void HeaderPad::Draw(const char* /* opt */)
  {
    fView->Clear();

    this->HeaderDraw()->Header(fView);

    this->Pad()->Clear();
    this->Pad()->cd();
    fView->Draw();
  }

  //......................................................................

} // namespace
//////////////////////////////////////////////////////////////////////////
