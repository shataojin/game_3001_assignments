#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Arrival.h"
#include "Avoidance .h"
#include "Scene.h"
#include "Plane.h"
#include "Player.h"
#include "Button.h"
#include "flee.h"
#include "Label.h"
#include "Obstacle.h"
#include "Seeking.h"
#include "Target.h"
#include "TargetOutlCircle.h"


class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Scene LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;
private:
	// IMGUI Function
	void GUI_Function() const;
	std::string m_guiTitle;
	
	glm::vec2 m_mousePosition;

	//game obj
	Target* m_pTarget;
	Seeking* m_pSeeking;
	flee* m_flee;
	TargetOutlCircle* m_TargetOutlCircle;
	Arrival* m_Arrival;
	Obstacle* m_pObstacle;
	Avoidance* m_Avoidance;
	// UI Items
	
	Label* m_pInstructionsLabel;
	Label* m_pInstructions;

	
};

#endif /* defined (__PLAY_SCENE__) */