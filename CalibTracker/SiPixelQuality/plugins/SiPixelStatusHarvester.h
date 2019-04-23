#ifndef SiPixelStatusHarvester_H
#define SiPixelStatusHarvester_H

/** \class SiPixelStatusHarvester
 *  harvest per-lumi prduced SiPixelDetector status and make the payload for SiPixelQualityFromDB
 *
 */
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
// Pixel quality harvester
#include "CalibTracker/SiPixelQuality/interface/SiPixelStatusManager.h"
#include "CondFormats/SiPixelObjects/interface/SiPixelQuality.h"
// PixelDQM Framework
#include "DQM/SiPixelPhase1Common/interface/SiPixelPhase1Base.h"
// PixelPhase1 HelperClass
#include "DQM/SiPixelPhase1Common/interface/SiPixelCoordinates.h"

// Threshold testing
#include "TH1.h"
#include "TFile.h"

class SiPixelStatusHarvester : public one::DQMEDAnalyzer<edm::one::WatchLuminosityBlocks>, private HistogramManagerHolder {
    enum {
      BADROCp001,
      GOODROCp001,
      BADROCp005,
      GOODROCp005,
      BADROCp01,
      GOODROCp01,
      BADROCp05,
      GOODROCp05,
      BADROCp1,
      GOODROCp1,
      BADROCp2,
      GOODROCp2,
      BADROCp5,
      GOODROCp5,
      BADROC,
      PERMANENTBADROC,
      FEDERRORROC,
      STUCKTBMROC,
      OTHERBADROC,
      PROMPTBADROC
    };

 public:

  // Constructor
  SiPixelStatusHarvester(const edm::ParameterSet&);

  // Destructor
  ~SiPixelStatusHarvester() override;
  
  // Operations
  void beginJob            () override;
  void endJob              () override;  
  void bookHistograms      (DQMStore::IBooker& iBooker, edm::Run const&, edm::EventSetup const& iSetup ) final;
  void endRunProduce       (edm::Run&, const edm::EventSetup&) final;
  void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) final;

  void beginLuminosityBlock(const edm::LuminosityBlock&, const edm::EventSetup&) final;
  void endLuminosityBlock  (const edm::LuminosityBlock&, const edm::EventSetup&) final;

 private:

  // Parameters
  double threshold_;
  std::string outputBase_;
  int aveDigiOcc_;
  int nLumi_;
  std::string moduleName_;
  std::string label_;  
  // harvest helper classs that setup the IOV structure
  SiPixelStatusManager siPixelStatusManager_;
  // debug mode
  bool debug_;
  // for DB output naming
  std::string recordName_;

  // permanent known bad components
  const SiPixelQuality* badPixelInfo_;

  // totoal number of lumi blocks with non-zero pixel DIGIs
  int countLumi_ = 0;
  // last lumi section of the SiPixeDetectorStatus data
  edm::LuminosityBlockNumber_t endLumiBlock_;

  const TrackerGeometry* trackerGeometry_ = nullptr;
  const SiPixelFedCabling* cablingMap_ = nullptr;
  std::map<int, unsigned int> sensorSize_;

  SiPixelCoordinates coord_;

  // pixel online to offline pixel row/column
  std::map<int, std::map<int, std::pair<int,int> > > pixelO2O_;

  //Helper functions
  std::vector<std::string> substructures;
  double perLayerRingAverage(int detid, SiPixelDetectorStatus tmpSiPixelStatus);
  std::string substructure(int detid);

  // "step function" for IOV
  edm::LuminosityBlockNumber_t stepIOV(edm::LuminosityBlockNumber_t pin, std::map<edm::LuminosityBlockNumber_t,edm::LuminosityBlockNumber_t> IOV);

  // boolean function to check whether two SiPixelQualitys (pyloads) are identical
  bool equal(SiPixelQuality* a, SiPixelQuality* b);

  // Tag constructor
  void constructTag(std::map<int, SiPixelQuality*> siPixelQualityTag,
                    edm::Service<cond::service::PoolDBOutputService>& poolDbService,
                    std::string tagName,
                    edm::Run& iRun);

  // for testing threshold
  TFile * histoFile;
  std::map<std::string, TH1F*> p001, p005, p01, p05, p1, p2, p5;

};


#endif
