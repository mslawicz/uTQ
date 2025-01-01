/*
 * median_filter.h
 *
 *  Created on: 10 wrz 2022
 *      Author: marci
 */

#ifndef INC_MEDIAN_FILTER_H_
#define INC_MEDIAN_FILTER_H_

#define MED_FLT_SIZE    31

#include <array>

template<class T>
class MedianFilter
{
public:
    MedianFilter();
    void perform(T newValue);
    T getMedian() const { return _sortedBuffer[MED_FLT_SIZE / 2]; }
private:
    std::array<T, MED_FLT_SIZE> _unsortedBuffer;
    std::array<T, MED_FLT_SIZE> _sortedBuffer;
    size_t _currentIndex{ 0 };
};

template<class T>
inline MedianFilter<T>::MedianFilter()
{
    _unsortedBuffer.fill(static_cast<T>(0));
    _sortedBuffer.fill(static_cast<T>(0));
}

template<class T>
inline void MedianFilter<T>::perform(T newValue)
{
    //store the new value in the circular (unsorted) buffer
    T oldestValue = _unsortedBuffer[_currentIndex];
    _unsortedBuffer[_currentIndex] = newValue;
    _currentIndex = (_currentIndex + 1) % MED_FLT_SIZE;

    //replace the oldest value in the sorted buffer and replace it with the new value
    size_t index = MED_FLT_SIZE / 2;
    while (_sortedBuffer[index] != oldestValue)
    {
        index += (_sortedBuffer[index] > oldestValue) ? -1 : 1;
    }
    _sortedBuffer[index] = newValue;

    //make the buffer sorted again
    T tempValue;
    while ((index > 0) && (_sortedBuffer[index - 1] > _sortedBuffer[index]))
    {
        tempValue = _sortedBuffer[index - 1];
        _sortedBuffer[index - 1] = _sortedBuffer[index];
        _sortedBuffer[index] = tempValue;
        index--;
    }
    while ((index < MED_FLT_SIZE - 1) && (_sortedBuffer[index + 1] < _sortedBuffer[index]))
    {
        tempValue = _sortedBuffer[index + 1];
        _sortedBuffer[index + 1] = _sortedBuffer[index];
        _sortedBuffer[index] = tempValue;
        index++;
    }
}

#endif /* INC_MEDIAN_FILTER_H_ */
