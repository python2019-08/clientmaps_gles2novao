//  MNOTE:DashArray is also named DashPattern. 
#ifndef MD_DRAWENGINE_RSCMGRS_DASH_ARRAY_H_INCLUDED
#define MD_DRAWENGINE_RSCMGRS_DASH_ARRAY_H_INCLUDED

#include <list>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <stack>

#include "glm/glm.hpp"   
   
namespace clientmap {
    
    
    class DashArray {
    public:
        DashArray( ) = delete;
        
        ~DashArray() = delete;
            
        static int  normalize(std::vector<float>& aDashArr);
        
        static size_t calcHash(const std::vector<float>& aDashArr);
        
        static int32_t calcLen(const std::vector<float>& aDashArr,
                                double& aRawLen);
             
        static int renderRow(const std::vector<float>& aNomalizedDashArr, 
            std::vector<uint8_t>& aDashImgRow);
    };
}// namespace clientmap

#endif //!MD_DRAWENGINE_RSCMGRS_DASH_ARRAY_H_INCLUDED
