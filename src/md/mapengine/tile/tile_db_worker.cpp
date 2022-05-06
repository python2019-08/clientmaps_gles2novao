#include <md/util/file_io.hpp>
#include <md/util/logtool.hpp>

#include "md/mapengine/tile/response.hpp"
#include "md/util/msg_que_worker.h" 
#include <md/mapengine/tile/vec_tile.hpp>
#include "md/md_global.h" 

#include "md/mapengine/tile/tile_db_worker.h"
 

using namespace clientmap;
 
CTileDbWorker& CTileDbWorker::inst()
{
    static CTileDbWorker s_tileWorker{1024};
    return s_tileWorker;
}

CTileDbWorker::CTileDbWorker(int aCapacity)
{
	m_capacity = aCapacity;
	m_thread = std::thread(&CTileDbWorker::run, this);
}

CTileDbWorker::~CTileDbWorker()
{
	stop();
} 

void CTileDbWorker::setMsgQWeakPtr( std::weak_ptr<CMsgQWorker> aMsgQPtr )
{
    std::unique_lock<std::mutex> lock(m_cs);
    m_msgQWeakPtr = aMsgQPtr;
}
        
void CTileDbWorker::queryTiles(TypedTileIdSet& aTLIds,
    int64_t aTmStamp)
{
    do{   //m_cs.lock();
		std::unique_lock<std::mutex> lock(m_cs);
 		if (!m_running ) {
            break;
		}

        //// move previous valid tasks to newQue
        std::deque<ShTileLoadTask >  newQue;        
        for (auto& task : m_taskQue)
        {
            if ( (aTLIds.count(task->_TTId) > 0)&&
                 (m_taskQue.size() < m_capacity)
                )
            {
                newQue.push_back(task);
                aTLIds.erase(task->_TTId);
            }
        }

        // append new tasks to newQue
        for (auto& tid : aTLIds)
        {
            if (m_taskQue.size() < m_capacity) {
                newQue.push_back( 
                    std::make_shared<STTileLoadTask>( tid, aTmStamp)
                );
            }
        }

        // swap newQue with m_taskQue
        m_taskQue.swap(newQue);
        newQue.clear();
    } while (false); //m_cs.unlock();
	m_condition.notify_all();
}
         
void CTileDbWorker::startJobs()
{
	{
		std::unique_lock<std::mutex> lock(m_cs);
		//LOGTO("Poking TileWorker - enqueued %d", m_taskQue.size());
		if (!m_running || m_taskQue.empty()) {
			return;
		}

		m_condition.notify_all();
	}
}
void CTileDbWorker::stop()
{
#ifdef EVENT_DEBUG
	uint32 ts = Time::getMillisecondCounter();
#endif
	{
		std::unique_lock<std::mutex> lock(m_cs);
		m_running = false;  
		m_condition.notify_all();
	}

	m_thread.join();
	m_taskQue.clear();
#ifdef EVENT_DEBUG
	EVT_LOG("QueneThread", "%s thread stopped %d-%d",
		getThreadName().toRawUTF8(), timeOut,
		Time::getMillisecondCounter() - ts);
#endif
}
          
 
/* implement Thread method */
void CTileDbWorker::run(void)
{
	while (true)
	{
		std::shared_ptr<STTileLoadTask>  task;
		{
			std::unique_lock<std::mutex> lock(m_cs);
			m_condition.wait(lock, [&] {
				return !m_running || !m_taskQue.empty();
			});

			if (!m_running)
			{
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

            task = std::move(m_taskQue.front());
			m_taskQue.pop_front();
		}

        doTileLoadTask( *task );
		  
		std::this_thread::yield();
	}

	{
		/* clear event queue */
		std::unique_lock<std::mutex> lock(m_cs);
		m_taskQue.clear();
	}

} //ENDOF CTileDbWorker::run(void)
	 
int CTileDbWorker::doTileLoadTask(const STTileLoadTask& aTask)
{
    int ret = 0;
    ShTileResponse  respon =nullptr;

    TileID     taskTileId = aTask._TTId.tileId();
    ETileType tileType = (ETileType)aTask._TTId.w;

    do
    {
        ////==== 
        //if (tileType != ETileType::eRaster) 
        //{
        //}

        //==================================
        if (tileType != ETileType::eVector){
            break;
        }
        ShVectTile   vecTile = loadVectTile(taskTileId);
        if (vecTile) {
            respon = std::make_shared<CTileResponse>(aTask._TTId,
                200, vecTile);
        }

    } while (false);

    
    //--------------------------------------
    std::shared_ptr<CMsgQWorker > shMsgQ =nullptr;
    {
        std::unique_lock<std::mutex> lock(m_cs);
        shMsgQ = m_msgQWeakPtr.lock();
    }
    if (shMsgQ){
        shMsgQ->postMsg(  (int8_t)(EMviewEvent::kDataRcv),
            respon, aTask._TTId.w,// ETileType
            0.0f,  currentMillisec() );
    }
    return 0;
}

 
ShVectTile CTileDbWorker::loadVectTile(const TileID& aTileId )
{
	const char*  mvtRootDir = CMdGlobal::unique.m_mvtFilesPath.c_str();

    char tilePath[256] = { 0 };
#ifdef CHANGE_MBTILE_2_OURTILE
	uint32_t tileNoXMax = (1<< aTileId.z) -1;
	snprintf(tilePath, sizeof(tilePath), "%s/%d/%d-%d-%d.pbf",
		mvtRootDir, aTileId.z, aTileId.z, aTileId.x, tileNoXMax - aTileId.y);
#else
    snprintf(tilePath, sizeof(tilePath), "%s/%d/%d-%d-%d.pbf",
		mvtRootDir,aTileId.z, aTileId.z, aTileId.x, aTileId.y);
#endif

    ShVectTile   vecTile = nullptr;
    RawTileData  rawData;
    int ret = readFile(tilePath, rawData);
    if (ret == 0 && !rawData.empty()) {
        vecTile = VectTile::doParse(aTileId, rawData);

        Log::Debug("%s read ok\n", tilePath);
    }
    else {
        Log::Debug("%s read failed\n", tilePath);
    }

    return vecTile;
}