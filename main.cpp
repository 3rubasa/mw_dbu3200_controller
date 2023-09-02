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

#include "i2c-dev.h"
#include "i2cbusses.h"


#define BUS 4 // /dev/i2c-4
#define DEV_ADDR 0x47
#define VOUT_MODE_VAL 0x17

uint8_t ReadByte(uint8_t command) {
    char filename[20];
    int file = open_i2c_dev(BUS, filename, sizeof(filename), 0);
	if (file < 0 || set_slave_addr(file, DEV_ADDR, 0)) {
        std::cout << "Failed to open i2c_dev or set slave addr, exiting...";
		exit(1);
    }

    // Set PEC
	if (ioctl(file, I2C_PEC, 1) < 0) {
		std::cout << "Error: Could not set PEC: " << strerror(errno);
		close(file);
		exit(1);
	}

    unsigned int res = i2c_smbus_read_byte_data(file, command);

	close(file);

	if (res < 0) {
		std::cout << "Error: Read failed";
		exit(2);
	}

    return uint8_t(res);
}

uint16_t ReadWord(uint16_t command) {
    char filename[20];
    int file = open_i2c_dev(BUS, filename, sizeof(filename), 0);
	if (file < 0 || set_slave_addr(file, DEV_ADDR, 0)) {
        std::cout << "Failed to open i2c_dev or set slave addr, exiting...";
		exit(1);
    }

    // Set PEC
	if (ioctl(file, I2C_PEC, 1) < 0) {
		std::cout << "Error: Could not set PEC: " << strerror(errno);
		close(file);
		exit(1);
	}

    unsigned int res = i2c_smbus_read_word_data(file, command);

	close(file);

	if (res < 0) {
		std::cout << "Error: Read failed";
		exit(2);
	}

    return uint16_t(res);
}

int ReadBlock(uint8_t command, uint8_t *block, uint16_t block_size)
{
    char filename[20];
    int file = open_i2c_dev(BUS, filename, sizeof(filename), 0);
	if (file < 0 || set_slave_addr(file, DEV_ADDR, 0)) {
        std::cout << "Failed to open i2c_dev or set slave addr, exiting...";
		exit(1);
    }

    // Set PEC
	if (ioctl(file, I2C_PEC, 1) < 0) {
		std::cout << "Error: Could not set PEC: " << strerror(errno);
		close(file);
		exit(1);
	}

    int count = i2c_smbus_read_i2c_block_data(file, command, block_size, block);
	close(file);

	if (count == -1) {
		std::cout << "Error: Read block failed: " << strerror(errno);
        exit(1);
	}

    return count;
}

uint16_t WriteWord(uint16_t command, unsigned short value) {
    char filename[20];
    int file = open_i2c_dev(BUS, filename, sizeof(filename), 0);
	if (file < 0 || set_slave_addr(file, DEV_ADDR, 0)) {
        std::cout << "Failed to open i2c_dev or set slave addr, exiting...";
		exit(1);
    }

    // Set PEC
	if (ioctl(file, I2C_PEC, 1) < 0) {
		std::cout << "Error: Could not set PEC: " << strerror(errno);
		close(file);
		exit(1);
	}

    unsigned int res = i2c_smbus_write_word_data(file, DEV_ADDR, value);

	close(file);

	if (res < 0) {
		std::cout << "Error: Write failed: " << strerror(errno);
		exit(2);
	}

    return uint16_t(res);
}

uint16_t Float_to_L16_mode(uint8_t vout_mode, float input_val)
{
  // Assume Linear 16, pull out 5 bits of exponent, and use signed value.
  int8_t exponent = vout_mode & 0x1F;

  // Sign extend exponent from 5 to 8 bits
  if (exponent > 0x0F) exponent |= 0xE0;

  // Scale the value to a mantissa based on the exponent
  uint16_t mantissa = (uint16_t)(input_val / pow(2.0, exponent));

  return mantissa;
}

float readVin() {
    return (new LT_PMBusMath())->lin11_to_float(ReadWord(READ_VIN));
}

float readVout() {
    return (new LT_PMBusMath())->lin16_to_float(ReadWord(READ_VOUT), (LT_PMBusMath::lin16_t)(VOUT_MODE_VAL & 0x1F));
}

float readIout() {
    return (new LT_PMBusMath())->lin11_to_float(ReadWord(READ_IOUT));
}

float readTemp1() {
    return (new LT_PMBusMath())->lin11_to_float(ReadWord(READ_TEMPERATURE_1));
}

float readFanSpeed1() {
    return (new LT_PMBusMath())->lin11_to_float(ReadWord(READ_FAN_SPEED_1));
}

float readFanSpeed2() {
    return (new LT_PMBusMath())->lin11_to_float(ReadWord(READ_FAN_SPEED_2));
}

uint16_t readStatusWord() {
    return ReadWord(STATUS_WORD);
}

uint8_t readStatusVout() {
    return ReadByte(STATUS_VOUT);
}

uint8_t readStatusIout() {
    return ReadByte(STATUS_IOUT);
}

uint8_t readStatusInput() {
    return ReadByte(STATUS_INPUT);
}

uint8_t readStatusTemp() {
    return ReadByte(STATUS_TEMP);
}

uint8_t readStatusCml() {
    return ReadByte(STATUS_CML);
}

uint8_t readStatusMfrSpecific() {
    return ReadByte(STATUS_MFR_SPECIFIC);
}

uint8_t readStatusFans_1_2() {
    return ReadByte(STATUS_FANS_1_2);
}

float readIoutOvercurrent() {
    return (new LT_PMBusMath())->lin11_to_float(ReadWord(IOUT_OC_FAULT_LIMIT));
}

float readVoutTrim() {
    return (new LT_PMBusMath())->lin16_to_float(ReadWord(0x22), (LT_PMBusMath::lin16_t)(VOUT_MODE_VAL & 0x1F));
}

// ON_OFF_CONFIG byte is equal to 0x1F

char *dev = (char*)"/dev/i2c-4";

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

int print_read_vin(std::ostream& os);
int print_read_vout(std::ostream& os);
int print_read_iout(std::ostream& os);
int print_read_temp_1(std::ostream& os);
int print_read_fan_speed_1(std::ostream& os);
int print_read_fan_speed_2(std::ostream& os);
int print_status_word(std::ostream& os);
void print_status_vout(std::ostream& os);
void print_status_iout(std::ostream& os);
void print_status_input(std::ostream& os);
void print_status_temp(std::ostream& os);
void print_status_cml(std::ostream& os);
void print_status_mfr_specific(std::ostream& os);
void print_status_fans_1_2(std::ostream& os);
void print_read_iout_oc(std::ostream& os);

int main(int, char**){
    while (true) {
        std::stringstream sso;
        print_read_vin(sso);
        print_read_vout(sso);
        print_read_iout(sso);
        print_read_temp_1(sso);
        print_read_fan_speed_1(sso);
        print_read_fan_speed_2(sso);

        print_status_word(sso);
        print_status_vout(sso);
        print_status_iout(sso);
        print_status_input(sso);
        print_status_temp(sso);
        print_status_cml(sso);
        print_status_mfr_specific(sso);
        print_status_fans_1_2(sso);
        print_read_iout_oc(sso);

        std::this_thread::sleep_for(std::chrono::milliseconds{1000});

        system("clear");
        std::cout << sso.str();

        //uint8_t cfg = devices[0]->readOnOffConfig(true);
        //std::cout << std::endl << "ON_OFF_CONFIG: " << (int)(devices[0]->readOnOffConfig(true)) << std::endl;
    }

    std::cout << "This is the end!\n";
}


int print_read_vin(std::ostream& os) {
    os << "VIN  = " << readVin() << " V AC NOTE: No reading below 10V"<< std::endl;
    return 1;
}
int print_read_vout(std::ostream& os) {
    os << "VOUT = " << readVout() << "V DC" << std::endl;
    return 1;
}
int print_read_iout(std::ostream& os) {
    os << "IOUT = " << readIout() << " A DC NOTE: No reading below 5.32Amp" << std::endl;
    return 1;
}
int print_read_temp_1(std::ostream& os) {
    os << "TEMP = " << readTemp1() << " Celsius NOTE: this is the temperature of the device" << std::endl;
    return 1;
}
int print_read_fan_speed_1(std::ostream& os) {
    os << "FAN1 = " << readFanSpeed1() << " RPM NOTE: No reading below 2000 RPM" << std::endl;
    return 1;
}
int print_read_fan_speed_2(std::ostream& os) {
    os << "FAN2 = " << readFanSpeed2() << " RPM NOTE: No reading below 2000 RPM" << std::endl;
    return 1;
}

int print_status_word(std::ostream& os) {
    //The STATUS_WORD command returns two bytes of information with a summary of the unit’s fault condition. 
    // Based on the information in these bytes, the host can get more information by reading the appropriate status registers.

    int lines_count = 0;
    uint16_t status = readStatusWord();
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

void print_status_temp(std::ostream& os){
    uint8_t status = readStatusTemp();
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

void print_status_cml(std::ostream& os){
    uint8_t status = readStatusCml();
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
void print_status_mfr_specific(std::ostream& os){
    uint8_t status = readStatusMfrSpecific();
    os << "Status MFR Specific: 0x" << std::hex << (int)status << std::endl;
}
void print_status_fans_1_2(std::ostream& os){
    uint8_t status = readStatusFans_1_2();
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

void print_status_vout(std::ostream& os) {
    uint8_t status = readStatusVout();
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
void print_status_iout(std::ostream& os){
    uint8_t status = readStatusIout();
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
void print_status_input(std::ostream& os){
    uint8_t status = readStatusInput();
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

// void print_mfr_date(std::ostream& os) {
//     readMfrDate();
// }


void print_read_iout_oc(std::ostream& os){
    os << "IOUT OC = " << readIoutOvercurrent() << " Amp" << std::endl;
}