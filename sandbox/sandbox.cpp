
#include <utils/Config.hpp>
#include <swizzle/Swizzle.hpp>

#include <glm/glm.hpp>

#include <swizzle/events/Event.hpp>

#include <array>
#include <iostream>

#include <utils/FpsCounter.hpp>
#include <utils/HighResolutionClock.hpp>

#include <swizzle/gfxutils/Camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

class ConsoleLogger : public swizzle::LoggerIfc
{
	// Inherited via LoggerIfc
	virtual void logTrace(const char* message) override
	{
		printf("Trace: %s\n", message);
	}
	virtual void logInfo(const char* message) override
	{
		printf("Info: %s\n", message);
	}
	virtual void logWarning(const char* message) override
	{
		printf("Warning: %s\n", message);
	}
	virtual void logError(const char* message) override
	{
		printf("Error: %s\n", message);
	}
	virtual void logCritical(const char* message) override
	{
		printf("Critical: %s\n", message);
	}
};

class Application : sw::EventHandler<sw::WindowEvent>
{
public:

	Application()
		: mWindow()
		, mRenderer()
		, mAppConfig("App.cfg")
		, mCamera(glm::radians(45.0F), 1280, 720)
	{

		mWindow = sw::SwCreateWindow(1280, 720, "App");

		mWindow->addEventListener(this);

		utils::Value api = mAppConfig.getValue("backend", static_cast<int32_t>(sw::eRendererBackend::eRendererVulkan));

		mRenderer = sw::SwCreateRenderer(mWindow.get(), static_cast<sw::eRendererBackend>(api.getInt()));
		//mRenderer->setVsync(sw::VSyncTypes::eSync_off);

		sw::input::setInputSource(mWindow.get());
	}
	
	void publishEvent(const sw::WindowEvent& evt)
	{
		if (evt.getEventType() == sw::eWindowEventType::eType_resize)
		{
			sw::ResizeEvent& e = (sw::ResizeEvent&)evt;
			mCamera.changeAspect(static_cast<float>(e.mWidth), static_cast<float>(e.mHeight));
			mResize = true;
		}
		else if (evt.getEventType() == sw::eWindowEventType::eType_input)
		{
			sw::InputEvent& e = (sw::InputEvent&)evt;
			if (e.mKey == 87) // F11 key
			{
				mWindow->setBorderless(e.mPressed);
			}
			
		}
	}

	~Application()
	{
	}

	void mainLoop();

protected:

	sw::Resource<sw::Window> mWindow;
	sw::Resource<sw::Renderer> mRenderer;

	utils::Config mAppConfig;

	sw::PerspectiveCamera mCamera;

	bool mResize = false;

};

void Application::mainLoop()
{
	float simpleQuad[] = {
		0.5F, -.5F, 0.0F,
		-.5F, -.5F, 0.0F,
		-.5F, 0.5F, 0.0F,

		0.5F, -.5F, 0.0F,
		-.5F, 0.5F, 0.0F,
		0.5F, 0.5F, 0.0F,
	};

	mWindow->show();
	
	auto vertexBuffer = mRenderer->createBuffer(sw::eBufferType::eVertexBuffer);
	auto commandBuffer = mRenderer->createCommandBuffer(sw::eCmdBufferType::ePrimary);

	auto shader = mRenderer->createShaderProgram(mRenderer->getDefaultFramebuffer(), sw::eShaderProgramType::GraphicsProgram);
	shader->load("shaders/simple.shader");
	
	auto drawBuffer = mRenderer->createFrameBuffer(2, sw::eDepthType::eDepthStencil, 1280, 720);
	drawBuffer->setClearDepth(0, 1.0F, 0);
	drawBuffer->setClearColor(1, {1.0F, 1.0F, 0.0F, 0.0F});

	auto shaderTest = mRenderer->createShaderProgram(drawBuffer, sw::eShaderProgramType::GraphicsProgram);
	shaderTest->load("shaders/simple.shader");

	
	vertexBuffer->setBufferData(simpleQuad, sizeof(simpleQuad));
	meshloader::loadObjMeshIntoBuffer("meshes/shape.obj", vertexBuffer);

	mCamera.setPosition({ 0.0F, 0.0F, 0.5F });

	utils::HighResolutionClock highRes;
	utils::FpsCounter fpsCount;

	while (mWindow->isVisible())
	{
		uint32_t x = 0;
		uint32_t y = 0;
		mWindow->getSize(x, y);

		if (mResize)
		{
			drawBuffer->resize(x, y);
			mResize = false;
		}

		sw::input::inputFrameReset();
		mWindow->pollEvents();

		float_t dt = highRes.secondsAsFloat(true);
		fpsCount.tick(dt);
		
		auto pos = mCamera.getPosition();
		if (sw::input::isKeyPressed(sw::input::Keys::eKey_A))
		{
			pos.x -= 1.0F * dt;
		}
		if (sw::input::isKeyPressed(sw::input::Keys::eKey_D))
		{
			pos.x += 1.0F * dt;
		}
		if (sw::input::isKeyPressed(sw::input::Keys::eKey_W))
		{
			pos.z -= 1.0F * dt;
		}
		if (sw::input::isKeyPressed(sw::input::Keys::eKey_S))
		{
			pos.z += 1.0F * dt;
		}

		mCamera.setPosition(pos);

		std::string str = "Fps: " + std::to_string(1.0F / dt);
		mWindow->setTitle(str.c_str());


		commandBuffer->resetBuffer();
		commandBuffer->beginRecording();
		
		mRenderer->prepare(commandBuffer);

		// test frame buffer
		commandBuffer->bindFrameBuffer(drawBuffer);
		commandBuffer->setViewport(x, y);
		commandBuffer->bindShaderProgram(shaderTest);
		glm::mat4 vp = mCamera.getViewProjection();

		commandBuffer->setShaderConstantData(shaderTest, &vp[0][0], sizeof(glm::mat4));
		commandBuffer->draw(vertexBuffer);

		commandBuffer->unbindFrameBuffer();

		// swapchain framebuffer
		auto frameBuffer = mRenderer->getDefaultFramebuffer();
		
		commandBuffer->bindFrameBuffer(frameBuffer);
		commandBuffer->unbindFrameBuffer();

		commandBuffer->resolve(drawBuffer, frameBuffer);

		/*commandBuffer->setViewport(x, y);

		commandBuffer->bindShaderProgram(shader);

		commandBuffer->setShaderConstantData(shader, &vp[0][0], sizeof(glm::mat4));
		commandBuffer->draw(vertexBuffer);*/

		//commandBuffer->unbindFrameBuffer();

		commandBuffer->endRecording();

		mRenderer->submit(commandBuffer);

		mRenderer->present();

	}
}

int main()
{
	ConsoleLogger logger;

	sw::SwAddLogger(&logger);
	sw::SwInitialize();
	Application app;

	app.mainLoop();

	sw::SwCleanup();

	
	return 0;
}