
#ifndef EVENT_QUEUE_THREAD_H__INCLUDED
#define EVENT_QUEUE_THREAD_H__INCLUDED

#pragma once

#include <stdint.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

#include <condition_variable>
#include <deque>
#include <atomic>
#include <mutex>
#include <thread>

namespace clientmap {
	struct STMsg
	{
		uint32_t    _evtId = 0;	/* event ID defined by some app module */
		uint32_t	_senderId = 0;/* message sender,an app module ID */
		int64_t	    _tmPoint = 0;	/* Time stamp when some app module sending event */

		/* The event parameter data corresponding to the event ID*/
		std::shared_ptr<void>  _ptrParam;
		uint32_t               _nParam = 0; 
        float                  _fParam = 0.0;

		STMsg()
		{
		}
         

		STMsg( uint32_t id,
            uint32_t aSender,
            int64_t aTmStamp,
			std::shared_ptr<void> aPtrParam,
            uint32_t              aNParam,
            float                 aFParam
			)
		{
			_evtId = id;
			_senderId = aSender;
            _tmPoint = aTmStamp;

			_ptrParam = aPtrParam;
            _nParam = aNParam;
            _fParam = aFParam;
		}

		STMsg(const STMsg& aEvt)
		{
			_evtId    = aEvt._evtId;
			_senderId = aEvt._senderId;
			_tmPoint  = aEvt._tmPoint;

			_ptrParam = aEvt._ptrParam;
			_nParam   = aEvt._nParam ; 
            _fParam   = aEvt._fParam;
		}

		void operator = (const STMsg& aEvt)
		{
			if (this != &aEvt)
			{
				_evtId = aEvt._evtId;
				_senderId = aEvt._senderId;
                _tmPoint = aEvt._tmPoint;

				_ptrParam = aEvt._ptrParam;
				_nParam  = aEvt._nParam ; 
                _fParam = aEvt._fParam;
			}
		}

		~STMsg()
		{
		}
	};

	typedef std::function<void(const STMsg& evt)>   EventHandleFn;
	typedef std::unordered_map<int32_t, EventHandleFn > CMapEventId2Fn;

	enum class EMviewEvent : uint32_t {
		kAPP_EXIT =0,
        kViewChange,
        kDataRcv,
        kDataExpire,
        kClickAt,
        kMapCtrl,
        kStyleChange,
        kRefresh,
        kLayerSwitch,
        kAnnotMesh,
        kTimer,
        kFlyStopping,
        kFlyStopped,
        kFocusedIndoorChanged,
	};
     

	/*
		Thread with message queue.
	*/
	class CMsgQWorker
	{
	public:
		CMsgQWorker(int aCapacity)
		{
			m_capacity = aCapacity;
			m_thread = std::thread(&CMsgQWorker::run, this);
		}

		~CMsgQWorker()
		{
			stop();
		}
        // register msg handling function
		void regMsgFn(uint32_t aEventId, EventHandleFn  aEventFn)
		{
			std::unique_lock<std::mutex> lock(m_cs);
			m_msgId2Fn.emplace(aEventId, aEventFn);
		}
        
		void postMsg(uint32_t   id, 
            std::shared_ptr<void> aPtrParam,
            uint32_t aIntParam ,
            float    aFParam,
			int64_t  aTmStamp, 
            uint32_t sender = 0 )
		{
			{   //m_cs.lock();
				std::unique_lock<std::mutex> lock(m_cs);

				if (m_running &&
					(m_msgId2Fn.count(id) > 0) &&
					m_msgQue.size() < m_capacity)
				{
					m_msgQue.push_back(std::make_shared<STMsg>(id, sender, aTmStamp,
                        aPtrParam, aIntParam, aFParam));
				}
                //m_cs.unlock();
			}
			m_condition.notify_all();
		}
         
		void startJobs()
		{
			{
				std::unique_lock<std::mutex> lock(m_cs);
				//LOGTO("Poking TileWorker - enqueued %d", m_msgQue.size());
				if (!m_running || m_msgQue.empty()) {
					return;
				}

				m_condition.notify_all();
			}
		}
		void stop()
		{
#ifdef EVENT_DEBUG
			uint32 ts = Time::getMillisecondCounter();
#endif
			{
				std::unique_lock<std::mutex> lock(m_cs);
				m_running = false; 
                m_msgQue.push_back(
                    std::make_shared<STMsg>( ((uint32_t)EMviewEvent::kAPP_EXIT), 
                        00, 0LL, nullptr, 0, 0)
                );
				m_condition.notify_all();
			}

			m_thread.join();
			m_msgQue.clear();
#ifdef EVENT_DEBUG
			EVT_LOG("QueneThread", "%s thread stopped %d-%d",
				getThreadName().toRawUTF8(), timeOut,
				Time::getMillisecondCounter() - ts);
#endif
		}


	protected:
		bool isRegedMsgFn(int32_t aEventId)//not used
		{
			std::unique_lock<std::mutex> lock(m_cs);
			return (m_msgId2Fn.count(aEventId) > 0);
		}

		bool isLatestMsg(uint32_t id, uint32_t stamp, uint32_t dur)//not used
		{
			bool bRet = false;
			m_cs.lock();
			if (m_msgQue.size() > 0)
			{
				auto pEvent = m_msgQue.back();
				if (pEvent->_evtId == id && ((stamp - pEvent->_tmPoint) <= dur))
				{
					bRet = true;
				}
			}
			m_cs.unlock();
			return(bRet);
		};

		bool dispatchMsg(const STMsg& evt)
		{
			if (m_msgId2Fn.count(evt._evtId) > 0)
			{
				EventHandleFn pfn = m_msgId2Fn[evt._evtId];
				pfn(evt);
				return (true);
			}
			return(false);
		}


		/* implement Thread method */
		void run(void)
		{
			while (true)
			{
				std::shared_ptr<STMsg>  evt;
				{
					std::unique_lock<std::mutex> lock(m_cs);
					m_condition.wait(lock, [&] {
						return !m_running || !m_msgQue.empty();
					});

					if (!m_running)
					{
						break;
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(1));

					evt = std::move(m_msgQue.front());
					m_msgQue.pop_front();
				}

				uint32_t eventID = evt->_evtId;
				/* 分发事件 */
				if (dispatchMsg(*evt) == false) {}
				if (eventID == (uint32_t)EMviewEvent::kAPP_EXIT)
				{
					m_running = false;
					break;
				}
				std::this_thread::yield();
			}

			{
				/* clear event queue */
				std::unique_lock<std::mutex> lock(m_cs);
				m_msgQue.clear();
			}

		};
	protected:
		std::thread        m_thread;
		std::atomic<bool>  m_running{ true };
		std::atomic<int>   m_capacity{ 1024 };

		std::mutex                            m_cs;
		std::deque<std::shared_ptr<STMsg> > m_msgQue;
		std::condition_variable  m_condition;

		CMapEventId2Fn	 m_msgId2Fn;		/* 注册的消息处理函数 */
	};

}// namespace clientmap
 
#endif // EVENT_QUEUE_THREAD_H__INCLUDED
