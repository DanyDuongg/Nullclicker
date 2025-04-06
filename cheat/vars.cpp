#include "vars.h"

std::atomic<bool> isActived{ false };
std::atomic<bool> isLeft{ false };
std::atomic<bool> isRight{ false };
std::atomic<bool> isHoldingleft{ false };
std::atomic<bool> isHoldingright{ false };
int Leftkeybind{ 0 };    
int Rightkeybind{ 0 };
float cps{ 10.0f };






