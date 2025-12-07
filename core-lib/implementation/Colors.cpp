#include "Colors.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>

constexpr std::size_t N_GRADIENTS_STEPS=10;
const std::array<ColorRGB, N_GRADIENTS_STEPS> GRADIENT_STEPS={{
    {0,     0,      0   },  //BLACK         (EMPTY)
    {66,    66,     66  },  //IRON
    {94,    69,     0   },  //BRONZE
    {191,   193,    194 },  //SILVER
    {255,   215,    0   },  //GOLD
    {1,     121,    111 },  //PINE          (PLAT)
    {152,   32,     197 },  //PURPLE        (DIAMOND)
    {0,     181,    113 },  //EMERALD       (ASCENDANT)
    {202,   44,     44  },  //RUBY          (IMMORTAL)
    {178,   243,    243 }   //SOFT CYAN     (RADIANT)
}};


using GradientArr= std::array<std::pair<ColorRGB, ColorRGB>, N_GRADIENTS_STEPS-1>;

GradientArr initGradientArray(){
    GradientArr gradient_arr;
    int i=0;
    for(auto it=GRADIENT_STEPS.begin(), end=GRADIENT_STEPS.end()-1; it!=end; ++it, i++){
        gradient_arr[i].first=*it;
        gradient_arr[i].second=*(it+1);
    }
    return gradient_arr;
}


#include <print>

ColorHSL rgbToHsl(const ColorRGB& color){
    const uint8_t RED=0;
    const uint8_t GREEN=1;
    const uint8_t BLUE=2;
    if(color.r==94&&color.g==69&&color.b==0){
        std::println("Siamo stati qua");
    }
    std::array<double, sizeof(color)/sizeof(char)> norms{
        color.r/255.,
        color.g/255.,
        color.b/255.
    };
    auto [min, max]= std::minmax_element(norms.begin(), norms.end());
    double light= (*min+*max)/2;
    if(min==max){
        return {0, 0, light};
    }
    double saturation=0;
    if((*max-*min)!=0){
        saturation=(*max-*min)/(1-std::abs(2*light-1));
    }
    uint8_t max_index=std::abs(norms.data()-max);
    double d_hue=0;
    switch (max_index){
    case RED:
        d_hue=std::fmod((norms[GREEN]-norms[BLUE])/(*max-*min), 6);
        break;
    case GREEN:
        d_hue=(norms[BLUE]-norms[RED])/(*max-*min)+2;
        break;
    case BLUE:
        d_hue=(norms[RED]-norms[GREEN])/(*max-*min)+4;
        break;
    default:
        break;
    }
    int16_t hue=(d_hue/6)*360;
    return {hue, saturation, light};
}
ColorRGB mapValueToGradient(int64_t val, int64_t low_end, int64_t high_end){
    static GradientArr gradient_arr=initGradientArray();
    
    double norm_val=(double)val/(high_end-low_end);
    double gradient_d_index= norm_val*gradient_arr.size();
    std::size_t gradient_index= gradient_d_index;
    double sub_index_norm=gradient_d_index-gradient_index;

    ColorRGB start_color=gradient_arr[gradient_index].first;
    ColorRGB end_color=gradient_arr[gradient_index].second;
    uint8_t r= start_color.r + sub_index_norm*(end_color.r-start_color.r);
    uint8_t g= start_color.g + sub_index_norm*(end_color.g-start_color.g);
    uint8_t b= start_color.b + sub_index_norm*(end_color.b-start_color.b);
    return {r, g, b};
}