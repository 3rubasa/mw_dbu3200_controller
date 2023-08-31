#include <iostream>
#include <memory>

#include "LT_Exception.h"
#include "LT_SMBusNoPec.h"
#include "LT_SMBusPec.h"
#include "LT_PMBus.h"
#include "LT_PMBusMath.h"
#include "LT_PMBusDevice.h"
#include "LT_SMBusGroup.h"
#include "LT_PMBusRail.h"
#include "LT_PMBusDetect.h"

char *dev = (char*)"/dev/i2c-1";

// READ_VOUT
void print_vout(const std::vector<LT_PMBusDevice*>& devices);

// ON_OFF_CONFIG
void print_on_off_config(LT_PMBusDevice* device);

// MFR_DATE
void print_mfr_date(LT_PMBusDevice* device);

int main(int, char**){
    std::cout << "Hello, from pmbus1!\n";
    std::cout << "Using I2C device: " << dev << std::endl;

    std::shared_ptr<LT_SMBusNoPec> smbusNoPec(new LT_SMBusNoPec(dev));
    std::shared_ptr<LT_SMBusPec> smbusPec(new LT_SMBusPec(dev));
    std::shared_ptr<LT_PMBus> pmbusNoPec(new LT_PMBus(smbusNoPec.get()));
    std::shared_ptr<LT_PMBus> pmbusPec(new LT_PMBus(smbusPec.get()));

    std::shared_ptr<LT_SMBus> smbus = smbusNoPec;
    std::shared_ptr<LT_PMBus> pmbus = pmbusNoPec;

    std::shared_ptr<LT_PMBusDetect> detector(new LT_PMBusDetect(pmbus.get()));
    detector->detect();
    std::vector<LT_PMBusDevice*> devices = detector->getDevices();

    std::cout << "Found " << devices.size() << " devices" << std::endl;
    for (auto d: devices) {
        std::cout << "{" << std::endl;
        std::cout << "  address: 0x" << std::hex << (int)(d->getAddress()) << std::endl;
        std::cout << "}" << std::endl;
    }

    print_vout(devices);
   // print_on_off_config(devices[0]);
   print_mfr_date(devices[0]);
    std::cout << "This is the end!\n";
}


void print_vout(const std::vector<LT_PMBusDevice*>& devices)
{
	float   voltage;

    for (LT_PMBusDevice* d: devices) {
        if (d == NULL) 
            continue;

        if(d->hasCapability(HAS_VOUT)) {
			voltage = d->readVout(true);
			printf("VOUT %f @ 0x%02x\n", voltage, d->getAddress());
		}
    }
}

void print_mfr_date(LT_PMBusDevice* device) {
    device->readMfrDate(true);
}

void print_on_off_config(LT_PMBusDevice* device) {
    bool on = device->readOnOffConfig(true);
    if (on)
        std::cout << "State: ON" << std::endl;
    else
        std::cout << "State: OFF" << std::endl;
}