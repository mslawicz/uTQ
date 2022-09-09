/*
 * filter.cpp
 *
 *  Created on: 9 wrz 2022
 *      Author: marci
 */

#include "filter.h"

MedianFilter::MedianFilter(const size_t size) :
    _size(size)
{
    _unsortedBuffer.resize(_size, 0);
    _sortedBuffer.resize(_size, 0);
}
