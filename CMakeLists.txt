cmake_minimum_required(VERSION 3.30)
project(avr LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(AVR_MCU avr128db48)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(AVR_MCU avrxmega4)
  set(DEVICE __AVR_AVR128DB48__)
endif()

set(CPU_FREQUENCY 24000000)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
  PRIVATE
    main.cxx
    interrupts.cxx
  PRIVATE FILE_SET modules TYPE CXX_MODULES FILES
    io.cxx
    tinyarray.cxx
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_C_FLAGS "-mmcu=${AVR_MCU} -DF_CPU=${CPU_FREQUENCY} -Os -ffunction-sections -fdata-sections -funsigned-bitfields -mrelax")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_C_FLAGS "--target=avr -mmcu=${AVR_MCU} -D${DEVICE} -DF_CPU=${CPU_FREQUENCY} --sysroot=/home/g-zu/avr -Os -nostdlib")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -funsigned-char -fpack-struct -fshort-enums -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-exceptions -fno-rtti")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_LD_FLAGS "-Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(AVR_LIBC_ROOT /usr)
  target_include_directories(${PROJECT_NAME} PRIVATE ${AVR_LIBC_ROOT}/avr/include)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    ${AVR_LIBC_ROOT}/avr/lib/avrxmega4/libavr128db48.a
    ${AVR_LIBC_ROOT}/avr/lib/avrxmega4/crtavr128db48.o
  )
endif()

target_link_directories(${PROJECT_NAME} PRIVATE ${AVR_LIBC_ROOT}/avr/lib/avrxmega4/)
target_include_directories(${PROJECT_NAME} PRIVATE /usr/lib/gcc/avr/14/include/)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

set(NOIPO_BUILD_TYPES "Debug;RelWithDebInfo")
if(NOT ";${NOIPO_BUILD_TYPES};" MATCHES ";${CMAKE_BUILD_TYPE};")
  message(STATUS "Enabling IPO for build type: ${CMAKE_BUILD_TYPE}")
  set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO disabled for build type: ${CMAKE_BUILD_TYPE}")
endif()

