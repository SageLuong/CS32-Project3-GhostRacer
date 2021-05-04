#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

const double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
const double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;


class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void setNumSouls(int newSouls);
    int getBonusPoints();
    void addNewHolyWaterProjectile(int xCoord, int yCoord, int direction);
    void addNewHealingGoodie(int xCoord, int yCoord);
    void addNewOilSlick(int xCoord, int yCoord);
    bool holyProjectileOverlap(double projectileX, double projectileY, double projectileR);
    int determineLaneNum(double xCoord);
    bool closestActorInFront(double zombieCabX, double zombieCabY);
    bool closestActorBehind(double zombieCabX, double zombieCabY);
    GhostRacer* returnGhostPointer();
private:
    GhostRacer* GhostRacer_Pointer;
    std::vector <Actor*> gameWorld;
    double lastWhiteBorderLineYCoord;
    int numSouls;
    int bonusPoints;
};



#endif // STUDENTWORLD_H_
