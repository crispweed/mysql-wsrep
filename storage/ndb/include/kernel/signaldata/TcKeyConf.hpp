/*
   Copyright (c) 2003, 2013, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef TC_KEY_CONF_H
#define TC_KEY_CONF_H

#include "SignalData.hpp"

#define JAM_FILE_ID 58


/**
 * 
 */
class TcKeyConf {
  /**
   * Reciver(s)
   */
  friend class Ndb;
  friend class NdbTransaction;
  friend class Ndbcntr;
  friend class DbUtil;

  friend class TransporterFacade;

  /**
   * Sender(s)
   */
  friend class Dbtc;
  friend class Dbspj;

  /**
   * For printing
   */
  friend bool printTCKEYCONF(FILE *, const Uint32 *, Uint32, Uint16);

public:
  /**
   * Length of signal
   */
  STATIC_CONST( StaticLength = 5 );
  STATIC_CONST( OperationLength = 2 );
  STATIC_CONST( DirtyReadBit = (((Uint32)1) << 31) );
  
private:

  /**
   * DATA VARIABLES
   */
  //-------------------------------------------------------------
  // Unconditional part. First 5 words
  //-------------------------------------------------------------

  Uint32 apiConnectPtr; // if RNIL, transaction is found from op
  Uint32 gci_hi;   // gci_lo is stored after operations...
  Uint32 confInfo;
  Uint32 transId1;
  Uint32 transId2;

  struct OperationConf {
    Uint32 apiOperationPtr;
    Uint32 attrInfoLen;
  };
  //-------------------------------------------------------------
  // Operations confirmations,
  // No of actually sent = getNoOfOperations(confInfo)
  //-------------------------------------------------------------
  OperationConf operations[10];
  
  /**
   * Get:ers for confInfo
   */
  static Uint32 getNoOfOperations(const Uint32 & confInfo);
  static Uint32 getCommitFlag(const Uint32 & confInfo);
  static bool getMarkerFlag(const Uint32 & confInfo);
  
  /**
   * Set:ers for confInfo
   */
  static void setCommitFlag(Uint32 & confInfo, Uint8 flag);
  static void setNoOfOperations(Uint32 & confInfo, Uint32 noOfOps);
  static void setMarkerFlag(Uint32 & confInfo, Uint32 flag);
};

inline
Uint32
TcKeyConf::getNoOfOperations(const Uint32 & confInfo){
  return confInfo & 65535;
}

inline
Uint32
TcKeyConf::getCommitFlag(const Uint32 & confInfo){
  return ((confInfo >> 16) & 1);
}

inline
bool
TcKeyConf::getMarkerFlag(const Uint32 & confInfo){
  const Uint32 bits = 3 << 16; // Marker only valid when doing commit
  return (confInfo & bits) == bits;
}

inline
void 
TcKeyConf::setNoOfOperations(Uint32 & confInfo, Uint32 noOfOps){
  ASSERT_MAX(noOfOps, 65535, "TcKeyConf::setNoOfOperations");
  confInfo = (confInfo & 0xFFFF0000) | noOfOps;
}

inline
void 
TcKeyConf::setCommitFlag(Uint32 & confInfo, Uint8 flag){
  ASSERT_BOOL(flag, "TcKeyConf::setCommitFlag");
  confInfo |= (flag << 16);
}

inline
void
TcKeyConf::setMarkerFlag(Uint32 & confInfo, Uint32 flag){
  ASSERT_BOOL(flag, "TcKeyConf::setMarkerFlag");
  confInfo |= (flag << 17);
}


#undef JAM_FILE_ID

#endif
