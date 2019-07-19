#pragma once
#include "Event.h"
#include "EventBuilder.h"
#include "CallWrapper.h"
#include "Kx/Core/Utility.h"
#include "Events/IndirectCallEvent.h"
#include <vector>
#include <variant>
class wxWindow;
class KxEventFilter;
class KxEventCallWrapper;

class KX_API KxEvtHandler
{
	private:
		wxEvtHandler* m_EvtHandler = nullptr;

	private:
		bool DoProcessEvent(wxEvent& event)
		{
			event.SetEventObject(m_EvtHandler);
			return m_EvtHandler->ProcessEvent(event);
		}
		void DoQueueEvent(std::unique_ptr<wxEvent> event)
		{
			event->SetEventObject(m_EvtHandler);
			m_EvtHandler->QueueEvent(event.release());
		}

	public:
		KxEvtHandler(wxEvtHandler* evtHandler);
		KxEvtHandler(wxWindow* window);
		KxEvtHandler(wxEvtHandler& evtHandler)
			:KxEvtHandler(&evtHandler)
		{
		}
		KxEvtHandler(KxEvtHandler&& other)
		{
			*this = std::move(other);
		}
		KxEvtHandler(const KxEvtHandler&) = delete;
		virtual ~KxEvtHandler() = default;

	public:
		bool IsEvtHandlerEnabled() const
		{
			return m_EvtHandler->GetEvtHandlerEnabled();
		}
		void EnableEvtHandler(bool enabled = true)
		{
			m_EvtHandler->SetEvtHandlerEnabled(enabled);
		}

		const wxEvtHandler* GetEvtHandler() const
		{
			return m_EvtHandler;
		}
		wxEvtHandler* GetEvtHandler()
		{
			return m_EvtHandler;
		}

	public:
		// EvtHandlers chain
		wxEvtHandler* GetPreviousHandler() const
		{
			return m_EvtHandler->GetPreviousHandler();
		}
		wxEvtHandler* GetNextHandler() const
		{
			return m_EvtHandler->GetNextHandler();
		}
		
		void SetPrevHandler(wxEvtHandler* evtHandler)
		{
			m_EvtHandler->SetPreviousHandler(evtHandler);
		}
		void SetPrevHandler(KxEvtHandler* evtHandler)
		{
			m_EvtHandler->SetPreviousHandler(evtHandler ? evtHandler->m_EvtHandler : nullptr);
		}
		void SetNextHandler(wxEvtHandler* evtHandler)
		{
			m_EvtHandler->SetNextHandler(evtHandler);
		}
		void SetNextHandler(KxEvtHandler* evtHandler)
		{
			m_EvtHandler->SetNextHandler(evtHandler ? evtHandler->m_EvtHandler : nullptr);
		}

		void Unlink()
		{
			m_EvtHandler->Unlink();
		}
		bool IsUnlinked() const
		{
			return m_EvtHandler->IsUnlinked();
		}

	public:
		// User-supplied data
		void* GetClientData() const
		{
			return m_EvtHandler->GetClientData();
		}
		void SetClientData(void* clientData)
		{
			m_EvtHandler->SetClientData(clientData);
		}
		
		wxClientData* GetClientObject() const
		{
			return m_EvtHandler->GetClientObject();
		}
		void SetClientObject(std::unique_ptr<wxClientData> clientObject)
		{
			m_EvtHandler->SetClientObject(clientObject.release());
		}

	public:
		// Bind free or static function
		template<class TEvent, class TEventArg>
		void Bind(KxEventTag<TEvent> eventTag, void(*func)(TEventArg&))
		{
			return m_EvtHandler->Bind(eventTag, func);
		}
		
		// Unbind free or static function
		template<class TEvent, class TEventArg>
		bool Unbind(KxEventTag<TEvent> eventTag, void(*func)(TEventArg&))
		{
			return m_EvtHandler->Unbind(eventTag, func);
		}

		// Bind functor or a lambda function
		template<class TEvent, class TFunctor>
		void Bind(KxEventTag<TEvent> eventTag, const TFunctor& func)
		{
			return m_EvtHandler->Bind(eventTag, func);
		}

		// Unbind functor or lambda function
		template<class TEvent, class TFunctor>
		bool Unbind(KxEventTag<TEvent> eventTag, const TFunctor& func)
		{
			return m_EvtHandler->Unbind(eventTag, func);
		}

		// Bind a member function
		template<class TEvent, class TClass, class TEventArg, class TEventHandler>
		void Bind(KxEventTag<TEvent> eventTag, void(TClass::* method)(TEventArg&), TEventHandler* handler)
		{
			return m_EvtHandler->Bind(eventTag, method, handler);
		}

		template<class TEvent, class TClass, class TEventArg>
		void Bind(KxEventTag<TEvent> eventTag, void(TClass::* method)(TEventArg&))
		{
			return m_EvtHandler->Bind(eventTag, method, m_EvtHandler);
		}

		// Unbind a member function
		template<class TEvent, class TClass, class TEventArg, class TEventHandler>
		bool Unbind(KxEventTag<TEvent> eventTag, void(TClass::* method)(TEventArg&), TEventHandler* handler)
		{
			return m_EvtHandler->Unbind(eventTag, method, handler);
		}

		template<class TEvent, class TClass, class TEventArg>
		bool Unbind(KxEventTag<TEvent> eventTag, void(TClass::* method)(TEventArg&))
		{
			return m_EvtHandler->Unbind(eventTag, method, m_EvtHandler);
		}

	public:
		// Regular event sending functions
		bool ProcessEvent(wxEvent& event)
		{
			return DoProcessEvent(event);
		}
		template<class TEvent, class... Args> bool ProcessEvent(Args&&... arg)
		{
			TEvent event(std::forward<Args>(arg)...);
			return DoProcessEvent(event);
		}
		template<class TEvent, class... Args> bool ProcessEvent(KxEventTag<TEvent> eventTag, Args&&... arg)
		{
			TEvent event(std::forward<Args>(arg)...);
			event.SetEventType(eventTag);
			return DoProcessEvent(event);
		}

		void QueueEvent(std::unique_ptr<KxEvent> event)
		{
			DoQueueEvent(std::move(event));
		}
		template<class TEvent, class... Args> void QueueEvent(Args&&... arg)
		{
			DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...));
		}
		template<class TEvent, class... Args> void QueueEvent(KxEventTag<TEvent> eventTag, Args&&... arg)
		{
			auto event = std::make_unique<TEvent>(std::forward<Args>(arg)...);
			event->SetEventType(eventTag);

			DoQueueEvent(std::move(event));
		}

		// Construct and send the event using event builder
		template<class TEvent, class... Args>
		KxEventBuilder<TEvent> ProcessEventEx(KxEventTag<TEvent> eventTag, Args&&... arg)
		{
			TEvent event(std::forward<Args>(arg)...);
			event.SetEventType(eventTag);
			event.SetEventObject(m_EvtHandler);

			return KxEventBuilder<TEvent>(*this, event);
		}

		template<class TEvent, class... Args>
		KxEventBuilder<TEvent> QueueEventEx(KxEventTag<TEvent> eventTag, Args&&... arg)
		{
			auto event = std::make_unique<TEvent>(std::forward<Args>(arg)...);
			event.SetEventType(eventTag);
			event.SetEventObject(m_EvtHandler);

			return KxEventBuilder<TEvent>(*this, event);
		}

		// Queue execution of a class member, free, static, lambda function or a class which implements 'operator()'
		template<class TCallable, class... Args>
		void CallAfter(TCallable callable, Args&&... arg)
		{
			using namespace Kx::EventSystem;
			using TCallableTraits = typename Kx::Utility::CallableTraits<TCallable, Args...>;

			if constexpr(TCallableTraits::IsMemberFunction)
			{
				DoQueueEvent(std::make_unique<MethodIndirectCall<TCallable, Args...>>(*this, callable, std::forward<Args>(arg)...));
			}
			else if constexpr(TCallableTraits::IsFreeFunction)
			{
				DoQueueEvent(std::make_unique<FunctionIndirectCall<TCallable, Args...>>(*this, callable, std::forward<Args>(arg)...));
			}
			else if constexpr(TCallableTraits::IsFunctor)
			{
				DoQueueEvent(std::make_unique<FunctorIndirectCall<TCallable, Args...>>(*this, std::move(callable), std::forward<Args>(arg)...));
			}
			else
			{
				static_assert(false, "Unsupported callable type or the type is not callable");
			}
		}

	public:
		KxEvtHandler& operator=(const KxEvtHandler&) = delete;
		KxEvtHandler& operator=(KxEvtHandler&& other);
};
