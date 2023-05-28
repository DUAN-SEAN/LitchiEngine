
#pragma once
#include <Runtime/Function/UI/Internal/WidgetContainer.h>

#include "glm.hpp"
#include "gtc/quaternion.hpp"
#include "Runtime/Function/Renderer/Resources/Texture.h"
#include "Runtime/Function/Renderer/Resources/texture2d.h"
#include "Runtime/Function/UI/Types/Color.h"
#include "Runtime/Function/UI/Widgets/Texts/Text.h"
#include "Runtime/Function/UI/Widgets/Visual/Image.h"


namespace LitchiRuntime
{
	/**
	* Provide some helpers to draw UI elements
	*/
	class GUIDrawer
	{
	public:
		static const Color TitleColor;
		static const Color ClearButtonColor;

		static const float _MIN_FLOAT;
		static const float _MAX_FLOAT;

		/**
		* Defines the texture to use when there is no texture in a texture resource field
		* @param p_emptyTexture
		*/
		static void ProvideEmptyTexture(Texture& p_emptyTexture);

		/**
		* Draw a title with the title color
		* @param p_root
		* @param p_name
		*/
		static void CreateTitle(WidgetContainer& p_root, const std::string& p_name);

		template <typename T>
		static void DrawScalar(WidgetContainer& p_root, const std::string& p_name, T& p_data, float p_step = 1.f, T p_min = std::numeric_limits<T>::min(), T p_max = std::numeric_limits<T>::max());
		static void DrawBoolean(WidgetContainer& p_root, const std::string& p_name, bool& p_data);
		static void DrawVec2(WidgetContainer& p_root, const std::string& p_name, glm::vec2& p_data, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawVec3(WidgetContainer& p_root, const std::string& p_name, glm::vec3& p_data, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawVec4(WidgetContainer& p_root, const std::string& p_name, glm::vec4& p_data, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawQuat(WidgetContainer& p_root, const std::string& p_name, glm::quat& p_data, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawString(WidgetContainer& p_root, const std::string& p_name, std::string& p_data);
	/*	static void DrawColor(WidgetContainer& p_root, const std::string& p_name, Color& p_color, bool p_hasAlpha = false);
		static Text& DrawMesh(WidgetContainer& p_root, const std::string& p_name, LitchiRuntime::Model*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static Image& DrawTexture(WidgetContainer& p_root, const std::string& p_name, LitchiRuntime::Texture*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static Text& DrawShader(WidgetContainer& p_root, const std::string& p_name, LitchiRuntime::Shader*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static Text& DrawMaterial(WidgetContainer& p_root, const std::string& p_name, OvCore::LitchiRuntime::Resource::Material*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static Text& DrawSound(WidgetContainer& p_root, const std::string& p_name, OvAudio::LitchiRuntime::Sound*& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);
		static Text& DrawAsset(WidgetContainer& p_root, const std::string& p_name, std::string& p_data, OvTools::Eventing::Event<>* p_updateNotifier = nullptr);*/

		template <typename T>
		static void DrawScalar(WidgetContainer& p_root, const std::string& p_name, std::function<T(void)> p_gatherer, std::function<void(T)> p_provider, float p_step = 1.f, T p_min = std::numeric_limits<T>::min(), T p_max = std::numeric_limits<T>::max());
		static void DrawBoolean(WidgetContainer& p_root, const std::string& p_name, std::function<bool(void)> p_gatherer, std::function<void(bool)> p_provider);
		static void DrawVec2(WidgetContainer& p_root, const std::string& p_name, std::function<glm::vec2(void)> p_gatherer, std::function<void(glm::vec2)> p_provider, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawVec3(WidgetContainer& p_root, const std::string& p_name, std::function<glm::vec3(void)> p_gatherer, std::function<void(glm::vec3)> p_provider, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawVec4(WidgetContainer& p_root, const std::string& p_name, std::function<glm::vec4(void)> p_gatherer, std::function<void(glm::vec4)> p_provider, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawQuat(WidgetContainer& p_root, const std::string& p_name, std::function<glm::quat(void)> p_gatherer, std::function<void(glm::quat)> p_provider, float p_step = 1.f, float p_min = _MIN_FLOAT, float p_max = _MAX_FLOAT);
		static void DrawDDString(WidgetContainer& p_root, const std::string& p_name, std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider, const std::string& p_identifier);
		static void DrawString(WidgetContainer& p_root, const std::string& p_name, std::function<std::string(void)> p_gatherer, std::function<void(std::string)> p_provider);
		static void DrawColor(WidgetContainer& p_root, const std::string& p_name, std::function<Color(void)> p_gatherer, std::function<void(Color)> p_provider, bool p_hasAlpha = false);

		static void LitchiRuntime::GUIDrawer::DrawInputField4Int(WidgetContainer& p_root, const std::string& p_name, std::function<int(void)> p_gatherer, std::function<void(int)> p_provider);
		static void LitchiRuntime::GUIDrawer::DrawInputField4Int32(WidgetContainer& p_root, const std::string& p_name, std::function<int32_t(void)> p_gatherer, std::function<void(int32_t)> p_provider);
		static void DrawInputField4Float(WidgetContainer& p_root, const std::string& p_name, std::function<float(void)> p_gatherer, std::function<void(float)> p_provider);
		static void DrawInputField4Double(WidgetContainer& p_root, const std::string& p_name, std::function<double(void)> p_gatherer, std::function<void(double)> p_provider);

		template <typename T>
		static ImGuiDataType_ GetDataType();

		template <typename T>
		static std::string GetFormat();

	private:
		static Texture* __EMPTY_TEXTURE;
	};
}

#include "GUIDrawer.inl"