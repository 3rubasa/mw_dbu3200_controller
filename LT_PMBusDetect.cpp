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

#include <string.h>
    
#include "LT_PMBusDetect.h"
#include "LT_PMBusDeviceMWDBU3200.h"

LT_PMBusDetect::LT_PMBusDetect(LT_PMBus *pmbus):pmbus_(pmbus)
{}

LT_PMBusDetect::~LT_PMBusDetect()
{
  for (LT_PMBusDevice * d: devices_) {
    if (d != NULL) {
      delete d;
    }
  }
}

std::vector<LT_PMBusDevice*> LT_PMBusDetect::getDevices(
)
{
  return devices_;
}


void LT_PMBusDetect::detect ()
{
  uint8_t *addresses;
  LT_PMBusDevice *device;
  unsigned int i, j;

   for (LT_PMBusDevice * d: devices_) {
    if (d != NULL) {
      delete d;
    }
  }

  devices_.clear();

  addresses = pmbus_->smbus()->probeUnique(CAPABILITY);

  // Check each device type one by one. Trading generality and composability
  // for performance. For better performance, cache IDs or write decoder here
  // and then call detect on the correct device first time.
  for (i = 0; i < strlen((char *)addresses); i++)
  {
    if ((device = LT_PMBusDeviceMWDBU3200::detect(pmbus_, addresses[i])) != NULL)
      devices_.push_back(device);
  }
}