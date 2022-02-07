#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

PlayScene::PlayScene()
{
	PlayScene::start();
	SoundManager::Instance().load("../Assets/audio/Bgm.mp3", "Bgm", SOUND_MUSIC);
	SoundManager::Instance().load("../Assets/audio/open.wav", "open", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/close.wav", "close", SOUND_SFX);
	SoundManager::Instance().playMusic("Bgm", -1, 0);
	SoundManager::Instance().setMusicVolume(15);
	SoundManager::Instance().setSoundVolume(20);
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	drawDisplayList();
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 255, 255, 255);
}

void PlayScene::update()
{
	updateDisplayList();
	if(m_Arrival->isEnabled())
	{
		if (CollisionManager::circleAABBCheck(m_Arrival, m_pTarget))
		{
			m_Arrival->setAccelerationRate(0);
			m_Arrival->setMaxSpeed(0);
			m_Arrival->getTransform()->position = m_pTarget->getTransform()->position;
			m_Arrival->getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
		}
		if(CollisionManager::circleAABBCheck(m_Arrival, m_TargetOutlCircle))
		{
			m_Arrival->setAccelerationRate(-4.0f);	
		}
		
	}

	if (m_Avoidance->isEnabled())
	{
		CollisionManager::circleAABBCheck(m_pTarget, m_Avoidance);
		CollisionManager::AABBCheck(m_Avoidance, m_pObstacle);
		CollisionManager::rotateAABB(m_Avoidance, m_Avoidance->getCurrentHeading());

		// obstacle dimension information / aliases
		const auto boxWidth = m_pObstacle->getWidth();
		const int halfBoxWidth = boxWidth * 0.5f;
		const auto boxHeight = m_pObstacle->getHeight();
		const int halfBoxHeight = boxHeight * 0.5f;
		const auto boxStart = m_pObstacle->getTransform()->position - glm::vec2(halfBoxWidth, halfBoxHeight);

		// check every whisker to see if it is colliding with the Obstacle
		m_Avoidance->getCollisionWhiskers()[0] = CollisionManager::lineRectCheck(m_Avoidance->getTransform()->position,
			m_Avoidance->getLeftLOSEndPoint(), boxStart, boxWidth, boxHeight);
		m_Avoidance->getCollisionWhiskers()[1] = CollisionManager::lineRectCheck(m_Avoidance->getTransform()->position,
			m_Avoidance->getMiddleLOSEndPoint(), boxStart, boxWidth, boxHeight);
		m_Avoidance->getCollisionWhiskers()[2] = CollisionManager::lineRectCheck(m_Avoidance->getTransform()->position,
			m_Avoidance->getRightLOSEndPoint(), boxStart, boxWidth, boxHeight);

		for (int i = 0; i < 3; ++i)
		{
			m_Avoidance->setLineColour(i,
				(m_Avoidance->getCollisionWhiskers()[i]) ? glm::vec4(1, 0, 0, 1) : glm::vec4(0, 1, 0, 1));
		}
	}
}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance().quit();
	}
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		SoundManager::Instance().playSound("open", 0, -1);
		m_pTarget->setEnabled(true);
		m_pSeeking->setEnabled(true);
	}


	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		SoundManager::Instance().playSound("open", 0, -1);
		m_pTarget->setEnabled(true);
		m_flee->setEnabled(true);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_3))
	{
		SoundManager::Instance().playSound("open", 0, -1);
		m_pTarget->setEnabled(true);
		m_Arrival->setEnabled(true);
		m_TargetOutlCircle->setEnabled(true);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_4))
	{
		SoundManager::Instance().playSound("open", 0, -1);
		m_pTarget->setEnabled(true);
		m_pObstacle->setEnabled(true);
		m_Avoidance->setEnabled(true);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_0))
	{
		SoundManager::Instance().playSound("close", 0, -1);
		m_pTarget->setEnabled(false);
		m_pObstacle->setEnabled(false);
		m_Avoidance->setEnabled(false);
		m_pSeeking->setEnabled(false);
		m_flee->setEnabled(false);
		m_Arrival->setEnabled(false);
		m_TargetOutlCircle->setEnabled(false);

	}


}

void PlayScene::start()
{
	//Instructions

	const SDL_Color blue = { 0, 0, 255, 255 };
	m_pInstructions = new Label("1 for seek ,2 for flee ,3 for arrive, ",
		"Consolas", 20, blue, glm::vec2(400.0f, 40.0f));
	m_pInstructions->setParent(this);
	addChild(m_pInstructions);
	m_pInstructions = new Label(" 4 for Avoidance ,0 for close all ",
		"Consolas", 20, blue, glm::vec2(400.0f, 60.0f));
	m_pInstructions->setParent(this);
	addChild(m_pInstructions);
	
	//
	// Set GUI Title
	m_guiTitle = "Play Scene";
	
	// Add Target to Scene
	m_pTarget = new Target();
	m_pTarget->getTransform()->position = glm::vec2(600.0f, 300.0f);
	addChild(m_pTarget);
	m_pTarget->setEnabled(false);

	// Add Seeking to Scene
	m_pSeeking = new Seeking();
	m_pSeeking->getTransform()->position = glm::vec2(200.0f, 300.0f);
	m_pSeeking->setTargetPosition(m_pTarget->getTransform()->position);
	addChild(m_pSeeking);
	m_pSeeking->setEnabled(false);

	// Add flee to Scene
	m_flee = new flee();
	m_flee->getTransform()->position = glm::vec2(500.0f, 300.0f);
	m_flee->setTargetPosition(m_pTarget->getTransform()->position);
	addChild(m_flee);
	m_flee->setEnabled(false);

	// Add Arrival to Scene
	m_Arrival = new Arrival();
	m_Arrival->getTransform()->position = glm::vec2(100.0f, 300.0f);
	m_Arrival->setTargetPosition(m_pTarget->getTransform()->position);
	addChild(m_Arrival);
	m_Arrival->setEnabled(false);

	// Add TargetOutlCircle to Scene
	m_TargetOutlCircle = new TargetOutlCircle();
	m_TargetOutlCircle->getTransform()->position = glm::vec2(600.0f, 300.0f);
	addChild(m_TargetOutlCircle);
	m_TargetOutlCircle->setEnabled(false);

	// Add Obstacle to Scene
	m_pObstacle = new Obstacle();
	m_pObstacle->getTransform()->position = glm::vec2(400.0f, 300.0f);
	addChild(m_pObstacle);
	m_pObstacle->setEnabled(false);

	// Add Avoidance to Scene
	m_Avoidance = new Avoidance();
	m_Avoidance->getTransform()->position = glm::vec2(200.0f, 300.0f);
	m_Avoidance->setTargetPosition(m_pTarget->getTransform()->position);
	addChild(m_Avoidance);
	m_Avoidance->setEnabled(false);
	
	/* Instructions Label */
	m_pInstructionsLabel = new Label("Press the backtick (`) character to toggle Debug View", "Consolas");
	m_pInstructionsLabel->getTransform()->position = glm::vec2(Config::SCREEN_WIDTH * 0.5f, 500.0f);

	addChild(m_pInstructionsLabel);

	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::GUI_Function() const
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("GAME3001-M2021-assignment-1", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	if(ImGui::Button("My Button"))
	{
		std::cout << "My Button Pressed" << std::endl;
	}

	ImGui::Separator();

	static float float3[3] = { 0.0f, 1.0f, 1.5f };
	if(ImGui::SliderFloat3("My Slider", float3, 0.0f, 2.0f))
	{
		std::cout << float3[0] << std::endl;
		std::cout << float3[1] << std::endl;
		std::cout << float3[2] << std::endl;
		std::cout << "---------------------------\n";
	}
	
	ImGui::End();
}
