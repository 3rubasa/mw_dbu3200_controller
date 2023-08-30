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

    std::cout << "This is the end!\n";
}
