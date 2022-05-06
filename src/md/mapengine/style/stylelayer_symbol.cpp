// clang-format off
#include <md/util/cpp_misc.hpp> 
#include "md/drawengine/drawcontext.h"

#include <md/mapengine/style/stylelayer_symbol.hpp>

namespace clientmap {
  
/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////

SymbolStyle::SymbolStyle(const std::string& aLayerId,
    const std::string& aDsLayerId)
    : StyleLayer(aLayerId, aDsLayerId)
{
    mType = StyleLayerType::symbol;
}
 
SymbolStyle::~SymbolStyle() = default;
  
int 
SymbolStyle::parse(const JSValue& value)
{
    //if (aName == "icon-color" ||
    //    aName == "icon-halo-color" ||
    //    aName == "text-color" ||
    //    aName == "text-halo-color")
    //{
    //    TStyleExpColor typedValue;
    //    int ret = StyleConverter<TStyleExpColor>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -1;
    //    }

    //    if (aName == "icon-color") {
    //        setIconColor( typedValue);
    //        return 0;
    //    }

    //    if (aName == "icon-halo-color") {
    //        setIconHaloColor( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-color") {
    //        setTextColor( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-halo-color") {
    //        setTextHaloColor( typedValue);
    //        return 0;
    //    }
    //}
    //if (aName == "icon-halo-blur" || 
    //    aName == "icon-halo-width" ||
    //    aName == "icon-opacity" || 
    //    aName == "text-halo-blur" ||
    //    aName == "text-halo-width" || 
    //    aName == "text-opacity" || 
    //    aName == "icon-rotate" ||
    //    aName == "icon-size" || 
    //    aName == "symbol-sort-key" ||
    //    aName == "text-letter-spacing" ||
    //    aName == "text-max-width" ||
    //    aName == "text-radial-offset" || 
    //    aName == "text-rotate" ||
    //    aName == "text-size")
    //{
    //    TStyleExpF typedValue;
    //    int ret = StyleConverter<TStyleExpF>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -2;
    //    }

    //    if (aName == "icon-halo-blur") {
    //        setIconHaloBlur( typedValue);
    //        return 0;
    //    }

    //    if (aName == "icon-halo-width") {
    //        setIconHaloWidth( typedValue);
    //        return 0;
    //    }

    //    if (aName == "icon-opacity") {
    //        setIconOpacity( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-halo-blur") {
    //        setTextHaloBlur( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-halo-width") {
    //        setTextHaloWidth( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-opacity") {
    //        setTextOpacity( typedValue);
    //        return 0;
    //    }

    //    if (aName == "icon-rotate") {
    //        setIconRotate( typedValue);
    //        return 0;
    //    }

    //    if (aName == "icon-size") {
    //        setIconSize( typedValue);
    //        return 0;
    //    }

    //    if (aName == "symbol-sort-key") {
    //        setSymbolSortKey( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-letter-spacing") {
    //        setTextLetterSpacing( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-max-width") {
    //        setTextMaxWidth( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-radial-offset") {
    //        setTextRadialOffset( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-rotate") {
    //        setTextRotate( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-size") {
    //        setTextSize( typedValue);
    //        return 0;
    //    }
    //}
    //if (aName == "icon-translate" || 
    //    aName == "text-translate")
    //{
    //    TStyleExp2F typedValue;
    //    int ret = StyleConverter<TStyleExp2F>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -2;
    //    }

    //    if (aName == "icon-translate") {
    //        setIconTranslate( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-translate") {
    //        setTextTranslate( typedValue);
    //        return 0;
    //    }
    //}
    //if (aName == "icon-translate-anchor" ||
    //    aName == "text-translate-anchor")
    //{
    //    TStyleStepTranslateAnchor typedValue;
    //    int ret = StyleConverter< TStyleStepTranslateAnchor>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    if (aName == "icon-translate-anchor") {
    //        setIconTranslateAnchor( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-translate-anchor") {
    //        setTextTranslateAnchor( typedValue);
    //        return 0;
    //    }
    //}
    //if (aName == "icon-allow-overlap" ||
    //    aName == "icon-ignore-placement" ||
    //    aName == "icon-keep-upright" || 
    //    aName == "icon-optional" ||
    //    aName == "symbol-avoid-edges" || 
    //    aName == "text-allow-overlap" ||
    //    aName == "text-ignore-placement" || 
    //    aName == "text-keep-upright" ||
    //    aName == "text-optional")
    //{
    //    TStyleStepB typedValue;
    //    int ret = StyleConverter<TStyleStepB>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    if (aName == "icon-allow-overlap") {
    //        setIconAllowOverlap(typedValue);
    //        return 0;
    //    }

    //    if (aName == "icon-ignore-placement") {
    //        setIconIgnorePlacement( typedValue);
    //        return 0;
    //    }

    //    if (aName == "icon-keep-upright") {
    //        setIconKeepUpright( typedValue);
    //        return 0;
    //    }

    //    if (aName == "icon-optional") {
    //        setIconOptional( typedValue);
    //        return 0;
    //    }

    //    if (aName == "symbol-avoid-edges") {
    //        setSymbolAvoidEdges( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-allow-overlap") {
    //        setTextAllowOverlap( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-ignore-placement") {
    //        setTextIgnorePlacement( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-keep-upright") {
    //        setTextKeepUpright( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-optional") {
    //        setTextOptional( typedValue);
    //        return 0;
    //    }
    //}
    //if (aName == "icon-anchor" || 
    //    aName == "text-anchor")
    //{
    //    TStyleStepSymbolAnchor typedValue;
    //    int ret = StyleConverter< TStyleStepSymbolAnchor>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    if (aName == "icon-anchor") {
    //        setIconAnchor( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-anchor") {
    //        setTextAnchor( typedValue);
    //        return 0;
    //    }
    //}
    //if (aName == "icon-image") {
    //    md::TStyleStepImg typedValue;
    //    int ret = StyleConverter<md::TStyleStepImg>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    setIconImage(typedValue);
    //    return 0;
    //}
    //if (aName == "icon-offset" ||
    //    aName == "text-offset")
    //{
    //    TStyleExp2F typedValue;
    //    int ret = StyleConverter<TStyleExp2F>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -2;
    //    }

    //    if (aName == "icon-offset") {
    //        setIconOffset( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-offset") {
    //        setTextOffset( typedValue);
    //        return 0;
    //    }
    //}
    //if (aName == "icon-padding" ||
    //    aName == "symbol-spacing" ||
    //    aName == "text-line-height" || 
    //    aName == "text-max-angle" ||
    //    aName == "text-padding")
    //{
    //    TStyleExpF typedValue;
    //    int ret = StyleConverter<TStyleExpF>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -2;
    //    }

    //    if (aName == "icon-padding") {
    //        setIconPadding( typedValue);
    //        return 0;
    //    }

    //    if (aName == "symbol-spacing") {
    //        setSymbolSpacing(typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-line-height") {
    //        setTextLineHeight( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-max-angle") {
    //        setTextMaxAngle( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-padding") {
    //        setTextPadding( typedValue);
    //        return 0;
    //    }
    //}
    //if (aName == "icon-pitch-alignment" ||
    //    aName == "icon-rotation-alignment" ||
    //    aName == "text-pitch-alignment" ||
    //    aName == "text-rotation-alignment")
    //{
    //    TStyleStepAlignment typedValue;
    //    int ret = StyleConverter<TStyleStepAlignment>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    if (aName == "icon-pitch-alignment") {
    //        setIconPitchAlignment(typedValue);
    //        return 0;
    //    }

    //    if (aName == "icon-rotation-alignment") {
    //        setIconRotationAlignment( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-pitch-alignment") {
    //        setTextPitchAlignment( typedValue);
    //        return 0;
    //    }

    //    if (aName == "text-rotation-alignment") {
    //        setTextRotationAlignment( typedValue);
    //        return 0;
    //    }
    //}
    //if (aName == "icon-text-fit") {
    //    TStyleStepIconTextFit typedValue;
    //    int ret = StyleConverter<TStyleStepIconTextFit>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    setIconTextFit( typedValue);
    //    return 0;
    //}
    //if (aName == "icon-text-fit-padding" ){
    //    TStyleExp4F typedValue;
    //    int ret = StyleConverter<TStyleExp4F>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -2;
    //    }

    //    setIconTextFitPadding( typedValue);
    //    return 0;
    //}
    //if (aName == "symbol-placement") {
    //    TStyleStepSymbolPlacement typedValue;
    //    int ret = StyleConverter<TStyleStepSymbolPlacement>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    setSymbolPlacement( typedValue);
    //    return 0;
    //}
    //if (aName == "symbol-z-order") {
    //    TStyleStepSymbolZOrder typedValue;
    //    int ret = StyleConverter<TStyleStepSymbolZOrder>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    setSymbolZOrder( typedValue);
    //    return 0;
    //}
    //if (aName == "text-field") {
    //    TStyleStepStr typedValue;
    //    int ret = StyleConverter<TStyleStepStr>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    setTextField( typedValue);
    //    return 0;
    //}
    //if (aName == "text-font") {
    //    TStyleStepStrs typedValue;
    //    int ret = StyleConverter<TStyleStepStrs>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    setTextFont( typedValue);
    //    return 0;
    //}
    //if (aName == "text-justify") {
    //    TStyleStepTextJustify typedValue;
    //    int ret = StyleConverter<TStyleStepTextJustify>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    setTextJustify( typedValue);
    //    return 0;
    //}
    //if (aName == "text-transform") {
    //    TStyleStepTextTransform typedValue;
    //    int ret = StyleConverter<TStyleStepTextTransform>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    setTextTransform( typedValue);
    //    return 0;
    //}
    //if (aName == "text-variable-anchor") {
    //    TStyleStepTextVariableAnchors typedValue;
    //    int ret = StyleConverter<TStyleStepTextVariableAnchors>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    setTextVariableAnchor( typedValue);
    //    return 0;
    //}
    //if (aName == "text-writing-mode") {
    //    TStyleStepTextWritingModes typedValue;
    //    int ret = StyleConverter<TStyleStepTextWritingModes>{}(aValue, typedValue);
    //    if (ret < 0) {
    //        return -3;
    //    }

    //    setTextWritingMode( typedValue);
    //    return 0;
    //}

     
    return -100;
}
 
 
} // namespace md

// clang-format on
