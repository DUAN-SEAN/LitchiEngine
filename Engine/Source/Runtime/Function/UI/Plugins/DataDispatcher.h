
#pragma once

#include <functional>

#include "Runtime/Function/UI//Plugins/IPlugin.h"
#include "Runtime/Core/Tools/Eventing/Event.h"

#define TRY_GATHER(type, output)	if (auto plugin = GetPlugin<LitchiRuntime::DataDispatcher<type>>(); plugin) output = plugin->Gather();
#define TRY_PROVIDE(type, output)	if (auto plugin = GetPlugin<LitchiRuntime::DataDispatcher<type>>(); plugin) plugin->Provide(output);
#define TRY_NOTIFY_CHANGE(type)		if (auto plugin = GetPlugin<LitchiRuntime::DataDispatcher<type>>(); plugin) plugin->NotifyChange();

namespace LitchiRuntime
{
	/**
	* Plugin that allow automatic data update of any DataWidget
	*/
	template <typename T>
	class DataDispatcher : public LitchiRuntime::IPlugin
	{
	public:

		DataDispatcher() = default;

		DataDispatcher(Event<>* p_updateNotifier);

		/**
		* Register a reference
		* @param p_reference
		*/
		void RegisterReference(T& p_reference)
		{
			m_dataPointer = &p_reference;
		}

		/**
		* Register a provider (Which function should be called when the widget data is modified)
		* @param p_provider
		*/
		void RegisterProvider(std::function<void(T)> p_provider)
		{
			m_provider = p_provider;
		}

		/**
		* Register a gather (Which function should be called when the widget data needs to be updated)
		* @param p_provider
		*/
		void RegisterGatherer(std::function<T(void)> p_gatherer)
		{
			m_gatherer = p_gatherer;
		}

		/**
		* Provide data to the dispatcher
		* @param p_data
		*/
		void Provide(T p_data)
		{
			if (m_valueChanged)
			{
				if (m_dataPointer)
					*m_dataPointer = p_data;
				else
					m_provider(p_data);

				if(m_updateNotifier)
				{
					m_updateNotifier->Invoke();
				}
				m_valueChanged = false;
			}
		}

		/**
		* Notify that a change occured
		*/
		void NotifyChange()
		{
			m_valueChanged = true;
		}

		/**
		* Returns the data from the dispatcher
		*/
		T Gather()
		{
			return m_dataPointer ? *m_dataPointer : m_gatherer();
		}

		/**
		* Execute the data dispatcher behaviour (No effect)
		*/
		virtual void Execute() override {}

	private:
		bool m_valueChanged = false;
		T* m_dataPointer = nullptr;
		std::function<void(T)> m_provider;
		std::function<T(void)> m_gatherer;
		Event<>* m_updateNotifier = nullptr;
	};

	template<typename T>
	inline DataDispatcher<T>::DataDispatcher(Event<>* p_updateNotifier)
	{
		m_updateNotifier = p_updateNotifier;
	}
}