#pragma once
#include "Event.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf::EventSystem
{
	class IndirectInvocationEvent: public RTTI::Implementation<IndirectInvocationEvent, BasicEvent, IIndirectInvocationEvent>
	{
		public:
			IndirectInvocationEvent() noexcept = default;

		public:
			// IEvent
			EventID GetEventID() const override
			{
				return EvtIndirectInvocation;
			}
			FlagSet<EventCategory> GetEventCategory() const override
			{
				return EventCategory::Unknown;
			}

			void SetEventSource(IEvtHandler* evtHandler) override
			{
				if (!BasicEvent::GetEventSource())
				{
					BasicEvent::SetEventSource(evtHandler);
				}
			}

			bool IsSkipped() const override
			{
				return false;
			}
			void Skip(bool skip = true) override
			{
			}

			bool IsAllowed() const override
			{
				return true;
			}
			void Allow(bool allow = true) override
			{
			}
	};
}

namespace kxf::EventSystem
{
	// Wrapper for free/static/lambda function or a class that implements 'operator()'
	template<class TCallable, class... Args>
	class CallableIndirectInvocation: public IndirectInvocationEvent
	{
		protected:
			TCallable m_Callable;
			std::tuple<Args...> m_Parameters;

		public:
			CallableIndirectInvocation(TCallable&& callable, Args&&... arg)
				:m_Callable(std::forward<TCallable>(callable)), m_Parameters(std::forward<Args>(arg)...)
			{
			}
			
		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<CallableIndirectInvocation>(std::move(*this));
			}

			// IIndirectInvocationEvent
			void Execute() override
			{
				std::apply(m_Callable, std::move(m_Parameters));
			}
	};

	// Wrapper for class member function
	template<class TMethod, class... Args>
	class MethodIndirectInvocation: public IndirectInvocationEvent
	{
		protected:
			using TClass = typename Utility::MethodTraits<TMethod>::TInstance;

		protected:
			std::tuple<Args...> m_Parameters;
			TMethod m_Method = nullptr;

		public:
			MethodIndirectInvocation(TMethod method, Args&&... arg)
				:m_Method(method), m_Parameters(std::forward<Args>(arg)...)
			{
				static_assert(std::is_base_of_v<IEvtHandler, TClass>, "IEvtHandler descendant required");
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<MethodIndirectInvocation>(std::move(*this));
			}

			// IIndirectInvocationEvent
			void Execute() override
			{
				std::apply([this](auto&&... arg)
				{
					TClass* evtHandler = static_cast<TClass*>(GetEventSource());
					std::invoke(m_Method, evtHandler, std::forward<decltype(arg)>(arg)...);
				}, std::move(m_Parameters));
			}
	};
}
