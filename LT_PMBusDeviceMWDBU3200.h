/*
Copyright (c) 2020, Analog Devices Inc
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the Analog Devices, Inc. nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LT_PMBusDeviceMWDBU3200_H_
#define LT_PMBusDeviceMWDBU3200_H_

#include "LT_PMBusDeviceController.h"
#include "LT_PMBusRail.h"
#include "LT_MWDBU3200FaultLog.h"

class LT_PMBusDeviceMWDBU3200 : public LT_PMBusDeviceController
{
  public:

    static uint32_t cap_;

    LT_PMBusDeviceMWDBU3200(LT_PMBus *pmbus, uint8_t address) : LT_PMBusDeviceController(pmbus, address, 2)
    {
    }

    static LT_PMBusDevice *detect(LT_PMBus *pmbus, uint8_t address)
    {
      uint16_t id;
      LT_PMBusDeviceMWDBU3200 *device;

// TODO: implement
//      id = pmbus->getMfrSpecialId(address);

      if (address == 0x47) {
          printf("Found device at address 0x%x\n", address);
          device = new LT_PMBusDeviceMWDBU3200(pmbus, address);
          device->probeSpeed();
          return device;
      }
      else {
          return NULL;
      }
    }
    
    void reset()
    {
        pmbus_->reset(address_);
        pmbus_->smbus()->waitForAck(address_, 0x00);
        pmbus_->waitForNotBusy(address_);
    }

    uint32_t getCapabilities (
    )
    {
      return cap_;
    }

    //! Is/are these capability(s) supported?
    //! @return true if yes
    bool hasCapability(
      uint32_t capability          //!< List of capabilities
    )
    {
      return (cap_ & capability) == capability;
    }

    uint8_t getNumPages(void)
    {
      return 1;
    }

    void enableFaultLog()
    {
      LT_MWDBU3200FaultLog *faultLog = new LT_MWDBU3200FaultLog(pmbus_);
      faultLog->enableFaultLog(address_);
      delete faultLog;
    }

    void disableFaultLog()
    {
      LT_MWDBU3200FaultLog *faultLog = new LT_MWDBU3200FaultLog(pmbus_);
      faultLog->disableFaultLog(address_);
      delete faultLog;
    }

    bool hasFaultLog()
    {
      LT_MWDBU3200FaultLog *faultLog = new LT_MWDBU3200FaultLog(pmbus_);
      if (faultLog->hasFaultLog(address_))
      {
        delete faultLog;
        return true;
      }
      else
      {
        delete faultLog;
        return false;
      }
    }

    char *getFaultLog()
    {
      LT_MWDBU3200FaultLog *faultLog = new LT_MWDBU3200FaultLog(pmbus_);
      if (faultLog->hasFaultLog(address_))
      {
        faultLog->read(address_);
//        faultLog->print();
//        faultLog->dumpBinary();
        faultLog->release();
        delete faultLog;
        return NULL;
      }
      else
      {
        delete faultLog;
        return NULL;
      }
    }

    void printFaultLog()
    {
      LT_MWDBU3200FaultLog *faultLog = new LT_MWDBU3200FaultLog(pmbus_);
      if (faultLog->hasFaultLog(address_))
      {
        faultLog->read(address_);
        faultLog->print();
//        faultLog->dumpBinary();
        faultLog->release();
        delete faultLog;
      }
    }

    void clearFaultLog()
    {
      LT_MWDBU3200FaultLog *faultLog = new LT_MWDBU3200FaultLog(pmbus_);
      if (faultLog->hasFaultLog(address_))
      {
        faultLog->clearFaultLog(address_);
        delete faultLog;
      }
      else
      {
        delete faultLog;
      }
    }

    void storeFaultLog()
    {
      LT_MWDBU3200FaultLog *faultLog = new LT_MWDBU3200FaultLog(pmbus_);
      if (!faultLog->hasFaultLog(address_))
      {
        faultLog->storeFaultLog(address_);
        delete faultLog;
      }
      else
      {
        delete faultLog;
      }
    }
};

#endif /* LT_PMBusDeviceMWDBU3200_H_ */
