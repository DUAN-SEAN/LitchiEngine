
#pragma once

#include <functional>


namespace LitchiRuntime
{
	/**
	 * @brief The ID of a listener (Registered callback).
	 * This value is needed to remove a listener from an event
	*/
	using ListenerID = uint64_t;

	/**
	 * @brief A simple event that contains a set of function callbacks. These functions will be called on invoke
	 * @tparam ArgTypes (Variadic) event parameter list
	*/
	template<class... ArgTypes>
	class Event
	{
	public:

		/**
		 * @brief Simple shortcut for a generic function without return value
		*/
		using Callback = std::function<void(ArgTypes...)>;

		/**
		 * @brief Add a function callback to this event
		 * Also return the ID of the new listener (You should store the returned ID if you want to remove the listener later)
		 * @param p_callback 
		 * @return 
		*/
		ListenerID AddListener(Callback p_callback);

		/**
		 * @brief Add a function callback to this event
		 * Also return the ID of the new listener (You should store the returned ID if you want to remove the listener later)
		 * @param p_callback 
		 * @return 
		*/
		ListenerID operator+=(Callback p_callback);

		/**
		 * @brief Remove a function callback to this event using a Listener (Created when calling AddListener)
		 * @param p_listenerID 
		 * @return 
		*/
		bool RemoveListener(ListenerID p_listenerID);

		/**
		 * @brief Remove a function callback to this event using a Listener (Created when calling AddListener)
		 * @param p_listenerID 
		 * @return 
		*/
		bool operator-=(ListenerID p_listenerID);

		/**
		 * @brief Remove every listeners to this event
		*/
		void RemoveAllListeners();

		/**
		 * @brief Return the number of callback registered
		 * @return 
		*/
		uint64_t GetListenerCount();

		/**
		 * @brief Call every callbacks attached to this event
		 * @param p_args (Variadic) event parameter list
		*/
		void Invoke(ArgTypes... p_args);

	private:

		/**
		 * @brief event callback list
		*/
		std::unordered_map<ListenerID, Callback>	m_callbacks;

		/**
		 * @brief event using a Listener (Created when calling AddListener)
		*/
		ListenerID									m_availableListenerID = 0;
	};
}

#include "Event.inl"