
#include "Standalone/include/ApplicationStandalone.h"

#include "Runtime/Function/Renderer/Rendering/Renderer.h"
#include "Runtime/Function/Renderer/Resource/ResourceCache.h"
#include "Runtime/Function/Renderer/Resource/Import/FontImporter.h"

namespace LitchiStandalone
{
	ApplicationStandalone* ApplicationStandalone::instance_;

	ApplicationStandalone::ApplicationStandalone() : ApplicationBase()
	{
	}
	ApplicationStandalone::~ApplicationStandalone()
	{
		// Îö¹¹ÆäËû
		sceneManager = nullptr;
		modelManager->UnloadResources();
		modelManager = nullptr;
		shaderManager->UnloadResources();
		shaderManager = nullptr;
		materialManager->UnloadResources();
		materialManager = nullptr;
		fontManager->UnloadResources();
		fontManager = nullptr;
		textureManager->UnloadResources();
		textureManager = nullptr;
		window = nullptr;
		ResourceCache::Shutdown();
		Renderer::Shutdown();
		FontImporter::Shutdown();

	}
	void ApplicationStandalone::Init()
	{
		instance_ = this;

		// Init Base
		ApplicationBase::Init();

		// prepare swapChain window

		// Setup RendererPath
		SetupRendererPath();

		// test 2
		{
			sceneManager->LoadScene("Scenes\\New Scene4.scene", false);
			sceneManager->GetCurrentScene()->Resolve();

			m_rendererPath4Game->SetScene(sceneManager->GetCurrentScene());
		}

	}
	void ApplicationStandalone::Run()
	{
	}
	void ApplicationStandalone::Update()
	{
	}
	bool ApplicationStandalone::IsRunning() const
	{
		return false;
	}
	void ApplicationStandalone::SetupRendererPath()
	{
	}
}

