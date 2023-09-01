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
//void print_mfr_date(LT_PMBusDevice* device);

void print_status_word(LT_PMBusDevice* device);

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
    print_status_word(devices[0]);
   // print_on_off_config(devices[0]);
   //print_mfr_date(devices[0]);
    std::cout << "This is the end!\n";
}

void print_status_word(LT_PMBusDevice* device) {
    uint16_t status = device->readStatusWord(true);
    std::cout << "Status word: 0x" << std::hex << status << std::endl;
    if (status & 0x800)
        std::cout << "  Power NOT Good!" << std::endl;
    else 
        std::cout << "  Power is Good" << std::endl;

    if (status & 0x2)
        std::cout << "  FAULT: A communications, memory or logic fault has occurred!" << std::endl;
    if (status & 0x4)
        std::cout << "  FAULT: A temperature fault or warning has occurred!" << std::endl;
    if (status & 0x8)
        std::cout << "  FAULT: An input undervoltage fault has occurred!" << std::endl;
    if (status & 0x10)
        std::cout << "  FAULT: An output overcurrent fault has occurred!" << std::endl;
    if (status & 0x20)
        std::cout << "  FAULT: An output overvoltage fault has occurred!" << std::endl;
    if (status & 0x40)
        std::cout << "  Unit is not providing power to the output, regardless of the reason, including simply not being enabled" << std::endl;
    if (status & 0x80)
        std::cout << "  Device was busy and unable to respond" << std::endl;
    if (status & 0x100)
        std::cout << "  FAULT: UNKNOWN FAULT!" << std::endl;
    if (status & 0x200)
        std::cout << "  STATUS_OTHER is set!" << std::endl;
    if (status & 0x400)
        std::cout << "  FAULT: A fan or airflow fault or warning has occurred!" << std::endl;
    if (status & 0x1000)
        std::cout << "  FAULT: A manufacturer specific fault or warning has occurred!" << std::endl;
    if (status & 0x2000)
        std::cout << "  FAULT: An input voltage, input current, or input power fault or warning has occurred!" << std::endl;
    if (status & 0x4000)
        std::cout << "  FAULT: An output current or output power fault or warning has occurred!" << std::endl;
    if (status & 0x8000)
        std::cout << "  FAULT: An output voltage fault or warning has occurred!" << std::endl;
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