/*
 * filter.h
 *
 *  Created on: 9 wrz 2022
 *      Author: marci
 */

#ifndef INC_FILTER_H_
#define INC_FILTER_H_

#include "stm32f4xx_hal.h"
#include <vector>

class MedianFilter
{
public:
    MedianFilter(const size_t size);
private:
    const size_t _size;
    std::vector<uint16_t> _unsortedBuffer;
    std::vector<uint16_t> _sortedBuffer;
    size_t _currentIndex{0};
};


#endif /* INC_FILTER_H_ */
