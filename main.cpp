#include <iostream>
#include <memory>
#include <thread>
#include <string>
#include <iostream>
#include <sstream> 

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

enum class MESSAGE_COLOR {
    Red,
    Green,
    Yellow
};

std::string FormatMessage(const std::string& message, MESSAGE_COLOR color) {
    switch (color) {
        case MESSAGE_COLOR::Red:
            return std::string{"\033[1;31m"} + message + std::string{"\033[0m"};
        break;
        case MESSAGE_COLOR::Green:
            return std::string{"\033[1;32m"} + message + std::string{"\033[0m"};
        break;
        case MESSAGE_COLOR::Yellow:
            return std::string{"\033[1;33m"} + message + std::string{"\033[0m"};
        break;
        default:
            return message;
            break;
    }
}

int print_read_vin(LT_PMBusDevice* device, std::ostream& os);
int print_read_vout(LT_PMBusDevice* device, std::ostream& os);
int print_read_iout(LT_PMBusDevice* device, std::ostream& os);
int print_read_temp_1(LT_PMBusDevice* device, std::ostream& os);
int print_read_fan_speed_1(LT_PMBusDevice* device, std::ostream& os);
int print_read_fan_speed_2(LT_PMBusDevice* device, std::ostream& os);

int print_status_word(LT_PMBusDevice* device, std::ostream& os);
int print_divider();
// ON_OFF_CONFIG
void print_on_off_config(LT_PMBusDevice* device, std::ostream& os);

// MFR_DATE
//void print_mfr_date(LT_PMBusDevice* device, std::ostream& os);


void print_status_vout(LT_PMBusDevice* device, std::ostream& os);
void print_status_iout(LT_PMBusDevice* device, std::ostream& os);
void print_status_input(LT_PMBusDevice* device, std::ostream& os);

void print_status_temp(LT_PMBusDevice* device, std::ostream& os);
void print_status_cml(LT_PMBusDevice* device, std::ostream& os);
void print_status_mfr_specific(LT_PMBusDevice* device, std::ostream& os);
void print_status_fans_1_2(LT_PMBusDevice* device, std::ostream& os);

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

    std::cout << "***********************************************************" << std::endl;

    std::string output;

    while (true) {
        std::stringstream sso;
        print_read_vin(devices[0], sso);
        print_read_vout(devices[0], sso);
        print_read_iout(devices[0], sso);
        print_read_temp_1(devices[0], sso);
        print_read_fan_speed_1(devices[0], sso);
        print_read_fan_speed_2(devices[0], sso);

        print_status_word(devices[0], sso);
        print_status_vout(devices[0], sso);
        print_status_iout(devices[0], sso);
        print_status_input(devices[0], sso);
        print_status_temp(devices[0], sso);
        print_status_cml(devices[0], sso);
        print_status_mfr_specific(devices[0], sso);
        print_status_fans_1_2(devices[0], sso);

        std::this_thread::sleep_for(std::chrono::milliseconds{100});

        system("clear");
        std::cout << sso.str();
    }

    std::cout << "This is the end!\n";
}


int print_read_vin(LT_PMBusDevice* device, std::ostream& os) {
    os << "VIN  = " << device->readVin(true) << " V AC NOTE: No reading below 10V"<< std::endl;
    return 1;
}
int print_read_vout(LT_PMBusDevice* device, std::ostream& os) {
    os << "VOUT = " << device->readVout(true) << "V DC" << std::endl;
    return 1;
}
int print_read_iout(LT_PMBusDevice* device, std::ostream& os) {
    os << "IOUT = " << device->readIout(true) << " A DC NOTE: No reading below 5.32Amp" << std::endl;
    return 1;
}
int print_read_temp_1(LT_PMBusDevice* device, std::ostream& os) {
    os << "TEMP = " << device->readExternalTemperature(true) << " Celsius NOTE: this is the temperature of the device" << std::endl;
    return 1;
}
int print_read_fan_speed_1(LT_PMBusDevice* device, std::ostream& os) {
    os << "FAN1 = " << device->readFanSpeed1(true) << " RPM NOTE: No reading below 2000 RPM" << std::endl;
    return 1;
}
int print_read_fan_speed_2(LT_PMBusDevice* device, std::ostream& os) {
    os << "FAN2 = " << device->readFanSpeed2(true) << " RPM NOTE: No reading below 2000 RPM" << std::endl;
    return 1;
}

int print_status_word(LT_PMBusDevice* device, std::ostream& os) {
    //The STATUS_WORD command returns two bytes of information with a summary of the unit’s fault condition. 
    // Based on the information in these bytes, the host can get more information by reading the appropriate status registers.

    int lines_count = 0;
    uint16_t status = device->readStatusWord(true);
    os << "Status word: 0x" << std::hex << status << std::endl;
    lines_count++;
    if (status & 0x800)
        os << FormatMessage("  Power NOT Good!", MESSAGE_COLOR::Red) << std::endl;
    else 
        os << FormatMessage("  Power Is Good!", MESSAGE_COLOR::Green) << std::endl;
    lines_count++;

    if (status & 0x2)
      {os << "  FAULT: A communications, memory or logic fault has occurred!" << std::endl; lines_count++;}
    if (status & 0x4)
        {os << "  FAULT: A temperature fault or warning has occurred!" << std::endl;lines_count++;}
    if (status & 0x8)
        {os << "  FAULT: An input undervoltage fault has occurred!" << std::endl;lines_count++;}
    if (status & 0x10)
        {os << "  FAULT: An output overcurrent fault has occurred!" << std::endl;lines_count++;}
    if (status & 0x20)
        {os << "  FAULT: An output overvoltage fault has occurred!" << std::endl;lines_count++;}
    if (status & 0x40)
        {os << FormatMessage("  Unit is not providing power to the output, regardless of the reason, including simply not being enabled", MESSAGE_COLOR::Yellow) << std::endl;lines_count++;}
    if (status & 0x80)
        {os << "  Device was busy and unable to respond" << std::endl;lines_count++;}
    if (status & 0x100)
        {os << "  FAULT: UNKNOWN FAULT!" << std::endl;lines_count++;}
    if (status & 0x200)
        {os << "  STATUS_OTHER is set!" << std::endl;lines_count++;}
    if (status & 0x400)
        {os << "  FAULT: A fan or airflow fault or warning has occurred!" << std::endl;lines_count++;}
    if (status & 0x1000)
        {os << "  FAULT: A manufacturer specific fault or warning has occurred!" << std::endl;lines_count++;}
    if (status & 0x2000)
        {os << "  FAULT: An input voltage, input current, or input power fault or warning has occurred!" << std::endl;lines_count++;}
    if (status & 0x4000)
        {os << "  FAULT: An output current or output power fault or warning has occurred!" << std::endl;lines_count++;}
    if (status & 0x8000)
        {os << "  FAULT: An output voltage fault or warning has occurred!" << std::endl;lines_count++;}

    return lines_count;
}

void print_status_temp(LT_PMBusDevice* device, std::ostream& os){
    uint8_t status = device->readStatusTemp(true);
    os << "Status TEMP: 0x" << std::hex << (int)status << std::endl;
    if (status & 0x10)
        os << "  FAULT: Undertemperature fault!" << std::endl;
    if (status & 0x20)
        os << "  WARNING: Undertemperature Warning!" << std::endl;
    if (status & 0x40)
        os << "  WARNING: Overtemperature Warning!" << std::endl;
    if (status & 0x80)
        os << "  FAULT: Overtemperature Fault!" << std::endl;
}

void print_status_cml(LT_PMBusDevice* device, std::ostream& os){
    uint8_t status = device->readStatusCml(true);
    os << "Status CML: 0x" << std::hex << (int)status << std::endl;
    if (status & 0x1)
        os << "  FAULT: Other Memory Or Logic Fault!" << std::endl;
    if (status & 0x2)
        os << "  FAULT: Other Communication Fault!" << std::endl;
    if (status & 0x8)
        os << "  FAULT: Processor Fault!" << std::endl;
    if (status & 0x10)
        os << "  FAULT: Memory fault!" << std::endl;
    if (status & 0x20)
        os << "  Packet Error Check Failed!" << std::endl;
    if (status & 0x40)
        os << "  Invalid Or Unsupported Data Received!" << std::endl;
    if (status & 0x80)
        os << "  Invalid Or Unsupported Command Received!" << std::endl;
}
void print_status_mfr_specific(LT_PMBusDevice* device, std::ostream& os){
    uint8_t status = device->readStatusMfrSpecific(true);
    os << "Status MFR Specific: 0x" << std::hex << (int)status << std::endl;
}
void print_status_fans_1_2(LT_PMBusDevice* device, std::ostream& os){
    uint8_t status = device->readStatusFans_1_2(true);
    os << "Status Fans_1_2: 0x" << std::hex << (int)status << std::endl;
    if (status & 0x1)
        os << "  WARNING: Airflow Warning!" << std::endl;
    if (status & 0x2)
        os << "  FAULT: Airflow Fault!" << std::endl;
    if (status & 0x4)
        os << "  Fan 2 Speed Overridden!" << std::endl;
    if (status & 0x8)
        os << "  Fan 1 Speed Overridden" << std::endl;
    if (status & 0x10)
        os << "  WARNING: Fan 2 Warning!" << std::endl;
    if (status & 0x20)
        os << "  WARNING: Fan 1 Warning!!" << std::endl;
    if (status & 0x40)
        os << "  FAULT: Fan 2 Fault!" << std::endl;
    if (status & 0x80)
        os << "  FAULT: Fan 1 Fault!" << std::endl;
}

void print_status_vout(LT_PMBusDevice* device, std::ostream& os) {
    uint8_t status = device->readStatusVout(true);
    os << "Status VOUT: 0x" << std::hex << (int)status << std::endl;
    if (status & 0x1)
        os << "  VOUT Tracking Error!" << std::endl;
    if (status & 0x2)
        os << "  TOFF_MAX Warning!" << std::endl;
    if (status & 0x4)
        os << "  TON_MAX_FAULT!" << std::endl;
    if (status & 0x8)
        os << "  VOUT_MAX Warning (An attempt has been made to set the output voltage to value higher than allowed by the VOUT_MAX command (Section 13.5)." << std::endl;
    if (status & 0x10)
        os << "  FAULT: VOUT Undervoltage fault!" << std::endl;
    if (status & 0x20)
        os << "  WARNING: VOUT Undervoltage Warning!" << std::endl;
    if (status & 0x40)
        os << "  WARNING: VOUT Overvoltage Warning!" << std::endl;
    if (status & 0x80)
        os << "  FAULT: VOUT Overvoltage Fault!" << std::endl;
}
void print_status_iout(LT_PMBusDevice* device, std::ostream& os){
    uint8_t status = device->readStatusIout(true);
    os << "Status IOUT: 0x" << std::hex << (int)status << std::endl;
    if (status & 0x1)
        os << "  WARNING: POUT Overpower Warning!" << std::endl;
    if (status & 0x2)
        os << "  FAULT: POUT Overpower Fault!" << std::endl;
    if (status & 0x4)
        os << "  Power Limiting! This bit is to be asserted when the unit is operating with the output in constant power mode at the power set by the POUT_MAX command (Section 14.2)." << std::endl;
    if (status & 0x8)
        os << "  FAULT: Current Share Fault! The conditions that cause the Current Share Fault bit to be set are defined by each device manufacturer." << std::endl;
    if (status & 0x10)
        os << "  FAULT: IOUT Undercurrent fault!" << std::endl;
    if (status & 0x20)
        os << "  WARNING: IOUT Overcurrent Warning!" << std::endl;
    if (status & 0x40)
        os << "  FAULT: IOUT overcurrent And Low Voltage Shutdown Fault!" << std::endl;
    if (status & 0x80)
        os << "  FAULT: IOUT Overcurrent Fault!" << std::endl;
}
void print_status_input(LT_PMBusDevice* device, std::ostream& os){
    uint8_t status = device->readStatusInput(true);
    os << "Status INPUT: 0x" << std::hex << (int)status << std::endl;
    if (status & 0x1)
        os << "  WARNING: PIN Overpower Warning!" << std::endl;
    if (status & 0x2)
        os << "  WORNING: IIN Overcurrent Warning!" << std::endl;
    if (status & 0x4)
        os << "  FAULT: IIN Overcurrent Fault!" << std::endl;
    if (status & 0x8)
        os << "  Unit Is Off For Insufficient Input Voltage. Either the input voltage has never exceeded the input turn-on threshold (Section14.5)or if the unit did start, the input voltage decreased below the turn-off threshold (Section14.6)." << std::endl;
    if (status & 0x10)
        os << "  FAULT: VIN Undervaltage fault!" << std::endl;
    if (status & 0x20)
        os << "  WARNING: VIN Undervoltage Warning!" << std::endl;
    if (status & 0x40)
        os << "  WARNING: VIN Overvoltage Warning!" << std::endl;
    if (status & 0x80)
        os << "  FAULT: VIN Overvaltage Fault!" << std::endl;
}

void print_mfr_date(LT_PMBusDevice* device, std::ostream& os) {
    device->readMfrDate(true);
}

void print_on_off_config(LT_PMBusDevice* device, std::ostream& os) {
    bool on = device->readOnOffConfig(true);
    if (on)
        os << "State: ON" << std::endl;
    else
        os << "State: OFF" << std::endl;
}