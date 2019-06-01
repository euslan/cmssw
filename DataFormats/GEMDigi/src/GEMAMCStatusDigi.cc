#include "DataFormats/GEMDigi/interface/GEMAMCStatusDigi.h"

GEMAMCStatusDigi::GEMAMCStatusDigi(const uint8_t AMCnum_,
                                   const uint32_t L1A_,
                                   const uint16_t BX_,
                                   const uint32_t Dlength_,
                                   const uint8_t FV_,
                                   const uint8_t Rtype_,
                                   const uint8_t Param1_,
                                   const uint8_t Param2_,
                                   const uint8_t Param3_,
                                   const uint16_t Onum_,
                                   const uint16_t BID_,
                                   const uint32_t GEMDAV_,
                                   const uint64_t Bstatus_,
                                   const uint8_t GDcount_,
                                   const uint8_t Tstate_,
                                   const uint32_t ChamT_,
                                   const uint8_t OOSG_)
    : m_AMCnum(AMCnum_),
      m_L1A(L1A_),
      m_BX(BX_),
      m_Dlength(Dlength_),
      m_FV(FV_),
      m_Rtype(Rtype_),
      m_Param1(Param1_),
      m_Param2(Param2_),
      m_Param3(Param3_),
      m_Onum(Onum_),
      m_BID(BID_),
      m_GEMDAV(GEMDAV_),
      m_Bstatus(Bstatus_),
      m_GDcount(GDcount_),
      m_Tstate(Tstate_),
      m_ChamT(ChamT_),
      m_OOSG(OOSG_){};
