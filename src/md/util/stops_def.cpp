#pragma once 
#include <md/util/interpolate.hpp>

#include <md/util/stops_def.hpp>
 
  
namespace clientmap
{      
	template<class T>
	T  ExponentialStops<T>::evaluate(float z) const
	{
		if (stops.empty())
		{
			return T();//{T()};
		}

		auto it = stops.upper_bound(z);
		if (it == stops.end()) {
			return stops.rbegin()->second;
		}
		else if (it == stops.begin()) {
			return stops.begin()->second;
		}
		else {
			float interpoFactor = interpolationFactor(base,
				 std::prev(it)->first, it->first , z);
			return interpolate(std::prev(it)->second,
				it->second,
				interpoFactor);
		}
	}

    template<class T>  
    T  IntervalStops<T>::evaluate(float z) const
    {
        if (stops.empty()) {
            return T();
        }
            
        auto it = stops.upper_bound(z);
        if (it == stops.end()) {
            return stops.rbegin()->second;
        }
        else if (it == stops.begin()) {
            return stops.begin()->second;
        } else {
            return std::prev(it)->second;
        }
    }
         
    // ������ʵ�ַ����ƫ�ػ�ģ����,��Ҫ��ʽʵ����  
    // ���� ����exe�б��cpp���ø�ģ���࣬�����������Ĵ���
    // error LNK2019 : �޷��������ⲿ����
    template class ExponentialStops<float>;
    template class ExponentialStops<ColorI>;
    template class ExponentialStops< std::array<float, 2> >;
    template class ExponentialStops< std::array<float, 4> >;
    template class ExponentialStops< std::array<double, 3>  >;


    template class  IntervalStops<float>;
    template class  IntervalStops<std::string>;
    template class  IntervalStops<std::vector<std::string> >;
    template class  IntervalStops<std::vector<float> >;
    template class  IntervalStops<bool>;
    template class  IntervalStops<uint8_t>; // enum
} // namespace mapboxmap