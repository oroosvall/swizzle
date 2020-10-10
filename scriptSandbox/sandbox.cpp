
#include <utils/Config.hpp>
#include <swizzle/Swizzle.hpp>

#include <Windows.h>

class Application
{
public:

	Application()
		: mAppConfig("App.cfg")
	{
	}

	~Application()
	{
	}

	void mainLoop();

protected:


	utils::Config mAppConfig;

};

void Application::mainLoop()
{

	/*script::Script scr;
	scr.compileFromFile("scripts/test.sws");*/

}

int main()
{
	Application app;

	app.mainLoop();




	return 0;
}