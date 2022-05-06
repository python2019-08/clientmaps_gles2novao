#include <sstream>
#include <iostream>
#include <chrono>
#include <mapbox/vector_tile.hpp> 
#include <mapbox/geometry/wagyu/wagyu.hpp>

#include <md/util/logtool.hpp>
#include <md/util/math_tools.hpp>
#include <md/util/merc_prj.hpp>
#include <md/util/geom.hpp>

#include "md/md_global.h" 

#include <md/mapengine/tile/vec_tile_mvt.hpp>
 
using namespace clientmap;
 
//--------------------------------------------
// local functions,start
//--------------------------------------------
namespace {
	using EmptyGeometry = mapbox::geometry::empty;

	template <class T>
	using Point = mapbox::geometry::point<T>;

	template <class T>
	using LineString = mapbox::geometry::line_string<T>;

	template <class T>
	using Polygon = mapbox::geometry::polygon<T>;

	template <class T>
	using MultiPoint = mapbox::geometry::multi_point<T>;

	template <class T>
	using MultiLineString = mapbox::geometry::multi_line_string<T>;

	template <class T>
	using MultiPolygon = mapbox::geometry::multi_polygon<T>;

	template <class T>
	using LinearRing = mapbox::geometry::linear_ring<T>;

	template <class T>
	using Geometry = mapbox::geometry::geometry<T>;


	//------------local functions declaration----------------------------
	std::string propMap_getStrValue(const mapbox::feature::property_map& aProps,
		const std::string&  aKey);

	int64_t propMap_getIntValue(const mapbox::feature::property_map& aProps,
		const std::string&  aKey);

	std::string classifyFeature(
		const uint8_t  aGeomType,
		const std::string& aOldLayerId,
		const mapbox::feature::property_map& aProps);

	std::string classify_roadLineFeature(
		const uint8_t  aGeomType,
		const std::string& aOldLayerId,
		const mapbox::feature::property_map& aProps);


	double signedArea(const InTilePts& ring);
	LinearRing<int32_t> toWagyuPath(const InTilePts& ring);
	GeomCollection toGeometryCollection(MultiPolygon<int16_t>&& multipolygon);
	//------------local functions definition------------------------------
	std::string propMap_getStrValue( const mapbox::feature::property_map& aProps,
		const std::string&  aKey)
	{
		std::string  strVal;
		if (aProps.count(aKey) > 0)
		{
			const auto& vVar = aProps.at(aKey);
			strVal = *(vVar.getString());
		}
		return strVal;
	}

	int64_t propMap_getIntValue( const mapbox::feature::property_map& aProps,
		const std::string&  aKey)
	{
		int64_t  iVal = -1;
		if (aProps.count(aKey) > 0)
		{
			const auto& vVar = aProps.at(aKey);
			iVal = *(vVar.getInt());
		}
		return iVal;
	} 

	// assign newLayerId to the feature 
	std::string classifyFeature(const uint8_t  aGeomType,
		const std::string& aOldLayerId,
		const mapbox::feature::property_map& aProps)
	{
		//for (auto const& prop : aProps)
		//{  
		//	std::cout << "      " << prop.first << ": " 
		//            << prop.second.getString() << "\n";
		//} 

		std::string  newLayerId;
		do
		{
			/*
				"id": "water", "source": "composite",
				"source-layer": "water",
			*/
			if (aGeomType == mapbox::vector_tile::GeomType::POLYGON &&
				aOldLayerId == "water")
			{
				newLayerId = "water";
				break;
			}
			//--------------------------------------------------
			std::string strCls = propMap_getStrValue(aProps, "class");
			//--------------------------------------------------
			//"id": "landcover_grass", "source-layer" : "landcover", 
			//"filter" : ["==", "class", "grass"], 
			if (aGeomType == mapbox::vector_tile::GeomType::POLYGON &&
				aOldLayerId == "landcover")
			{				
				// snow||wood||scrub||grass||crop
				if ( strCls=="snow"|| strCls == "wood"||
					strCls == "scrub" || strCls == "grass"||
					strCls == "crop"
					) 
				{
					newLayerId = "landcover_" + strCls;
				}
				break;
			}

			//"id": "national_park", "source-layer" : "landuse_overlay",
			//	"filter" : ["==", "class", "national_park"], 
			if (aGeomType == mapbox::vector_tile::GeomType::POLYGON &&
				aOldLayerId == "landuse_overlay")
			{				
				if ("national_park" == strCls) {
					newLayerId = "national_park";
				}
				break;
			}
			//"id": "hospital", "source-layer": "landuse",
			//"filter": ["==", "class", "hospital"], 
			if (aGeomType == mapbox::vector_tile::GeomType::POLYGON &&
				aOldLayerId == "landuse")
			{ 
				if (strCls=="hospital" || strCls == "school"||
					strCls == "park" || strCls == "pitch" ||
					strCls == "cemetery" || strCls == "industrial" ||
					strCls == "sand"
					)
				{
					//class ==hospital || school || park
					//        pitch|| cemetery || industrial || sand
					newLayerId = strCls;
				}
				// There are also landuse-{grass|wood|agriculture|rock|scrub} 
				// in mapbox pbf,but we abandon them.^-^ We think they should be 
				// landcover,but labeled wrong.

				break;
			}
			/*
				"id": "waterway-river-canal", "source-layer": "waterway",
				"filter": ["in", "class", "canal", "river"],
			*/
			if (aGeomType == mapbox::vector_tile::GeomType::LINESTRING &&
				aOldLayerId == "waterway")
			{ 
				//class ==canal || river  
				if (strCls == "canal" || strCls == "river")
				{
					newLayerId = "waterway-river-canal";
				}
				else {
					newLayerId = "waterway-small";
				}
				break;
			}

			/*
				"id": "barrier_line-land-polygon",
				"source-layer": "barrier_line",
				"filter": ["all", ["==", "$type", "Polygon"],
					["==", "class", "land"] ]
			*/
			if (aGeomType == mapbox::vector_tile::GeomType::POLYGON &&
				aOldLayerId == "barrier_line")
			{				
				if (strCls == "land") {
					newLayerId = "barrier_line-land-polygon";
				}
				break;
			}
			if (aGeomType == mapbox::vector_tile::GeomType::LINESTRING &&
				aOldLayerId == "barrier_line")
			{ 
				if (strCls == "land") {
					newLayerId = "barrier_line-land-line";
				}
				break;
			}
			//--------------------------------------------------
			std::string strType = propMap_getStrValue(aProps, "type");
			//--------------------------------------------------
			/*
				"id": "aeroway-polygon",	"source-layer": "aeroway",
				"filter": ["all", ["==", "$type", "Polygon"],
					["!=", "type", "apron"]],
			*/
			if (aGeomType == mapbox::vector_tile::GeomType::POLYGON &&
				aOldLayerId == "aeroway")
			{				
				if (strType != "apron") {
					newLayerId = "aeroway-polygon";					
				}				
				break;
			}
			/*
				"source-layer": "aeroway",
			*/
			if (aGeomType == mapbox::vector_tile::GeomType::LINESTRING &&
				aOldLayerId == "aeroway")
			{
				//"id": "aeroway-runway", 	"source-layer": "aeroway", 
				//"filter": ["all", ["==", "$type", "LineString"],
				//	["==", "type", "runway"]	], 
				if (strType == "runway") {
					newLayerId = "aeroway-runway"; 
				}
				//"id": "aeroway-taxiway",  "source-layer": "aeroway", 
				//"filter": ["all", ["==", "$type", "LineString"],
				//	["==", "type", "taxiway"]	], 
				else  if (strType == "taxiway")
				{
					newLayerId = "aeroway-taxiway";					
				}
				break;
			}
			//-------------------------------------
			// [ignore]  "id": "building-line",
			//-------------------------------------
			/*
				"id": "building",	"type": "fill",  "source-layer": "building",
				"filter": ["all", ["!=", "type", "building:part"],
					["==", "underground", "false"]	],
			*/
			if (aGeomType == mapbox::vector_tile::GeomType::POLYGON &&
				aOldLayerId == "building")
			{ 
				std::string  strUnderground = propMap_getStrValue(aProps, "underground");
				if (strType != "building:part" &&  strUnderground == "false")
				{
					newLayerId = "building";
					break;
				}
			}
			/*   aProps:::
			"properties": { "class": "primary", 	"oneway": "false",
							"structure": "none",	"vt_layer": "road"	}
			----------------------
			 style.json:::  "source-layer": "road",
			*/
			if (aGeomType == mapbox::vector_tile::GeomType::LINESTRING &&
				aOldLayerId == "road")
			{
				// too many road layers,so use a function 
				newLayerId = classify_roadLineFeature(aGeomType, aOldLayerId, aProps);
				break;
			}
			//--------------------------------------------

			//--------------------------------------------

			if (aGeomType == mapbox::vector_tile::GeomType::POLYGON &&
				aOldLayerId == "road")
			{ 
				std::string  strStruct = propMap_getStrValue(aProps, "structure");
				//"id": "road-pedestrian-polygon",  "source-layer": "road", 
				//"filter": ["all", ["==", "$type", "Polygon"],
				//	["all", ["in", "class", "path", "pedestrian"],
				//			["==", "structure", "none"]	 ] ], 
				if ((strCls == "path" || strCls == "pedestrian") &&
					(strStruct == "none"))
				{
					newLayerId = "road-pedestrian-polygon";
					break;
				}
				//"id": "road-polygon", "source-layer": "road", 
				//"filter": ["all", ["==", "$type", "Polygon"],
				//	["all", ["!in", "class", "motorway", "path", "pedestrian", "trunk"],
				//		["!in", "structure", "bridge", "tunnel"] ] ], 
				if ((strCls != "motorway" && strCls != "path"
					&& strCls != "pedestrian" && strCls != "trunk") &&
					(strStruct != "bridge" && strStruct != "tunnel"))
				{
					newLayerId = "road-polygon";
					break;
				}
			}

			//--------------------------------------------------------
			if (aGeomType == mapbox::vector_tile::GeomType::LINESTRING &&
				aOldLayerId == "admin")
			{
				int64_t  iAdminLvl = propMap_getIntValue(aProps, "admin_level");
				int64_t  iMaritime = propMap_getIntValue(aProps, "maritime");
				//"id": "admin-3-4-boundaries", "type": "line", 
				//"source-layer": "admin",
				//"filter": ["all", [">=", "admin_level", 3],
				//	["==", "maritime", 0] ], 
				if (iAdminLvl >= 3 && iMaritime == 0)
				{
					newLayerId = "admin-3-4-boundaries";
					break;
				}
				//"id": "admin-2-boundaries-bg",	"type": "line", 
				//"source-layer": "admin", 
				//"filter": ["all", ["==", "admin_level", 2],
				//	                ["==", "maritime", 0] ], 
				if (iAdminLvl == 2 && iMaritime == 0)
				{
					newLayerId = "admin-2-boundaries-bg";
					break;
				}
				
				//"id": "admin-2-boundaries", "type": "line", 
				//"source-layer": "admin", 
				//"filter": ["all", ["==", "admin_level", 2],
				//	              ["==", "disputed", 0],
				//	              ["==", "maritime", 0] 	], 
				int64_t  iDisputed = propMap_getIntValue(aProps, "disputed");
				if (iAdminLvl == 2 && iDisputed == 0 && iMaritime == 0)
				{
					newLayerId = "admin-2-boundaries-bg";
					break;
				}
			}

		} while (false);
		if (newLayerId == "grass")
		{
			std::cout << newLayerId << std::endl;
		}
		return newLayerId;
	}


	std::string classify_roadLineFeature(
		const uint8_t  aGeomType,
		const std::string& aOldLayerId,
		const mapbox::feature::property_map& aProps)
	{
		assert(aGeomType == mapbox::vector_tile::GeomType::LINESTRING &&
			aOldLayerId == "road");
		
		std::string newLayerId;

		do
		{			
			std::string  strCls = propMap_getStrValue(aProps, "class");
			std::string  strStruct = propMap_getStrValue(aProps, "structure");

			//"id": "tunnel-street", "source-layer" : "road",
			//	"filter" : ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "street"],
			//	["==", "structure", "tunnel"]]],
			if (strCls == "street" && strStruct == "tunnel")
			{
				newLayerId = "tunnel-street";
				break;
			}
			//"id": "tunnel-street_limited", "source-layer" : "road",
			//	"filter" : ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "street_limited"],
			//	["==", "structure", "tunnel"]]] 
			if (strCls == "street_limited" && strStruct == "tunnel")
			{
				newLayerId = "tunnel-street_limited";
				break;
			}
			//"id": "tunnel-service-link-track", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["in", "class", "link", "service", "track"],
			//		["==", "structure", "tunnel"],
			//		["!=", "type", "trunk_link"] ]		],   
			std::string  strType = propMap_getStrValue(aProps, "type");
			if ((strCls == "link" || strCls == "service" || strCls == "track") &&
				strStruct == "tunnel" &&
				strType != "trunk_link")
			{
				newLayerId = "tunnel-service-link-track";
				break;
			}
			//"id": "tunnel-trunk",  "source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//	   ["all", ["==", "class", "trunk"],
			//		       ["==", "structure", "tunnel"] ] ], 
			if ((strCls == "trunk") && strStruct == "tunnel")
			{
				newLayerId = "tunnel-trunk";
				break;
			}
			//"id": "ferry", "source-layer" : "road",
			//"filter" : ["all", ["==", "$type", "LineString"],
			//			 ["==", "type", "ferry"]], 
			if (strType == "ferry")
			{
				newLayerId = "ferry";
				break;
			}
			//"id": "ferry_auto",  "source-layer" : "road",
			//	"filter" : ["all", ["==", "$type", "LineString"],
			//	["==", "type", "ferry_auto"] ], 
			if (strType == "ferry_auto")
			{
				newLayerId = "ferry_auto";
				break;
			}
			//"id": "road-path", "source-layer" : "road",
			//	"filter" : ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "path"],
			//	        ["!in", "structure", "bridge", "tunnel"],
			//	        ["!in", "type", "crossing", "sidewalk", "steps"]]],
			if (strCls == "path" &&
				(strStruct != "bridge" && strStruct != "tunnel") &&
				(strType != "crossing" && strType != "sidewalk" && strType != "steps")
				)
			{
				newLayerId = "road-path";
				break;
			}
			//"id": "road-steps", "source-layer" : "road",
			//	"filter" : ["all", ["==", "$type", "LineString"],
			//	       ["all", ["!in", "structure", "bridge", "tunnel"],
			//	               ["==", "type", "steps"]]],
			if ((strStruct != "bridge" && strStruct != "tunnel") &&
				strType == "steps")
			{
				newLayerId = "road-steps";
				break;
			}
			//pavement是人行道，是英国英语的词，美国英语称为sidewalk
			//crosswalk是人行横道线，是美国英语.
			//pedestrian crossing是人行横道，zebra crossing是斑马线.
			//
			//"id": "road-sidewalk",  "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["!in", "structure", "bridge", "tunnel"],
			//		["in", "type", "crossing", "sidewalk"] ] ],	 
			if ((strStruct != "bridge" && strStruct != "tunnel") &&
				(strType == "crossing" || strType == "sidewalk"))
			{
				newLayerId = "road-sidewalk";
				break;
			}
			//"id": "road-pedestrian",  "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "pedestrian"],
			//		    ["==", "structure", "none"] ] ], 
			if ((strStruct != "none") && (strCls == "pedestrian"))
			{
				newLayerId = "road-pedestrian";
				break;
			}
			//"id": "road-street", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "street"],
			//			["==", "structure", "none"] ] ], 
			if ((strStruct == "none") && (strCls == "street"))
			{
				newLayerId = "road-street";
				break;
			}
			//"id": "road-street_limited", "source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "street_limited"],
			//		    ["==", "structure", "none"]    ] ], 
			if ((strStruct == "none") && (strCls == "street_limited"))
			{
				newLayerId = "road-street_limited";
				break;
			}
			//"id": "road-service-link-track", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["in", "class", "link", "service", "track"],
			//		["!in", "structure", "bridge", "tunnel"],
			//		["!=", "type", "trunk_link"] ] ], 
			if ((strCls == "link" || strCls == "service" || strCls == "track") &&
				(strStruct != "bridge" && strStruct != "tunnel") &&
				(strType != "trunk_link"))
			{
				newLayerId = "road-service-link-track";
				break;
			}
			//"id": "road-secondary-tertiary",
			//"type": "line",  
			//"source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["in", "class", "secondary", "tertiary"],
			//		["!in", "structure", "bridge", "tunnel"] ] ], 
			if ((strCls == "secondary" || strCls == "tertiary") &&
				(strStruct != "bridge" && strStruct != "tunnel"))
			{
				newLayerId = "road-secondary-tertiary";
				break;
			}
			//"id": "road-primary", 	"source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "primary"],
			//		["!in", "structure", "bridge", "tunnel"] ] ],
			if ((strCls == "primary") &&
				(strStruct != "bridge" && strStruct != "tunnel"))
			{
				newLayerId = "road-secondary-tertiary";
				break;
			}
			//"id": "road-motorway_link", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "motorway_link"],
			//		["!in", "structure", "bridge", "tunnel"] ] ], 
			if ((strCls == "motorway_link") &&
				(strStruct != "bridge" && strStruct != "tunnel"))
			{
				newLayerId = "road-motorway_link";
				break;
			}
			//"id": "road-trunk_link", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["!in", "structure", "bridge", "tunnel"],
			//		["==", "type", "trunk_link"] ]], 
			if ((strType == "trunk_link") &&
				(strStruct != "bridge" && strStruct != "tunnel"))
			{
				newLayerId = "road-trunk_link";
				break;
			}
			//"id": "road-trunk",   "source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "trunk"],
			//		["!in", "structure", "bridge", "tunnel"] ] ], 
			if ((strCls == "trunk") &&
				(strStruct != "bridge" && strStruct != "tunnel"))
			{
				newLayerId = "road-trunk";
				break;
			}
			//"id": "road-motorway", "source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "motorway"],
			//		["!in", "structure", "bridge", "tunnel"] ] ], 
			if ((strCls == "motorway") &&
				(strStruct != "bridge" && strStruct != "tunnel"))
			{
				newLayerId = "road-motorway";
				break;
			}
			//"id": "road-construction", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "construction"],
			//		["==", "structure", "none"]	 ] ],
			if ((strCls == "construction") && (strStruct == "none"))
			{
				newLayerId = "road-construction";
				break;
			}

			// 铁路  "id": "road-rail",  "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["in", "class", "major_rail", "minor_rail"],
			//		["!in", "structure", "bridge", "tunnel"] ] ],
			if ((strCls == "major_rail" || strCls == "minor_rail") &&
				(strStruct != "bridge" && strStruct != "tunnel"))
			{
				newLayerId = "road-rail";
				break;
			}

			//"id": "bridge-path", "source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "path"],
			//		    ["==", "structure", "bridge"],
			//		    ["!=", "type", "steps"] ] ],
			if (strCls == "path"  &&
				strStruct == "bridge" &&
				strType != "steps")
			{
				newLayerId = "bridge-path";
				break;
			}
			//"id": "bridge-steps",		"type": "line", 
			//"source": "composite",		"source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "structure", "bridge"],
			//		["==", "type", "steps"]	 ] ], 
			if (strStruct == "bridge" && strType == "steps")
			{
				newLayerId = "bridge-steps";
				break;
			}

			//"id": "bridge-pedestrian",	 "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "pedestrian"],
			//		["==", "structure", "bridge"] ] ], 
			if (strStruct == "bridge" && strCls == "pedestrian")
			{
				newLayerId = "bridge-pedestrian";
				break;
			}


			//"id": "bridge-street", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "street"],
			//		["==", "structure", "bridge"]	]	], 
			if (strStruct == "bridge" && strCls == "street")
			{
				newLayerId = "bridge-street";
				break;
			}
			//"id": "bridge-street_limited", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "street_limited"],
			//		["==", "structure", "bridge"] ] ], 
			if (strStruct == "bridge" && strCls == "street_limited")
			{
				newLayerId = "bridge-street_limited";
				break;
			}
			//"id": "bridge-service-link-track", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//    ["all", ["in", "class", "link", "service", "track"],
			//            ["==", "structure", "bridge"],
			//            ["!=", "type", "trunk_link"]  ]  ],
			if ((strCls == "link" || strCls == "service" || strCls == "track") &&
				strStruct == "bridge" &&
				strType != "trunk_link")
			{
				newLayerId = "bridge-service-link-track";
				break;
			}
			//"id": "bridge-secondary-tertiary-case", "source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["in", "class", "secondary", "tertiary"],
			//		["==", "structure", "bridge"] ] ], 
			if ((strCls == "secondary" || strCls == "tertiary") &&
				strStruct == "bridge")
			{
				newLayerId = "bridge-secondary-tertiary-case";
				break;
			}
			//"id": "bridge-primary-case",
			//"ref???": "bridge-primary???", "source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "class", "primary"],
			//		["==", "structure", "bridge"]	]	],
			if ((strCls == "primary") && strStruct == "bridge")
			{
				newLayerId = "bridge-primary-case";
				break;
			}

			//-----
			//"id": "bridge-trunk_link", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//    ["all", ["!in", "layer", 2, 3, 4, 5],
			//        ["==", "structure", "bridge"],
			//        ["==", "type", "trunk_link"]   ]   ],
			int64_t  iLayer = propMap_getIntValue(aProps, "layer");
			if ((iLayer != 2 && iLayer != 3 && iLayer != 4 && iLayer != 5) &&
				strStruct == "bridge" &&
				strType == "trunk_link")
			{
				newLayerId = "bridge-trunk_link";
				break;
			}
			//"id": "bridge-motorway_link", "source-layer": "road",  
			//"filter": ["all", ["==", "$type", "LineString"],
			//    ["all", ["==", "class", "motorway_link"],
			//        ["!in", "layer", 2, 3, 4, 5],
			//        ["==", "structure", "bridge"]  ]  ],
			if ((iLayer != 2 && iLayer != 3 && iLayer != 4 && iLayer != 5) &&
				strStruct == "bridge" &&
				strCls == "motorway_link")
			{
				newLayerId = "bridge-motorway_link";
				break;
			}
			//"id": "bridge-trunk",  "source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//    ["all", ["==", "class", "trunk"],
			//        ["!in", "layer", 2, 3, 4, 5],
			//        ["==", "structure", "bridge"]  ]  ],
			if ((iLayer != 2 && iLayer != 3 && iLayer != 4 && iLayer != 5) &&
				strStruct == "bridge" &&
				strCls == "trunk")
			{
				newLayerId = "bridge-trunk";
				break;
			}
			//"id": "bridge-motorway", "source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//    ["all", ["==", "class", "motorway"],
			//        ["!in", "layer", 2, 3, 4, 5],
			//        ["==", "structure", "bridge"] ]  ],
			if ((iLayer != 2 && iLayer != 3 && iLayer != 4 && iLayer != 5) &&
				strStruct == "bridge" &&
				strCls == "motorway")
			{
				newLayerId = "bridge-motorway";
				break;
			}
			//"id": "bridge-construction", 桥梁施工 "source-layer": "road" 
			//"filter": ["all", ["==", "$type", "LineString"],
			//    ["all", ["==", "class", "construction"],
			//        ["==", "structure", "bridge"]       ]     ],
			if (strStruct == "bridge" && strCls == "construction")
			{
				newLayerId = "bridge-construction";
				break;
			}
			//"id": "bridge-secondary-tertiary",
			//"ref???":"bridge-secondary-tertiary-case???",  "source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "structure", "bridge"],
			//		["in", "type", "secondary", "tertiary"]	 ] ],
			if (strStruct == "bridge" &&
				(strType == "secondary" || strType == "tertiary"))
			{
				newLayerId = "bridge-secondary-tertiary";
				break;
			}
			//"id": "bridge-primary", "ref???": "bridge-primary-case???", 
			//"source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["==", "structure", "bridge"],
			//		["==", "type", "primary"]	]	],
			if (strStruct == "bridge" &&
				(strType == "secondary" || strType == "primary"))
			{
				newLayerId = "bridge-primary";
				break;
			}
			//"id": "bridge-rail",  "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", ["in", "class", "major_rail", "minor_rail"],
			//		["==", "structure", "bridge"]		]	], 
			if (strStruct == "bridge" &&
				(strCls == "major_rail" || strCls == "minor_rail"))
			{
				newLayerId = "bridge-rail";
				break;
			}
			//"id": "bridge-trunk_link-2", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	["all", [">=", "layer", 2],
			//		["==", "structure", "bridge"],
			//		["==", "type", "trunk_link"] ] ], 
			if (iLayer >= 2 &&
				strStruct == "bridge" &&
				(strType == "trunk_link"))
			{
				newLayerId = "bridge-trunk_link-2";
				break;
			}
			//"id": "bridge-motorway_link-2", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//    ["all", ["==", "class", "motorway_link"],
			//        [">=", "layer", 2],
			//        ["==", "structure", "bridge"]   ]   ],
			if (iLayer >= 2 &&
				strStruct == "bridge" &&
				(strCls == "motorway_link"))
			{
				newLayerId = "bridge-motorway_link-2";
				break;
			}
			//"id": "bridge-trunk-2", "source-layer": "road",
			//"filter": ["all", ["==", "$type", "LineString"],
			//    ["all", ["==", "class", "trunk"],
			//        [">=", "layer", 2],
			//        ["==", "structure", "bridge"] ] ],
			if (iLayer >= 2 && strStruct == "bridge" && (strCls == "trunk"))
			{
				newLayerId = "bridge-trunk-2";
				break;
			}
			//"id": "aerialway", "source-layer": "road", 
			//"filter": ["all", ["==", "$type", "LineString"],
			//	               ["==", "class", "aerialway"]	 ],
			if (strCls == "aerialway")
			{
				newLayerId = "aerialway";
				break;
			}
		} while (false);

		return newLayerId;
	}


	double signedArea(const InTilePts& ring)
	{
		double sum = 0;
		std::size_t len = ring.size();
		for (std::size_t i = 0, j = len - 1; i < len; j = i++)
		{
			const InTilePt& p1 = ring[i];
			const InTilePt& p2 = ring[j];
			sum += (p2.x - p1.x) * (p1.y + p2.y);
		}

		return sum;
	}

	LinearRing<int32_t> toWagyuPath(const InTilePts& ring)
	{
		LinearRing<int32_t> result;
		result.reserve(ring.size());
		for (const auto& p : ring)
		{
			result.emplace_back(p.x, p.y);
		}
		return result;
	}

	GeomCollection toGeometryCollection(MultiPolygon<int16_t>&& multipolygon)
	{
		GeomCollection result;
		for (auto& polygon : multipolygon)
		{
			for (auto& ring : polygon) {
				// ring --> tileInRing
				InTileLineRing tileInRing;
				for (auto& pt : ring) {
					tileInRing.emplace_back(pt.x, pt.y);
				}
				// append tileInRing
				result.emplace_back(std::move(tileInRing)); // ???
			}
		}
		return result;
	}

}// local functions,end

//--------------------------------------------------
// class MapboxVectTile 
//--------------------------------------------------
MapboxVectTile::MapboxVectTile( ) 
{ 
}

MapboxVectTile::~MapboxVectTile() = default;
  

ShVectTile 
MapboxVectTile::parse(const TileID& aID, 
    const std::string& aTileRawData)
{
	mapbox::vector_tile::buffer tileBuf(aTileRawData);
	if (tileBuf.layerNames().empty())
	{
		return nullptr;
	}
    //Log::Error("Decoding tile: "+ tile_path);  
	ShVectTile  retTile = std::make_shared<VectTile>(aID);

	std::ostringstream osstream;// used as string converter
	for (auto const& layerName : tileBuf.layerNames())
	{
		const mapbox::vector_tile::layer layer = tileBuf.getLayer(layerName);
		std::size_t feature_count = layer.featureCount();
		if (feature_count == 0) {
			Log::Debug("Layer '%s (empty)" ,layerName.c_str() );
			continue;
		}


		Log::Debug("LayerName=%s, Features: \n", layerName.c_str()); 
		for (std::size_t i = 0; i < feature_count; ++i)
		{
 			auto const feature = mapbox::vector_tile::feature(
                layer.getFeature(i), layer);
 

			auto const& feature_id = feature.getID();
			uint64_t ftId = 0;
			if (feature_id.is<uint64_t>())
			{
				ftId = feature_id.get<uint64_t>(); 
			}
			else {
				//osstream.clear();
				//osstream << "    id: (no id set)\n";
				//Log::Info(osstream.str());
			}

			int ftType = int(feature.getType());  
 
 
			auto props = feature.getProperties();
			std::string  newLayerId = classifyFeature((uint8_t)ftType, 
													layerName, props); 
			if (newLayerId.empty())	{
				continue;
			} 

			//-- parse geom  
			ShGeomTileFeature ftPtr = parseFeatureGeom(feature);
			if (ftPtr) 
			{
                ftPtr->_ftId = ftId;

				if (newLayerId == "building") 
				{
					ftPtr->_ftType = FeatureType::Building;

					float buildingH = propMap_getIntValue(props,"height");
					ftPtr->_buildingHeight = buildingH;
				}

				ShVecTileLayer  tileLayer = getTileLayer(retTile, 
                    newLayerId, ftPtr->_ftType);
                //assert(tileLayer->_type == ftPtr->_ftType);
				tileLayer->_features.push_back(ftPtr);
			} 
		}//for (std::size_t i = 0; i < feature_count; ++i)
	}//for (auto const& layerName : tile.layerNames())

	return retTile;
}

ShVecTileLayer 
MapboxVectTile::getTileLayer(ShVectTile  aVT,
	const std::string& aLayerId,
	FeatureType aType)
{
	if (aVT->m_Id2layer.count(aLayerId) == 0)
	{
		aVT->m_Id2layer[aLayerId] = std::make_shared<VecTileLayer>(aLayerId, aType);
	}

	ShVecTileLayer  tileLayer = aVT->m_Id2layer[aLayerId];
	return tileLayer;
}

ShGeomTileFeature 
MapboxVectTile::parseFeatureGeom(const mapbox::vector_tile::feature& aMbFt)
{
    mapbox::vector_tile::points_arrays_type geom =
        aMbFt.getGeometries<mapbox::vector_tile::points_arrays_type>(1.0);
	uint32_t extent = aMbFt.getExtent();

    ShGeomTileFeature ftPtr= nullptr;
    if (aMbFt.getType() == mapbox::vector_tile::GeomType::POINT)
    {        
        if ( (geom.size() > 0) && (geom[0].size() > 0))
        {
            ftPtr = std::make_shared<VecTileFeature>();
            ftPtr->_ftType = FeatureType::Point;

            for (auto const& point_array : geom)
            {
                for (auto const& point : point_array)
                {
                    //std::clog << "(" << point.x << "," << point.y << ")"; 
#ifdef CHANGE_MBTILE_2_OURTILE
					ftPtr->_points.emplace_back(point.x, extent - point.y);
#else

                    ftPtr->_points.emplace_back(point.x, point.y);
#endif
                }
            }
        }
    }
    else if (aMbFt.getType() == mapbox::vector_tile::GeomType::LINESTRING)
    {
        if ((geom.size() > 0) && (geom[0].size() > 0))
        {
            ftPtr = std::make_shared<VecTileFeature>();
            ftPtr->_ftType = FeatureType::LineString;

            for (auto const& point_array : geom)
            {
                InTileLineStr  line;
                for (auto const& point : point_array)
                {
#ifdef CHANGE_MBTILE_2_OURTILE
					line.emplace_back(point.x, extent - point.y);
#else
                    //std::clog << "(" << point.x << "," << point.y << ")";                    
                    line.emplace_back(point.x, point.y);
#endif
                }
                ftPtr->_lines.push_back( std::move(line) );
            }
        }
    }
    else if (aMbFt.getType() == mapbox::vector_tile::GeomType::POLYGON)
    {
        if ((geom.size() > 0) && (geom[0].size() > 0))
        {
            ftPtr = std::make_shared<VecTileFeature>();
            ftPtr->_ftType = FeatureType::Polygon;//also may be a building

            //
            GeomCollection  rings;
            for (auto const& point_array : geom)
            {
                InTileLineRing  ring;
                for (auto const& point : point_array)
                {
#ifdef CHANGE_MBTILE_2_OURTILE
					ring.emplace_back(point.x, extent - point.y);
#else
                    //std::clog << "(" << point.x << "," << point.y << ")";                    
                    ring.emplace_back(point.x, point.y);
#endif
                }
                rings.push_back( std::move(ring) );
            }
            //
            if (aMbFt.getVersion() < 2 )
            {
                rings = fixupPolygons(rings);
            }

            //
            ftPtr->_polygons = rings2polygons(rings); 
			for (auto& poly : ftPtr->_polygons)
			{
				// Optimize polygons with many interior rings for earcut tesselation.
				limitHoles(poly, 500);
			}
        }//if ((geom.size() > 0) && (geom[0].size() > 0))
    }//(aMbFt.getType() == mapbox::vector_tile::GeomType::POLYGON)
    return std::move(ftPtr);
}


GeomCollection
MapboxVectTile::fixupPolygons(const GeomCollection& rings)
{
    using namespace mapbox::geometry::wagyu;

    wagyu<int32_t> clipper;
    for (const auto& ring : rings)
    {
        clipper.add_ring( toWagyuPath(ring) );
    }

    MultiPolygon<int16_t>  multipolygon;
    clipper.execute(clip_type_union, multipolygon,
        fill_type_even_odd, fill_type_even_odd);

    return toGeometryCollection( std::move(multipolygon) );
}

std::vector<GeomCollection>
MapboxVectTile::rings2polygons(const GeomCollection& rings)
{
    std::vector<GeomCollection> polygons;

    std::size_t len = rings.size();

    if (len <= 1) {
        polygons.emplace_back( rings );
        return polygons;
    }

    GeomCollection polygon;
    int8_t ccw = 0;

    for (const auto& ring : rings)
    {
        double area = signedArea(ring);
        if (area == 0)
            continue;

        if (ccw == 0) {
            ccw = (area < 0 ? -1 : 1);
        }

        if (ccw == (area < 0 ? -1 : 1) && !polygon.empty())
        {
            polygons.emplace_back(std::move(polygon));
            polygon = GeomCollection();
        }

        polygon.emplace_back(ring);
    }

    if (!polygon.empty()) {
        polygons.emplace_back(std::move(polygon));
    }

    return polygons;
}


void MapboxVectTile::limitHoles(GeomCollection& polygon,
	uint32_t maxHoles)
{
	if (polygon.size() > 1 + maxHoles)
	{
		std::nth_element(polygon.begin() + 1,
			polygon.begin() + 1 + maxHoles,
			polygon.end(),
			[](const auto& a, const auto& b) {
			return std::fabs(signedArea(a)) > std::fabs(signedArea(b));
		});
		polygon.resize(1 + maxHoles);
	}
}

 
 
