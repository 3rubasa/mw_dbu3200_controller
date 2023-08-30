cmake_minimum_required(VERSION 3.0.0)
project(pmbus1 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD_LIBRARIES -li2c)

add_executable(pmbus1 
    main.cpp
    LT_PMBusDetect.cpp
LT_PMBusDevice.cpp
LT_PMBusRail.cpp
LT_PMBusSpeedTest.cpp
LT_PMBus.cpp
LT_SMBus.cpp
LT_SMBusGroup.cpp
LT_PMBusMath.cpp
LT_SMBusBase.cpp
LT_Exception.cpp
LT_PMBusDeviceMWDBU3200.cpp
LT_MWDBU3200FaultLog.cpp
LT_FaultLog.cpp
    )
