/*
 * menu.cpp
 *
 *  Created on: Oct 2, 2022
 *      Author: marci
 */

#include "menu.h"

Menu::Menu(Display* pDisplay) :
    _pDisplay(pDisplay)
{

}

void Menu::registerItem(MenuId itemId, std::string text)
{
    _items.emplace_back(MenuItem{itemId, text});
}

void Menu::display()
{
    if(_itemIdx < _items.size())
    {
        _pDisplay->putText(0, PosY, _items[_itemIdx].text, FontTahoma14b, false, _pDisplay->getMaxX());
    }
}

void Menu::changeItem(int8_t change)
{
    auto menuSize = _items.size();
    if(menuSize < 2)
    {
        //0 or 1 element - do nothing
        return;
    }
    int8_t newIdx = _itemIdx + change;
    if(newIdx >= static_cast<int8_t>(menuSize))
    {
        newIdx -= menuSize;
    }
    else if(newIdx < 0)
    {
        newIdx += menuSize;
    }
    _itemIdx = static_cast<uint8_t>(newIdx);
}
