////////////////////////////////////////////////////////////////////////
/// \file RawDrawingOptions_service.cc
///
/// \author  brebel@fnal.gov

// Framework includes
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"

/// LArSoft includes
#include "lareventdisplay/EventDisplay/RawDrawingOptions.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"

DEFINE_ART_SERVICE(evd::RawDrawingOptions)
