 
#include <stdio.h>
#include <assert.h>
 
#include "glm/glm.hpp"
#include "glm/gtc/round.hpp" // ceilPowerOfTwo
//#include <glm/ext/scalar_integer.hpp>// glm::nextPowerOfTwo
#include "md/util/cpp_misc.hpp" 
#include "md/md_global.h"

#include "md/drawengine/rscmgr/dash_array.h"

  
namespace clientmap
{ 
    //DashArray::DashArray( )
    //{
    //     
    //}
    //
    //DashArray::~DashArray() 
    //{
 
    //} 

 
    size_t DashArray::calcHash(const std::vector<float>& dasharray)
    {
        //size_t key = aPatternCap == LinePatternCap::Round 
        //    ? std::numeric_limits<size_t>::min() 
        //    : std::numeric_limits<size_t>::max();
        size_t key = std::numeric_limits<size_t>::max();
        for (const float part : dasharray)
        {
            hash_combine<float>(key, part);
        }
        return key;
    } 


    int DashArray::normalize(std::vector<float>& aDashArr)
    {
        std::vector<float >::iterator it = aDashArr.begin();
        // Collapse any zero-length dashPart
        for (; it != aDashArr.end(); )
        {
            float  dashPartLen = *it;
            if (dashPartLen <= 0.0)
            {
                it = aDashArr.erase(it);
            }
            else {
                ++it;
            }
        }

        // if aDashArr has less than 2 elements,it is invalid ,so clear it.
        if (aDashArr.size() < 2)
        {
            aDashArr.clear();
        }

        if (aDashArr.size() % 2 == 1) {
            aDashArr.insert(aDashArr.end(),
                aDashArr.begin(), aDashArr.end());
        }

        return (int)aDashArr.size();
    }

    int32_t DashArray::calcLen(const std::vector<float>& aDashArr,
         double& aRawLen)
    {
	    double width = 0.0;
	    for (auto len : aDashArr) {
	        width += len;
	    }
	    width *= CMdGlobal::unique.m_pixelRatio;
	    aRawLen = width;// ----

	    //---- extWidth
	    //glm::ceilPowerOfTwo(); glm::nextPowerOfTwo
	    int32_t extWidth = (int32_t)glm::ceilPowerOfTwo((int32_t)width);
	    if (extWidth < 32) {
	        extWidth = 32;
	    }
	    //aExLen = extWidth;
     
	    return extWidth;
    }
     
    // aDashScale is used for stretching dash pattern.
    int DashArray::renderRow(const std::vector<float>& aNomalizedDashArr,
        std::vector<uint8_t>& aDashImgRow)
    { 
	    double  rawLen = 32;
	    int32_t extLen = DashArray::calcLen(aNomalizedDashArr, rawLen);
	    double dashScale = ((double)extLen) / rawLen;
		
        bool dash = true;
        const uint8_t  dashA = 255;//dash alpha
        const uint8_t  bgA = 0; // background alpha
 
        int sumSegLen = 0;
        int segCnt = (int)aNomalizedDashArr.size();
        for (int i =0;i< segCnt; i++)
        {
            double pat = aNomalizedDashArr[i];
            int segment = std::floor(pat * dashScale);

            sumSegLen += segment;
            if (i == segCnt - 1){
                // ensure that (aDashImgRow.size() == extLen)
                segment += extLen - sumSegLen;
            }

            for (int i = 0; i < segment; ++i)
            {
                aDashImgRow.push_back(dash ? dashA : bgA);
            }
            dash = !dash;
        }

        return (int)aDashImgRow.size();
    }
     
}// clientmap
