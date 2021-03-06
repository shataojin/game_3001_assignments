#pragma once
#ifndef __START_SCENE__
#define __START_SCENE__

#include "Scene.h"
#include "Label.h"
#include "ship.h"
#include "Button.h"
#include "Heuristic.h"
#include "Target.h"
#include "SpaceShip.h"
#include "Tile.h"

class StartScene final : public Scene
{
public:
	StartScene();
	~StartScene();

	// Inherited via Scene
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;

private:
	Label* m_pStartLabel{};
	Label* m_pInstructionsLabel{};
	Label* m_TextLabels{};
	Label* m_TextID{};
	Ship* m_pShip{};

	Button* m_pStartButton;


	void GUI_Function();
	std::string m_guiTitle;

	glm::vec2 m_mousePosition;

	// Game Objects
	Target* m_pTarget;
	SpaceShip* m_pSpaceShip;


	// Pathfinding Objects and Functions
	std::vector<Tile*> m_pGrid;
	bool m_isGridEnabled;

	// Create our Division Scheme (Grid)
	void m_buildGrid();
	bool m_getGridEnabled() const;
	void m_setGridEnabled(bool state);
	void m_computeTileCosts();

	// Calculate the Shortest Path
	void m_findShortestPath();
	void m_displayPathList();
	void m_resetPathFinding();
	void m_resetSimulation();

	// tile lists for pathfinding
	std::vector<Tile*> m_pOpenList;
	std::vector<Tile*> m_pClosedList;
	std::vector<Tile*> m_pPathList;

	// convenience functions to convert world to grid space
	Tile* m_getTile(int col, int row);
	Tile* m_getTile(glm::vec2 grid_position);

	// heuristic
	Heuristic m_currentHeuristic;

	// Ship Movement
	int m_moveCounter = 0;
	bool m_shipIsMoving = false;
	void m_moveShip();

	static int start_position[2];
	static int goal_position[2];
};

#endif /* defined (__START_SCENE__) */