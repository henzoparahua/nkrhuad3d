#include "../Win/systemclass.h"

int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int iCmdshow)
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