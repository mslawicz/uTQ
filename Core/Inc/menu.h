/*
 * menu.h
 *
 *  Created on: Oct 2, 2022
 *      Author: marci
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

#include "stm32f4xx_hal.h"
#include <string>
#include <vector>

enum class MenuId : uint8_t
{
    AircraftType,
    Heading
};

class Menu
{
public:
    void registerItem(MenuId itemId, std::string text);
    void display() {}
private:
    struct MenuItem
    {
        MenuId itemId;
        std::string text;
    };
    uint8_t _menuIdx{0};
    std::vector<MenuItem> _items;
};


#endif /* INC_MENU_H_ */
