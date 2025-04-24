#include "../header/applicationclass.h"

ApplicationClass::ApplicationClass()
{

}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{

}

ApplicationClass::~ApplicationClass()
{

}

bool ApplicationClass::Initialize(int scrnWidth, int scrnHeight, HWND hwnd)
{
	return true;
}

void ApplicationClass::Shutdown()
{
	return;
}
bool ApplicationClass::Frame()
{
	return true;
}

bool ApplicationClass::Render()
{
	return true;
}