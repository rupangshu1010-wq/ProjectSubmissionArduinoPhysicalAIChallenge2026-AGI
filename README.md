# ProjectSubmissionArduinoPhysicalAIChallenge2026-AGI

# Steps to use the library
1. Include library using "#include <NeuralNetwork.h>"
2. Use Manual Commands for giving input to the library
   a. NetInput("1 or 0") //Give input to the network
   b. NetAlloc("1-100000") //Input total number of nodes/neurons to generate, max number of nodes "100000" or 200kb of RAM (2-bytes per node)
   c. NetTrigg() //Execute a single cycle of Network
   d. NetTarget("1 or 0") //Input Target to the Network
3. Use the manual commands in correct combination for properly use lbrary

# Compatibility
1. Microcontroller
   This supports all microcontrollers including major brands like Arduino, Espressif, STMicroelectronics, etc.
2. Microprocessor
   This supports all microprocessors including oldest processors like Intel 4004
3. DIY builds
   This only supports DIY hardware with minimum requirements of 2-bit processor and 4-bit/8-bit co-processor for memory or I/O

# Extra Files
1. NeuralNetwork.so, for Linux systems including Arduino UNO Q debian linux.
2. Example.c, for example commands and library testing.
