#include "DXAppSingleColor.h"
#include "DXAppBox.h"
#include "DXAppHills.h"
#include "DXAppShapes.h"
#pragma comment(lib, "d3d11.lib")

//#define DEMO_SINGLE_COLOR
//#define DEMO_BOX
//#define DEMO_HILLS
#define DEMO_SHAPES

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	//MessageBox(nullptr, "Hello world", "Test", NULL);
	DXApp* app;
#ifdef DEMO_SINGLE_COLOR
	app = new DXAppSingleColor(hInstance);
#elif defined(DEMO_BOX)
	app = new DXAppBox(hInstance);
#elif defined(DEMO_HILLS)
	app = new DXAppHills(hInstance);
#elif defined(DEMO_SHAPES)
	app = new DXAppShapes(hInstance);
#endif
	if (!app->Init()) {
		return 1;
	}
	return app->Run();
}