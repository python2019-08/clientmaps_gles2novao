#ifdef GL_ES
precision highp float;
#else
#if !defined(lowp)
#define lowp
#endif
#if !defined(mediump)
#define mediump
#endif
#if !defined(highp)
#define highp
#endif
#endif
vec2 unpack_float(const float packedValue) {int packedIntValue=int(packedValue);int v0=packedIntValue/256;return vec2(v0,packedIntValue-v0*256);}vec2 unpack_opacity(const float packedOpacity) {int intOpacity=int(packedOpacity)/2;return vec2(float(intOpacity)/127.0,mod(packedOpacity,2.0));}vec4 decode_color(const vec2 encodedColor) {return vec4(unpack_float(encodedColor[0])/255.0,unpack_float(encodedColor[1])/255.0
);}float unpack_mix_vec2(const vec2 packedValue,const float t) {return mix(packedValue[0],packedValue[1],t);}vec4 unpack_mix_color(const vec4 packedColors,const float t) {vec4 minColor=decode_color(vec2(packedColors[0],packedColors[1]));vec4 maxColor=decode_color(vec2(packedColors[2],packedColors[3]));return mix(minColor,maxColor,t);}vec2 get_pattern_pos(const vec2 pixel_coord_upper,const vec2 pixel_coord_lower,const vec2 pattern_size,const float tile_units_to_pixels,const vec2 pos) {vec2 offset=mod(mod(mod(pixel_coord_upper,pattern_size)*256.0,pattern_size)*256.0+pixel_coord_lower,pattern_size);return (tile_units_to_pixels*pos+offset)/pattern_size;}
 #ifdef GL_ES
precision mediump float;
#else
#if !defined(lowp)
#define lowp
#endif
#if !defined(mediump)
#define mediump
#endif
#if !defined(highp)
#define highp
#endif
#endif
 attribute vec2 a_pos;uniform mat4 u_matrix;void main() {gl_Position=u_matrix*vec4(a_pos,0,1);}
 uniform vec4 u_color;uniform float u_opacity;void main() {gl_FragColor=u_color*u_opacity;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 uniform mat4 u_matrix;uniform vec2 u_pattern_size_a;uniform vec2 u_pattern_size_b;uniform vec2 u_pixel_coord_upper;uniform vec2 u_pixel_coord_lower;uniform float u_scale_a;uniform float u_scale_b;uniform float u_tile_units_to_pixels;attribute vec2 a_pos;varying vec2 v_pos_a;varying vec2 v_pos_b;void main() {gl_Position=u_matrix*vec4(a_pos,0,1);v_pos_a=get_pattern_pos(u_pixel_coord_upper,u_pixel_coord_lower,u_scale_a*u_pattern_size_a,u_tile_units_to_pixels,a_pos);v_pos_b=get_pattern_pos(u_pixel_coord_upper,u_pixel_coord_lower,u_scale_b*u_pattern_size_b,u_tile_units_to_pixels,a_pos);}
 uniform vec2 u_pattern_tl_a;uniform vec2 u_pattern_br_a;uniform vec2 u_pattern_tl_b;uniform vec2 u_pattern_br_b;uniform vec2 u_texsize;uniform float u_mix;uniform float u_opacity;uniform sampler2D u_image;varying vec2 v_pos_a;varying vec2 v_pos_b;void main() {vec2 imagecoord=mod(v_pos_a,1.0);vec2 pos=mix(u_pattern_tl_a/u_texsize,u_pattern_br_a/u_texsize,imagecoord);vec4 color1=texture2D(u_image,pos);vec2 imagecoord_b=mod(v_pos_b,1.0);vec2 pos2=mix(u_pattern_tl_b/u_texsize,u_pattern_br_b/u_texsize,imagecoord_b);vec4 color2=texture2D(u_image,pos2);gl_FragColor=mix(color1,color2,u_mix)*u_opacity;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 uniform mat4 u_matrix;uniform bool u_scale_with_map;uniform bool u_pitch_with_map;uniform vec2 u_extrude_scale;uniform lowp float u_device_pixel_ratio;uniform highp float u_camera_to_center_distance;attribute vec2 a_pos;varying vec3 v_data;
#ifndef HAS_UNIFORM_u_color
uniform lowp float u_color_t;attribute highp vec4 a_color;varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif
#ifndef HAS_UNIFORM_u_radius
uniform lowp float u_radius_t;attribute mediump vec2 a_radius;varying mediump float radius;
#else
uniform mediump float u_radius;
#endif
#ifndef HAS_UNIFORM_u_blur
uniform lowp float u_blur_t;attribute lowp vec2 a_blur;varying lowp float blur;
#else
uniform lowp float u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;attribute lowp vec2 a_opacity;varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_stroke_color
uniform lowp float u_stroke_color_t;attribute highp vec4 a_stroke_color;varying highp vec4 stroke_color;
#else
uniform highp vec4 u_stroke_color;
#endif
#ifndef HAS_UNIFORM_u_stroke_width
uniform lowp float u_stroke_width_t;attribute mediump vec2 a_stroke_width;varying mediump float stroke_width;
#else
uniform mediump float u_stroke_width;
#endif
#ifndef HAS_UNIFORM_u_stroke_opacity
uniform lowp float u_stroke_opacity_t;attribute lowp vec2 a_stroke_opacity;varying lowp float stroke_opacity;
#else
uniform lowp float u_stroke_opacity;
#endif
void main(void) {
#ifndef HAS_UNIFORM_u_color
color=unpack_mix_color(a_color,u_color_t);
#else
highp vec4 color=u_color;
#endif
#ifndef HAS_UNIFORM_u_radius
radius=unpack_mix_vec2(a_radius,u_radius_t);
#else
mediump float radius=u_radius;
#endif
#ifndef HAS_UNIFORM_u_blur
blur=unpack_mix_vec2(a_blur,u_blur_t);
#else
lowp float blur=u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
lowp float opacity=u_opacity;
#endif
#ifndef HAS_UNIFORM_u_stroke_color
stroke_color=unpack_mix_color(a_stroke_color,u_stroke_color_t);
#else
highp vec4 stroke_color=u_stroke_color;
#endif
#ifndef HAS_UNIFORM_u_stroke_width
stroke_width=unpack_mix_vec2(a_stroke_width,u_stroke_width_t);
#else
mediump float stroke_width=u_stroke_width;
#endif
#ifndef HAS_UNIFORM_u_stroke_opacity
stroke_opacity=unpack_mix_vec2(a_stroke_opacity,u_stroke_opacity_t);
#else
lowp float stroke_opacity=u_stroke_opacity;
#endif
vec2 extrude=vec2(mod(a_pos,2.0)*2.0-1.0);vec2 circle_center=floor(a_pos*0.5);if (u_pitch_with_map) {vec2 corner_position=circle_center;if (u_scale_with_map) {corner_position+=extrude*(radius+stroke_width)*u_extrude_scale;} else {vec4 projected_center=u_matrix*vec4(circle_center,0,1);corner_position+=extrude*(radius+stroke_width)*u_extrude_scale*(projected_center.w/u_camera_to_center_distance);}gl_Position=u_matrix*vec4(corner_position,0,1);} else {gl_Position=u_matrix*vec4(circle_center,0,1);if (u_scale_with_map) {gl_Position.xy+=extrude*(radius+stroke_width)*u_extrude_scale*u_camera_to_center_distance;} else {gl_Position.xy+=extrude*(radius+stroke_width)*u_extrude_scale*gl_Position.w;}}lowp float antialiasblur=1.0/u_device_pixel_ratio/(radius+stroke_width);v_data=vec3(extrude.x,extrude.y,antialiasblur);}
 varying vec3 v_data;
#ifndef HAS_UNIFORM_u_color
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif
#ifndef HAS_UNIFORM_u_radius
varying mediump float radius;
#else
uniform mediump float u_radius;
#endif
#ifndef HAS_UNIFORM_u_blur
varying lowp float blur;
#else
uniform lowp float u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_stroke_color
varying highp vec4 stroke_color;
#else
uniform highp vec4 u_stroke_color;
#endif
#ifndef HAS_UNIFORM_u_stroke_width
varying mediump float stroke_width;
#else
uniform mediump float u_stroke_width;
#endif
#ifndef HAS_UNIFORM_u_stroke_opacity
varying lowp float stroke_opacity;
#else
uniform lowp float u_stroke_opacity;
#endif
void main() {
#ifdef HAS_UNIFORM_u_color
highp vec4 color=u_color;
#endif
#ifdef HAS_UNIFORM_u_radius
mediump float radius=u_radius;
#endif
#ifdef HAS_UNIFORM_u_blur
lowp float blur=u_blur;
#endif
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
#ifdef HAS_UNIFORM_u_stroke_color
highp vec4 stroke_color=u_stroke_color;
#endif
#ifdef HAS_UNIFORM_u_stroke_width
mediump float stroke_width=u_stroke_width;
#endif
#ifdef HAS_UNIFORM_u_stroke_opacity
lowp float stroke_opacity=u_stroke_opacity;
#endif
vec2 extrude=v_data.xy;float extrude_length=length(extrude);lowp float antialiasblur=v_data.z;float antialiased_blur=-max(blur,antialiasblur);float opacity_t=smoothstep(0.0,antialiased_blur,extrude_length-1.0);float color_t=stroke_width < 0.01 ? 0.0 : smoothstep(antialiased_blur,0.0,extrude_length-radius/(radius+stroke_width));gl_FragColor=opacity_t*mix(color*opacity,stroke_color*stroke_opacity,color_t);
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 attribute vec2 a_pos;uniform mat4 u_matrix;void main() {gl_Position=u_matrix*vec4(a_pos,0,1);}
 void main() {gl_FragColor=vec4(1.0);}
 uniform mat4 u_matrix;uniform float u_extrude_scale;uniform float u_opacity;uniform float u_intensity;attribute vec2 a_pos;varying vec2 v_extrude;
#ifndef HAS_UNIFORM_u_weight
uniform lowp float u_weight_t;attribute highp vec2 a_weight;varying highp float weight;
#else
uniform highp float u_weight;
#endif
#ifndef HAS_UNIFORM_u_radius
uniform lowp float u_radius_t;attribute mediump vec2 a_radius;
#else
uniform mediump float u_radius;
#endif
const highp float ZERO=1.0/255.0/16.0;
#define GAUSS_COEF 0.3989422804014327
void main(void) {
#ifndef HAS_UNIFORM_u_weight
weight=unpack_mix_vec2(a_weight,u_weight_t);
#else
highp float weight=u_weight;
#endif
#ifndef HAS_UNIFORM_u_radius
mediump float radius=unpack_mix_vec2(a_radius,u_radius_t);
#else
mediump float radius=u_radius;
#endif
vec2 unscaled_extrude=vec2(mod(a_pos,2.0)*2.0-1.0);float S=sqrt(-2.0*log(ZERO/weight/u_intensity/GAUSS_COEF))/3.0;v_extrude=S*unscaled_extrude;vec2 extrude=v_extrude*radius*u_extrude_scale;vec4 pos=vec4(floor(a_pos*0.5)+extrude,0,1);gl_Position=u_matrix*pos;}
 uniform highp float u_intensity;varying vec2 v_extrude;
#ifndef HAS_UNIFORM_u_weight
varying highp float weight;
#else
uniform highp float u_weight;
#endif
#define GAUSS_COEF 0.3989422804014327
void main() {
#ifdef HAS_UNIFORM_u_weight
highp float weight=u_weight;
#endif
float d=-0.5*3.0*3.0*dot(v_extrude,v_extrude);float val=weight*u_intensity*GAUSS_COEF*exp(d);gl_FragColor=vec4(val,1.0,1.0,1.0);
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 uniform mat4 u_matrix;uniform vec2 u_world;attribute vec2 a_pos;varying vec2 v_pos;void main() {gl_Position=u_matrix*vec4(a_pos*u_world,0,1);v_pos.x=a_pos.x;v_pos.y=1.0-a_pos.y;}
 uniform sampler2D u_image;uniform sampler2D u_color_ramp;uniform float u_opacity;varying vec2 v_pos;void main() {float t=texture2D(u_image,v_pos).r;vec4 color=texture2D(u_color_ramp,vec2(t,0.5));gl_FragColor=color*u_opacity;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(0.0);
#endif
}
 attribute vec2 a_pos;attribute vec2 a_anchor_pos;attribute vec2 a_extrude;attribute vec2 a_placed;attribute vec2 a_shift;uniform mat4 u_matrix;uniform vec2 u_extrude_scale;uniform float u_camera_to_center_distance;varying float v_placed;varying float v_notUsed;void main() {vec4 projectedPoint=u_matrix*vec4(a_anchor_pos,0,1);highp float camera_to_anchor_distance=projectedPoint.w;highp float collision_perspective_ratio=clamp(0.5+0.5*(u_camera_to_center_distance/camera_to_anchor_distance),0.0,4.0);gl_Position=u_matrix*vec4(a_pos,0.0,1.0);gl_Position.xy+=(a_extrude+a_shift)*u_extrude_scale*gl_Position.w*collision_perspective_ratio;v_placed=a_placed.x;v_notUsed=a_placed.y;}
 varying float v_placed;varying float v_notUsed;void main() {float alpha=0.5;gl_FragColor=vec4(1.0,0.0,0.0,1.0)*alpha;if (v_placed > 0.5) {gl_FragColor=vec4(0.0,0.0,1.0,0.5)*alpha;}if (v_notUsed > 0.5) {gl_FragColor*=.1;}}
 attribute vec2 a_pos;attribute vec2 a_anchor_pos;attribute vec2 a_extrude;attribute vec2 a_placed;uniform mat4 u_matrix;uniform vec2 u_extrude_scale;uniform float u_camera_to_center_distance;varying float v_placed;varying float v_notUsed;varying float v_radius;varying vec2 v_extrude;varying vec2 v_extrude_scale;void main() {vec4 projectedPoint=u_matrix*vec4(a_anchor_pos,0,1);highp float camera_to_anchor_distance=projectedPoint.w;highp float collision_perspective_ratio=clamp(0.5+0.5*(u_camera_to_center_distance/camera_to_anchor_distance),0.0,4.0);gl_Position=u_matrix*vec4(a_pos,0.0,1.0);highp float padding_factor=1.2;gl_Position.xy+=a_extrude*u_extrude_scale*padding_factor*gl_Position.w*collision_perspective_ratio;v_placed=a_placed.x;v_notUsed=a_placed.y;v_radius=abs(a_extrude.y);v_extrude=a_extrude*padding_factor;v_extrude_scale=u_extrude_scale*u_camera_to_center_distance*collision_perspective_ratio;}
 uniform float u_overscale_factor;varying float v_placed;varying float v_notUsed;varying float v_radius;varying vec2 v_extrude;varying vec2 v_extrude_scale;void main() {float alpha=0.5;vec4 color=vec4(1.0,0.0,0.0,1.0)*alpha;if (v_placed > 0.5) {color=vec4(0.0,0.0,1.0,0.5)*alpha;}if (v_notUsed > 0.5) {color*=.2;}float extrude_scale_length=length(v_extrude_scale);float extrude_length=length(v_extrude)*extrude_scale_length;float stroke_width=15.0*extrude_scale_length/u_overscale_factor;float radius=v_radius*extrude_scale_length;float distance_to_edge=abs(extrude_length-radius);float opacity_t=smoothstep(-stroke_width,0.0,-distance_to_edge);gl_FragColor=opacity_t*color;}
 attribute vec2 a_pos;varying vec2 v_uv;uniform mat4 u_matrix;uniform float u_overlay_scale;void main() {v_uv=a_pos/8192.0;gl_Position=u_matrix*vec4(a_pos*u_overlay_scale,0,1);}
 uniform highp vec4 u_color;uniform sampler2D u_overlay;varying vec2 v_uv;void main() {vec4 overlay_color=texture2D(u_overlay,v_uv);gl_FragColor=mix(u_color,overlay_color,overlay_color.a);}
 attribute vec2 a_pos;uniform mat4 u_matrix;
#ifndef HAS_UNIFORM_u_color
uniform lowp float u_color_t;attribute highp vec4 a_color;varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif
#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;attribute lowp vec2 a_opacity;varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
void main() {
#ifndef HAS_UNIFORM_u_color
color=unpack_mix_color(a_color,u_color_t);
#else
highp vec4 color=u_color;
#endif
#ifndef HAS_UNIFORM_u_opacity
opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
lowp float opacity=u_opacity;
#endif
gl_Position=u_matrix*vec4(a_pos,0,1);}
 #ifndef HAS_UNIFORM_u_color
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif
#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
void main() {
#ifdef HAS_UNIFORM_u_color
highp vec4 color=u_color;
#endif
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
gl_FragColor=color*opacity;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 attribute vec2 a_pos;uniform mat4 u_matrix;uniform vec2 u_world;varying vec2 v_pos;
#ifndef HAS_UNIFORM_u_outline_color
uniform lowp float u_outline_color_t;attribute highp vec4 a_outline_color;varying highp vec4 outline_color;
#else
uniform highp vec4 u_outline_color;
#endif
#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;attribute lowp vec2 a_opacity;varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
void main() {
#ifndef HAS_UNIFORM_u_outline_color
outline_color=unpack_mix_color(a_outline_color,u_outline_color_t);
#else
highp vec4 outline_color=u_outline_color;
#endif
#ifndef HAS_UNIFORM_u_opacity
opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
lowp float opacity=u_opacity;
#endif
gl_Position=u_matrix*vec4(a_pos,0,1);v_pos=(gl_Position.xy/gl_Position.w+1.0)/2.0*u_world;}
 varying vec2 v_pos;
#ifndef HAS_UNIFORM_u_outline_color
varying highp vec4 outline_color;
#else
uniform highp vec4 u_outline_color;
#endif
#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
void main() {
#ifdef HAS_UNIFORM_u_outline_color
highp vec4 outline_color=u_outline_color;
#endif
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
float dist=length(v_pos-gl_FragCoord.xy);float alpha=1.0-smoothstep(0.0,1.0,dist);gl_FragColor=outline_color*(alpha*opacity);
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 uniform mat4 u_matrix;uniform vec2 u_world;uniform vec2 u_pixel_coord_upper;uniform vec2 u_pixel_coord_lower;uniform vec4 u_scale;attribute vec2 a_pos;varying vec2 v_pos_a;varying vec2 v_pos_b;varying vec2 v_pos;
#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;attribute lowp vec2 a_opacity;varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_pattern_from
uniform lowp float u_pattern_from_t;attribute lowp vec4 a_pattern_from;varying lowp vec4 pattern_from;
#else
uniform lowp vec4 u_pattern_from;
#endif
#ifndef HAS_UNIFORM_u_pattern_to
uniform lowp float u_pattern_to_t;attribute lowp vec4 a_pattern_to;varying lowp vec4 pattern_to;
#else
uniform lowp vec4 u_pattern_to;
#endif
void main() {
#ifndef HAS_UNIFORM_u_opacity
opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
lowp float opacity=u_opacity;
#endif
#ifndef HAS_UNIFORM_u_pattern_from
pattern_from=a_pattern_from;
#else
mediump vec4 pattern_from=u_pattern_from;
#endif
#ifndef HAS_UNIFORM_u_pattern_to
pattern_to=a_pattern_to;
#else
mediump vec4 pattern_to=u_pattern_to;
#endif
vec2 pattern_tl_a=pattern_from.xy;vec2 pattern_br_a=pattern_from.zw;vec2 pattern_tl_b=pattern_to.xy;vec2 pattern_br_b=pattern_to.zw;float pixelRatio=u_scale.x;float tileRatio=u_scale.y;float fromScale=u_scale.z;float toScale=u_scale.w;gl_Position=u_matrix*vec4(a_pos,0,1);vec2 display_size_a=vec2((pattern_br_a.x-pattern_tl_a.x)/pixelRatio,(pattern_br_a.y-pattern_tl_a.y)/pixelRatio);vec2 display_size_b=vec2((pattern_br_b.x-pattern_tl_b.x)/pixelRatio,(pattern_br_b.y-pattern_tl_b.y)/pixelRatio);v_pos_a=get_pattern_pos(u_pixel_coord_upper,u_pixel_coord_lower,fromScale*display_size_a,tileRatio,a_pos);v_pos_b=get_pattern_pos(u_pixel_coord_upper,u_pixel_coord_lower,toScale*display_size_b,tileRatio,a_pos);v_pos=(gl_Position.xy/gl_Position.w+1.0)/2.0*u_world;}
 uniform vec2 u_texsize;uniform sampler2D u_image;uniform float u_fade;varying vec2 v_pos_a;varying vec2 v_pos_b;varying vec2 v_pos;
#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_pattern_from
varying lowp vec4 pattern_from;
#else
uniform lowp vec4 u_pattern_from;
#endif
#ifndef HAS_UNIFORM_u_pattern_to
varying lowp vec4 pattern_to;
#else
uniform lowp vec4 u_pattern_to;
#endif
void main() {
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
#ifdef HAS_UNIFORM_u_pattern_from
mediump vec4 pattern_from=u_pattern_from;
#endif
#ifdef HAS_UNIFORM_u_pattern_to
mediump vec4 pattern_to=u_pattern_to;
#endif
vec2 pattern_tl_a=pattern_from.xy;vec2 pattern_br_a=pattern_from.zw;vec2 pattern_tl_b=pattern_to.xy;vec2 pattern_br_b=pattern_to.zw;vec2 imagecoord=mod(v_pos_a,1.0);vec2 pos=mix(pattern_tl_a/u_texsize,pattern_br_a/u_texsize,imagecoord);vec4 color1=texture2D(u_image,pos);vec2 imagecoord_b=mod(v_pos_b,1.0);vec2 pos2=mix(pattern_tl_b/u_texsize,pattern_br_b/u_texsize,imagecoord_b);vec4 color2=texture2D(u_image,pos2);float dist=length(v_pos-gl_FragCoord.xy);float alpha=1.0-smoothstep(0.0,1.0,dist);gl_FragColor=mix(color1,color2,u_fade)*alpha*opacity;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 uniform mat4 u_matrix;uniform vec2 u_pixel_coord_upper;uniform vec2 u_pixel_coord_lower;uniform vec4 u_scale;attribute vec2 a_pos;varying vec2 v_pos_a;varying vec2 v_pos_b;
#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;attribute lowp vec2 a_opacity;varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_pattern_from
uniform lowp float u_pattern_from_t;attribute lowp vec4 a_pattern_from;varying lowp vec4 pattern_from;
#else
uniform lowp vec4 u_pattern_from;
#endif
#ifndef HAS_UNIFORM_u_pattern_to
uniform lowp float u_pattern_to_t;attribute lowp vec4 a_pattern_to;varying lowp vec4 pattern_to;
#else
uniform lowp vec4 u_pattern_to;
#endif
void main() {
#ifndef HAS_UNIFORM_u_opacity
opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
lowp float opacity=u_opacity;
#endif
#ifndef HAS_UNIFORM_u_pattern_from
pattern_from=a_pattern_from;
#else
mediump vec4 pattern_from=u_pattern_from;
#endif
#ifndef HAS_UNIFORM_u_pattern_to
pattern_to=a_pattern_to;
#else
mediump vec4 pattern_to=u_pattern_to;
#endif
vec2 pattern_tl_a=pattern_from.xy;vec2 pattern_br_a=pattern_from.zw;vec2 pattern_tl_b=pattern_to.xy;vec2 pattern_br_b=pattern_to.zw;float pixelRatio=u_scale.x;float tileZoomRatio=u_scale.y;float fromScale=u_scale.z;float toScale=u_scale.w;vec2 display_size_a=vec2((pattern_br_a.x-pattern_tl_a.x)/pixelRatio,(pattern_br_a.y-pattern_tl_a.y)/pixelRatio);vec2 display_size_b=vec2((pattern_br_b.x-pattern_tl_b.x)/pixelRatio,(pattern_br_b.y-pattern_tl_b.y)/pixelRatio);gl_Position=u_matrix*vec4(a_pos,0,1);v_pos_a=get_pattern_pos(u_pixel_coord_upper,u_pixel_coord_lower,fromScale*display_size_a,tileZoomRatio,a_pos);v_pos_b=get_pattern_pos(u_pixel_coord_upper,u_pixel_coord_lower,toScale*display_size_b,tileZoomRatio,a_pos);}
 uniform vec2 u_texsize;uniform float u_fade;uniform sampler2D u_image;varying vec2 v_pos_a;varying vec2 v_pos_b;
#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_pattern_from
varying lowp vec4 pattern_from;
#else
uniform lowp vec4 u_pattern_from;
#endif
#ifndef HAS_UNIFORM_u_pattern_to
varying lowp vec4 pattern_to;
#else
uniform lowp vec4 u_pattern_to;
#endif
void main() {
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
#ifdef HAS_UNIFORM_u_pattern_from
mediump vec4 pattern_from=u_pattern_from;
#endif
#ifdef HAS_UNIFORM_u_pattern_to
mediump vec4 pattern_to=u_pattern_to;
#endif
vec2 pattern_tl_a=pattern_from.xy;vec2 pattern_br_a=pattern_from.zw;vec2 pattern_tl_b=pattern_to.xy;vec2 pattern_br_b=pattern_to.zw;vec2 imagecoord=mod(v_pos_a,1.0);vec2 pos=mix(pattern_tl_a/u_texsize,pattern_br_a/u_texsize,imagecoord);vec4 color1=texture2D(u_image,pos);vec2 imagecoord_b=mod(v_pos_b,1.0);vec2 pos2=mix(pattern_tl_b/u_texsize,pattern_br_b/u_texsize,imagecoord_b);vec4 color2=texture2D(u_image,pos2);gl_FragColor=mix(color1,color2,u_fade)*opacity;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 uniform mat4 u_matrix;uniform vec3 u_lightcolor;uniform lowp vec3 u_lightpos;uniform lowp float u_lightintensity;uniform float u_vertical_gradient;uniform lowp float u_opacity;attribute vec2 a_pos;attribute vec4 a_normal_ed;varying vec4 v_color;
#ifndef HAS_UNIFORM_u_base
uniform lowp float u_base_t;attribute highp vec2 a_base;
#else
uniform highp float u_base;
#endif
#ifndef HAS_UNIFORM_u_height
uniform lowp float u_height_t;attribute highp vec2 a_height;
#else
uniform highp float u_height;
#endif
#ifndef HAS_UNIFORM_u_color
uniform lowp float u_color_t;attribute highp vec4 a_color;
#else
uniform highp vec4 u_color;
#endif
void main() {
#ifndef HAS_UNIFORM_u_base
highp float base=unpack_mix_vec2(a_base,u_base_t);
#else
highp float base=u_base;
#endif
#ifndef HAS_UNIFORM_u_height
highp float height=unpack_mix_vec2(a_height,u_height_t);
#else
highp float height=u_height;
#endif
#ifndef HAS_UNIFORM_u_color
highp vec4 color=unpack_mix_color(a_color,u_color_t);
#else
highp vec4 color=u_color;
#endif
vec3 normal=a_normal_ed.xyz;base=max(0.0,base);height=max(0.0,height);float t=mod(normal.x,2.0);gl_Position=u_matrix*vec4(a_pos,t > 0.0 ? height : base,1);float colorvalue=color.r*0.2126+color.g*0.7152+color.b*0.0722;v_color=vec4(0.0,0.0,0.0,1.0);vec4 ambientlight=vec4(0.03,0.03,0.03,1.0);color+=ambientlight;float directional=clamp(dot(normal/16384.0,u_lightpos),0.0,1.0);directional=mix((1.0-u_lightintensity),max((1.0-colorvalue+u_lightintensity),1.0),directional);if (normal.y !=0.0) {directional*=((1.0-u_vertical_gradient)+(u_vertical_gradient*clamp((t+base)*pow(height/150.0,0.5),mix(0.7,0.98,1.0-u_lightintensity),1.0)));}v_color.r+=clamp(color.r*directional*u_lightcolor.r,mix(0.0,0.3,1.0-u_lightcolor.r),1.0);v_color.g+=clamp(color.g*directional*u_lightcolor.g,mix(0.0,0.3,1.0-u_lightcolor.g),1.0);v_color.b+=clamp(color.b*directional*u_lightcolor.b,mix(0.0,0.3,1.0-u_lightcolor.b),1.0);v_color*=u_opacity;}
 varying vec4 v_color;void main() {gl_FragColor=v_color;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 uniform mat4 u_matrix;uniform vec2 u_pixel_coord_upper;uniform vec2 u_pixel_coord_lower;uniform float u_height_factor;uniform vec4 u_scale;uniform float u_vertical_gradient;uniform lowp float u_opacity;uniform vec3 u_lightcolor;uniform lowp vec3 u_lightpos;uniform lowp float u_lightintensity;attribute vec2 a_pos;attribute vec4 a_normal_ed;varying vec2 v_pos_a;varying vec2 v_pos_b;varying vec4 v_lighting;
#ifndef HAS_UNIFORM_u_base
uniform lowp float u_base_t;attribute lowp vec2 a_base;varying lowp float base;
#else
uniform lowp float u_base;
#endif
#ifndef HAS_UNIFORM_u_height
uniform lowp float u_height_t;attribute lowp vec2 a_height;varying lowp float height;
#else
uniform lowp float u_height;
#endif
#ifndef HAS_UNIFORM_u_pattern_from
uniform lowp float u_pattern_from_t;attribute lowp vec4 a_pattern_from;varying lowp vec4 pattern_from;
#else
uniform lowp vec4 u_pattern_from;
#endif
#ifndef HAS_UNIFORM_u_pattern_to
uniform lowp float u_pattern_to_t;attribute lowp vec4 a_pattern_to;varying lowp vec4 pattern_to;
#else
uniform lowp vec4 u_pattern_to;
#endif
void main() {
#ifndef HAS_UNIFORM_u_base
base=unpack_mix_vec2(a_base,u_base_t);
#else
lowp float base=u_base;
#endif
#ifndef HAS_UNIFORM_u_height
height=unpack_mix_vec2(a_height,u_height_t);
#else
lowp float height=u_height;
#endif
#ifndef HAS_UNIFORM_u_pattern_from
pattern_from=a_pattern_from;
#else
mediump vec4 pattern_from=u_pattern_from;
#endif
#ifndef HAS_UNIFORM_u_pattern_to
pattern_to=a_pattern_to;
#else
mediump vec4 pattern_to=u_pattern_to;
#endif
vec2 pattern_tl_a=pattern_from.xy;vec2 pattern_br_a=pattern_from.zw;vec2 pattern_tl_b=pattern_to.xy;vec2 pattern_br_b=pattern_to.zw;float pixelRatio=u_scale.x;float tileRatio=u_scale.y;float fromScale=u_scale.z;float toScale=u_scale.w;vec3 normal=a_normal_ed.xyz;float edgedistance=a_normal_ed.w;vec2 display_size_a=vec2((pattern_br_a.x-pattern_tl_a.x)/pixelRatio,(pattern_br_a.y-pattern_tl_a.y)/pixelRatio);vec2 display_size_b=vec2((pattern_br_b.x-pattern_tl_b.x)/pixelRatio,(pattern_br_b.y-pattern_tl_b.y)/pixelRatio);base=max(0.0,base);height=max(0.0,height);float t=mod(normal.x,2.0);float z=t > 0.0 ? height : base;gl_Position=u_matrix*vec4(a_pos,z,1);vec2 pos=normal.x==1.0 && normal.y==0.0 && normal.z==16384.0
? a_pos
: vec2(edgedistance,z*u_height_factor);v_pos_a=get_pattern_pos(u_pixel_coord_upper,u_pixel_coord_lower,fromScale*display_size_a,tileRatio,pos);v_pos_b=get_pattern_pos(u_pixel_coord_upper,u_pixel_coord_lower,toScale*display_size_b,tileRatio,pos);v_lighting=vec4(0.0,0.0,0.0,1.0);float directional=clamp(dot(normal/16383.0,u_lightpos),0.0,1.0);directional=mix((1.0-u_lightintensity),max((0.5+u_lightintensity),1.0),directional);if (normal.y !=0.0) {directional*=((1.0-u_vertical_gradient)+(u_vertical_gradient*clamp((t+base)*pow(height/150.0,0.5),mix(0.7,0.98,1.0-u_lightintensity),1.0)));}v_lighting.rgb+=clamp(directional*u_lightcolor,mix(vec3(0.0),vec3(0.3),1.0-u_lightcolor),vec3(1.0));v_lighting*=u_opacity;}
 uniform vec2 u_texsize;uniform float u_fade;uniform sampler2D u_image;varying vec2 v_pos_a;varying vec2 v_pos_b;varying vec4 v_lighting;
#ifndef HAS_UNIFORM_u_base
varying lowp float base;
#else
uniform lowp float u_base;
#endif
#ifndef HAS_UNIFORM_u_height
varying lowp float height;
#else
uniform lowp float u_height;
#endif
#ifndef HAS_UNIFORM_u_pattern_from
varying lowp vec4 pattern_from;
#else
uniform lowp vec4 u_pattern_from;
#endif
#ifndef HAS_UNIFORM_u_pattern_to
varying lowp vec4 pattern_to;
#else
uniform lowp vec4 u_pattern_to;
#endif
void main() {
#ifdef HAS_UNIFORM_u_base
lowp float base=u_base;
#endif
#ifdef HAS_UNIFORM_u_height
lowp float height=u_height;
#endif
#ifdef HAS_UNIFORM_u_pattern_from
mediump vec4 pattern_from=u_pattern_from;
#endif
#ifdef HAS_UNIFORM_u_pattern_to
mediump vec4 pattern_to=u_pattern_to;
#endif
vec2 pattern_tl_a=pattern_from.xy;vec2 pattern_br_a=pattern_from.zw;vec2 pattern_tl_b=pattern_to.xy;vec2 pattern_br_b=pattern_to.zw;vec2 imagecoord=mod(v_pos_a,1.0);vec2 pos=mix(pattern_tl_a/u_texsize,pattern_br_a/u_texsize,imagecoord);vec4 color1=texture2D(u_image,pos);vec2 imagecoord_b=mod(v_pos_b,1.0);vec2 pos2=mix(pattern_tl_b/u_texsize,pattern_br_b/u_texsize,imagecoord_b);vec4 color2=texture2D(u_image,pos2);vec4 mixedColor=mix(color1,color2,u_fade);gl_FragColor=mixedColor*v_lighting;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 uniform mat4 u_matrix;uniform vec2 u_dimension;attribute vec2 a_pos;attribute vec2 a_texture_pos;varying vec2 v_pos;void main() {gl_Position=u_matrix*vec4(a_pos,0,1);highp vec2 epsilon=1.0/u_dimension;float scale=(u_dimension.x-2.0)/u_dimension.x;v_pos=(a_texture_pos/8192.0)*scale+epsilon;}
 #ifdef GL_ES
precision highp float;
#endif
uniform sampler2D u_image;varying vec2 v_pos;uniform vec2 u_dimension;uniform float u_zoom;uniform float u_maxzoom;uniform vec4 u_unpack;float getElevation(vec2 coord,float bias) {vec4 data=texture2D(u_image,coord)*255.0;data.a=-1.0;return dot(data,u_unpack)/4.0;}void main() {vec2 epsilon=1.0/u_dimension;float a=getElevation(v_pos+vec2(-epsilon.x,-epsilon.y),0.0);float b=getElevation(v_pos+vec2(0,-epsilon.y),0.0);float c=getElevation(v_pos+vec2(epsilon.x,-epsilon.y),0.0);float d=getElevation(v_pos+vec2(-epsilon.x,0),0.0);float e=getElevation(v_pos,0.0);float f=getElevation(v_pos+vec2(epsilon.x,0),0.0);float g=getElevation(v_pos+vec2(-epsilon.x,epsilon.y),0.0);float h=getElevation(v_pos+vec2(0,epsilon.y),0.0);float i=getElevation(v_pos+vec2(epsilon.x,epsilon.y),0.0);float exaggeration=u_zoom < 2.0 ? 0.4 : u_zoom < 4.5 ? 0.35 : 0.3;vec2 deriv=vec2((c+f+f+i)-(a+d+d+g),(g+h+h+i)-(a+b+b+c))/ pow(2.0,(u_zoom-u_maxzoom)*exaggeration+19.2562-u_zoom);gl_FragColor=clamp(vec4(deriv.x/2.0+0.5,deriv.y/2.0+0.5,1.0,1.0),0.0,1.0);
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 uniform mat4 u_matrix;attribute vec2 a_pos;attribute vec2 a_texture_pos;varying vec2 v_pos;void main() {gl_Position=u_matrix*vec4(a_pos,0,1);v_pos=a_texture_pos/8192.0;}
 uniform sampler2D u_image;varying vec2 v_pos;uniform vec2 u_latrange;uniform vec2 u_light;uniform vec4 u_shadow;uniform vec4 u_highlight;uniform vec4 u_accent;
#define PI 3.141592653589793
void main() {vec4 pixel=texture2D(u_image,v_pos);vec2 deriv=((pixel.rg*2.0)-1.0);float scaleFactor=cos(radians((u_latrange[0]-u_latrange[1])*(1.0-v_pos.y)+u_latrange[1]));float slope=atan(1.25*length(deriv)/scaleFactor);float aspect=deriv.x !=0.0 ? atan(deriv.y,-deriv.x) : PI/2.0*(deriv.y > 0.0 ? 1.0 :-1.0);float intensity=u_light.x;float azimuth=u_light.y+PI;float base=1.875-intensity*1.75;float maxValue=0.5*PI;float scaledSlope=intensity !=0.5 ? ((pow(base,slope)-1.0)/(pow(base,maxValue)-1.0))*maxValue : slope;float accent=cos(scaledSlope);vec4 accent_color=(1.0-accent)*u_accent*clamp(intensity*2.0,0.0,1.0);float shade=abs(mod((aspect+azimuth)/PI+0.5,2.0)-1.0);vec4 shade_color=mix(u_shadow,u_highlight,shade)*sin(scaledSlope)*clamp(intensity*2.0,0.0,1.0);gl_FragColor=accent_color*(1.0-shade_color.a)+shade_color;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 
#define scale 0.015873016
attribute vec2 a_pos_normal;attribute vec4 a_data;uniform mat4 u_matrix;uniform mediump float u_ratio;uniform vec2 u_units_to_pixels;uniform lowp float u_device_pixel_ratio;varying vec2 v_normal;varying vec2 v_width2;varying float v_gamma_scale;varying highp float v_linesofar;
#ifndef HAS_UNIFORM_u_color
uniform lowp float u_color_t;attribute highp vec4 a_color;varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif
#ifndef HAS_UNIFORM_u_blur
uniform lowp float u_blur_t;attribute lowp vec2 a_blur;varying lowp float blur;
#else
uniform lowp float u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;attribute lowp vec2 a_opacity;varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_gapwidth
uniform lowp float u_gapwidth_t;attribute mediump vec2 a_gapwidth;
#else
uniform mediump float u_gapwidth;
#endif
#ifndef HAS_UNIFORM_u_offset
uniform lowp float u_offset_t;attribute lowp vec2 a_offset;
#else
uniform lowp float u_offset;
#endif
#ifndef HAS_UNIFORM_u_width
uniform lowp float u_width_t;attribute mediump vec2 a_width;
#else
uniform mediump float u_width;
#endif
void main() {
#ifndef HAS_UNIFORM_u_color
color=unpack_mix_color(a_color,u_color_t);
#else
highp vec4 color=u_color;
#endif
#ifndef HAS_UNIFORM_u_blur
blur=unpack_mix_vec2(a_blur,u_blur_t);
#else
lowp float blur=u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
lowp float opacity=u_opacity;
#endif
#ifndef HAS_UNIFORM_u_gapwidth
mediump float gapwidth=unpack_mix_vec2(a_gapwidth,u_gapwidth_t);
#else
mediump float gapwidth=u_gapwidth;
#endif
#ifndef HAS_UNIFORM_u_offset
lowp float offset=unpack_mix_vec2(a_offset,u_offset_t);
#else
lowp float offset=u_offset;
#endif
#ifndef HAS_UNIFORM_u_width
mediump float width=unpack_mix_vec2(a_width,u_width_t);
#else
mediump float width=u_width;
#endif
float ANTIALIASING=1.0/u_device_pixel_ratio/2.0;vec2 a_extrude=a_data.xy-128.0;float a_direction=mod(a_data.z,4.0)-1.0;v_linesofar=(floor(a_data.z/4.0)+a_data.w*64.0)*2.0;vec2 pos=floor(a_pos_normal*0.5);mediump vec2 normal=a_pos_normal-2.0*pos;normal.y=normal.y*2.0-1.0;v_normal=normal;gapwidth=gapwidth/2.0;float halfwidth=width/2.0;offset=-1.0*offset;float inset=gapwidth+(gapwidth > 0.0 ? ANTIALIASING : 0.0);float outset=gapwidth+halfwidth*(gapwidth > 0.0 ? 2.0 : 1.0)+(halfwidth==0.0 ? 0.0 : ANTIALIASING);mediump vec2 dist=outset*a_extrude*scale;mediump float u=0.5*a_direction;mediump float t=1.0-abs(u);mediump vec2 offset2=offset*a_extrude*scale*normal.y*mat2(t,-u,u,t);vec4 projected_extrude=u_matrix*vec4(dist/u_ratio,0.0,0.0);gl_Position=u_matrix*vec4(pos+offset2/u_ratio,0.0,1.0)+projected_extrude;float extrude_length_without_perspective=length(dist);float extrude_length_with_perspective=length(projected_extrude.xy/gl_Position.w*u_units_to_pixels);v_gamma_scale=extrude_length_without_perspective/extrude_length_with_perspective;v_width2=vec2(outset,inset);}
 uniform lowp float u_device_pixel_ratio;varying vec2 v_width2;varying vec2 v_normal;varying float v_gamma_scale;
#ifndef HAS_UNIFORM_u_color
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif
#ifndef HAS_UNIFORM_u_blur
varying lowp float blur;
#else
uniform lowp float u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
void main() {
#ifdef HAS_UNIFORM_u_color
highp vec4 color=u_color;
#endif
#ifdef HAS_UNIFORM_u_blur
lowp float blur=u_blur;
#endif
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
float dist=length(v_normal)*v_width2.s;float blur2=(blur+1.0/u_device_pixel_ratio)*v_gamma_scale;float alpha=clamp(min(dist-(v_width2.t-blur2),v_width2.s-dist)/blur2,0.0,1.0);gl_FragColor=color*(alpha*opacity);
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 
#define MAX_LINE_DISTANCE 32767.0
#define scale 0.015873016
attribute vec2 a_pos_normal;attribute vec4 a_data;uniform mat4 u_matrix;uniform mediump float u_ratio;uniform lowp float u_device_pixel_ratio;uniform vec2 u_units_to_pixels;varying vec2 v_normal;varying vec2 v_width2;varying float v_gamma_scale;varying highp float v_lineprogress;
#ifndef HAS_UNIFORM_u_blur
uniform lowp float u_blur_t;attribute lowp vec2 a_blur;varying lowp float blur;
#else
uniform lowp float u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;attribute lowp vec2 a_opacity;varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_gapwidth
uniform lowp float u_gapwidth_t;attribute mediump vec2 a_gapwidth;
#else
uniform mediump float u_gapwidth;
#endif
#ifndef HAS_UNIFORM_u_offset
uniform lowp float u_offset_t;attribute lowp vec2 a_offset;
#else
uniform lowp float u_offset;
#endif
#ifndef HAS_UNIFORM_u_width
uniform lowp float u_width_t;attribute mediump vec2 a_width;
#else
uniform mediump float u_width;
#endif
void main() {
#ifndef HAS_UNIFORM_u_blur
blur=unpack_mix_vec2(a_blur,u_blur_t);
#else
lowp float blur=u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
lowp float opacity=u_opacity;
#endif
#ifndef HAS_UNIFORM_u_gapwidth
mediump float gapwidth=unpack_mix_vec2(a_gapwidth,u_gapwidth_t);
#else
mediump float gapwidth=u_gapwidth;
#endif
#ifndef HAS_UNIFORM_u_offset
lowp float offset=unpack_mix_vec2(a_offset,u_offset_t);
#else
lowp float offset=u_offset;
#endif
#ifndef HAS_UNIFORM_u_width
mediump float width=unpack_mix_vec2(a_width,u_width_t);
#else
mediump float width=u_width;
#endif
float ANTIALIASING=1.0/u_device_pixel_ratio/2.0;vec2 a_extrude=a_data.xy-128.0;float a_direction=mod(a_data.z,4.0)-1.0;v_lineprogress=(floor(a_data.z/4.0)+a_data.w*64.0)*2.0/MAX_LINE_DISTANCE;vec2 pos=floor(a_pos_normal*0.5);mediump vec2 normal=a_pos_normal-2.0*pos;normal.y=normal.y*2.0-1.0;v_normal=normal;gapwidth=gapwidth/2.0;float halfwidth=width/2.0;offset=-1.0*offset;float inset=gapwidth+(gapwidth > 0.0 ? ANTIALIASING : 0.0);float outset=gapwidth+halfwidth*(gapwidth > 0.0 ? 2.0 : 1.0)+(halfwidth==0.0 ? 0.0 : ANTIALIASING);mediump vec2 dist=outset*a_extrude*scale;mediump float u=0.5*a_direction;mediump float t=1.0-abs(u);mediump vec2 offset2=offset*a_extrude*scale*normal.y*mat2(t,-u,u,t);vec4 projected_extrude=u_matrix*vec4(dist/u_ratio,0.0,0.0);gl_Position=u_matrix*vec4(pos+offset2/u_ratio,0.0,1.0)+projected_extrude;float extrude_length_without_perspective=length(dist);float extrude_length_with_perspective=length(projected_extrude.xy/gl_Position.w*u_units_to_pixels);v_gamma_scale=extrude_length_without_perspective/extrude_length_with_perspective;v_width2=vec2(outset,inset);}
 uniform lowp float u_device_pixel_ratio;uniform sampler2D u_image;varying vec2 v_width2;varying vec2 v_normal;varying float v_gamma_scale;varying highp float v_lineprogress;
#ifndef HAS_UNIFORM_u_blur
varying lowp float blur;
#else
uniform lowp float u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
void main() {
#ifdef HAS_UNIFORM_u_blur
lowp float blur=u_blur;
#endif
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
float dist=length(v_normal)*v_width2.s;float blur2=(blur+1.0/u_device_pixel_ratio)*v_gamma_scale;float alpha=clamp(min(dist-(v_width2.t-blur2),v_width2.s-dist)/blur2,0.0,1.0);vec4 color=texture2D(u_image,vec2(v_lineprogress,0.5));gl_FragColor=color*(alpha*opacity);
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 
#define scale 0.015873016
#define LINE_DISTANCE_SCALE 2.0
attribute vec2 a_pos_normal;attribute vec4 a_data;uniform mat4 u_matrix;uniform vec2 u_units_to_pixels;uniform mediump float u_ratio;uniform lowp float u_device_pixel_ratio;varying vec2 v_normal;varying vec2 v_width2;varying float v_linesofar;varying float v_gamma_scale;
#ifndef HAS_UNIFORM_u_blur
uniform lowp float u_blur_t;attribute lowp vec2 a_blur;varying lowp float blur;
#else
uniform lowp float u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;attribute lowp vec2 a_opacity;varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_offset
uniform lowp float u_offset_t;attribute lowp vec2 a_offset;
#else
uniform lowp float u_offset;
#endif
#ifndef HAS_UNIFORM_u_gapwidth
uniform lowp float u_gapwidth_t;attribute mediump vec2 a_gapwidth;
#else
uniform mediump float u_gapwidth;
#endif
#ifndef HAS_UNIFORM_u_width
uniform lowp float u_width_t;attribute mediump vec2 a_width;
#else
uniform mediump float u_width;
#endif
#ifndef HAS_UNIFORM_u_pattern_from
uniform lowp float u_pattern_from_t;attribute lowp vec4 a_pattern_from;varying lowp vec4 pattern_from;
#else
uniform lowp vec4 u_pattern_from;
#endif
#ifndef HAS_UNIFORM_u_pattern_to
uniform lowp float u_pattern_to_t;attribute lowp vec4 a_pattern_to;varying lowp vec4 pattern_to;
#else
uniform lowp vec4 u_pattern_to;
#endif
void main() {
#ifndef HAS_UNIFORM_u_blur
blur=unpack_mix_vec2(a_blur,u_blur_t);
#else
lowp float blur=u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
lowp float opacity=u_opacity;
#endif
#ifndef HAS_UNIFORM_u_offset
lowp float offset=unpack_mix_vec2(a_offset,u_offset_t);
#else
lowp float offset=u_offset;
#endif
#ifndef HAS_UNIFORM_u_gapwidth
mediump float gapwidth=unpack_mix_vec2(a_gapwidth,u_gapwidth_t);
#else
mediump float gapwidth=u_gapwidth;
#endif
#ifndef HAS_UNIFORM_u_width
mediump float width=unpack_mix_vec2(a_width,u_width_t);
#else
mediump float width=u_width;
#endif
#ifndef HAS_UNIFORM_u_pattern_from
pattern_from=a_pattern_from;
#else
mediump vec4 pattern_from=u_pattern_from;
#endif
#ifndef HAS_UNIFORM_u_pattern_to
pattern_to=a_pattern_to;
#else
mediump vec4 pattern_to=u_pattern_to;
#endif
float ANTIALIASING=1.0/u_device_pixel_ratio/2.0;vec2 a_extrude=a_data.xy-128.0;float a_direction=mod(a_data.z,4.0)-1.0;float a_linesofar=(floor(a_data.z/4.0)+a_data.w*64.0)*LINE_DISTANCE_SCALE;vec2 pos=floor(a_pos_normal*0.5);mediump vec2 normal=a_pos_normal-2.0*pos;normal.y=normal.y*2.0-1.0;v_normal=normal;gapwidth=gapwidth/2.0;float halfwidth=width/2.0;offset=-1.0*offset;float inset=gapwidth+(gapwidth > 0.0 ? ANTIALIASING : 0.0);float outset=gapwidth+halfwidth*(gapwidth > 0.0 ? 2.0 : 1.0)+(halfwidth==0.0 ? 0.0 : ANTIALIASING);mediump vec2 dist=outset*a_extrude*scale;mediump float u=0.5*a_direction;mediump float t=1.0-abs(u);mediump vec2 offset2=offset*a_extrude*scale*normal.y*mat2(t,-u,u,t);vec4 projected_extrude=u_matrix*vec4(dist/u_ratio,0.0,0.0);gl_Position=u_matrix*vec4(pos+offset2/u_ratio,0.0,1.0)+projected_extrude;float extrude_length_without_perspective=length(dist);float extrude_length_with_perspective=length(projected_extrude.xy/gl_Position.w*u_units_to_pixels);v_gamma_scale=extrude_length_without_perspective/extrude_length_with_perspective;v_linesofar=a_linesofar;v_width2=vec2(outset,inset);}
 uniform lowp float u_device_pixel_ratio;uniform vec2 u_texsize;uniform float u_fade;uniform mediump vec4 u_scale;uniform sampler2D u_image;varying vec2 v_normal;varying vec2 v_width2;varying float v_linesofar;varying float v_gamma_scale;
#ifndef HAS_UNIFORM_u_pattern_from
varying lowp vec4 pattern_from;
#else
uniform lowp vec4 u_pattern_from;
#endif
#ifndef HAS_UNIFORM_u_pattern_to
varying lowp vec4 pattern_to;
#else
uniform lowp vec4 u_pattern_to;
#endif
#ifndef HAS_UNIFORM_u_blur
varying lowp float blur;
#else
uniform lowp float u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
void main() {
#ifdef HAS_UNIFORM_u_pattern_from
mediump vec4 pattern_from=u_pattern_from;
#endif
#ifdef HAS_UNIFORM_u_pattern_to
mediump vec4 pattern_to=u_pattern_to;
#endif
#ifdef HAS_UNIFORM_u_blur
lowp float blur=u_blur;
#endif
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
vec2 pattern_tl_a=pattern_from.xy;vec2 pattern_br_a=pattern_from.zw;vec2 pattern_tl_b=pattern_to.xy;vec2 pattern_br_b=pattern_to.zw;float pixelRatio=u_scale.x;float tileZoomRatio=u_scale.y;float fromScale=u_scale.z;float toScale=u_scale.w;vec2 display_size_a=vec2((pattern_br_a.x-pattern_tl_a.x)/pixelRatio,(pattern_br_a.y-pattern_tl_a.y)/pixelRatio);vec2 display_size_b=vec2((pattern_br_b.x-pattern_tl_b.x)/pixelRatio,(pattern_br_b.y-pattern_tl_b.y)/pixelRatio);vec2 pattern_size_a=vec2(display_size_a.x*fromScale/tileZoomRatio,display_size_a.y);vec2 pattern_size_b=vec2(display_size_b.x*toScale/tileZoomRatio,display_size_b.y);float dist=length(v_normal)*v_width2.s;float blur2=(blur+1.0/u_device_pixel_ratio)*v_gamma_scale;float alpha=clamp(min(dist-(v_width2.t-blur2),v_width2.s-dist)/blur2,0.0,1.0);float x_a=mod(v_linesofar/pattern_size_a.x,1.0);float x_b=mod(v_linesofar/pattern_size_b.x,1.0);float y_a=0.5+(v_normal.y*clamp(v_width2.s,0.0,(pattern_size_a.y+2.0)/2.0)/pattern_size_a.y);float y_b=0.5+(v_normal.y*clamp(v_width2.s,0.0,(pattern_size_b.y+2.0)/2.0)/pattern_size_b.y);vec2 pos_a=mix(pattern_tl_a/u_texsize,pattern_br_a/u_texsize,vec2(x_a,y_a));vec2 pos_b=mix(pattern_tl_b/u_texsize,pattern_br_b/u_texsize,vec2(x_b,y_b));vec4 color=mix(texture2D(u_image,pos_a),texture2D(u_image,pos_b),u_fade);gl_FragColor=color*alpha*opacity;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 
#define scale 0.015873016
#define LINE_DISTANCE_SCALE 2.0
attribute vec2 a_pos_normal;attribute vec4 a_data;uniform mat4 u_matrix;uniform mediump float u_ratio;uniform lowp float u_device_pixel_ratio;uniform vec2 u_patternscale_a;uniform float u_tex_y_a;uniform vec2 u_patternscale_b;uniform float u_tex_y_b;uniform vec2 u_units_to_pixels;varying vec2 v_normal;varying vec2 v_width2;varying vec2 v_tex_a;varying vec2 v_tex_b;varying float v_gamma_scale;
#ifndef HAS_UNIFORM_u_color
uniform lowp float u_color_t;attribute highp vec4 a_color;varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif
#ifndef HAS_UNIFORM_u_blur
uniform lowp float u_blur_t;attribute lowp vec2 a_blur;varying lowp float blur;
#else
uniform lowp float u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;attribute lowp vec2 a_opacity;varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_gapwidth
uniform lowp float u_gapwidth_t;attribute mediump vec2 a_gapwidth;
#else
uniform mediump float u_gapwidth;
#endif
#ifndef HAS_UNIFORM_u_offset
uniform lowp float u_offset_t;attribute lowp vec2 a_offset;
#else
uniform lowp float u_offset;
#endif
#ifndef HAS_UNIFORM_u_width
uniform lowp float u_width_t;attribute mediump vec2 a_width;varying mediump float width;
#else
uniform mediump float u_width;
#endif
#ifndef HAS_UNIFORM_u_floorwidth
uniform lowp float u_floorwidth_t;attribute lowp vec2 a_floorwidth;varying lowp float floorwidth;
#else
uniform lowp float u_floorwidth;
#endif
void main() {
#ifndef HAS_UNIFORM_u_color
color=unpack_mix_color(a_color,u_color_t);
#else
highp vec4 color=u_color;
#endif
#ifndef HAS_UNIFORM_u_blur
blur=unpack_mix_vec2(a_blur,u_blur_t);
#else
lowp float blur=u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
lowp float opacity=u_opacity;
#endif
#ifndef HAS_UNIFORM_u_gapwidth
mediump float gapwidth=unpack_mix_vec2(a_gapwidth,u_gapwidth_t);
#else
mediump float gapwidth=u_gapwidth;
#endif
#ifndef HAS_UNIFORM_u_offset
lowp float offset=unpack_mix_vec2(a_offset,u_offset_t);
#else
lowp float offset=u_offset;
#endif
#ifndef HAS_UNIFORM_u_width
width=unpack_mix_vec2(a_width,u_width_t);
#else
mediump float width=u_width;
#endif
#ifndef HAS_UNIFORM_u_floorwidth
floorwidth=unpack_mix_vec2(a_floorwidth,u_floorwidth_t);
#else
lowp float floorwidth=u_floorwidth;
#endif
float ANTIALIASING=1.0/u_device_pixel_ratio/2.0;vec2 a_extrude=a_data.xy-128.0;float a_direction=mod(a_data.z,4.0)-1.0;float a_linesofar=(floor(a_data.z/4.0)+a_data.w*64.0)*LINE_DISTANCE_SCALE;vec2 pos=floor(a_pos_normal*0.5);mediump vec2 normal=a_pos_normal-2.0*pos;normal.y=normal.y*2.0-1.0;v_normal=normal;gapwidth=gapwidth/2.0;float halfwidth=width/2.0;offset=-1.0*offset;float inset=gapwidth+(gapwidth > 0.0 ? ANTIALIASING : 0.0);float outset=gapwidth+halfwidth*(gapwidth > 0.0 ? 2.0 : 1.0)+(halfwidth==0.0 ? 0.0 : ANTIALIASING);mediump vec2 dist=outset*a_extrude*scale;mediump float u=0.5*a_direction;mediump float t=1.0-abs(u);mediump vec2 offset2=offset*a_extrude*scale*normal.y*mat2(t,-u,u,t);vec4 projected_extrude=u_matrix*vec4(dist/u_ratio,0.0,0.0);gl_Position=u_matrix*vec4(pos+offset2/u_ratio,0.0,1.0)+projected_extrude;float extrude_length_without_perspective=length(dist);float extrude_length_with_perspective=length(projected_extrude.xy/gl_Position.w*u_units_to_pixels);v_gamma_scale=extrude_length_without_perspective/extrude_length_with_perspective;v_tex_a=vec2(a_linesofar*u_patternscale_a.x/floorwidth,normal.y*u_patternscale_a.y+u_tex_y_a);v_tex_b=vec2(a_linesofar*u_patternscale_b.x/floorwidth,normal.y*u_patternscale_b.y+u_tex_y_b);v_width2=vec2(outset,inset);}
 uniform lowp float u_device_pixel_ratio;uniform sampler2D u_image;uniform float u_sdfgamma;uniform float u_mix;varying vec2 v_normal;varying vec2 v_width2;varying vec2 v_tex_a;varying vec2 v_tex_b;varying float v_gamma_scale;
#ifndef HAS_UNIFORM_u_color
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif
#ifndef HAS_UNIFORM_u_blur
varying lowp float blur;
#else
uniform lowp float u_blur;
#endif
#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_width
varying mediump float width;
#else
uniform mediump float u_width;
#endif
#ifndef HAS_UNIFORM_u_floorwidth
varying lowp float floorwidth;
#else
uniform lowp float u_floorwidth;
#endif
void main() {
#ifdef HAS_UNIFORM_u_color
highp vec4 color=u_color;
#endif
#ifdef HAS_UNIFORM_u_blur
lowp float blur=u_blur;
#endif
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
#ifdef HAS_UNIFORM_u_width
mediump float width=u_width;
#endif
#ifdef HAS_UNIFORM_u_floorwidth
lowp float floorwidth=u_floorwidth;
#endif
float dist=length(v_normal)*v_width2.s;float blur2=(blur+1.0/u_device_pixel_ratio)*v_gamma_scale;float alpha=clamp(min(dist-(v_width2.t-blur2),v_width2.s-dist)/blur2,0.0,1.0);float sdfdist_a=texture2D(u_image,v_tex_a).a;float sdfdist_b=texture2D(u_image,v_tex_b).a;float sdfdist=mix(sdfdist_a,sdfdist_b,u_mix);alpha*=smoothstep(0.5-u_sdfgamma/floorwidth,0.5+u_sdfgamma/floorwidth,sdfdist);gl_FragColor=color*(alpha*opacity);
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 uniform mat4 u_matrix;uniform vec2 u_tl_parent;uniform float u_scale_parent;uniform float u_buffer_scale;attribute vec2 a_pos;attribute vec2 a_texture_pos;varying vec2 v_pos0;varying vec2 v_pos1;void main() {gl_Position=u_matrix*vec4(a_pos,0,1);v_pos0=(((a_texture_pos/8192.0)-0.5)/u_buffer_scale )+0.5;v_pos1=(v_pos0*u_scale_parent)+u_tl_parent;}
 uniform float u_fade_t;uniform float u_opacity;uniform sampler2D u_image0;uniform sampler2D u_image1;varying vec2 v_pos0;varying vec2 v_pos1;uniform float u_brightness_low;uniform float u_brightness_high;uniform float u_saturation_factor;uniform float u_contrast_factor;uniform vec3 u_spin_weights;void main() {vec4 color0=texture2D(u_image0,v_pos0);vec4 color1=texture2D(u_image1,v_pos1);if (color0.a > 0.0) {color0.rgb=color0.rgb/color0.a;}if (color1.a > 0.0) {color1.rgb=color1.rgb/color1.a;}vec4 color=mix(color0,color1,u_fade_t);color.a*=u_opacity;vec3 rgb=color.rgb;rgb=vec3(dot(rgb,u_spin_weights.xyz),dot(rgb,u_spin_weights.zxy),dot(rgb,u_spin_weights.yzx));float average=(color.r+color.g+color.b)/3.0;rgb+=(average-rgb)*u_saturation_factor;rgb=(rgb-0.5)*u_contrast_factor+0.5;vec3 u_high_vec=vec3(u_brightness_low,u_brightness_low,u_brightness_low);vec3 u_low_vec=vec3(u_brightness_high,u_brightness_high,u_brightness_high);gl_FragColor=vec4(mix(u_high_vec,u_low_vec,rgb)*color.a,color.a);
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 const float PI=3.141592653589793;attribute vec4 a_pos_offset;attribute vec4 a_data;attribute vec4 a_pixeloffset;attribute vec3 a_projected_pos;attribute float a_fade_opacity;uniform bool u_is_size_zoom_constant;uniform bool u_is_size_feature_constant;uniform highp float u_size_t;uniform highp float u_size;uniform highp float u_camera_to_center_distance;uniform highp float u_pitch;uniform bool u_rotate_symbol;uniform highp float u_aspect_ratio;uniform float u_fade_change;uniform mat4 u_matrix;uniform mat4 u_label_plane_matrix;uniform mat4 u_coord_matrix;uniform bool u_is_text;uniform bool u_pitch_with_map;uniform vec2 u_texsize;varying vec2 v_tex;varying float v_fade_opacity;
#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;attribute lowp vec2 a_opacity;varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
void main() {
#ifndef HAS_UNIFORM_u_opacity
opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
lowp float opacity=u_opacity;
#endif
vec2 a_pos=a_pos_offset.xy;vec2 a_offset=a_pos_offset.zw;vec2 a_tex=a_data.xy;vec2 a_size=a_data.zw;float a_size_min=floor(a_size[0]*0.5);vec2 a_pxoffset=a_pixeloffset.xy;vec2 a_minFontScale=a_pixeloffset.zw/256.0;highp float segment_angle=-a_projected_pos[2];float size;if (!u_is_size_zoom_constant && !u_is_size_feature_constant) {size=mix(a_size_min,a_size[1],u_size_t)/128.0;} else if (u_is_size_zoom_constant && !u_is_size_feature_constant) {size=a_size_min/128.0;} else {size=u_size;}vec4 projectedPoint=u_matrix*vec4(a_pos,0,1);highp float camera_to_anchor_distance=projectedPoint.w;highp float distance_ratio=u_pitch_with_map ?
camera_to_anchor_distance/u_camera_to_center_distance :
u_camera_to_center_distance/camera_to_anchor_distance;highp float perspective_ratio=clamp(0.5+0.5*distance_ratio,0.0,4.0);size*=perspective_ratio;float fontScale=u_is_text ? size/24.0 : size;highp float symbol_rotation=0.0;if (u_rotate_symbol) {vec4 offsetProjectedPoint=u_matrix*vec4(a_pos+vec2(1,0),0,1);vec2 a=projectedPoint.xy/projectedPoint.w;vec2 b=offsetProjectedPoint.xy/offsetProjectedPoint.w;symbol_rotation=atan((b.y-a.y)/u_aspect_ratio,b.x-a.x);}highp float angle_sin=sin(segment_angle+symbol_rotation);highp float angle_cos=cos(segment_angle+symbol_rotation);mat2 rotation_matrix=mat2(angle_cos,-1.0*angle_sin,angle_sin,angle_cos);vec4 projected_pos=u_label_plane_matrix*vec4(a_projected_pos.xy,0.0,1.0);gl_Position=u_coord_matrix*vec4(projected_pos.xy/projected_pos.w+rotation_matrix*(a_offset/32.0*max(a_minFontScale,fontScale)+a_pxoffset/16.0),0.0,1.0);v_tex=a_tex/u_texsize;vec2 fade_opacity=unpack_opacity(a_fade_opacity);float fade_change=fade_opacity[1] > 0.5 ? u_fade_change :-u_fade_change;v_fade_opacity=max(0.0,min(1.0,fade_opacity[0]+fade_change));}
 uniform sampler2D u_texture;varying vec2 v_tex;varying float v_fade_opacity;
#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
void main() {
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
lowp float alpha=opacity*v_fade_opacity;gl_FragColor=texture2D(u_texture,v_tex)*alpha;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 const float PI=3.141592653589793;attribute vec4 a_pos_offset;attribute vec4 a_data;attribute vec4 a_pixeloffset;attribute vec3 a_projected_pos;attribute float a_fade_opacity;uniform bool u_is_size_zoom_constant;uniform bool u_is_size_feature_constant;uniform highp float u_size_t;uniform highp float u_size;uniform mat4 u_matrix;uniform mat4 u_label_plane_matrix;uniform mat4 u_coord_matrix;uniform bool u_is_text;uniform bool u_pitch_with_map;uniform highp float u_pitch;uniform bool u_rotate_symbol;uniform highp float u_aspect_ratio;uniform highp float u_camera_to_center_distance;uniform float u_fade_change;uniform vec2 u_texsize;varying vec2 v_data0;varying vec3 v_data1;
#ifndef HAS_UNIFORM_u_fill_color
uniform lowp float u_fill_color_t;attribute highp vec4 a_fill_color;varying highp vec4 fill_color;
#else
uniform highp vec4 u_fill_color;
#endif
#ifndef HAS_UNIFORM_u_halo_color
uniform lowp float u_halo_color_t;attribute highp vec4 a_halo_color;varying highp vec4 halo_color;
#else
uniform highp vec4 u_halo_color;
#endif
#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;attribute lowp vec2 a_opacity;varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_halo_width
uniform lowp float u_halo_width_t;attribute lowp vec2 a_halo_width;varying lowp float halo_width;
#else
uniform lowp float u_halo_width;
#endif
#ifndef HAS_UNIFORM_u_halo_blur
uniform lowp float u_halo_blur_t;attribute lowp vec2 a_halo_blur;varying lowp float halo_blur;
#else
uniform lowp float u_halo_blur;
#endif
void main() {
#ifndef HAS_UNIFORM_u_fill_color
fill_color=unpack_mix_color(a_fill_color,u_fill_color_t);
#else
highp vec4 fill_color=u_fill_color;
#endif
#ifndef HAS_UNIFORM_u_halo_color
halo_color=unpack_mix_color(a_halo_color,u_halo_color_t);
#else
highp vec4 halo_color=u_halo_color;
#endif
#ifndef HAS_UNIFORM_u_opacity
opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
lowp float opacity=u_opacity;
#endif
#ifndef HAS_UNIFORM_u_halo_width
halo_width=unpack_mix_vec2(a_halo_width,u_halo_width_t);
#else
lowp float halo_width=u_halo_width;
#endif
#ifndef HAS_UNIFORM_u_halo_blur
halo_blur=unpack_mix_vec2(a_halo_blur,u_halo_blur_t);
#else
lowp float halo_blur=u_halo_blur;
#endif
vec2 a_pos=a_pos_offset.xy;vec2 a_offset=a_pos_offset.zw;vec2 a_tex=a_data.xy;vec2 a_size=a_data.zw;float a_size_min=floor(a_size[0]*0.5);vec2 a_pxoffset=a_pixeloffset.xy;highp float segment_angle=-a_projected_pos[2];float size;if (!u_is_size_zoom_constant && !u_is_size_feature_constant) {size=mix(a_size_min,a_size[1],u_size_t)/128.0;} else if (u_is_size_zoom_constant && !u_is_size_feature_constant) {size=a_size_min/128.0;} else {size=u_size;}vec4 projectedPoint=u_matrix*vec4(a_pos,0,1);highp float camera_to_anchor_distance=projectedPoint.w;highp float distance_ratio=u_pitch_with_map ?
camera_to_anchor_distance/u_camera_to_center_distance :
u_camera_to_center_distance/camera_to_anchor_distance;highp float perspective_ratio=clamp(0.5+0.5*distance_ratio,0.0,4.0);size*=perspective_ratio;float fontScale=u_is_text ? size/24.0 : size;highp float symbol_rotation=0.0;if (u_rotate_symbol) {vec4 offsetProjectedPoint=u_matrix*vec4(a_pos+vec2(1,0),0,1);vec2 a=projectedPoint.xy/projectedPoint.w;vec2 b=offsetProjectedPoint.xy/offsetProjectedPoint.w;symbol_rotation=atan((b.y-a.y)/u_aspect_ratio,b.x-a.x);}highp float angle_sin=sin(segment_angle+symbol_rotation);highp float angle_cos=cos(segment_angle+symbol_rotation);mat2 rotation_matrix=mat2(angle_cos,-1.0*angle_sin,angle_sin,angle_cos);vec4 projected_pos=u_label_plane_matrix*vec4(a_projected_pos.xy,0.0,1.0);gl_Position=u_coord_matrix*vec4(projected_pos.xy/projected_pos.w+rotation_matrix*(a_offset/32.0*fontScale+a_pxoffset),0.0,1.0);float gamma_scale=gl_Position.w;vec2 fade_opacity=unpack_opacity(a_fade_opacity);float fade_change=fade_opacity[1] > 0.5 ? u_fade_change :-u_fade_change;float interpolated_fade_opacity=max(0.0,min(1.0,fade_opacity[0]+fade_change));v_data0=a_tex/u_texsize;v_data1=vec3(gamma_scale,size,interpolated_fade_opacity);}
 #define SDF_PX 8.0
uniform bool u_is_halo;uniform sampler2D u_texture;uniform highp float u_gamma_scale;uniform lowp float u_device_pixel_ratio;uniform bool u_is_text;varying vec2 v_data0;varying vec3 v_data1;
#ifndef HAS_UNIFORM_u_fill_color
varying highp vec4 fill_color;
#else
uniform highp vec4 u_fill_color;
#endif
#ifndef HAS_UNIFORM_u_halo_color
varying highp vec4 halo_color;
#else
uniform highp vec4 u_halo_color;
#endif
#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_halo_width
varying lowp float halo_width;
#else
uniform lowp float u_halo_width;
#endif
#ifndef HAS_UNIFORM_u_halo_blur
varying lowp float halo_blur;
#else
uniform lowp float u_halo_blur;
#endif
void main() {
#ifdef HAS_UNIFORM_u_fill_color
highp vec4 fill_color=u_fill_color;
#endif
#ifdef HAS_UNIFORM_u_halo_color
highp vec4 halo_color=u_halo_color;
#endif
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
#ifdef HAS_UNIFORM_u_halo_width
lowp float halo_width=u_halo_width;
#endif
#ifdef HAS_UNIFORM_u_halo_blur
lowp float halo_blur=u_halo_blur;
#endif
float EDGE_GAMMA=0.105/u_device_pixel_ratio;vec2 tex=v_data0.xy;float gamma_scale=v_data1.x;float size=v_data1.y;float fade_opacity=v_data1[2];float fontScale=u_is_text ? size/24.0 : size;lowp vec4 color=fill_color;highp float gamma=EDGE_GAMMA/(fontScale*u_gamma_scale);lowp float buff=(256.0-64.0)/256.0;if (u_is_halo) {color=halo_color;gamma=(halo_blur*1.19/SDF_PX+EDGE_GAMMA)/(fontScale*u_gamma_scale);buff=(6.0-halo_width/fontScale)/SDF_PX;}lowp float dist=texture2D(u_texture,tex).a;highp float gamma_scaled=gamma*gamma_scale;highp float alpha=smoothstep(buff-gamma_scaled,buff+gamma_scaled,dist);gl_FragColor=color*(alpha*opacity*fade_opacity);
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 const float PI=3.141592653589793;attribute vec4 a_pos_offset;attribute vec4 a_data;attribute vec3 a_projected_pos;attribute float a_fade_opacity;uniform bool u_is_size_zoom_constant;uniform bool u_is_size_feature_constant;uniform highp float u_size_t;uniform highp float u_size;uniform mat4 u_matrix;uniform mat4 u_label_plane_matrix;uniform mat4 u_coord_matrix;uniform bool u_is_text;uniform bool u_pitch_with_map;uniform highp float u_pitch;uniform bool u_rotate_symbol;uniform highp float u_aspect_ratio;uniform highp float u_camera_to_center_distance;uniform float u_fade_change;uniform vec2 u_texsize;uniform vec2 u_texsize_icon;varying vec4 v_data0;varying vec4 v_data1;
#ifndef HAS_UNIFORM_u_fill_color
uniform lowp float u_fill_color_t;attribute highp vec4 a_fill_color;varying highp vec4 fill_color;
#else
uniform highp vec4 u_fill_color;
#endif
#ifndef HAS_UNIFORM_u_halo_color
uniform lowp float u_halo_color_t;attribute highp vec4 a_halo_color;varying highp vec4 halo_color;
#else
uniform highp vec4 u_halo_color;
#endif
#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;attribute lowp vec2 a_opacity;varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_halo_width
uniform lowp float u_halo_width_t;attribute lowp vec2 a_halo_width;varying lowp float halo_width;
#else
uniform lowp float u_halo_width;
#endif
#ifndef HAS_UNIFORM_u_halo_blur
uniform lowp float u_halo_blur_t;attribute lowp vec2 a_halo_blur;varying lowp float halo_blur;
#else
uniform lowp float u_halo_blur;
#endif
void main() {
#ifndef HAS_UNIFORM_u_fill_color
fill_color=unpack_mix_color(a_fill_color,u_fill_color_t);
#else
highp vec4 fill_color=u_fill_color;
#endif
#ifndef HAS_UNIFORM_u_halo_color
halo_color=unpack_mix_color(a_halo_color,u_halo_color_t);
#else
highp vec4 halo_color=u_halo_color;
#endif
#ifndef HAS_UNIFORM_u_opacity
opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
lowp float opacity=u_opacity;
#endif
#ifndef HAS_UNIFORM_u_halo_width
halo_width=unpack_mix_vec2(a_halo_width,u_halo_width_t);
#else
lowp float halo_width=u_halo_width;
#endif
#ifndef HAS_UNIFORM_u_halo_blur
halo_blur=unpack_mix_vec2(a_halo_blur,u_halo_blur_t);
#else
lowp float halo_blur=u_halo_blur;
#endif
vec2 a_pos=a_pos_offset.xy;vec2 a_offset=a_pos_offset.zw;vec2 a_tex=a_data.xy;vec2 a_size=a_data.zw;float a_size_min=floor(a_size[0]*0.5);float is_sdf=a_size[0]-2.0*a_size_min;highp float segment_angle=-a_projected_pos[2];float size;if (!u_is_size_zoom_constant && !u_is_size_feature_constant) {size=mix(a_size_min,a_size[1],u_size_t)/128.0;} else if (u_is_size_zoom_constant && !u_is_size_feature_constant) {size=a_size_min/128.0;} else {size=u_size;}vec4 projectedPoint=u_matrix*vec4(a_pos,0,1);highp float camera_to_anchor_distance=projectedPoint.w;highp float distance_ratio=u_pitch_with_map ?
camera_to_anchor_distance/u_camera_to_center_distance :
u_camera_to_center_distance/camera_to_anchor_distance;highp float perspective_ratio=clamp(0.5+0.5*distance_ratio,0.0,4.0);size*=perspective_ratio;float fontScale=size/24.0;highp float symbol_rotation=0.0;if (u_rotate_symbol) {vec4 offsetProjectedPoint=u_matrix*vec4(a_pos+vec2(1,0),0,1);vec2 a=projectedPoint.xy/projectedPoint.w;vec2 b=offsetProjectedPoint.xy/offsetProjectedPoint.w;symbol_rotation=atan((b.y-a.y)/u_aspect_ratio,b.x-a.x);}highp float angle_sin=sin(segment_angle+symbol_rotation);highp float angle_cos=cos(segment_angle+symbol_rotation);mat2 rotation_matrix=mat2(angle_cos,-1.0*angle_sin,angle_sin,angle_cos);vec4 projected_pos=u_label_plane_matrix*vec4(a_projected_pos.xy,0.0,1.0);gl_Position=u_coord_matrix*vec4(projected_pos.xy/projected_pos.w+rotation_matrix*(a_offset/32.0*fontScale),0.0,1.0);float gamma_scale=gl_Position.w;vec2 fade_opacity=unpack_opacity(a_fade_opacity);float fade_change=fade_opacity[1] > 0.5 ? u_fade_change :-u_fade_change;float interpolated_fade_opacity=max(0.0,min(1.0,fade_opacity[0]+fade_change));v_data0.xy=a_tex/u_texsize;v_data0.zw=a_tex/u_texsize_icon;v_data1=vec4(gamma_scale,size,interpolated_fade_opacity,is_sdf);}
 #define SDF_PX 8.0
#define SDF 1.0
#define ICON 0.0
uniform bool u_is_halo;uniform sampler2D u_texture;uniform sampler2D u_texture_icon;uniform highp float u_gamma_scale;uniform lowp float u_device_pixel_ratio;varying vec4 v_data0;varying vec4 v_data1;
#ifndef HAS_UNIFORM_u_fill_color
varying highp vec4 fill_color;
#else
uniform highp vec4 u_fill_color;
#endif
#ifndef HAS_UNIFORM_u_halo_color
varying highp vec4 halo_color;
#else
uniform highp vec4 u_halo_color;
#endif
#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif
#ifndef HAS_UNIFORM_u_halo_width
varying lowp float halo_width;
#else
uniform lowp float u_halo_width;
#endif
#ifndef HAS_UNIFORM_u_halo_blur
varying lowp float halo_blur;
#else
uniform lowp float u_halo_blur;
#endif
void main() {
#ifdef HAS_UNIFORM_u_fill_color
highp vec4 fill_color=u_fill_color;
#endif
#ifdef HAS_UNIFORM_u_halo_color
highp vec4 halo_color=u_halo_color;
#endif
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
#ifdef HAS_UNIFORM_u_halo_width
lowp float halo_width=u_halo_width;
#endif
#ifdef HAS_UNIFORM_u_halo_blur
lowp float halo_blur=u_halo_blur;
#endif
float fade_opacity=v_data1[2];if (v_data1.w==ICON) {vec2 tex_icon=v_data0.zw;lowp float alpha=opacity*fade_opacity;gl_FragColor=texture2D(u_texture_icon,tex_icon)*alpha;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
return;}vec2 tex=v_data0.xy;float EDGE_GAMMA=0.105/u_device_pixel_ratio;float gamma_scale=v_data1.x;float size=v_data1.y;float fontScale=size/24.0;lowp vec4 color=fill_color;highp float gamma=EDGE_GAMMA/(fontScale*u_gamma_scale);lowp float buff=(256.0-64.0)/256.0;if (u_is_halo) {color=halo_color;gamma=(halo_blur*1.19/SDF_PX+EDGE_GAMMA)/(fontScale*u_gamma_scale);buff=(6.0-halo_width/fontScale)/SDF_PX;}lowp float dist=texture2D(u_texture,tex).a;highp float gamma_scaled=gamma*gamma_scale;highp float alpha=smoothstep(buff-gamma_scaled,buff+gamma_scaled,dist);gl_FragColor=color*(alpha*opacity*fade_opacity);
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}
 