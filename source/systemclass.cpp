#include "../header/systemclass.h"

SystemClass::SystemClass()
{
	m_input = 0;
	m_application = 0;

}

SystemClass::SystemClass(const SystemClass& other)
{

}

SystemClass::~SystemClass()
{

}

bool SystemClass::Initialize()
{
	int scrnWidth, scrnHeight;
	bool result;

	InitializeWindows(scrnWidth, scrnHeight);

	m_input = new InputClass;
	m_input->Initialize();
	
	m_application = new ApplicationClass;

	result = m_application->Initialize(scrnWidth, scrnHeight, m_hwnd);
	if (!result)
	{
		return false;
	}
	
	return true;
}

void SystemClass::Shutdown()
{
	if (m_application)
	{
		m_application->Shutdown();
		delete m_application;
		m_application = 0;
	}
	if (m_input)
	{
		delete m_input;
		m_input = 0;
	}

	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else {
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}
	return;
}

bool SystemClass::Frame()
{
	bool result;

	if (m_input->isKeyDown(VK_ESCAPE))
	{
		return false;
	}

	result = m_application->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_KEYDOWN:
	{
		m_input->KeyDown((unsigned int)wparam);
		return 0;
	}
	case WM_KEYUP:
	{
		m_input->KeyUp((unsigned int)wparam);
		return 0;
	}

	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

//	Initialize and draw the window the screen size is set to the full screen or to the height and width written.
void SystemClass::InitializeWindows(int& scrnWidth, int& scrnHeight)
{
//	WNDCLASSEX is a structure in Windows programming that contains information about 
//	a window class. It is used with the RegisterClassEx function to register a window 
//	class, which defines the attributes of a window such as its style, icon, cursor, 
//	menu, and window procedure.
	WNDCLASSEX wc;

//	DEVMODE is a structure in Windows programming that contains information about the display device.
//	It is used with the ChangeDisplaySettings function to change the display settings of the device,
//	such as screen resolution, color depth, and refresh rate.
	DEVMODE dmScreenSettings;

	int posX, posY;

	ApplicationHandle = this;

	m_hinstance = GetModuleHandle(NULL);
	m_applicationName = L"nkrhua";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadCursor(NULL, IDC_ARROW);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDI_WINLOGO);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

//	After populating the WNDCLASSEX structure, it is passed to the RegisterClassEx function to 
//	register the class. This allows the application to create windows of that class, which will
//	inherit the attributes defined in the WNDCLASSEX structure.
	RegisterClassEx(&wc);

	scrnHeight = GetSystemMetrics(SM_CYSCREEN);
	scrnWidth = GetSystemMetrics(SM_CXSCREEN);

	if (FULL_SCREEN)
	{
//		memset is a function in C and C++ that fills a block of memory with a specified value.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)scrnWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)scrnHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = posY = 0;
	} else 	{
		scrnWidth = 1280;
		scrnHeight = 720;

		posX = (GetSystemMetrics(SM_CXSCREEN) - scrnWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - scrnHeight) / 2;
	}
		
//	CreateWindowEx is a function that creates an overlapped, pop-up, or child window.
//	It is used to create a window with extended styles, such as WS_EX_TOPMOST or WS_EX_LAYERED.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, scrnWidth, scrnHeight, NULL, NULL, m_hinstance, NULL);

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}