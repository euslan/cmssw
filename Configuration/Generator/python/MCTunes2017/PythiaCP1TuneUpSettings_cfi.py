import FWCore.ParameterSet.Config as cms

pythia8CP1TuneUpSettingsBlock = cms.PSet(
    pythia8CP1TuneUpSettings = cms.vstring(
        'Tune:pp 14',
	'Tune:ee 7',
	'MultipartonInteractions:bProfile=2',
        'MultipartonInteractions:ecmPow=0.154',
        'MultipartonInteractions:pT0Ref=2.3',
        'MultipartonInteractions:coreRadius=0.5832',
        'MultipartonInteractions:coreFraction=0.5064',
        'ColourReconnection:range=8.305',
	'SigmaTotal:zeroAXB=off',
	'SpaceShower:rapidityOrder=off',
        'SigmaTotal:mode = 0',
        'SigmaTotal:sigmaEl = 21.89',
        'SigmaTotal:sigmaTot = 100.309',
        'PDF:pSet=LHAPDF6:NNPDF31_lo_as_0130',
	)
)
