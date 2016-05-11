#include <iostream>
#include <fstream>
#include <strstream>

#include "CondTools/L1TriggerExt/interface/L1ConfigOnlineProdBaseExt.h"
#include "CondFormats/L1TObjects/interface/L1TMuonGlobalParams.h"
#include "CondFormats/DataRecord/interface/L1TMuonGlobalParamsRcd.h"
#include "CondFormats/DataRecord/interface/L1TMuonGlobalParamsO2ORcd.h"
#include "L1Trigger/L1TCommon/interface/trigSystem.h"
#include "L1Trigger/L1TMuon/interface/L1TMuonGlobalParamsHelper.h"
#include "L1Trigger/L1TMuon/interface/L1TMuonGlobalParams_PUBLIC.h"

#include "xercesc/util/PlatformUtils.hpp"
using namespace XERCES_CPP_NAMESPACE;

class L1TMuonGlobalParamsOnlineProd : public L1ConfigOnlineProdBaseExt<L1TMuonGlobalParamsO2ORcd,L1TMuonGlobalParams> {
private:
public:
    virtual boost::shared_ptr<L1TMuonGlobalParams> newObject(const std::string& objectKey, const L1TMuonGlobalParamsO2ORcd &record) override ;

    L1TMuonGlobalParamsOnlineProd(const edm::ParameterSet&);
    ~L1TMuonGlobalParamsOnlineProd(void){}
};

L1TMuonGlobalParamsOnlineProd::L1TMuonGlobalParamsOnlineProd(const edm::ParameterSet& iConfig) : L1ConfigOnlineProdBaseExt<L1TMuonGlobalParamsO2ORcd,L1TMuonGlobalParams>(iConfig) {}

boost::shared_ptr<L1TMuonGlobalParams> L1TMuonGlobalParamsOnlineProd::newObject(const std::string& objectKey, const L1TMuonGlobalParamsO2ORcd &record) {
    using namespace edm::es;

    const L1TMuonGlobalParamsRcd& baseRcd = record.template getRecord< L1TMuonGlobalParamsRcd >() ;
    edm::ESHandle< L1TMuonGlobalParams > baseSettings ;
    baseRcd.get( baseSettings ) ;

    if (objectKey.empty()) {
        edm::LogInfo( "L1-O2O: L1TMuonGlobalParamsOnlineProd" ) << "Key is empty, returning empty L1TMuonGlobalParams";
        return boost::shared_ptr< L1TMuonGlobalParams > ( new L1TMuonGlobalParams( *(baseSettings.product()) ) );
    }

    std::string tscKey = objectKey.substr(0, objectKey.find(":") );
    std::string  rsKey = objectKey.substr(   objectKey.find(":")+1, std::string::npos );

    std::string stage2Schema = "CMS_TRG_L1_CONF" ;
    edm::LogInfo( "L1-O2O: L1TMuonGlobalParamsOnlineProd" ) << "Producing L1TMuonGlobalParams with TSC key =" << tscKey << " and RS key = " << rsKey ;

        // first, find keys for the algo, HW, and RS tables

        // ALGO and HW
        std::vector< std::string > queryStrings ;
        queryStrings.push_back( "ALGO" ) ;
        queryStrings.push_back( "HW"   ) ;

        std::string algo_key, hw_key;

        // select ALGO,HW from CMS_TRG_L1_CONF.UGMT_KEYS where ID = tscKey ;
        l1t::OMDSReader::QueryResults queryResult =
            m_omdsReader.basicQuery( queryStrings,
                                     stage2Schema,
                                     "UGMT_KEYS",
                                     "UGMT_KEYS.ID",
                                     m_omdsReader.singleAttribute(tscKey)
                                   ) ;

        if( queryResult.queryFailed() || queryResult.numberRows() != 1 ){
            edm::LogError( "L1-O2O: L1TMuonGlobalParamsOnlineProd" ) << "Cannot get UGMT_KEYS.{ALGO,HW}" ;
            return boost::shared_ptr< L1TMuonGlobalParams >( new L1TMuonGlobalParams( *(baseSettings.product()) ) ) ;
        }

        if( !queryResult.fillVariable( "ALGO", algo_key) ) algo_key = "";
        if( !queryResult.fillVariable( "HW",   hw_key  ) ) hw_key   = "";

        // RS
        queryStrings.clear();
        queryStrings.push_back( "MP7"       );
        queryStrings.push_back( "MP7_MONI"  );
        queryStrings.push_back( "AMC13_MONI");

        std::string rs_mp7_key, rs_mp7moni_key, rs_amc13moni_key;

        // select RS from CMS_TRG_L1_CONF.BMTF_RS_KEYS where ID = rsKey ;
        queryResult =
            m_omdsReader.basicQuery( queryStrings,
                                     stage2Schema,
                                     "UGMT_RS_KEYS",
                                     "UGMT_RS_KEYS.ID",
                                     m_omdsReader.singleAttribute(rsKey)
                                   ) ;

        if( queryResult.queryFailed() || queryResult.numberRows() != 1 ){
            edm::LogError( "L1-O2O: L1TMuonGlobalParamsOnlineProd" ) << "Cannot get UGMT_RS_KEYS.{MP7,MP7_MONI,AMC13_MONI}" ;
            return boost::shared_ptr< L1TMuonGlobalParams > ( new L1TMuonGlobalParams( *(baseSettings.product()) ) );
        }

        if( !queryResult.fillVariable( "MP7",        rs_mp7_key      ) ) rs_mp7_key       = "";
        if( !queryResult.fillVariable( "MP7_MONI",   rs_mp7moni_key  ) ) rs_mp7moni_key   = "";
        if( !queryResult.fillVariable( "AMC13_MONI", rs_amc13moni_key) ) rs_amc13moni_key = "";


        // At this point we have four keys: one ALGO key, one HW key, and two RS keys; now query the payloads for these keys
        // Now querry the actual payloads
        enum {kALGO=0, kRS, kHW, NUM_TYPES};
        std::map<std::string,std::string> payloads[NUM_TYPES];  // associates key -> XML payload for a given type of payloads
        std::string xmlPayload;

        queryStrings.clear();
        queryStrings.push_back( "CONF" );

        // query ALGO configuration
        queryResult =
            m_omdsReader.basicQuery( queryStrings,
                                     stage2Schema,
                                     "UGMT_ALGO",
                                     "UGMT_ALGO.ID",
                                     m_omdsReader.singleAttribute(algo_key)
                                   ) ;

        if( queryResult.queryFailed() || queryResult.numberRows() != 1 ){
            edm::LogError( "L1-O2O: L1TMuonGlobalParamsOnlineProd" ) << "Cannot get BMTF_ALGO.CONF for ID="<<algo_key;
            return boost::shared_ptr< L1TMuonGlobalParams >( new L1TMuonGlobalParams( *(baseSettings.product()) ) ) ;
        }

        if( !queryResult.fillVariable( "CONF", xmlPayload ) ) xmlPayload = "";
        // remember ALGO configuration
        payloads[kALGO][algo_key] = xmlPayload;

        // query HW configuration
        queryResult =
            m_omdsReader.basicQuery( queryStrings,
                                     stage2Schema,
                                     "UGMT_HW",
                                     "UGMT_HW.ID",
                                     m_omdsReader.singleAttribute(hw_key)
                                   ) ;

        if( queryResult.queryFailed() || queryResult.numberRows() != 1 ){
            edm::LogError( "L1-O2O: L1TMuonGlobalParamsOnlineProd" ) << "Cannot get BMTF_HW.CONF for ID="<<hw_key;
            return boost::shared_ptr< L1TMuonGlobalParams >( new L1TMuonGlobalParams( *(baseSettings.product()) ) ) ;
        }

        if( !queryResult.fillVariable( "CONF", xmlPayload ) ) xmlPayload = "";
        // remember HW configuration
        payloads[kHW][hw_key] = xmlPayload;

        // query MP7 and AMC13 RS configuration
        queryResult =
            m_omdsReader.basicQuery( queryStrings,
                                     stage2Schema,
                                     "UGMT_RS",
                                     "UGMT_RS.ID",
                                     m_omdsReader.singleAttribute(rs_mp7_key)
                                   ) ;

        if( queryResult.queryFailed() || queryResult.numberRows() != 1 ){
            edm::LogError( "L1-O2O: L1TMuonGlobalParamsOnlineProd" ) << "Cannot get UGMT_RS.CONF for ID="<<rs_mp7_key;
            return boost::shared_ptr< L1TMuonGlobalParams >( new L1TMuonGlobalParams( *(baseSettings.product()) ) ) ;
        }

        if( !queryResult.fillVariable( "CONF", xmlPayload ) ) xmlPayload = "";
        // remember MP7 RS configuration
        payloads[kRS][rs_mp7_key] = xmlPayload;

        queryResult =
            m_omdsReader.basicQuery( queryStrings,
                                     stage2Schema,
                                     "UGMT_RS",
                                     "UGMT_RS.ID",
                                     m_omdsReader.singleAttribute(rs_mp7moni_key)
                                   ) ;

        if( queryResult.queryFailed() || queryResult.numberRows() != 1 ){
            edm::LogError( "L1-O2O: L1TMuonGlobalParamsOnlineProd" ) << "Cannot get UGMT_RS.CONF for ID="<<rs_mp7moni_key;
            return boost::shared_ptr< L1TMuonGlobalParams >( new L1TMuonGlobalParams( *(baseSettings.product()) ) ) ;
        }

        if( !queryResult.fillVariable( "CONF", xmlPayload ) ) xmlPayload = "";
        // remember MP7 RS configuration
        payloads[kRS][rs_mp7moni_key] = xmlPayload;

        // query AMC13 RS configuration
        queryResult =
            m_omdsReader.basicQuery( queryStrings,
                                     stage2Schema,
                                     "UGMT_RS",
                                     "UGMT_RS.ID",
                                     m_omdsReader.singleAttribute(rs_amc13moni_key)
                                   ) ;

        if( queryResult.queryFailed() || queryResult.numberRows() != 1 ){
            edm::LogError( "L1-O2O: L1TMuonBarrelParamsOnlineProd" ) << "Cannot get UGMT_RS.CONF for ID="<<rs_amc13moni_key;
            return boost::shared_ptr< L1TMuonGlobalParams >( new L1TMuonGlobalParams( *(baseSettings.product()) ) ) ;
        }

        if( !queryResult.fillVariable( "CONF", xmlPayload ) ) xmlPayload = "";
        // remember AMC13 RS configuration
        payloads[kRS][rs_amc13moni_key] = xmlPayload;


    // finally, push all payloads to the XML parser and construct the trigSystem objects with each of those
    l1t::XmlConfigReader xmlRdr;
    l1t::trigSystem trgSys;
    // HW settings should always go first
    for(auto &conf : payloads[ kHW ]){
        xmlRdr.readDOMFromString( conf.second );
        xmlRdr.readRootElement  ( trgSys      );
    }
    // now let's parse ALGO and then RS settings 
    for(auto &conf : payloads[ kALGO ]){
        xmlRdr.readDOMFromString( conf.second );
        xmlRdr.readRootElement  ( trgSys      );
    }
    for(auto &conf : payloads[ kRS ]){
        xmlRdr.readDOMFromString( conf.second );
        xmlRdr.readRootElement  ( trgSys      );
    }
    trgSys.setConfigured();

    L1TMuonGlobalParamsHelper m_params_helper( *(baseSettings.product()) );
    try {
        m_params_helper.loadFromOnline(trgSys, "ugmt_processor");
        return boost::shared_ptr< L1TMuonGlobalParams >( new L1TMuonGlobalParams( cast_to_L1TMuonGlobalParams(m_params_helper) ) ) ;
///    } catch (std::runtime_error e){
    } catch (...){
///        edm::LogError( "L1-O2O: L1TMuonGlobalParamsOnlineProd" ) << "Exception thrown ... resorting to the default payload ("<<e.what()<<")";
        edm::LogError( "L1-O2O: L1TMuonGlobalParamsOnlineProd" ) << "Exception thrown ... resorting to the default payload";
    }

    return boost::shared_ptr< L1TMuonGlobalParams >( new L1TMuonGlobalParams( *(baseSettings.product()) ) ) ;
}

//define this as a plug-in
DEFINE_FWK_EVENTSETUP_MODULE(L1TMuonGlobalParamsOnlineProd);
