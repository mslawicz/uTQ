/*
 * menu.h
 *
 *  Created on: Oct 2, 2022
 *      Author: marci
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

#include "stm32f4xx_hal.h"
#include "display.h"
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
    Menu(Display* pDisplay);
    void registerItem(MenuId itemId, std::string text);
    void display();
    uint8_t getItemIdx() const { return _itemIdx; }
    void incItem() { changeItem(1); }
    void decItem() { changeItem(-1); }
    MenuId getItemId() const { return _items[_itemIdx].itemId; }
private:
    void changeItem(int8_t change);
    struct MenuItem
    {
        MenuId itemId;
        std::string text;
    };
    uint8_t _itemIdx{0};
    std::vector<MenuItem> _items;
    Display* _pDisplay;
    static constexpr uint8_t PosY{48};
};


#endif /* INC_MENU_H_ */
