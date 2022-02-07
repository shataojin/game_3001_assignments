#include "Agens3.h"

#include <iostream>

#include "Util.h"

Agens3::Agens3()
{
	// initialize whisker colour
	m_lineColour[0] = glm::vec4(0, 1, 0, 1); // left line colour
	m_lineColour[1] = glm::vec4(0, 1, 0, 1); // middle line colour
	m_lineColour[2] = glm::vec4(0, 1, 0, 1); // right line colour
	
	// initialize whisker collisions
	m_collisionWhiskers[0] = false;
	m_collisionWhiskers[1] = false;
	m_collisionWhiskers[2] = false;

	// initialize whisker angle
	m_whiskerAngle = 50;

	// initialize the LOS distance
	m_LOSDistance = 300; // 300px
}

Agens3::~Agens3()
= default;

glm::vec2 Agens3::getTargetPosition() const
{
	return m_targetPosition;
}

glm::vec2 Agens3::getCurrentDirection() const
{
	return m_currentDirection;
}

float Agens3::getLOSDistance() const
{
	return m_LOSDistance;
}

bool Agens3::hasLOS() const
{
	return m_hasLOS;
}

float Agens3::getCurrentHeading() const
{
	return m_currentHeading;
}

glm::vec4 Agens3::getLOSColour() const
{
	return m_LOSColour;
}

glm::vec2 Agens3::getLeftLOSEndPoint() const
{
	return m_leftLOSEndPoint;
}

glm::vec2 Agens3::getMiddleLOSEndPoint() const
{
	return m_middleLOSEndPoint;
}

glm::vec2 Agens3::getRightLOSEndPoint() const
{
	return m_rightLOSEndPoint;
}

bool* Agens3::getCollisionWhiskers()
{
	return m_collisionWhiskers;
}

glm::vec4 Agens3::getLineColour(const int index)
{
	return m_lineColour[index];
}

float Agens3::getWhiskerAngle() const
{
	return m_whiskerAngle;
}

void Agens3::setLeftLOSEndPoint(const glm::vec2 point)
{
	m_leftLOSEndPoint = point;
}

void Agens3::setMiddleLOSEndPoint(const glm::vec2 point)
{
	m_middleLOSEndPoint = point;
}

void Agens3::setRightLOSEndPoint(const glm::vec2 point)
{
	m_rightLOSEndPoint = point;
}

void Agens3::setLineColour(const int index, const glm::vec4 colour)
{
	m_lineColour[index] = colour;
}

void Agens3::setWhiskerAngle(const float angle)
{
	m_whiskerAngle = angle;
}

void Agens3::updateWhiskers(const float angle)
{
	m_whiskerAngle = angle;
	// middle whisker
	setMiddleLOSEndPoint(getTransform()->position + getCurrentDirection() * getLOSDistance());

	// left whisker
	float x = sin((getCurrentHeading() - m_whiskerAngle + 90) * Util::Deg2Rad);
	float y = cos((getCurrentHeading() - m_whiskerAngle + 90) * Util::Deg2Rad);
	setLeftLOSEndPoint(getTransform()->position + glm::vec2(x, -y) * getLOSDistance() * 0.75f);

	// right whisker
	x = sin((getCurrentHeading() + m_whiskerAngle + 90) * Util::Deg2Rad);
	y = cos((getCurrentHeading() + m_whiskerAngle + 90) * Util::Deg2Rad);
	setRightLOSEndPoint(getTransform()->position + glm::vec2(x, -y) * getLOSDistance() * 0.75f);
}

void Agens3::setTargetPosition(const glm::vec2 new_position)
{
	m_targetPosition = new_position;
}

void Agens3::setCurrentDirection(const glm::vec2 new_direction)
{
	m_currentDirection = new_direction;
}

void Agens3::setLOSDistance(const float distance)
{
	m_LOSDistance = distance;
}

void Agens3::setHasLOS(const bool state)
{
	m_hasLOS = state;
	m_LOSColour = (m_hasLOS) ? glm::vec4(0, 1, 0, 1) : glm::vec4(1, 0, 0, 1);
}

void Agens3::setCurrentHeading(const float heading)
{
	m_currentHeading = heading;
	m_changeDirection();
}

void Agens3::setLOSColour(const glm::vec4 colour)
{
	m_LOSColour = colour;
}

void Agens3::m_changeDirection()
{
	const auto x = cos(m_currentHeading * Util::Deg2Rad);
	const auto y = sin(m_currentHeading * Util::Deg2Rad);
	m_currentDirection = glm::vec2(x, y);
}
