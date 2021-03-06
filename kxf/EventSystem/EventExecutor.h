#pragma once
#include "IEventExecutor.h"
#include "IEvent.h"
#include "kxf/Utility/ScopeGuard.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf::EventSystem
{
	// Wrapper for free/static/lambda or a class which implements 'operator()'
	template<class TEvent_, class TCallable_>
	class CallableEventExecutor: public IEventExecutor
	{
		public:
			using TEvent = TEvent_;
			using TCallable = TCallable_;

		protected:
			TCallable m_Callable;
			const void* m_OriginalAddress = nullptr;

		public:
			CallableEventExecutor(TCallable&& func)
				:m_Callable(std::forward<TCallable>(func)), m_OriginalAddress(std::addressof(func))
			{
				static_assert(std::is_void_v<std::invoke_result_t<TCallable, TEvent&>>, "return type must be void");
			}

		public:
			void Execute(IEvtHandler& evtHandler, IEvent& event) override
			{
				std::invoke(m_Callable, static_cast<TEvent&>(event));
			}
			bool IsSameAs(const IEventExecutor& other) const noexcept override
			{
				if (typeid(*this) == typeid(other))
				{
					return m_OriginalAddress == static_cast<const CallableEventExecutor&>(other).m_OriginalAddress;
				}
				return false;
			}
			IEvtHandler* GetTargetHandler() noexcept override
			{
				return nullptr;
			}
	};

	// Wrapper for class member function
	template<class TEvent_, class TClass_, class TEventArg_, class THandler_>
	class MethodEventExecutor: public IEventExecutor
	{
		public:
			using TEvent = TEvent_;

			using TClass = TClass_;
			using TEventArg = TEventArg_;
			using THandler = THandler_;
			using TMethod = void(TClass::*)(TEventArg&);

		protected:
			THandler* m_Handler = nullptr;
			TMethod m_Method = nullptr;

		public:
			MethodEventExecutor(TMethod func, THandler& handler)
				:m_Handler(&handler), m_Method(func)
			{
			}

		public:
			void Execute(IEvtHandler& evtHandler, IEvent& event) override
			{
				TClass* handler = m_Handler;
				if (!handler)
				{
					if constexpr(std::is_base_of_v<IEvtHandler, TClass>)
					{
						handler = static_cast<TClass*>(&evtHandler);
					}
				}

				std::invoke(m_Method, handler, static_cast<TEventArg&>(event));
			}
			bool IsSameAs(const IEventExecutor& other) const noexcept override
			{
				if (typeid(*this) == typeid(other))
				{
					const MethodEventExecutor& otherRef = static_cast<const MethodEventExecutor&>(other);

					return (m_Method == otherRef.m_Method || otherRef.m_Method == nullptr) &&
						(m_Handler == otherRef.m_Handler || otherRef.m_Handler == nullptr);
				}
				return false;
			}
			IEvtHandler* GetTargetHandler() noexcept override
			{
				if constexpr(std::is_base_of_v<IEvtHandler, THandler>)
				{
					return m_Handler;
				}
				return nullptr;
			}
	};
}
