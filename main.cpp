#include "header/systemclass.h"

// WinMain is the conventional name for the application entry point.
// It initializes the app, displays its main window, and enters a message
// retrieval-and-dispatch loop—the top-level control structure for the
// remainder of the application's execution.
// When the WM_QUIT message is received, the message loop terminates and the application ends.
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	System = new SystemClass;

	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}