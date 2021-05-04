#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    bonusPoints = 5000; //start any level with 5000 bonus points
    numSouls = 2 * getLevel() + 5;
    GhostRacer_Pointer = new GhostRacer(IID_GHOST_RACER, 128, 32, 90, 4.0, 0, this); //create Ghost Racer 

    for (int i = 0; i < (VIEW_HEIGHT / SPRITE_HEIGHT); i++) 
    {
        gameWorld.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, i * SPRITE_HEIGHT, 0, 2.0, 2, this)); //add left yellow border lines
        gameWorld.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i * SPRITE_HEIGHT, 0, 2.0, 2, this));    //add right yelloy border line
    }

    for (int i = 0; i < VIEW_HEIGHT / (4 * SPRITE_HEIGHT); i++)
    {
        gameWorld.push_back(new BorderLine(IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH / 3, i * (4 * SPRITE_HEIGHT), 0, 2.0, 2, this));   //add white border line between left-most and middle lanes
        gameWorld.push_back(new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH / 3, i * (4 * SPRITE_HEIGHT), 0, 2.0, 2, this));  //add white border line between right-most and middle lanes
        lastWhiteBorderLineYCoord = gameWorld.back()->getY();   //will save the yCoord for the last white border line
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    vector <Actor*>::iterator it;
    it = gameWorld.begin();

    bonusPoints--; 

    GhostRacer_Pointer->doSomething();

    while (it != gameWorld.end())    //all active actors do something
    {
        if ((*it)->isAlive() == true)
        {
            (*it)->doSomething();
            if (GhostRacer_Pointer->getHealth() <= 0 || returnGhostPointer()->isAlive() == false)   //if ghost racer has died
            {
                decLives();
                playSound(SOUND_PLAYER_DIE);
                return GWSTATUS_PLAYER_DIED;
            }
            if (numSouls == 0)
            {
                increaseScore(getBonusPoints());    //add remaining bonus points
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
        it++;
    }

    for (it = gameWorld.begin(); it != gameWorld.end(); )   //remove and delete dead actors
    {
        if ((*it)->isAlive() == false)
        {
            delete *it;
            it = gameWorld.erase(it);
        }
        else
            it++;
    }

    //add new border lines
    lastWhiteBorderLineYCoord = lastWhiteBorderLineYCoord + (-4 - GhostRacer_Pointer->getVertSpeed());  //determine new position of the last white borderline
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    int delta_y = new_border_y - lastWhiteBorderLineYCoord;

    if (delta_y >= SPRITE_HEIGHT)
    {
        gameWorld.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2, new_border_y, 0, 2.0, 2, this));
        gameWorld.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2, new_border_y, 0, 2.0, 2, this));
    }
    if (delta_y >= 4 * SPRITE_HEIGHT)
    {
        gameWorld.push_back(new BorderLine(IID_WHITE_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3, new_border_y, 0, 2.0, 2, this));
        gameWorld.push_back(new BorderLine(IID_WHITE_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3, new_border_y, 0, 2.0, 2, this));
        lastWhiteBorderLineYCoord = new_border_y;
    }

    //add zombie cabs
    int ChanceVehicle = max(100 - getLevel() * 10, 20);
    if (randInt(0, ChanceVehicle - 1) == 0)
    {
        int cur_lane = randInt(0, 2);   //start adding a new zombie cab in a random lane

        for (int i = 0; i < 3; i++, cur_lane++)
        {
            if (cur_lane % 3 == 0)
                cur_lane = 0;

            double closestYBot = VIEW_HEIGHT;
            double closestYTop = 0;

            it = gameWorld.begin();
            for (int i = 0; i < gameWorld.size(); i++, it++)
            {
                switch (cur_lane)
                {
                case 0:     //left lane
                    if ((*it)->getX() >= LEFT_EDGE && (*it)->getX() < LEFT_EDGE + ROAD_WIDTH / 3)   //if actor is in the left lane
                    {
                        if ((*it)->isCollisionAvoidance() && (*it)->getY() < closestYBot)   //and is collision avoidance worthy and is closer to the bottom of the screen
                            closestYBot = (*it)->getY();
                        if ((*it)->isCollisionAvoidance() && (*it)->getY() > closestYTop)    //and is collision avoidance worthy and is closer to the top of the screen
                            closestYTop = (*it)->getY();
                    }
                    break;
                case 1:     //middle lane
                    if ((*it)->getX() >= LEFT_EDGE + ROAD_WIDTH / 3 && (*it)->getX() < RIGHT_EDGE - ROAD_WIDTH / 3) //if actor is in middle lane
                    {
                        if ((*it)->isCollisionAvoidance() && (*it)->getY() < closestYBot)   //and is collision avoidance worthy and is closer to the bottom of the screen
                            closestYBot = (*it)->getY();
                        if ((*it)->isCollisionAvoidance() && (*it)->getY() > closestYTop)   //and is collision avoidance worthy and is closer to the top of the screen
                            closestYTop = (*it)->getY();
                    }
                    break;
                case 2:     //right lane
                    if ((*it)->getX() >= RIGHT_EDGE - ROAD_WIDTH / 3 && (*it)->getX() < RIGHT_EDGE) //if actor is in the right lane
                    {
                        if ((*it)->isCollisionAvoidance() && (*it)->getY() < closestYBot)   //and is collision avoidance worthy and is closer to the bottom of the screen
                            closestYBot = (*it)->getY();
                        if ((*it)->isCollisionAvoidance() && (*it)->getY() > closestYTop)   //and is collision avoidance worthy and is closer to the top of the screen
                            closestYTop = (*it)->getY();
                    }
                    break;
                }
            }

            switch (cur_lane)
            {
            case 0:
                if (returnGhostPointer()->getX() >= LEFT_EDGE && returnGhostPointer()->getX() < LEFT_EDGE + ROAD_WIDTH / 3) //if Ghost Racer is in the left lane
                {
                    if (returnGhostPointer()->getY() < closestYBot) //and is closer to the bottom of the screen
                        closestYBot = returnGhostPointer()->getY();
                    if (returnGhostPointer()->getY() > closestYTop) //and is closer to the top of the screen
                        closestYTop = returnGhostPointer()->getY();
                }
                break;
            case 1:
                if (returnGhostPointer()->getX() >= LEFT_EDGE + ROAD_WIDTH / 3 && returnGhostPointer()->getX() < RIGHT_EDGE - ROAD_WIDTH / 3)   //if Ghost Racer is in the middle lane
                {
                    if (returnGhostPointer()->getY() < closestYBot) //and is closer to the bottom of the screen
                        closestYBot = returnGhostPointer()->getY();
                    if (returnGhostPointer()->getY() > closestYTop) //and is closer to the top of the screen
                        closestYTop = returnGhostPointer()->getY(); 
                }
                break;
            case 2:
                if (returnGhostPointer()->getX() >= RIGHT_EDGE - ROAD_WIDTH / 3 && returnGhostPointer()->getX() < RIGHT_EDGE)   //if Ghost Racer is in the right lane
                {
                    if (returnGhostPointer()->getY() < closestYBot) //and is closer to the bottom of the screen
                        closestYBot = returnGhostPointer()->getY();
                    if (returnGhostPointer()->getY() && returnGhostPointer()->getY() > closestYTop) //and is closer to the top of the screen
                        closestYTop = returnGhostPointer()->getY();
                }
                break;
            }

            Actor* tempPtr;

            if (closestYBot == VIEW_HEIGHT || closestYBot > VIEW_HEIGHT / 3)    //if there is no actor in the desired lane, or there is but it is far enough away from the bottom of the screen
            {
                switch (cur_lane)
                {
                    case 0:
                    {
                        tempPtr = new ZombieCab(IID_ZOMBIE_CAB, ROAD_CENTER - ROAD_WIDTH / 3, SPRITE_HEIGHT / 2, 90, 4.0, 0, this); //add new zombie cab in left lane
                        tempPtr->setVertSpeed(returnGhostPointer()->getVertSpeed() + randInt(2, 4));
                        gameWorld.push_back(tempPtr);
                        break;
                    }
                    case 1:
                    {
                        tempPtr = new ZombieCab(IID_ZOMBIE_CAB, ROAD_CENTER, SPRITE_HEIGHT / 2, 90, 4.0, 0, this);  //add new zombie cab in middle lane
                        tempPtr->setVertSpeed(returnGhostPointer()->getVertSpeed() + randInt(2, 4));
                        gameWorld.push_back(tempPtr);
                        break;
                    }
                    case 2:
                    {
                        tempPtr = new ZombieCab(IID_ZOMBIE_CAB, ROAD_CENTER + ROAD_WIDTH / 3, SPRITE_HEIGHT / 2, 90, 4.0, 0, this); //add new zombie cab in right lane
                        tempPtr->setVertSpeed(returnGhostPointer()->getVertSpeed() + randInt(2, 4));
                        gameWorld.push_back(tempPtr);
                        break;
                    }
                }
                break;
            }

            if (closestYTop == 0 || closestYTop < VIEW_HEIGHT * (2 / 3))    //if there is no actor in the desired lane, or there is but it is far enough away from the top of the screen
            {
                switch (cur_lane)
                {
                    case 0:
                    {
                        tempPtr = new ZombieCab(IID_ZOMBIE_CAB, ROAD_CENTER - ROAD_WIDTH / 3, SPRITE_HEIGHT / 2, 90, 4.0, 0, this); //add new zombie cab in left lane
                        tempPtr->setVertSpeed(returnGhostPointer()->getVertSpeed() - randInt(2, 4));
                        gameWorld.push_back(tempPtr);
                        break;
                    }
                    case 1:
                    {
                        tempPtr = new ZombieCab(IID_ZOMBIE_CAB, ROAD_CENTER, SPRITE_HEIGHT / 2, 90, 4.0, 0, this);  //add new zombie cab in middle lane
                        tempPtr->setVertSpeed(returnGhostPointer()->getVertSpeed() - randInt(2, 4));
                        gameWorld.push_back(tempPtr);
                        break;
                    }
                    case 2:
                    {
                        tempPtr = new ZombieCab(IID_ZOMBIE_CAB, ROAD_CENTER + ROAD_WIDTH / 3, SPRITE_HEIGHT / 2, 90, 4.0, 0, this); //add new zombie cab in right lane
                        tempPtr->setVertSpeed(returnGhostPointer()->getVertSpeed() - randInt(2, 4));
                        gameWorld.push_back(tempPtr);
                        break;
                    }
                }
                break;
            }
        }
    }
    

    //add oil slicks
    int ChanceOilSlick = max(150 - getLevel() * 10, 40);
    if (randInt(0, ChanceOilSlick - 1) == 0)
        gameWorld.push_back(new OilSlick(IID_OIL_SLICK, randInt(LEFT_EDGE , RIGHT_EDGE), VIEW_HEIGHT, 0, randInt(2, 5), 2, this));

    //add zombie peds
    int ChanceZombiePed = max(100 - getLevel() * 10, 20);
    if (randInt(0, ChanceZombiePed - 1) == 0)
        gameWorld.push_back(new ZombiePed(IID_ZOMBIE_PED, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, 0, 3.0, 0, this));

    //add human peds
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    if (randInt(0, ChanceHumanPed - 1) == 0)
        gameWorld.push_back(new HumanPed(IID_HUMAN_PED, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, 0, 2.0, 0, this));

    //add holy water goodies
    int ChanceOfHolyWater = 100 + 10 * getLevel();
    if (randInt(0, ChanceOfHolyWater - 1) == 0)
        gameWorld.push_back(new HolyWaterGoodie(IID_HOLY_WATER_GOODIE, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, 90, 2.0, 2, this));

    //add new lost soul goodies
    int ChanceOfLostSoul = 100;
    if (randInt(0, ChanceOfLostSoul - 1) == 0)
        gameWorld.push_back(new SoulGoodie(IID_SOUL_GOODIE, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT, 0, 4.0, 2, this));

    //update display text
    ostringstream displayText;
    displayText << "  Score: " << getScore() << "  Lvl: " << getLevel() << "  Souls2Save: " << numSouls << "  Lives: " << getLives() << "  Health: "
        << returnGhostPointer()->getHealth() << "  Sprays: " << returnGhostPointer()->getNumSprays() << "  Bonus: " << getBonusPoints() << endl;

    string gameDisplayText = displayText.str();
    setGameStatText(gameDisplayText);

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete GhostRacer_Pointer;

    vector <Actor*>::iterator it;
    it = gameWorld.begin();

    while (!gameWorld.empty())
    {
        delete* it;
        it = gameWorld.erase(it);
    }
}

void StudentWorld::setNumSouls(int newSouls)
{
    numSouls += newSouls;
}

int StudentWorld::getBonusPoints()
{
    return bonusPoints;
}

void StudentWorld::addNewHolyWaterProjectile(int xCoord, int yCoord, int direction)
{
    gameWorld.push_back(new HolyWaterProjectile(IID_HOLY_WATER_PROJECTILE, xCoord, yCoord, direction, 1.0, 1, this));
}

void StudentWorld::addNewHealingGoodie(int xCoord, int yCoord)
{
    gameWorld.push_back(new HealingGoodie(IID_HEAL_GOODIE, xCoord, yCoord, 0, 1.0, 2, this));
}

void StudentWorld::addNewOilSlick(int xCoord, int yCoord)
{
    gameWorld.push_back(new OilSlick(IID_OIL_SLICK, xCoord, yCoord, 0, randInt(2, 5), 2, this));
}

bool StudentWorld::holyProjectileOverlap(double projectileX, double projectileY, double projectileR)    //determine if an actor and the holy water projectile overlap
{
    vector <Actor*>::iterator it;
    it = gameWorld.begin();

    while (it != gameWorld.end())
    {
        if ((*it)->isAffectedByProjectile() && (*it)->overlap(projectileX, projectileY, projectileR)) //if actor is affected by holy water, activate projectile
        {
            (*it)->shotByHolyWater();   //possibly decrease its health by 1
            return true;    //holy water projectile is activated
        }
        it++;
    }
    return false;   //holy water projectile is not activated
}

int StudentWorld::determineLaneNum(double xCoord)
{
    if (xCoord >= LEFT_EDGE && xCoord < LEFT_EDGE + ROAD_WIDTH / 3)
        return 1;
    else if (xCoord >= LEFT_EDGE + ROAD_WIDTH / 3 && xCoord < RIGHT_EDGE - ROAD_WIDTH / 3)
        return 2;
    else if (xCoord >= RIGHT_EDGE - ROAD_WIDTH / 3 && xCoord < RIGHT_EDGE)
       return 3;
}

bool StudentWorld::closestActorInFront(double zombieCabX, double zombieCabY)    //determine closest actor in front of zombie cab
{
    double ghostRacerX = returnGhostPointer()->getX();
    double ghostRacerY = returnGhostPointer()->getY();
    double closestYCoord = VIEW_HEIGHT;

    int zombieCabLane = determineLaneNum(zombieCabX);   //determine the lane zombie cab is in

    //if ghost racer is in the same lane as the zombie cab and is higher on the road than the zombie cab
    if (determineLaneNum(returnGhostPointer()->getX()) == zombieCabLane && returnGhostPointer()->getY() > zombieCabY)
        closestYCoord = returnGhostPointer()->getY();

    vector <Actor*>::iterator it;
    it = gameWorld.begin();

    while (it != gameWorld.end())   
    {
        //if there is an actor that is collision-avoidance worthy in the same lane as the zombie cab
        if ((*it)->isCollisionAvoidance() && determineLaneNum((*it)->getX() == zombieCabLane))
        {
            if ((*it)->getY() > zombieCabY && (*it)->getY() < closestYCoord)    //if this actor is in front of the zombie cab and is closer than the previous closest actor
                closestYCoord = (*it)->getY();
        }
        it++;
    }

    if (closestYCoord - zombieCabY < 96)    //if actor is close enough in front of zombie cab
        return true;
    else
        return false;
}

bool StudentWorld::closestActorBehind(double zombieCabX, double zombieCabY) //determine closest actor behind zombie cab
{
    double closestYCoord = 0;

    double zombieCabLane = determineLaneNum(zombieCabX);   //determine the lane zombie cab is in

    vector <Actor*>::iterator it;
    it = gameWorld.begin();

    while (it != gameWorld.end())
    {
        //if there is an actor that is collision-avoidance worthy in the same lane as the zombie cab
        if ((*it)->isCollisionAvoidance() && determineLaneNum((*it)->getX() == zombieCabLane))
        {
            if ((*it)->getY() < zombieCabY && (*it)->getY() > closestYCoord)    //if this actor is behind zombie cab and is closer than the previous closest actor
                closestYCoord = (*it)->getY();
        }
        it++;
    }

    if (zombieCabY - closestYCoord < 96)    //if actor is close enough behind zombie cab
        return true;
    else
        return false;
}

GhostRacer* StudentWorld::returnGhostPointer()
{
    return GhostRacer_Pointer;
}


