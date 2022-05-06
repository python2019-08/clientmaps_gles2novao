#include <assert.h>
#include "md/util/constants.hpp"

#include "md/util/cpp_misc.hpp"  
#include "md/util/math_tools.hpp" 
#include "md/util/merc_prj.hpp"  
 
#include "md/md_global.h" 

namespace clientmap {
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
CMdGlobal CMdGlobal::unique;

CMdGlobal::CMdGlobal( ) 
{
	 
}

CMdGlobal::~CMdGlobal()
{
    done();
}
 
bool CMdGlobal::init(
	double             aPixelRatio,
	const std::string& aRootPath,
	const std::string& aBaseUrl,
	const std::string& aAccessToken
)
{
    bool ret = true; 
	do
	{		
		m_rootPath = aRootPath;
		if (m_rootPath.empty())
		{
			m_rootPath = "../../resroot";
		} 
		//-------------
		m_assetPath = m_rootPath + "/md/assets/" ;
		m_fontDir   = m_assetPath;
		 
		//-------------
		m_mvtFilesPath = m_rootPath + R"(/md/cache/)";

		m_cacheDbPath  = m_rootPath + "/md/cache/";
		m_cacheMaxSize = 50 * 1024 * 1024;// 50M

		//-----------------------
		m_pixelRatio = aPixelRatio;
		//-----------------------
		m_baseUrl = aBaseUrl;
		if (m_baseUrl.empty()) {
			m_baseUrl = "https://api.mapbox.com";
		} 

		m_token = aAccessToken;
		
	} while (false);


    return ret;
}

void CMdGlobal::done()
{
 
}
 /*
void CMdGlobal::setToken(const std::string& aAccessToken)
{ 
	m_token = aAccessToken;     
}
 */
  

}//namespace clientmap 
