// clang-format off
 
#include <md/util/cpp_misc.hpp> 
#include <md/util/logtool.hpp> 
#include <md/util/merc_prj.hpp>

#include <md/mapengine/style/style_value.hpp>  

#include "md/drawengine/glpainters/glpainters.h" 
#include "md/drawengine/rscmgr/dash_array.h"
#include "md/drawengine/drawcontext.h"
#include <md/mapengine/tile/vec_tile.hpp> 

#include <md/drawengine/transform_state.hpp>
#include <md/mapengine/style/stylelayer_raster.hpp>


#include <md/mapengine/scenelayer/scenelayer_line.hpp>


namespace clientmap {
 
///////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////
LineSceneLayer::LineSceneLayer(const std::string& aName,  
	const glm::dvec4&  aOriginPt)
    : SceneLayer(aName, VecTileLayerType::LineString, aOriginPt)
{

}
  
LineSceneLayer ::~LineSceneLayer () = default;    


//--------
int LineSceneLayer::layout(ShVecTileLayer aVtLayer )
{
    int ret = 0;

    do
    {
        if (!aVtLayer || 
            aVtLayer->_type != VecTileLayerType::LineString||
			!m_evaledStyle)
        {
            ret = -1;
            break;
        }
        

        double tileZoom = _originPt.z;
        double styleZoom = _originPt.w;

        double lineHW = _calcLineHalfWidth();
		
		auto   style  = std::static_pointer_cast<LineStyle::TEvaluated>(m_evaledStyle); 
        DashArray::normalize( style->_dasharray );
        double dashArrLen = _calcDashArrayLen();



        const  int32_t  c_uint16Max = (int32_t)std::numeric_limits<uint16_t>::max();
        for (auto& ft : aVtLayer->_features)
        {
            for (auto& ln : ft->_lines)
            {
                int c_oneSegExtrudedPtCnt = 10;
                int nVert = (int)ln.size();

                // to extrude...
                TriStrip2DPart  newPart;
                if (style->_dasharray.empty() ) // solid line
                { 
                    if (nVert *c_oneSegExtrudedPtCnt > c_uint16Max) {
                        continue; // too long a path,abandon it.
                    }
                    
                    // if thin line,extrude it as thick line;
                    // draw it as thin line
                    if (lineHW > 2) {
                        _extrudeAAPath_round(ln, lineHW, newPart);
                    }
                    else {
                        _extrudeAAPath_roundHead(ln, 2, newPart);
                    }                     
                }
                else {// layout as dash line
                    c_oneSegExtrudedPtCnt = 4;
                    if (nVert *c_oneSegExtrudedPtCnt > c_uint16Max) {
                        continue; // too long a path,abandon it.
                    }

                    _extrudeAAPath_butt(ln, lineHW, dashArrLen, newPart);
                }

                // to store newPart into m_triStripParts
                if (m_triStripParts.empty() ||
                    m_triStripParts.back().size() + nVert * c_oneSegExtrudedPtCnt > c_uint16Max)
                {
                    m_triStripParts.emplace_back(newPart);
                }
                else {
                    TriStrip2DPart&  curPart = m_triStripParts.back();
                    curPart.insert(curPart.end(), newPart.begin(), newPart.end());
                }

            }
        }
    } while (false);

    return ret;
}
void   LineSceneLayer::debugInf2Log()
{
    auto   style = std::static_pointer_cast<LineStyle::TEvaluated>(m_evaledStyle);
    Log::Debug("LineSceneLayer =%s , lineW=%f, dashCnt=%d，clr(%d,%d,%d,%d)\n",
        this->_id.c_str(), style->_width, style->_dasharray.size(),
        style->_color.r, style->_color.g, style->_color.b, style->_color.a);
 
}
double LineSceneLayer::_calcLineHalfWidth()
{
    auto  style = std::static_pointer_cast<LineStyle::TEvaluated>(m_evaledStyle);
    double lineHW = style->_width * 0.5;
#if 0
    double tileZoom  = _originPt.z;
    //double styleZoom = _originPt.w;
    //double diffScale = glm::pow(2, styleZoom - tileZoom);
	double pxGeoW    = MercPrj::pxGeoWidth(tileZoom);
    lineHW = lineHW * pxGeoW;

    //// convert bpx to tileEXTENT coordinate.
    double tileLen = MercPrj::tileBpxWidth(tileZoom);
    lineHW *= (tileEXTENT / tileLen);
#endif
    return lineHW;
}

double LineSceneLayer::_calcDashArrayLen()
{
    auto  style = std::static_pointer_cast<LineStyle::TEvaluated>(m_evaledStyle);
    double dashArrRawLen = 1;
    double dashArrExLen = (double)DashArray::calcLen(style->_dasharray, dashArrRawLen);
#if 0
    /// convert dashArrExLen to Bpx unit
    // double styleZoom = _originPt.w; 
    double tileZoom = _originPt.z;
    double pxGeoW = MercPrj::pxGeoWidth(tileZoom);
    dashArrExLen *=  pxGeoW ;

    //// convert dashArrExLen from bpx to tileEXTENT coordinate.
    double tileLen = MercPrj::tileBpxWidth(tileZoom);
    dashArrExLen *= (tileEXTENT / tileLen);
#endif
    return dashArrExLen;
}
 
//--coefficients for extruding path...
//    If  the cos of the turn-angle is less than this value,the line cap
//will be flat.
#define  FLAT_CAP_MAXCOS     (-0.7071)// Angle is 135 degrees
//    If the miter width is less than MITER_JOIN_MAX_MULTIPLE times of the
//line width,the line join will be miter.
#define  MITER_JOIN_MAX_MULTIPLE (1.1)

int LineSceneLayer::_calcLineHeadCap(glm::dvec2* p0,
    glm::dvec2& p1, // start point
    glm::dvec2& p2, // end   point
    double  aLineR,//line radius ,namely line Thickness
    glm::vec2  aLineCap[10],
    glm::vec2  aLineCapTexSTs[10]
)
{
    int retPtCnt = 0;
    do
    {
        glm::dvec2  e12 = p2 - p1;
        double segLen = glm::length(e12);
        if (segLen < 0.000001) {//if (p2 == p1)
            retPtCnt = 0;
            break;
        }
        e12 = glm::normalize(e12);


        //-- Does the line head have a round cap or a flat cap?
        bool  isRoundCap = true;
        if (!p0) {
            isRoundCap = false;
        }
        else
        {
            glm::dvec2 e10 = (*p0) - p1;// 1 --> 0
            e10 = glm::normalize(e10);

            float  cosAngle012 = glm::dot(e10, e12);
            if (cosAngle012 < FLAT_CAP_MAXCOS)// Angle012 (- (110,180]
            {
                isRoundCap = false;
            }
        }

        //--
        e12 = e12 * ((double)aLineR);
        glm::dvec2 N12(-e12.y, e12.x);
        glm::dvec2 S12 = -N12;

        int        iVert = 0;
        int     iAATexST = 0;
        if (!isRoundCap)// Flat cap
        {
            if (p0)// seal the seam
            {
                glm::dvec2 e01 = p1 - (*p0);// 0 --> 1
                e01 = glm::normalize(e01);
                e01 = e01 * ((double)aLineR);
                glm::dvec2  N01(-e01.y, e01.x);
                glm::dvec2  S01 = -N01;


                aLineCap[iVert++] = p1 + S01;// 1+//isolate this gap from previous gap.
                aLineCap[iVert++] = p1 + S01;// 1
                aLineCap[iVert++] = p1 + N01;// 2

                aLineCapTexSTs[iAATexST++] = { 0.0, 0.0 };
                aLineCapTexSTs[iAATexST++] = { 0.5, 0.0 };
                aLineCapTexSTs[iAATexST++] = { 0.5, 1.0 };
            }
            else {
                aLineCap[iVert++] = p1 + S12;// 1+// isolate this gap from previous gap.
                aLineCapTexSTs[iAATexST++] = { 0.0, 0.0 };//1+
            }

            aLineCap[iVert++] = p1 + S12;//1
            aLineCap[iVert++] = p1 + N12;//2

            aLineCapTexSTs[iAATexST++] = { 0.5, 0.0 };
            aLineCapTexSTs[iAATexST++] = { 0.5, 1.0 };

            retPtCnt = iVert;
        }
        else // Round cap
        {
            glm::dvec2 NE12 = N12 + e12;
            glm::dvec2 NW12 = N12 - e12;
            glm::dvec2 SW12 = -NE12;
            //Vec2d SE12 = -NW12 ;

            aLineCap[iVert++] = p1 + SW12;//1+
            aLineCap[iVert++] = p1 + SW12;//1
            aLineCap[iVert++] = p1 + NW12;//2
            aLineCap[iVert++] = p1 + S12;//3
            aLineCap[iVert++] = p1 + N12;//4


            aLineCapTexSTs[iAATexST++] = { 0.0, 0.0 };//1+
            aLineCapTexSTs[iAATexST++] = { 0.0, 0.0 };//1
            aLineCapTexSTs[iAATexST++] = { 0.0, 1.0 };//2
            aLineCapTexSTs[iAATexST++] = { 0.5, 0.0 };//3
            aLineCapTexSTs[iAATexST++] = { 0.5, 1.0 };//4

            retPtCnt = iVert;
        }
    } while (0);

    return retPtCnt;
}



int LineSceneLayer::_calcLineTailCap(glm::dvec2& aP1, // start point
    glm::dvec2& aP2, // end   point
    glm::dvec2* aP3,
    double  aLineR,//line radius ,namely line Thickness
    glm::vec2  aLineCap[10],
    glm::vec2  aLineCapTexSTs[10]
)
{
    int retPtCnt = 0;
    do
    {

        glm::dvec2 e12 = aP2 - aP1;
        double segLen = glm::length(e12);
        if (segLen < 0.000001) {//if (p2 == p1)
            retPtCnt = 0;
            break;
        }
        e12 = glm::normalize(e12);


        int        iVert = 0;
        int     iAATexST = 0;
        //-- Does the line tail have a round cap or a flat cap?
        bool  isRoundCap = true;
        glm::dvec2 e23;

        if (!aP3) {
            isRoundCap = false;
        }
        else
        {
            glm::dvec2& p3 = (*aP3);
            e23 = p3 - aP2;// 2 --> 3
            e23 = glm::normalize(e23);

            glm::dvec2          e21 = -e12;
            float  cosAngle123 = glm::dot(e23, e21);
            if (cosAngle123 < FLAT_CAP_MAXCOS)// cosAngle123 (- (110,180]
            {
                isRoundCap = false;
            }
        }

        //-- calculate the shape of the cap...
        e12 = e12 * ((double)aLineR);
        glm::dvec2 N12(-e12.y, e12.x);
        glm::dvec2 S12 = -N12;

        if (!isRoundCap)//  FlatCap
        {

            aLineCap[iVert++] = aP2 + S12;//1
            aLineCap[iVert++] = aP2 + N12;//2

            aLineCapTexSTs[iAATexST++] = { 0.5, 0.0 };
            aLineCapTexSTs[iAATexST++] = { 0.5, 1.0 };

            if (aP3)
            {
                // seal the seam
                e23 = e23 * ((double)aLineR);
                glm::dvec2  N23(-e23.y, e23.x);
                glm::dvec2  S23 = -N23;

                aLineCap[iVert++] = aP2 + S23;// 1
                aLineCap[iVert++] = aP2 + N23;// 2
                aLineCap[iVert++] = aP2 + N23;// 2+

                aLineCapTexSTs[iAATexST++] = { 0.5, 0.0 };
                aLineCapTexSTs[iAATexST++] = { 0.5, 1.0 };
                aLineCapTexSTs[iAATexST++] = { 0.0, 0.0 };
            }
            else {
                // isolate this gap from next gap.
                aLineCap[iVert++] = aP2 + N12;//2+
                aLineCapTexSTs[iAATexST++] = { 0.0, 0.0 };
            }

            retPtCnt = iVert;
        }
        else //  round cap
        {
            glm::dvec2 NE12 = N12 + e12;
            glm::dvec2 NW12 = N12 - e12;
            //Vec2d SW12 = -NE12;
            glm::dvec2 SE12 = -NW12;

            aLineCap[iVert++] = aP2 + S12;// p2+S12;//,5
            aLineCap[iVert++] = aP2 + N12;// p2+N12;//,6
            aLineCap[iVert++] = aP2 + SE12;// p2+SE12;//,7
            aLineCap[iVert++] = aP2 + NE12;// p2+NE12;//,8
            aLineCap[iVert++] = aP2 + NE12;// p2+NE12;//,8+

            aLineCapTexSTs[iAATexST++] = { 0.5, 0.0 };//5
            aLineCapTexSTs[iAATexST++] = { 0.5, 1.0 };//6
            aLineCapTexSTs[iAATexST++] = { 1.0, 0.0 };//7
            aLineCapTexSTs[iAATexST++] = { 1.0, 1.0 };//8
            aLineCapTexSTs[iAATexST++] = { 0.0, 0.0 };//8+

            retPtCnt = iVert;
        }

    } while (0);

    return retPtCnt;
}




// 计算 线拐点连接
int LineSceneLayer::_calcLineJoin(glm::dvec2& p0,// previous point
    glm::dvec2& p1, // current point
    glm::dvec2& p2, // next point
    double  aLineR,//line radius ,namely line Thickness
    glm::vec2  aLineJoin[10],
    glm::vec2  aLineJoinTexSTs[10]
)
{
    int retPtCnt = 0;
    do
    {
        glm::dvec2 e01 = p1 - p0;
        glm::dvec2 e12 = p2 - p1;
        // skip if zero length
        double seg01Len = glm::length(e01);
        double seg12Len = glm::length(e12);
        if (seg01Len < 0.000001 || seg12Len < 0.000001)
        {
            retPtCnt = 0;
            //break;
        }

        e01 = glm::normalize(e01);
        e12 = glm::normalize(e12);

        glm::dvec2 N01(-e01.y, e01.x);
        glm::dvec2 N12(-e12.y, e12.x);

        // 3) find the tangent vector at both the end points:
        //        -if there are no segments before or after this one, use the line itself
        //        -otherwise, add the two normalized lines and average them by normalizing again
        glm::dvec2 tangent1 = e01 + e12;
        tangent1 = glm::normalize(tangent1);

        // 4) find the miter line, which is the normal of the tangent
        glm::dvec2 miter1(-tangent1.y, tangent1.x);

        // find length of miter by projecting the miter onto the n1,
        // take the length of the projection, invert it and multiply it by the thickness:
        //        length = thickness * ( 1 / |normal|.|miter| )
        double notCos = 1.0 / glm::dot(N12, miter1);
        double len1 = aLineR * notCos;//len1 = aLineR / n12.Dot(miter1);


        double miterLimit1 = fabs(MITER_JOIN_MAX_MULTIPLE * aLineR);
        double miterLimit2 = fabs(2.0  * aLineR);
        if (fabs(len1) < miterLimit1)//
        {// LineJoinMiter：尖角
            aLineJoin[0] = (p1 - miter1 * len1);
            aLineJoin[1] = (p1 + miter1 * len1);

            //  aLineJoinTexSTs[0]={0.5*notCos, 0.0};
            //  aLineJoinTexSTs[1]={0.5*notCos, 1.0*notCos};
            aLineJoinTexSTs[0] = { 0.5, 0.0 };
            aLineJoinTexSTs[1] = { 0.5, 1.0 };

            //  aLineJoinTexSTs[ 0]={0.875+0.0625,0.5};//(0.5, 0.0);
            //  aLineJoinTexSTs[ 1]={0.0625,      0.5};//(0.5, 1.0);

            retPtCnt = 2;/* 2个点 */
        }
        //            else
        //                if(len1 < miterLimit2)
        //            {
        //                // LineJoinBevel：缺角 ,bevel join 斜切连接
        //                N01 *= aLineR;
        //                e01 *= aLineR;
        //                Vec2f  S01 = -N01;
        //
        //
        //                N12 *= aLineR;
        //                e12 *= aLineR;
        //                glm::dvec2  S12 = -N12;
        //
        //
        //                aLineJoin[0] =p1 + S01 ;
        //                aLineJoin[1] =p1 + N01 ;
        //                aLineJoin[2] =p1 + S12 ;
        //                aLineJoin[3] =p1 + N12 ;
        //
        //                aLineJoinTexSTs[0]={ 0.5, 0.0};
        //                aLineJoinTexSTs[1]={0.5, 1.0};
        //                aLineJoinTexSTs[2]={0.5, 0.0};
        //                aLineJoinTexSTs[3]={0.5, 1.0};
        //
        //                retPtCnt=4;/* 4个点 */
        //            }
        else
        {
            // LineJoinRound：圆角
            N01 *= aLineR;
            e01 *= aLineR;
            glm::dvec2  S01 = -N01;
            glm::dvec2 NE01 = N01 + e01;
            glm::dvec2 NW01 = N01 - e01;
            ////  Vec2f SW01 = -NE01;//not used
            glm::dvec2 SE01 = -NW01;


            N12 *= aLineR;
            e12 *= aLineR;
            glm::dvec2  S12 = -N12;
            glm::dvec2 NE12 = N12 + e12;
            glm::dvec2 NW12 = N12 - e12;
            glm::dvec2 SW12 = -NE12;
            ////  glm::dvec2 SE12 = -NW12;//not used


            int iVert = 0;
            int iTexST = 0;
            iVert = 0;
            aLineJoin[iVert++] = p1 + S01;// p2+S01,5
            aLineJoin[iVert++] = p1 + N01;// p2+N01,6
            aLineJoin[iVert++] = p1 + SE01;// p2+SE01,7
            aLineJoin[iVert++] = p1 + NE01;// p2+NE01,8
            aLineJoin[iVert++] = p1 + NE01;// p2+NE01,8+
            aLineJoin[iVert++] = p1 + SW12;// p1+SW12,1+
            aLineJoin[iVert++] = p1 + SW12;// p1+SW12,1
            aLineJoin[iVert++] = p1 + NW12;// p1+NW12,2
            aLineJoin[iVert++] = p1 + S12;// p1+S12,3
            aLineJoin[iVert++] = p1 + N12;// p1+N12,4


            iTexST = 0;
            aLineJoinTexSTs[iTexST++] = { 0.5, 0.0 };//5
            aLineJoinTexSTs[iTexST++] = { 0.5, 1.0 };//6
            aLineJoinTexSTs[iTexST++] = { 1.0, 0.0 };//7
            aLineJoinTexSTs[iTexST++] = { 1.0, 1.0 };//8
            aLineJoinTexSTs[iTexST++] = { 0.0, 0.0 };//8+
            aLineJoinTexSTs[iTexST++] = { 0.0, 0.0 };//1+
            aLineJoinTexSTs[iTexST++] = { 0.0, 0.0 };//1
            aLineJoinTexSTs[iTexST++] = { 0.0, 1.0 };//2
            aLineJoinTexSTs[iTexST++] = { 0.5, 0.0 };//3
            aLineJoinTexSTs[iTexST++] = { 0.5, 1.0 };//4


            retPtCnt = 10;/* 10个点 */
        }

    } while (0);


    return retPtCnt;
}


void LineSceneLayer::_extrudeAAPath(std::vector<glm::vec2>&  aPath,
    int      aDrawBegin,
    int      aDrawEnd,
    float    aLineWidth,
    std::vector<glm::vec2>& aTriVertices,
    std::vector<glm::vec2>& aTriAATexSTs
)
{
    int         ptCnt = (int)aPath.size();
    int      gapBegin = 0;    // if (aPathGapNo <0)
    int      gapEnd = ptCnt - 1;// if (aPathGapNo <0)
    // (1)另外，在_extrudeAAPath 前，要确保aPath中没有连续的
    // 重复点。
    // (2)在extrude每一gap时 重复扩展后的首末点以确保该gap的extrude后相对独立于前后的
    //gap，这样可使不连续的前后两个gap之间的部分被正确地画出来。
    if ((aDrawBegin < 0) || (aDrawBegin >= ptCnt) ||
        (aDrawEnd < 0) || (aDrawEnd >= ptCnt)
        )
    {
        return;
    }
    gapBegin = aDrawBegin;
    gapEnd = aDrawEnd;


    for (int i = gapBegin; i < gapEnd; ++i)
    {
        glm::dvec2  p1 = aPath[i];
        glm::dvec2  p2 = aPath[i + 1];

        //----construct thick line
        // 底色 or 路况信息红黄绿
        if (i == gapBegin) {// i指向头点
            glm::dvec2  p0V;
            glm::dvec2* p0 = NULL;
            if (i > 0)
            {
                p0V = aPath[i - 1];
                p0 = &p0V;
            }


            glm::vec2  lineCapPts[10];
            glm::vec2  lineCapTexSTs[10];

            int num = _calcLineHeadCap(p0, p1, p2, aLineWidth, lineCapPts, lineCapTexSTs);

            aTriVertices.insert(aTriVertices.end(), lineCapPts, lineCapPts + num);
            aTriAATexSTs.insert(aTriAATexSTs.end(), lineCapTexSTs, lineCapTexSTs + num);


        }
        else
        {
            glm::dvec2  p0 = aPath[i - 1];
            glm::vec2  lineJoinPts[10];
            glm::vec2  lineJoinTexSTs[10];

            int num = _calcLineJoin(p0, p1, p2, aLineWidth, lineJoinPts, lineJoinTexSTs);

            aTriVertices.insert(aTriVertices.end(), lineJoinPts, lineJoinPts + num);
            aTriAATexSTs.insert(aTriAATexSTs.end(), lineJoinTexSTs, lineJoinTexSTs + num);


        }

        if ((i + 1) == gapEnd)// p2是最后一个点
        {
            glm::dvec2  p3V;
            glm::dvec2* p3 = NULL;
            if ((i + 2) < aPath.size())
            {
                p3V = (aPath[i + 2]);
                p3 = &p3V;
            }

            glm::vec2  lineCapPts[10];
            glm::vec2  lineCapTexSTs[10];

            int num = _calcLineTailCap(p1, p2, p3, aLineWidth, lineCapPts, lineCapTexSTs);

            aTriVertices.insert(aTriVertices.end(), lineCapPts, lineCapPts + num);
            aTriAATexSTs.insert(aTriAATexSTs.end(), lineCapTexSTs, lineCapTexSTs + num);

        }

    }//for (int i=begin; i<end; ++i)
}


// 每个线段都是独立的圆头线段。
void LineSceneLayer::_extrudeAAPath_round(
    std::vector<glm::i16vec2>&  aPath,
    double    aLineHalfWidth,
    TriStrip2DPart& aTriStripVerts
)
{
    int      ptCnt = (int)aPath.size();

    const uint16_t  c_uint16Max = std::numeric_limits<uint16_t>::max();
    if (aTriStripVerts.size() + ptCnt * 10 > (int)c_uint16Max)
    {
        return;
    }
    //------- 
    double scaleToStyleZ = _calcScaleFromTileZToStyleZ();

    //-------
    XyUv   tmpPts[10];
    double totalLen = 0.0;
    glm::dvec2 e;
    glm::vec2 N;
    glm::vec2 S;
    glm::vec2 NE;
    glm::vec2 NW;
    glm::vec2 SW;
    glm::vec2 SE;

    for (int i = 0; i < ptCnt - 1; ++i)
    {
        glm::vec2  p1 = aPath[i];
        glm::vec2  p2 = aPath[i + 1];
        p1 *= scaleToStyleZ;
        p2 *= scaleToStyleZ;

        //---- construct thick line
        //---- ( round LineJoin ) 
        e = p2 - p1;
        double curSegLen = glm::length(e);
        totalLen += curSegLen;
        e = glm::normalize(e);
        e = e * aLineHalfWidth; 
        N = { -e.y , e.x };
        S = { -N.x, -N.y };//S = -N;
        NE = { N.x + e.x, N.y + e.y };//NE =N+e;
        NW = { N.x - e.x, N.y - e.y };//NW =N-e;
        SW = { -NE.x, -NE.y }; // SW = -NE;
        SE = { -NW.x, -NW.y }; // SE = -NW;


        int iVert = 0;
        
        tmpPts[iVert++] = { p1.x + SW.x, p1.y + SW.y,   0.0, 0.0 };// p1+SW,1+
        tmpPts[iVert++] = { p1.x + SW.x, p1.y + SW.y,   0.0, 0.0 };// p1+SW,1
        tmpPts[iVert++] = { p1.x + NW.x, p1.y + NW.y,	0.0, 1.0 };// p1+NW,2
    
        tmpPts[iVert++] = { p1.x + S.x,  p1.y + S.y,	0.5, 0.0 };// p1+S,3
        tmpPts[iVert++] = { p1.x + N.x,  p1.y + N.y,	0.5, 1.0 };// p1+N,4
        tmpPts[iVert++] = { p2.x + S.x,  p2.y + S.y,	0.5, 0.0 };// p2+S,5
        tmpPts[iVert++] = { p2.x + N.x,  p2.y + N.y,	0.5, 1.0 };// p2+N,6

        tmpPts[iVert++] = { p2.x + SE.x, p2.y + SE.y,	1.0, 0.0 };// p2+SE,7
        tmpPts[iVert++] = { p2.x + NE.x, p2.y + NE.y,	1.0, 1.0 };// p2+NE,8
        tmpPts[iVert++] = { p2.x + NE.x, p2.y + NE.y,	0.0, 0.0 };// p2+NE,8+
       

        aTriStripVerts.insert(aTriStripVerts.end(), tmpPts, tmpPts + iVert);

    }//for (int i=begin; i<end; ++i)

}

void LineSceneLayer::_extrudeAAPath_roundHead(
    std::vector<glm::i16vec2>& aPath,
    double    aLineHalfWidth,
    TriStrip2DPart& aTriStripVerts
)
{
    int      ptCnt = (int)aPath.size();

    const uint16_t  c_uint16Max = std::numeric_limits<uint16_t>::max();
    if (aTriStripVerts.size() + ptCnt * 10 > (int)c_uint16Max)
    {
        return;
    }
    //------- 
    double scaleToStyleZ = _calcScaleFromTileZToStyleZ();

    //-------
    XyUv   tmpPts[10];
    double totalLen = 0.0;
    glm::dvec2 e;
    glm::vec2 N;
    glm::vec2 S;
    glm::vec2 NE;
    glm::vec2 NW;
    glm::vec2 SW;
    glm::vec2 SE;

    for (int i = 0; i < ptCnt - 1; ++i)
    {
        glm::vec2  p1 = aPath[i];
        glm::vec2  p2 = aPath[i + 1];
        p1 *= scaleToStyleZ;
        p2 *= scaleToStyleZ;

        //---- construct thick line
        //---- ( round LineJoin ) 
        e = p2 - p1;
        double curSegLen = glm::length(e);
        totalLen += curSegLen;
        e = glm::normalize(e);
        e = e * aLineHalfWidth;
        N = { -e.y , e.x };
        S = { -N.x, -N.y };//S = -N;
        NE = { N.x + e.x, N.y + e.y };//NE =N+e;
        NW = { N.x - e.x, N.y - e.y };//NW =N-e;
        SW = { -NE.x, -NE.y }; // SW = -NE;
        SE = { -NW.x, -NW.y }; // SE = -NW;


        int iVert = 0;
        if (i == 0) {
            tmpPts[iVert++] = { p1.x + SW.x, p1.y + SW.y,   0.0, 0.0 };// p1+SW,1+
            tmpPts[iVert++] = { p1.x + SW.x, p1.y + SW.y,   0.0, 0.0 };// p1+SW,1
            tmpPts[iVert++] = { p1.x + NW.x, p1.y + NW.y,	0.0, 1.0 };// p1+NW,2
        }
        tmpPts[iVert++] = { p1.x + S.x,  p1.y + S.y,	0.5, 0.0 };// p1+S,3
        tmpPts[iVert++] = { p1.x + N.x,  p1.y + N.y,	0.5, 1.0 };// p1+N,4
        tmpPts[iVert++] = { p2.x + S.x,  p2.y + S.y,	0.5, 0.0 };// p2+S,5
        tmpPts[iVert++] = { p2.x + N.x,  p2.y + N.y,	0.5, 1.0 };// p2+N,6
        if (i == ptCnt - 2) {
            tmpPts[iVert++] = { p2.x + SE.x, p2.y + SE.y,	1.0, 0.0 };// p2+SE,7
            tmpPts[iVert++] = { p2.x + NE.x, p2.y + NE.y,	1.0, 1.0 };// p2+NE,8
            tmpPts[iVert++] = { p2.x + NE.x, p2.y + NE.y,	0.0, 0.0 };// p2+NE,8+
        }

        aTriStripVerts.insert(aTriStripVerts.end(), tmpPts, tmpPts + iVert);

    }//for (int i=begin; i<end; ++i)

}


void LineSceneLayer::_extrudeAAPath_butt(std::vector<glm::i16vec2>&  aPath,
    double         aLineHalfWidth,
    double         aDashArrLen,
    TriStrip2DPart& aTriStripVerts)
{
	int      ptCnt = (int)aPath.size();
	if (ptCnt < 2) {
		return;
	}

	const uint16_t  c_uint16Max = std::numeric_limits<uint16_t>::max();
	if (aTriStripVerts.size() + ptCnt * 4 + 2 > (int)c_uint16Max)
	{
		return;
	}

    double scaleToStyleZ = _calcScaleFromTileZToStyleZ();

	XyUv   tmpPts[10];
	double totalLen = 0.0;
	glm::dvec2 e;
	glm::vec2 N;
	glm::vec2 S;

	for (int i = 0; i < ptCnt - 1; ++i)
	{
		glm::vec2  p1 = aPath[i];
		glm::vec2  p2 = aPath[i + 1];
        p1 *= scaleToStyleZ;
        p2 *= scaleToStyleZ;
		//---- construct thick line
		//---- ( bevel LineJoin ) 
		e = p2 - p1;
        totalLen += glm::length(e); 
        float texU = (float)(totalLen / aDashArrLen);

		e = glm::normalize(e);
		e = e * aLineHalfWidth;// e=e * geoWidth
		N = { -e.y , e.x };
		S = { -N.x, -N.y };//S = -N;


		int iVert = 0;
		if (i == 0) {
			tmpPts[iVert++] = { p1.x + S.x,  p1.y + S.y, 0.0, 0.0 };// p1+S,3+
		}		
		tmpPts[iVert++] = { p1.x + S.x,  p1.y + S.y,	texU, 0.0 };// p1+S,3
		tmpPts[iVert++] = { p1.x + N.x,  p1.y + N.y,	texU, 1.0 };// p1+N,4
		tmpPts[iVert++] = { p2.x + S.x,  p2.y + S.y,	texU, 0.0 };// p2+S,5
		tmpPts[iVert++] = { p2.x + N.x,  p2.y + N.y,	texU, 1.0 };// p2+N,6
		if (i == ptCnt - 2) {
			tmpPts[iVert++] = { p2.x + N.x,  p2.y + N.y,	0.0, 0.0 };// p2+N,6+ 
		}		
         
		aTriStripVerts.insert(aTriStripVerts.end(), tmpPts, tmpPts + iVert);

	}//for (int i=begin; i<end; ++i)
}

bool LineSceneLayer::draw(const GlPainters& aPainter,
    const CPaintContext& aPC
    )
{  
    bool ret = false;   

	GlPainters& painter = const_cast<GlPainters&>(aPainter);
	auto&       cam = const_cast<TransformState&>(aPC._transState);

	auto pStyle = std::static_pointer_cast<LineStyle::TEvaluated>(m_evaledStyle);
	if (pStyle->_color.a == 0  ) {
		return ret;
	}

	if (!needDraw(aPC)) {
		return ret;
	}
    
    //----for debug,start
    //if (_id != "road-service-link-track-case")
    //{
    //    return ret;
    //}

    debugInf2Log();      
    //----for debug,end 
	Pt2D  originPt{ _originPt.x, _originPt.y };// Base coordinate     
    double scale = _calcScaleFromStyleZToDrawZ(cam.getZoom() );
    double lineWidth = pStyle->_width;

	int partCnt = (int)m_triStripParts.size();
	if (!pStyle->_dasharray.empty())
	{
		for (int i = 0; i < partCnt; i++)
		{
            TriStrip2DPart& curPart = m_triStripParts[i];
			if (curPart.size() < 4) {
				continue; // if curPart is not valid,skip it
			}

			painter.drawDashLine(aPC, pStyle->_dasharray,
                pStyle->_color, originPt, scale,
				(float*)curPart.data(), curPart.size());
		}
	}
	else {
		for (int i = 0; i < partCnt; i++)
		{
            TriStrip2DPart& curPart = m_triStripParts[i];
			if (curPart.size() < 4) {
				continue; // if curPart is not valid,skip it
			}

 
            painter.drawSolidLine(aPC, pStyle->_color, lineWidth,
                    originPt, scale,
                    (float*)curPart.data(), curPart.size());
             
           


		}
	} 
	return ret;
}
 
 
} // namespace md

// clang-format on
