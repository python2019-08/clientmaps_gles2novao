
#ifndef TILE_LOAD_WORKER_H__INCLUDED
#define TILE_LOAD_WORKER_H__INCLUDED

#pragma once

#include <stdint.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

#include <condition_variable>
#include <deque>
#include <atomic>
#include <mutex>
#include <thread>

#include "md/util/tile_id.hpp"

namespace clientmap {
class CMsgQWorker;



// STTileLoadReq //STTileLoadRequest
struct STTileLoadTask
{
    TypedTileId       _TTId{ 0,0,0,0 };
	int64_t	          _tmPoint = 0;	          

    STTileLoadTask()
	{ 
	}         

    STTileLoadTask(const TypedTileId& aTLId,
        int64_t aTmStamp 
		)
	{
        _TTId = aTLId;
        _tmPoint = aTmStamp;
	}

    STTileLoadTask(const STTileLoadTask& aTask)
	{
        _TTId = aTask._TTId;
        _tmPoint = aTask._tmPoint; 
	}

	void operator = (const STTileLoadTask& aTask)
	{
		if (this != &aTask)
		{
            _TTId = aTask._TTId;
            _tmPoint = aTask._tmPoint; 
		}
	}

	~STTileLoadTask()
	{
	}
};
using ShTileLoadTask = std::shared_ptr<STTileLoadTask>; 
//----------------------------
using RawTileData = std::string;// std::vector<uint8_t>;
using ShRawTileData = std::shared_ptr<RawTileData >;
//----------------------------
class VectTile;
using ShVectTile = std::shared_ptr<VectTile>;

//---------------------------- 
class CTileDbWorker
{
    CTileDbWorker(int aCapacity);
    CTileDbWorker(const CTileDbWorker&);
    const CTileDbWorker& operator=(const CTileDbWorker&);
public:        
    ~CTileDbWorker();
    static  CTileDbWorker& inst();
        
    void setMsgQWeakPtr(std::weak_ptr<CMsgQWorker> aMsgQPtr);
    void queryTiles(TypedTileIdSet& aTLIds,
        int64_t aTmStamp);
         
    void startJobs();
    void stop();

protected:   
	/* implement Thread method */
	void run(void);

    int doTileLoadTask(const STTileLoadTask& aTask);

    ShVectTile  loadVectTile(const TileID& aTileId);
protected:
	std::thread        m_thread;
	std::atomic<bool>  m_running{ true };
	std::atomic<int>   m_capacity{ 1024 };
    std::weak_ptr<CMsgQWorker> m_msgQWeakPtr;

	std::mutex                  m_cs;
	std::deque<ShTileLoadTask > m_taskQue;
	std::condition_variable     m_condition; 
};

}// namespace clientmap
 
#endif // !TILE_LOAD_WORKER_H__INCLUDED
