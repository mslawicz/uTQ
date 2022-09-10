/*
 * median_filter.h
 *
 *  Created on: 10 wrz 2022
 *      Author: marci
 */

#ifndef INC_MEDIAN_FILTER_H_
#define INC_MEDIAN_FILTER_H_

#include <vector>

template<class T>
class MedianFilter
{
public:
    MedianFilter(const size_t size);
    void filter(T newValue);
    T getMedian() const { return _sortedBuffer[_size / 2]; }
private:
    size_t _size;
    std::vector<T> _unsortedBuffer;
    std::vector<T> _sortedBuffer;
    size_t _currentIndex{ 0 };
};

template<class T>
inline MedianFilter<T>::MedianFilter(const size_t size) :
    _size(size)
{
    _unsortedBuffer.resize(_size, static_cast<T>(0));
    _sortedBuffer.resize(_size, static_cast<T>(0));
}

template<class T>
inline void MedianFilter<T>::filter(T newValue)
{
    //store the new value in the circular (unsorted) buffer
    T oldestValue = _unsortedBuffer[_currentIndex];
    _unsortedBuffer[_currentIndex] = newValue;
    _currentIndex = (_currentIndex + 1) % _size;

    //replace the oldest value in the sorted buffer and replace it with the new value
    size_t index = _size / 2;
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
    while ((index < _size - 1) && (_sortedBuffer[index + 1] < _sortedBuffer[index]))
    {
        tempValue = _sortedBuffer[index + 1];
        _sortedBuffer[index + 1] = _sortedBuffer[index];
        _sortedBuffer[index] = tempValue;
        index++;
    }
}

#endif /* INC_MEDIAN_FILTER_H_ */
