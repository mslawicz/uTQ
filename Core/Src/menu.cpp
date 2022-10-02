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
