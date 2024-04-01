
#pragma once

#include "Runtime/Core/App/ApplicationBase.h"
#include "Runtime/Function/Renderer/Rendering/RendererPath.h"

using namespace LitchiRuntime;

namespace LitchiStandalone
{
	class ApplicationStandalone :public ApplicationBase
	{
	public:
		ApplicationStandalone();
		~ApplicationStandalone();
		LitchiApplicationType GetApplicationType() override;
		bool Initialize() override;
		void Run() override;
		void Update() override;

		WindowSettings CreateWindowSettings() override;

		/**
		* Returns true if the app is running
		*/
		bool IsRunning() const;
		static ApplicationStandalone* Instance() { return instance_; }
	public:
		void SetupRendererPath();
		RendererPath* m_rendererPath4Game = nullptr;
	private:

		float m_restFixedTime = 0.0f;
		uint64_t m_elapsedFrames = 0;
		static ApplicationStandalone* instance_;
	};
}
