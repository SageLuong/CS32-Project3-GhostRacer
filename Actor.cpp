#include "Actor.h"
#include "StudentWorld.h"
#define _USE_MATH_DEFINES
#include <math.h>

bool Actor::isAlive()	
{
	return isAliveStatus;
}

void Actor::setNotAlive()
{
	isAliveStatus = false;
}

void Actor::setCollisionAvoidance(bool status)
{
	collisionWorthyStatus = status;
}

bool Actor::isCollisionAvoidance()
{
	return collisionWorthyStatus;
}

void Actor::setHorizSpeed(double h_speed)
{
	horiz_speed = h_speed;
}

void Actor::setVertSpeed(double v_speed)
{
	vert_speed = v_speed;
}

double Actor::getHorizSpeed()
{
	return horiz_speed;
}

double Actor::getVertSpeed()
{
	return vert_speed;
}

bool Actor::overlap(double B_x, double B_y, double B_radius)	//determine if two actors overlap
{
	double delta_x = abs(getX() - B_x);
	double delta_y = abs(getY() - B_y);
	double radius_sum = getRadius() + B_radius;
	if (delta_x < radius_sum * 0.25 && delta_y < radius_sum * .6)
		return true;
	else
		return false;
}

void Actor::setAffecetedByProjectile(bool status)
{
	affectedByProjectile = status;
}

bool Actor::isAffectedByProjectile()
{
	return affectedByProjectile;
}

void Actor::movementAlgorithm()	//moves some actors to a new position
{
	double vert_speed = getVertSpeed() - returnPointerToWorld()->returnGhostPointer()->getVertSpeed();
	double horiz_speed = getHorizSpeed();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;

	moveTo(new_x, new_y);	//move to new position

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)	//if actor has gone off screen
	{
		setNotAlive();
		return;
	}
}

StudentWorld* Actor::returnPointerToWorld()
{
	return m_studentWorldPtr;
}

void Collision_Avoidance_Worthy::setHealth(int health)
{
	hitPoints = health;
}

int Collision_Avoidance_Worthy::getHealth()
{
	return hitPoints;
}

int Collision_Avoidance_Worthy::getMovementPlanDistance()
{
	return movementPlanDistance;
}

void Collision_Avoidance_Worthy::setMovementPlanDistance(int newMovementPlan)
{
	movementPlanDistance = newMovementPlan;
}

void GhostRacer::incNumSprays(int nSprays)
{
	numSprays += nSprays;
}

int GhostRacer::getNumSprays()
{
	return numSprays;
}

void GhostRacer::spinGhostRacer()	//spin Ghost Racer if it overlaps with an oil slick
{
	double degreeRotate = randInt(5, 20);	//magnitude of degree of rotation
	int degreeDirection;
	if (randInt(0, 1) == 0)
		degreeDirection = -1;	//clockwise
	else
		degreeDirection = 1;	//counter-clockwise

	degreeRotate *= degreeDirection;	//final degree of rotation

	if (getDirection() + degreeRotate < 60)
		setDirection(60);	//set to minimum angle
	else if (getDirection() + degreeRotate > 120)
		setDirection(120);	// set to maximum angle
	else
		setDirection(getDirection() + degreeRotate);	//increase or decrease angle
}

void GhostRacer::possiblyKillGhostRacer()	//determine if Ghost Racer has been killed
{
	if (getHealth() <= 0)
	{
		setNotAlive();
		returnPointerToWorld()->playSound(SOUND_PLAYER_DIE);
	}
}

void GhostRacer::doSomething()
{
	int keyValue;

	if (getHealth() <= 0 || !isAlive())	//first determine if Ghost Racer is alive
	{ 
		setNotAlive();
		return;
	}

	if (getX() <= LEFT_EDGE)	//if swerving off the left side of the road
	{
		if (getDirection() > 90)	//still pointing left
		{
			setHealth(getHealth() - 10);	//hits left edge and damages Ghost Racer
			returnPointerToWorld()->returnGhostPointer()->possiblyKillGhostRacer();	//determine if Ghost Racer has died
		}
		setDirection(82);	//repel off left edge
		returnPointerToWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	else if (getX() >= RIGHT_EDGE)	//if swerving off the right side of road
	{
		if (getDirection() < 90)	//still pointing right
		{
			setHealth(getHealth() - 10); //hits right edge and damages Ghost Racer
			returnPointerToWorld()->returnGhostPointer()->possiblyKillGhostRacer();	//determine if Ghost Racer has died
		}
		setDirection(98);	//repel off right edge
		returnPointerToWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	else if (returnPointerToWorld()->getKey(keyValue))	//if player has pressed a key
	{
		switch (keyValue)
		{
			case KEY_PRESS_LEFT:
			{
				if (getDirection() < 114)
					setDirection(getDirection() + 8);	//move left
				break;
			}
			case KEY_PRESS_RIGHT:
			{
				if (getDirection() > 66)
					setDirection(getDirection() - 8);	//move right
				break;
			}
			case KEY_PRESS_UP:
			{
				if (getVertSpeed() < 5)
					setVertSpeed(getVertSpeed() + 1);	//speed up
				break;
			}
			case KEY_PRESS_DOWN:
			{
				if (getVertSpeed() > -1)
					setVertSpeed(getVertSpeed() - 1);	//slow down
				break;
			}
			case KEY_PRESS_SPACE:	//try to spray holy water
			{
				if (numSprays >= 1)
				{
					double delta_x = getX() + SPRITE_HEIGHT * (cos(getDirection() * (M_PI / 180)));	//convert to radians befpre using trig functions
					double delta_y = getY() + SPRITE_HEIGHT * (sin(getDirection() * (M_PI / 180)));
					returnPointerToWorld()->addNewHolyWaterProjectile(delta_x, delta_y, getDirection());	//add holy water projectile

					returnPointerToWorld()->playSound(SOUND_PLAYER_SPRAY);
					numSprays--;
					break;
				}
				else
					break;
			}
		}
	}

	//move Ghost Racer
	double max_shift_per_tick = 4.0;
	double direction = getDirection() * (M_PI / 180);	//convert to radians
	double delta_x = cos(direction) * max_shift_per_tick;
	double cur_x = getX();
	double cur_y = getY();
	moveTo(cur_x + delta_x, cur_y);	//moves Ghost Racer horizontally
}

void ZombieCab::doSomething()
{
	if (!isAlive() || getHealth() <= 0)	//first determine if zombie cab is alive
	{
		setNotAlive();
		return;
	}

	if (overlap(returnPointerToWorld()->returnGhostPointer()->getX(), returnPointerToWorld()->returnGhostPointer()->getY(),
		returnPointerToWorld()->returnGhostPointer()->getRadius()))	//if overlaps with Ghost Racer
	{
		if(hasDamaged() == false)	//if zombie cab has not damaged Ghost Racer yet
		{
			returnPointerToWorld()->playSound(SOUND_VEHICLE_CRASH);
			returnPointerToWorld()->returnGhostPointer()->setHealth(returnPointerToWorld()->returnGhostPointer()->getHealth() - 20);	//damage Ghost Racer
			returnPointerToWorld()->returnGhostPointer()->possiblyKillGhostRacer();	//determine if Ghost Racer has died

			if (getX() <= returnPointerToWorld()->returnGhostPointer()->getX())	//left or same x coord as GhostRacer
			{
				setHorizSpeed(-5);
				setDirection(120 + randInt(0, 19));
			}
			else   //right of Ghost Racer
			{
				setHorizSpeed(5);
				setDirection(60 + randInt(0, 19));
			}
			hasDoneDamage = true;	//zombie cab remembers it has damaged Ghost Racer
		}
	}

	movementAlgorithm();	//move zombie cab

	if (getVertSpeed() > returnPointerToWorld()->returnGhostPointer()->getVertSpeed())	//zombie cab is faster than Ghost Racer
	{
		if (returnPointerToWorld()->closestActorInFront(getX(), getY()))	//determine closest collision avoidance worthy actor in front of zombie cab
		{
			setVertSpeed(getVertSpeed() - 0.5);
			return;
		}
	}
	else   //zombie cab's speed is the same or less than Ghost Racer
	{
		if (returnPointerToWorld()->closestActorBehind(getX(), getY()))	//determine closest collision avoidance worthy actor behind zombie cab
		{
			setVertSpeed(getVertSpeed() + 0.5);
			return;
		}
	}

	setMovementPlanDistance(getMovementPlanDistance() - 1);
	if (getMovementPlanDistance() > 0)
	{
		return;
	}
	else  //new zombie cab movement plan
	{
		setMovementPlanDistance(randInt(4, 32));
		setVertSpeed(getVertSpeed() + randInt(-2, 2));
	}
}

bool ZombieCab::hasDamaged()
{
	return hasDoneDamage;
}

void ZombieCab::shotByHolyWater()	//hurt zombie cab and possibly kill it if shot by holy water
{
	setHealth(getHealth() - 1);
	possiblyKillZombieCab();
}

void ZombieCab::possiblyKillZombieCab()	//determine if zombie cab has been killed
{
	if (getHealth() <= 0)
	{
		setNotAlive();
		returnPointerToWorld()->playSound(SOUND_VEHICLE_DIE);

		if (randInt(0, 4) == 0)	//1 in 5 chance of spawning oil slick at same location
			returnPointerToWorld()->addNewOilSlick(getX(), getY());
		returnPointerToWorld()->increaseScore(200);	//increase score
		return;
	}
	else
		returnPointerToWorld()->playSound(SOUND_VEHICLE_HURT);
}

void Pedestrian::newMovementPlan()	//determine Pedestrian specific movement plan
{
	int negRandInt = randInt(-3, -1);
	int posRandInt = randInt(1, 3);
	if (randInt(0, 1) == 0)
		setHorizSpeed(negRandInt);
	else
		setHorizSpeed(posRandInt);

	setMovementPlanDistance(randInt(4, 32));
	if (getHorizSpeed() < 0)
		setDirection(180);
	else if (getHorizSpeed() > 0)
		setDirection(0);
}

void HumanPed::doSomething()
{
	if (getHealth() <= 0 || !isAlive())	//determine if human ped is alive
	{
		setNotAlive();
		return;
	}

	if (overlap(returnPointerToWorld()->returnGhostPointer()->getX(), returnPointerToWorld()->returnGhostPointer()->getY(),
		returnPointerToWorld()->returnGhostPointer()->getRadius()))	//ghost racer hits human ped
	{
		returnPointerToWorld()->returnGhostPointer()->setNotAlive();	//end level
		return;
	}

	movementAlgorithm();	//move human pedestrian

	setMovementPlanDistance(getMovementPlanDistance() - 1);
	if (getMovementPlanDistance() > 0)
		return;
	else	
	{
		newMovementPlan();	//new human pedestrian movement plan
	}
}

void HumanPed::shotByHolyWater()	//annoy human ped if shot by holy water projectile
{
	setHorizSpeed(getHorizSpeed() * -1);	//reverse direction
	if (getDirection() == 0)
		setDirection(180);
	else if (getDirection() == 180)
		setDirection(0);

	returnPointerToWorld()->playSound(SOUND_PED_HURT);
}

void ZombiePed::doSomething()
{
	if (getHealth() <= 0 || !isAlive())	//determine if zombie ped is alive
	{
		setNotAlive();
		return;
	}

	if (overlap(returnPointerToWorld()->returnGhostPointer()->getX(), returnPointerToWorld()->returnGhostPointer()->getY(),
		returnPointerToWorld()->returnGhostPointer()->getRadius()))	//if Ghost Racer hits zombie ped
	{
		returnPointerToWorld()->returnGhostPointer()->setHealth(returnPointerToWorld()->returnGhostPointer()->getHealth() - 5);	//damage Ghost Racer
		returnPointerToWorld()->returnGhostPointer()->possiblyKillGhostRacer();	//determine if Ghost Racer has died

		setHealth(getHealth() - 2);	//kill zombie pedestrian
		possiblyKillZombiePed();
		return;
	}

	if (getX() > returnPointerToWorld()->returnGhostPointer()->getX() - 30 || getX() < returnPointerToWorld()->returnGhostPointer()->getX() + 30
		&& getY() > returnPointerToWorld()->returnGhostPointer()->getY())	//within 30 pixels to the left or right of Ghost Racer and is in front of Ghost Racer
	{
		setDirection(270);	//face downward

		if (getX() < returnPointerToWorld()->returnGhostPointer()->getX())	//if left of Ghost Racer
			setHorizSpeed(1);
		else if (getX() > returnPointerToWorld()->returnGhostPointer()->getX())	//if right of Ghost Racer
			setHorizSpeed(-1);
		else
			setHorizSpeed(0);

		ticksBeforeGrunt--;
		if (ticksBeforeGrunt <= 0)
		{
			returnPointerToWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			ticksBeforeGrunt = 20;
		}
	}

	movementAlgorithm();	//move zombie ped

	if (getMovementPlanDistance() > 0)
	{
		setMovementPlanDistance(getMovementPlanDistance() - 1);
		return;
	}
	else   //new zombie ped movement plan
	{
		newMovementPlan();
	}
}

void ZombiePed::shotByHolyWater()	//hurt zombie ped and possibly kill it if shot by holy water projectile
{
	setHealth(getHealth() - 1);
	possiblyKillZombiePed();
}

void ZombiePed::possiblyKillZombiePed()	//determine if zombie ped has been killed
{
	if (getHealth() <= 0)
	{
		setNotAlive();
		returnPointerToWorld()->playSound(SOUND_PED_DIE);

		if (!overlap(returnPointerToWorld()->returnGhostPointer()->getX(), returnPointerToWorld()->returnGhostPointer()->getY(),
			returnPointerToWorld()->returnGhostPointer()->getRadius()))	//killed by holy water projectile
		{
			if (randInt(0, 4) == 0)	//1 in 5 chance of spawning healing goodie at same location
				returnPointerToWorld()->addNewHealingGoodie(getX(), getY());
		}
		returnPointerToWorld()->increaseScore(150);	//increase score
	}
}

void BorderLine::doSomething()
{
	movementAlgorithm();	//move border line
}

void OilSlick::doSomething()
{
	movementAlgorithm();	//move oil slick

	if (overlap(returnPointerToWorld()->returnGhostPointer()->getX(), returnPointerToWorld()->returnGhostPointer()->getY(),
		returnPointerToWorld()->returnGhostPointer()->getRadius()))	//if Ghost Racer hits oil slick
	{
		returnPointerToWorld()->playSound(SOUND_OIL_SLICK);
		returnPointerToWorld()->returnGhostPointer()->spinGhostRacer();
	}
}

void HealingGoodie::doSomething()
{
	movementAlgorithm();	//move healing goodie

	if (overlap(returnPointerToWorld()->returnGhostPointer()->getX(), returnPointerToWorld()->returnGhostPointer()->getY(),
		returnPointerToWorld()->returnGhostPointer()->getRadius()))	//if Ghost Racer picks up healing goodie
	{
		if (returnPointerToWorld()->returnGhostPointer()->getHealth() + 10 >= 100)
			returnPointerToWorld()->returnGhostPointer()->setHealth(100);	//set Ghost Racer's health as max
		else
			returnPointerToWorld()->returnGhostPointer()->setHealth(returnPointerToWorld()->returnGhostPointer()->getHealth() + 10);	//increase Ghost Racer's health
		
		setNotAlive();
		returnPointerToWorld()->playSound(SOUND_GOT_GOODIE);
		returnPointerToWorld()->increaseScore(250);	//increase score
	}
}

void HealingGoodie::shotByHolyWater()
{
	setNotAlive();	//destroy healing goodie if hit by holy water projectile
}

void HolyWaterGoodie::doSomething()
{
	movementAlgorithm();	//move holy water goodie

	if (overlap(returnPointerToWorld()->returnGhostPointer()->getX(), returnPointerToWorld()->returnGhostPointer()->getY(),
		returnPointerToWorld()->returnGhostPointer()->getRadius()))	//if Ghost Racer picks up holy water goodie
	{
		returnPointerToWorld()->returnGhostPointer()->incNumSprays(10);	//increase holy water chrages by 10
		setNotAlive();
		returnPointerToWorld()->playSound(SOUND_GOT_GOODIE);
		returnPointerToWorld()->increaseScore(50);	//increase score
	}
}

void HolyWaterGoodie::shotByHolyWater()
{
	setNotAlive();	//destroy holy water goodie if hit by holy water projectile
}

void SoulGoodie::doSomething()
{
	movementAlgorithm();	//move soul goodie

	if (overlap(returnPointerToWorld()->returnGhostPointer()->getX(), returnPointerToWorld()->returnGhostPointer()->getY(),
		returnPointerToWorld()->returnGhostPointer()->getRadius()))	//if Ghost Racer picks up lost soul
	{
		returnPointerToWorld()->setNumSouls(-1);	//decrease num of souls needed to be saved
		setNotAlive();
		returnPointerToWorld()->playSound(SOUND_GOT_SOUL);
		returnPointerToWorld()->increaseScore(100);	//increase score
	}

	setDirection(getDirection() - 10);	//rotates
}

void HolyWaterProjectile::doSomething()
{
	if (!isAlive())	//determine if holy water projectile is alive
		return;

	//possibly interact with an object
	if(returnPointerToWorld()->holyProjectileOverlap(getX(), getY(), getRadius()))
		{
			setNotAlive();	//kill projectile
			return;
		}

	//move holy water projectile
	double old_x = getX();
	double old_y = getY();

	moveForward(SPRITE_HEIGHT);
	double new_x = getX();
	double new_y = getY();

	double net_x = new_x - old_x;
	double net_y = new_y - old_y;

	totDistanceTraveled += pow(net_x * net_x + net_y * net_y, 0.5);	//add to total distance traveled

	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)	//if projectile has moved of screen
	{
		setNotAlive();
		return;
	}

	if (totDistanceTraveled >= travelDistance)	//determine if spray has traveled its max distance
		setNotAlive();
}