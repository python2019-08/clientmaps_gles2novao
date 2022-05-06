#ifndef MAPSTU_MD_GLOBAL_H__INCLUDED
#define MAPSTU_MD_GLOBAL_H__INCLUDED
#include <cstdint>
#include <string>
#include <memory> 

//#define OsID_WINDOWS 
//#define SWDevFW_QT 
//#define CHANGE_MBTILE_2_OURTILE  1
//#define USE_ViewCamera_AS_TransformState 1


namespace clientmap {
  
class CMdGlobal  
{
	CMdGlobal();// Prevent construction
	~CMdGlobal(); // Prevent unwanted destruction	
	CMdGlobal(const CMdGlobal&) = delete; // Prevent construction by copying
	CMdGlobal& operator=(const CMdGlobal&) = delete; // Prevent assignment	
public:  
	static CMdGlobal unique;
	//static Singleton& Instance()
	//{
	//	static Singleton inst;
	//	return inst;
	//}
    
	bool init(
		double             aPixelRatio,
        const std::string& aRootPath, 		
        const std::string& aBaseUrl,
        const std::string& aAccessToken
    );
    void done(); 
	 
	 
public:
	double      m_pixelRatio = 1.0;
	// paths
	std::string m_rootPath;
	//R"(C:/_abel/_stu/map/_my/mvt)"
	std::string m_mvtFilesPath = R"(F:/_sgit/_stu/map/_my/mvt)";

    std::string m_assetPath;
    std::string m_fontDir = R"(F:/_sgit/_stu/map/_my/mvt)";
    
	uint32_t    m_cacheMaxSize;
    std::string m_cacheDbPath;

    std::string m_baseUrl;    
	std::string m_token;
	
    // 
    std::string m_curStyleName;  

private:
	//-----------------------------
	//----   functions    
	//-----------------------------
};

}//namespace clientmap 
#endif// !MAPSTU_MD_GLOBAL_H__INCLUDED