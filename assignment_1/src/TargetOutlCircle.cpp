#include "TargetOutlCircle.h"
#include "TextureManager.h"


TargetOutlCircle::TargetOutlCircle()
{
	TextureManager::Instance().load("../Assets/textures/2.png","circlex");

	const auto size = TextureManager::Instance().getTextureSize("circlex");
	setWidth(size.x);
	setHeight(size.y);
	getTransform()->position = glm::vec2(100.0f, 100.0f);
	getRigidBody()->velocity = glm::vec2(0, 0);
	getRigidBody()->isColliding = false;

	setType(TARGET);
}

TargetOutlCircle::~TargetOutlCircle()
= default;

void TargetOutlCircle::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the TargetOutlCircle
	TextureManager::Instance().draw("circlex", x, y, 0, 255, true);
}

void TargetOutlCircle::update()
{
	m_move();
	m_checkBounds();
}

void TargetOutlCircle::clean()
{
}

void TargetOutlCircle::m_move()
{
	getTransform()->position = getTransform()->position + getRigidBody()->velocity * 5.0f;
}

void TargetOutlCircle::m_checkBounds()
{
}

void TargetOutlCircle::m_reset()
{
}
