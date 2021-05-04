#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;



class Actor : public GraphObject	//base class
{
public:
	Actor(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr)
		: GraphObject(ID, xCoord, yCoord, direction, size, depth)
	{
		m_studentWorldPtr = studentWorldPtr;
		isAliveStatus = true;	//all actors start out as alive
	}
	virtual void doSomething() = 0;
	bool isAlive();
	void setNotAlive();
	void setCollisionAvoidance(bool status);
	bool isCollisionAvoidance();
	void setHorizSpeed(double h_speed);
	void setVertSpeed(double v_speed);
	double getHorizSpeed();
	double getVertSpeed();
	bool overlap(double B_x, double B_y, double B_radius);
	void setAffecetedByProjectile(bool status);
	bool isAffectedByProjectile();
	virtual void shotByHolyWater()
	{
		return;
	}
	void movementAlgorithm();
	StudentWorld* returnPointerToWorld();
private:
	StudentWorld* m_studentWorldPtr;
	bool isAliveStatus;
	bool collisionWorthyStatus;
	bool affectedByProjectile;
	double horiz_speed;
	double vert_speed;
};

class Collision_Avoidance_Worthy :public Actor
{
public:
	Collision_Avoidance_Worthy(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) : Actor(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setCollisionAvoidance(true);
	}
	void setHealth(int health);
	int getHealth();
	int getMovementPlanDistance();
	void setMovementPlanDistance(int newMovementPlan);
private:
	int hitPoints;
	int movementPlanDistance;

};

class GhostRacer : public Collision_Avoidance_Worthy
{
public:
	GhostRacer(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) :Collision_Avoidance_Worthy(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setAffecetedByProjectile(false);
		numSprays = 10;	//Ghost Racer starts off with 10 holy water sprays
		setHealth(100);	//start with 100 hit/health points
		setVertSpeed(0);
	}
	void incNumSprays(int nSprays);
	int getNumSprays();
	void spinGhostRacer();
	void possiblyKillGhostRacer();
	virtual void doSomething();
private:
	int numSprays;
};

class ZombieCab :public Collision_Avoidance_Worthy
{
public:
	ZombieCab(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) :Collision_Avoidance_Worthy(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setAffecetedByProjectile(true);
		setHorizSpeed(0);
		setMovementPlanDistance(0);
		setHealth(3);	//start with 3 hit points
		hasDoneDamage = false;
	}
	virtual void doSomething();
	bool hasDamaged();
	virtual void shotByHolyWater();
	void possiblyKillZombieCab();
private:
	bool hasDoneDamage;
};

class Pedestrian :public Collision_Avoidance_Worthy
{
public:
	Pedestrian(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) :Collision_Avoidance_Worthy(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setAffecetedByProjectile(true);
		setMovementPlanDistance(0);
	}
	void newMovementPlan();
private:
};

class HumanPed :public Pedestrian
{
public:
	HumanPed(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) :Pedestrian(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setVertSpeed(-4);
		setHorizSpeed(0);
		setHealth(2);	//start with 2 hit points
	}
	virtual void doSomething();
	virtual void shotByHolyWater();
private:
};

class ZombiePed :public Pedestrian
{
public:
	ZombiePed(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) :Pedestrian(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setVertSpeed(-4);
		setHorizSpeed(0);
		setHealth(2);	//start with 2 hit poinsts
		ticksBeforeGrunt = 0;
	}
	virtual void doSomething();
	virtual void shotByHolyWater();
	void possiblyKillZombiePed();
private:
	int ticksBeforeGrunt;
};

class Not_Collision_Avoidance_Worthy :public Actor
{
public:
	Not_Collision_Avoidance_Worthy(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) : Actor(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setCollisionAvoidance(false);
	}
private:
	
};

class BorderLine : public Not_Collision_Avoidance_Worthy
{
public:
	BorderLine(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) :Not_Collision_Avoidance_Worthy(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setAffecetedByProjectile(false);
		setVertSpeed(-4);
		setHorizSpeed(0);
	}
	virtual void doSomething();
private:

};

class Goodie :public Not_Collision_Avoidance_Worthy
{
public:
	Goodie(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) :Not_Collision_Avoidance_Worthy(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{

	}
private:
};

class OilSlick :public Goodie
{
public:
	OilSlick(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) :Goodie(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setAffecetedByProjectile(false);
		setVertSpeed(-4);
		setHorizSpeed(0);
	}
	virtual void doSomething();
private:
};

class HealingGoodie :public Goodie
{
public:
	HealingGoodie(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) : Goodie(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setAffecetedByProjectile(true);
		setVertSpeed(-4);
		setHorizSpeed(0);
	}
	virtual void doSomething();
	virtual void shotByHolyWater();
private:
};

class HolyWaterGoodie : public Goodie
{
public:
	HolyWaterGoodie(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) : Goodie(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setAffecetedByProjectile(true);
		setVertSpeed(-4);
		setHorizSpeed(0);
	}
	virtual void doSomething();
	virtual void shotByHolyWater();
private:
};

class SoulGoodie : public Goodie
{
public:
	SoulGoodie(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) : Goodie(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setAffecetedByProjectile(false);
		setVertSpeed(-4);
		setHorizSpeed(0);
	}
	virtual void doSomething();
private:
};

class HolyWaterProjectile :public Goodie
{
public:
	HolyWaterProjectile(int ID, int xCoord, int yCoord, int direction, int size, int depth, StudentWorld* studentWorldPtr) : Goodie(ID, xCoord, yCoord, direction, size, depth, studentWorldPtr)
	{
		setAffecetedByProjectile(false);
		travelDistance = 160;
		totDistanceTraveled = 0;
	}
	virtual void doSomething();
private:
	int travelDistance;
	double totDistanceTraveled;
};

#endif // ACTOR_H_
