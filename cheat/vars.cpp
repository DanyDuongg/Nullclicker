#include "vars.h"

std::atomic<bool> isActived{ false };
std::atomic<bool> isLeft{ false };
std::atomic<bool> isRight{ false };
std::atomic<bool> isHoldingleft{ false };
std::atomic<bool> isHoldingright{ false };
std::atomic<bool> isSmartBlockHit{ false };

int Leftkeybind{ 0 };    
int Rightkeybind{ 0 };
float Leftcps{ 15.0f };
float Rightcps{ 15.0f };
float ActualBlockHitDelay{ 3.f };






