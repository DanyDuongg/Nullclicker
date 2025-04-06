
#include "clicker.h"
#include "gui.h"
#include "vars.h"
#include <chrono>

//keybind handle
std::unordered_map<std::string, int> keyCode = {
    {"LEFTMOUSE",VK_LBUTTON},{"LEFTCLICK",VK_LBUTTON},{"RIGHTCLICK",VK_RBUTTON},
	{"MIDLEMOUSE", VK_MBUTTON},
	{"MARCO2", VK_XBUTTON1},
	{"MARCO1", VK_XBUTTON2},
	{"F1", VK_F1}, {"F2", VK_F2}, {"F3", VK_F3}, {"F4", VK_F4}, {"F5", VK_F5},
	{"A", 'A'}, {"B", 'B'}, {"C", 'C'}, {"D", 'D'}, {"E", 'E'}, {"F", 'F'},
	{"G", 'G'}, {"H", 'H'}, {"I", 'I'}, {"J", 'J'}, {"K", 'K'}, {"L", 'L'},
	{"M", 'M'}, {"N", 'N'}, {"O", 'O'}, {"P", 'P'}, {"Q", 'Q'}, {"R", 'R'},
	{"S", 'S'}, {"T", 'T'}, {"U", 'U'}, {"V", 'V'}, {"W", 'W'}, {"X", 'X'},
	{"Y", 'Y'}, {"Z", 'Z'},  {"TAB", VK_TAB}, {"CONTROL",VK_LCONTROL},{"SHIFT",VK_SHIFT},
	{"0", '0'}, {"1", '1'}, {"2", '2'}, {"3", '3'}, {"4", '4'}, {"5", '5'},
	{"6", '6'}, {"7", '7'}, {"8", '8'}, {"9", '9'},
	{"SPACE", VK_SPACE}, {"ENTER", VK_RETURN}
};

int getKeybindFromUser(std::string keybindInputLeftorRight)
{
	for (auto& c : keybindInputLeftorRight)
	{
		c = toupper(c);
	}
	auto keySet = keyCode.find(keybindInputLeftorRight);
	if (keySet != keyCode.end())
	{
		return keySet->second;
	}
	return 0;
}
std::random_device rd;
std::mt19937 rng(rd());
std::uniform_real_distribution<float> randomOffset(-5.0f, 5.0f);


void PreciseSleep(double millisecond)
{
	auto duration = std::chrono::duration<double, std::milli>(millisecond);
	std::this_thread::sleep_for(duration);
}
void click_mouse(bool is_left)
{
	INPUT input = {};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = is_left ?
		MOUSEEVENTF_LEFTDOWN :
		MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &input, sizeof(INPUT));

	input.mi.dwFlags = is_left ?
		MOUSEEVENTF_LEFTUP :
		MOUSEEVENTF_RIGHTUP;
	SendInput(1, &input, sizeof(INPUT));
}
void Autoclicker(float cps, int keybind, bool toggleMode, bool is_left, std::atomic<bool>& isActive)
{
    bool isPressed = false;  //Tracking keypress
    while (isActived)
    {
        if (toggleMode)
        {
            // Handle toggle mode
            if (GetAsyncKeyState(keybind) & 0x8000) 
            {
                if (!isPressed)  
                {
                    isActive = !isActive;  
                    isPressed = true;  
                }
            }
            else
            {
                isPressed = false;
            }
            if (isActive)
            {
                float baseDelay = 1000.0f / cps;
                float delay = baseDelay + randomOffset(rng);
                delay = max(10.0f, delay);
                click_mouse(is_left);
                PreciseSleep(delay);
            }
        }
        //Handle hodling mode
        else
        {
            if (GetAsyncKeyState(keybind) & 0x8000)
            {
                float baseDelay = 1000.0f / cps;
                float delay = baseDelay + randomOffset(rng);
                delay = max(10.0f, delay);
                click_mouse(is_left);
                PreciseSleep(delay);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  //reduce CPU usage
    }
}

                       
