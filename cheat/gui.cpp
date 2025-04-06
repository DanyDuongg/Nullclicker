#include "gui.h"
#include "clicker.h"
#include "vars.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#define WNDPROC_FUNC LRESULT CALLBACK
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);

WNDPROC_FUNC WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED)
		{
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			gui::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter); // set click points
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(
					gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}

	}return 0;

	}

	return DefWindowProc(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = "class001";
	windowClass.hIconSm = 0;

	RegisterClassEx(&windowClass);

	window = CreateWindowEx(
		0,
		"class001",
		windowName,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}

void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT)
		{
			isRunning = !isRunning;
			return;
		}
	}

	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}
std::thread leftClickerThread;
std::thread rightClickerThread;
void gui::Render() noexcept
{
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin(
		"Autoclicker-1337",
		&isRunning,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove
	);
	//Handle click options
	static bool isLeftclick = false;
	static bool isRightcLick = false;
	if (ImGui::Checkbox("Left click", &isLeftclick))
	{
		isLeft = true;
	}
	if (ImGui::Checkbox("Right click", &isRightcLick))
	{
		isRight = true;
	}
	//Handle keybinds
	static char bufferKeybindLeft[10];
	static std::string keybindInputLeft;
	if (ImGui::InputText("Set left click keybind: ", bufferKeybindLeft,IM_ARRAYSIZE(bufferKeybindLeft)))
	{
		keybindInputLeft = bufferKeybindLeft;
		Leftkeybind = getKeybindFromUser(keybindInputLeft);
	}

	static char bufferKeybindRight[10];
	static std::string keybindInputRight;
	if (ImGui::InputText("Set right click keybind", bufferKeybindRight, IM_ARRAYSIZE(bufferKeybindRight)))
	{
		keybindInputRight = bufferKeybindRight;
		Rightkeybind = getKeybindFromUser(keybindInputRight);
	}
	//Handle cps
	static float Guicps = 15.0f;
	if (ImGui::SliderFloat("CPS", &Guicps, 1.0f, 50.0f, "%.1f"))
	{
		cps = Guicps;
	}

	//Handle holding/toggle
	static bool rightHold = false; 
	static bool leftHold = false;
	static bool rightToggle = false;
	static bool leftToggle = false;
	if (ImGui::Checkbox("Left hold",&leftHold))
	{
		leftToggle = false;
		
	}
	if (ImGui::Checkbox("Right hold", &rightHold))
	{
		rightToggle = false;
	}
	
	if (ImGui::Checkbox("Left toggle", &leftToggle))
	{
		leftHold = false;
	}
	if (ImGui::Checkbox("Right toggle", &rightToggle))
	{
		rightHold = false;
	}
	
	//button
	if (ImGui::Button("Apply"))
	{
		if (isActived) 
		{
			isActived = false;
			if (leftClickerThread.joinable()) leftClickerThread.join();
			if (rightClickerThread.joinable()) rightClickerThread.join();
		}
		else
		{
			isActived = true;
			if (isLeft)
			{
				if (leftHold)
				{
					leftClickerThread = std::thread(Autoclicker, cps, Leftkeybind, false, true, std::ref(isHoldingleft));  // Pass 'false' for toggle mode (hold mode)
				}
				else if (leftToggle)
				{
					leftClickerThread = std::thread(Autoclicker, cps, Leftkeybind, true, true, std::ref(isHoldingleft));  // Pass 'true' for toggle mode
				}
			}

			if (isRight)
			{
				if (rightHold)
				{
					//hold mode
					rightClickerThread = std::thread(Autoclicker, cps, Rightkeybind, false, false, std::ref(isHoldingright));  // Pass 'false' for toggle mode (hold mode)
				}
				else if (rightToggle)
				{
					//toggle mode
					rightClickerThread = std::thread(Autoclicker, cps, Rightkeybind, true, false, std::ref(isHoldingright));  // Pass 'true' for toggle mode
				}
			}
		}
	}
		
		
		

	ImGui::End();
}
