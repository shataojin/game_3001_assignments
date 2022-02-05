#include "Seeking.h"
#include "TextureManager.h"
#include "Util.h"
#include <iostream>

#include "Game.h"

Seeking::Seeking()
{
	TextureManager::Instance().load("../Assets/textures/ncl.png", "Seeking");

	auto size = TextureManager::Instance().getTextureSize("Seeking");
	setWidth(size.x);
	setHeight(size.y);

	getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	getRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	getRigidBody()->isColliding = false;
	setType(AGENT);

	
	setCurrentHeading(0.0f);

	
	m_maxSpeed = 10.0f; // 10 pixels per frame
	m_turnRate = 5.0f; // 5 degrees per frame
	m_accelerationRate = 2.0f; // 2 pixels per frame
	setLOSDistance(100.0f);
	setLOSColour(glm::vec4(0, 1, 0, 1));
}

Seeking::~Seeking()
= default;

void Seeking::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the Seeking
	TextureManager::Instance().draw("Seeking", x, y, getCurrentHeading(), 255, true);
	// draw LOS
	Util::DrawLine(getTransform()->position, getTransform()->position + getCurrentDirection() * getLOSDistance(), getLOSColour());
}



void Seeking::update()
{
	m_move();
}

void Seeking::clean()
{
}

float Seeking::getMaxSpeed() const
{
	return m_maxSpeed;
}

float Seeking::getTurnRate() const
{
	return m_turnRate;
}


glm::vec2 Seeking::getDesiredVelocity() const
{
	return m_desiredVelocity;
}

float Seeking::getAccelerationRate() const
{
	return m_accelerationRate;
}

void Seeking::setAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}



void Seeking::setDesiredVelocity(const glm::vec2 target_position)
{
	m_desiredVelocity = Util::normalize(target_position - getTransform()->position) * m_maxSpeed;
	getRigidBody()->velocity = m_desiredVelocity - getRigidBody()->velocity;
	//std::cout << "Desired Velocity: (" << m_desiredVelocity.x << ", " << m_desiredVelocity.y << ")" << std::endl;
}

void Seeking::LookWhereIamGoing(glm::vec2 target_direction)
{
	const auto target_rotation = Util::signedAngle(getCurrentDirection(), target_direction);

	const auto turn_sensitivity = 5.0f;

	if (abs(target_rotation) > turn_sensitivity)
	{
		if (target_rotation > 0.0f)
		{
			setCurrentHeading(getCurrentHeading() + getTurnRate());
		}
		else if (target_rotation < 0.0f)
		{
			setCurrentHeading(getCurrentHeading() - getTurnRate());
		}
	}
}

void Seeking::seek()
{
	// compute the target direction and magnitude
	auto target_direction = getTargetPosition() - getTransform()->position;

	// normalize the target direction
	target_direction = Util::normalize(target_direction);

	//std::cout << "Target Direction: (" << target_direction.x << ", " << target_direction.y << ")" << std::endl;

	// seek with LookWhereIamGoing
	LookWhereIamGoing(target_direction);

	// seek without LookWhereIamGoing
	//setCurrentDirection(target_direction); 
}

void Seeking::m_move()
{
	seek();
	auto deltaTime = TheGame::Instance().getDeltaTime();

	getRigidBody()->acceleration = getCurrentDirection() * getAccelerationRate();

	// use kinematic equation -- pf = pi + vi * t + 0.5ai * t^2

	// compute velocity and acceleration terms
	getRigidBody()->velocity += getCurrentDirection() * (deltaTime)+0.5f * getRigidBody()->acceleration * (deltaTime);

	// clamp velocity to maxSpeed
	getRigidBody()->velocity = Util::clamp(getRigidBody()->velocity, getMaxSpeed());

	// add velocity to position
	getTransform()->position += getRigidBody()->velocity;
}

void Seeking::setMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void Seeking::setTurnRate(const float angle)
{
	m_turnRate = angle;
}



