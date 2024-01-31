#include "menu.h"

Menu::Menu(QString n, QStringList list, Menu* p) {

    name = n;
    menuItems = list;
    parent = p;
}


Menu::~Menu() {

    for(int x = 0; x < subMenus.length(); x++) {
        delete subMenus[x];
    }
}


void Menu::addChildMenu(Menu* m) {

    subMenus.push_back(m);
}

void Menu::addMenuItem(QString i)
{
    menuItems.append(i);
}

void Menu::clearAllMenuItems(){
    menuItems.clear();
}

// getters
QString Menu::getName() { return name; }
QStringList Menu::getMenuItems() { return menuItems; }
Menu* Menu::getParent() { return parent; }
Menu* Menu::get(int i) { return subMenus[i]; }
