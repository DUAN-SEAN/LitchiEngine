
#pragma once

namespace LitchiRuntime
{
	class ApplicationBase;

	/**
	 * @brief Engine Application Entry Point
	*/
	class Application {
	public:

		/**
		 * @brief Initialize Application
		 * @param instance ApplicationInstance: Editor,Standalone etc
		*/
		static void Initialize(ApplicationBase* instance);

		/**
		 * @brief Run Application When Application Exit Return
		*/
		static void Run();

	private:

		/**
		 * @brief ApplicationInstance
		*/
		static ApplicationBase* s_instance;
	};
}
