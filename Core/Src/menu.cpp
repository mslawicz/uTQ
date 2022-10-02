/*
 * menu.cpp
 *
 *  Created on: Oct 2, 2022
 *      Author: marci
 */

#include "menu.h"

void Menu::registerItem(MenuId itemId, std::string text)
{
    _items.emplace_back(MenuItem{itemId, text});
}

