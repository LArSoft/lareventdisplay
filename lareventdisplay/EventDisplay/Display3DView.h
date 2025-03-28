///
/// \file    Display3DView.h
/// \brief   A view showing a 3D rendering of the detector
/// \author  messier@indiana.edu
///
#ifndef EVD_DISPLAY3DVIEW_H
#define EVD_DISPLAY3DVIEW_H
#include "RQ_OBJECT.h"

#include "nuevdb/EventDisplayBase/Canvas.h"

namespace evd {
  class Display3DPad;

  /// View of event shoing the XZ and YZ readout planes
  class Display3DView : public evdb::Canvas {
  public:
    RQ_OBJECT("evd::Display3DView")

  public:
    Display3DView(TGMainFrame* mf);

    const char* Description() const { return "3D Detector Display"; }
    const char* PrintTag() const { return "lar3d"; }
    void Draw(const char* opt = "");
    void CloseWindow();

  private:
    Display3DPad* fDisplay3DPad; /// Pad showing 3D view of the detector
  };
}

#endif
////////////////////////////////////////////////////////////////////////
