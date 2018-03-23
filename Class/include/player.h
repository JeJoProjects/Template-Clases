#pragma once

class player
{
private:
    int m_x;
public:
    player();
    player(const int& x);

    void setX();            //method for setting X from user input
    const int& getX()const; //Method for retrieving variable
};

