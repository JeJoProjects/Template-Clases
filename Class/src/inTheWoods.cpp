#include "inTheWoods.h"
#include "player.h"
#include <iostream>
inTheWoods::inTheWoods() {}

inTheWoods::~inTheWoods() {}

void inTheWoods::printInfo()
{
    //Player object to access methods in player class
    player playerObj;      // will beinitialized with 0
    playerObj.setX();
    std::cout << playerObj.getX() <<std::endl;
}
