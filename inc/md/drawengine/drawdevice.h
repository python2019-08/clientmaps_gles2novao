#pragma once
#ifndef _MAPSTU_IDrawDevice_H__
#define _MAPSTU_IDrawDevice_H__

namespace clientmap {
 
class IDrawDevice
{
public:
    // Called when the backend's GL context needs to be made active or inactive. 
    // These are called,as a matched pair.
	virtual bool activate() = 0;
	virtual void deactivate() = 0;

	// 需要显示
	virtual void triggerDraw() 
    {

    }
};

}  // namespace clientmap

#endif // #ifndef _MAPSTU_IDrawDevice_H__
