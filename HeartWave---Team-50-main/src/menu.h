#ifndef MENU_H
#define MENU_H

#include <QString>
#include <QStringList>
#include <QVector>

/* Purpose of class: The menu that is used by the ui to change and hold data about the current menu and its various submenus
 *
 * Data Members:
 *
 * -QString name : The name of the menu, which is used to determine the state of the program when running.
 * -QStringList menuItems : The names of all sub menus.
 * -QVector<Menu*> subMenus : A vector of all menu items stored as this menus children.
 * -Menu* parent : The menu that hold this as a child.
 *
 * Class functions:
 *
 * QString getName() : returns the name of the current menu item.
 * QstringList getMenuItems() : returns the names of all the children of the current menu.
 * Menu* getParent() : returns the parent menu of the current menu or nullptr if the current menu is the root of the tree.
 * Menu* getInt(int) : returns the child menu at the position of the provided int in the QVector<Menu*> subMenus variable.
 * void addChildMenu(Menu*) : adds a menu item to the end of the subMenus vector.
 * void clearAllMenuItems() : clears all menu items
 */

class Menu {

public:
    explicit Menu(QString, QStringList, Menu*);
    ~Menu();

    QString getName();
    QStringList getMenuItems();
    Menu* getParent();
    Menu* get(int);
    void addChildMenu(Menu*);
    void addMenuItem(QString);
    void clearAllMenuItems();

private:
    QString name;
    QStringList menuItems;
    QVector<Menu*> subMenus;
    Menu* parent;

};

#endif // MENU_H

