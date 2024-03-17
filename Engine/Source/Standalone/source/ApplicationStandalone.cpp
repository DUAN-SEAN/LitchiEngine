
#include "Standalone/include/ApplicationStandalone.h"

#include "Runtime/Core/Time/time.h"
#include "Runtime/Function/Physics/physics.h"
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
		// 析构其他
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

			sceneManager->GetCurrentScene()->Play();

			// create camera
			{
				auto cameraObject = sceneManager->GetCurrentScene()->CreateGameObject("Camera");
				auto camera = cameraObject->AddComponent<Camera>();
				// 设置相机默认的位置和姿态
				auto cameraPosition = Vector3(0.0f, 5.0f, -10.0f);

				auto cameraRotation = Quaternion::FromEulerAngles((Vector3(Math::Helper::DegreesToRadians(45.0f), Math::Helper::DegreesToRadians(0.0f), 0.0f)));

				camera->SetFovHorizontalDeg(60.0f);
				cameraObject->GetComponent<Transform>()->SetPosition(cameraPosition);
				cameraObject->GetComponent<Transform>()->SetRotation(cameraRotation);

			}
			
			m_rendererPath4Game->SetScene(sceneManager->GetCurrentScene());
			m_rendererPath4Game->SetActive(true);
		}

		// set fullscreen
		window->SetFullscreen(true);
	}
	void ApplicationStandalone::Run()
	{
		while (IsRunning())
		{
			EASY_BLOCK("Frame") {
				// PreUpdate
				window->PollEvents();

				EASY_BLOCK("Update") {
					Update();
				}  EASY_END_BLOCK;

				EASY_BLOCK("Renderer") {
					Renderer::Tick();
					Renderer::Present();
				}  EASY_END_BLOCK;

				//window->SwapBuffers();
				InputManager::ClearEvents();
				++m_elapsedFrames;
			}  EASY_END_BLOCK;
		}
	}
	void ApplicationStandalone::Update()
	{
		Time::Update();
		UpdateScreenSize();
		InputManager::Tick();

		auto scene = this->sceneManager->GetCurrentScene();

		if(scene->IsPlaying())
		{
			// Physics Tick
			m_restFixedTime += Time::delta_time();
			float fixedDeltaTime = Time::fixed_update_time();
			while (m_restFixedTime > fixedDeltaTime)
			{
				Physics::FixedUpdate(fixedDeltaTime);

				scene->FixedUpdate();

				m_restFixedTime -= fixedDeltaTime;
			}

			scene->Update();
			scene->LateUpdate();
		}

	}
	bool ApplicationStandalone::IsRunning() const
	{
		return true;
	}
	void ApplicationStandalone::SetupRendererPath()
	{
		m_rendererPath4Game = new RendererPath(RendererPathType_GameView);

		// update renderer path
		Renderer::UpdateRendererPath(RendererPathType_GameView, m_rendererPath4Game);
	}
}

