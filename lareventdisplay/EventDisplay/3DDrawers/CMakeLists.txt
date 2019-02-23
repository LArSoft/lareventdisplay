cet_enable_asserts()

art_make( TOOL_LIBRARIES
            lareventdisplay_EventDisplay
            lareventdisplay_EventDisplay_ColorDrawingOptions_service
            lareventdisplay_EventDisplay_RecoDrawingOptions_service
            lareventdisplay_EventDisplay_InfoTransfer_service
            lareventdisplay_EventDisplay_AnalysisDrawingOptions_service
            lareventdisplay_EventDisplay_EvdLayoutOptions_service
            lareventdisplay_EventDisplay_RawDrawingOptions_service
            lareventdisplay_EventDisplay_RecoDrawingOptions_service
            larsim_Simulation nutools_ParticleNavigation lardataobj_Simulation
            lardataobj_RawData
            lardataobj_RecoBase
            lardata_RecoObjects larreco_Deprecated
            lardata_ArtDataHelper
            lardata_Utilities
            lardataobj_AnalysisBase
            larreco_Calorimetry
            larevt_CalibrationDBI_IOVData
            larevt_CalibrationDBI_Providers
            nusimdata_SimulationBase
            nutools_EventDisplayBase
            larcorealg_Geometry
            larcore_Geometry_Geometry_service
            lardata_Utilities
            ${ART_FRAMEWORK_CORE}
            ${ART_FRAMEWORK_PRINCIPAL}
            ${ART_FRAMEWORK_SERVICES_REGISTRY}
            art_Persistency_Common
            art_Persistency_Provenance
            ${ART_FRAMEWORK_SERVICES_BASIC}
            ${ART_FRAMEWORK_SERVICES_OPTIONAL}
            ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
            ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
            ${ART_FRAMEWORK_SERVICES_SYSTEM_FILECATALOGMETADATA_SERVICE}
            art_Utilities
            canvas
            nutools_EventDisplayBase
            ${MF_MESSAGELOGGER}
            ${FHICLCPP}
            ${CETLIB}
            ${Boost_FILESYSTEM_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
            ${CLHEP}
            ${ROOT_BASIC_LIB_LIST}
            ${ROOT_EVE_LIB_LIST}
            ${ROOT_X3d}
    )

install_headers()
install_source()
install_fhicl()