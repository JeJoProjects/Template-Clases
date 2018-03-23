#include "player.h"
#include <iostream>

player::player()              // defualt constructor
    :m_x(0) {}

player::player(const int& x)
    :m_x(x) {}                // parameterised

void player::setX()
{
    std::cout << "Set X to a number:" << std::endl;
    std::cin >> m_x;
}

const int& player::getX()const
{
    return m_x;
}
