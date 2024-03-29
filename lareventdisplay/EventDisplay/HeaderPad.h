///
/// \file    HeaderPad.h
/// \brief   Drawing pad for time or charge histograms
/// \author  messier@indiana.edu
///
#ifndef EVD_HEADER_H
#define EVD_HEADER_H

#include "lareventdisplay/EventDisplay/DrawingPad.h"
namespace evdb {
  class View2D;
}

namespace evd {
  class HeaderPad : public DrawingPad {
  public:
    HeaderPad(const char* nm,
              const char* ti,
              double x1,
              double y1,
              double x2,
              double y2,
              const char* opt);
    ~HeaderPad();
    void Draw(const char* opt = "");

  private:
    evdb::View2D* fView; ///< Collection of drawn objects
  };
}

#endif
////////////////////////////////////////////////////////////////////////
