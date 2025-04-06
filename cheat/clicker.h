#pragma once

#include <unordered_map>
#include <chrono>
#include <random>
#include <Windows.h>
#include <thread>


int getKeybindFromUser(std::string keybindInputLeftorRight);
void PreciseSleep(double milliseconds);
void click_mouse(bool is_left);
void Autoclicker(float cps, int keybind, bool toggleMode, bool is_left, std::atomic<bool>& isActive);




