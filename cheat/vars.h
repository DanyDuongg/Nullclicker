#pragma once
#include <atomic>
#include <thread>

// Global variables for the autoclicker application
extern std::atomic<bool> isActived;       // To track if the autoclicker is active
extern std::atomic<bool> isLeft;         // To track if the left click is enabled
extern std::atomic<bool> isRight;        // To track if the right click is enabled
extern std::atomic<bool> isHoldingleft;  // To track if the left click is being held
extern std::atomic<bool> isHoldingright; // To track if the right click is being held

extern int Leftkeybind;                  // The keybind for the left click
extern int Rightkeybind;                 // The keybind for the right click
extern float cps;

