#include "StartScene.h"
#include <algorithm>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"
#include "EventManager.h"
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"
#include "Config.h"



StartScene::StartScene()
{
	StartScene::start();
}

StartScene::~StartScene()
= default;

void StartScene::draw()
{
	drawDisplayList();
}

void StartScene::update()
{
	updateDisplayList();
	if (m_shipIsMoving)
	{
		m_moveShip();
	}
}

void StartScene::clean()
{
	removeAllChildren();
}

void StartScene::handleEvents()
{
	EventManager::Instance().update();

	// Keyboard Events
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance().quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance().changeSceneState(PLAY_SCENE);
	}
	/*if (EventManager::Instance().isKeyDown(SDL_SCANCODE_G))
	{
		addChild(m_pTarget);
		addChild(m_pSpaceShip);
	}*/

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_H))
	{
		m_setGridEnabled(m_isGridEnabled);
		addChild(m_pTarget);
		addChild(m_pSpaceShip);
		m_isGridEnabled = true;
		m_displayPathList();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_F))
	{
		m_getTile(4, 3)->setTileStatus(IMPASSABLE);
		m_getTile(4, 4)->setTileStatus(IMPASSABLE);
		m_getTile(4, 5)->setTileStatus(IMPASSABLE);
		m_getTile(4, 6)->setTileStatus(IMPASSABLE);
		m_getTile(4, 7)->setTileStatus(IMPASSABLE);
		m_getTile(4, 8)->setTileStatus(IMPASSABLE);
		m_getTile(5, 8)->setTileStatus(IMPASSABLE);
		m_getTile(6, 8)->setTileStatus(IMPASSABLE);
		m_getTile(7, 8)->setTileStatus(IMPASSABLE);
		m_getTile(8, 8)->setTileStatus(IMPASSABLE);
		m_getTile(9, 8)->setTileStatus(IMPASSABLE);
		m_getTile(15, 8)->setTileStatus(IMPASSABLE);
		m_getTile(15, 9)->setTileStatus(IMPASSABLE);
		m_getTile(15, 10)->setTileStatus(IMPASSABLE);
		m_getTile(15, 7)->setTileStatus(IMPASSABLE);
		m_findShortestPath();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_M))
	{
		m_shipIsMoving = true;
	}

	if (m_isGridEnabled = true)
	{
		if (EventManager::Instance().mousePressed(SDL_BUTTON_LEFT))
		{
			std::cout << "aaaaaa" << std::endl;

		}
	}

}

void StartScene::start()
{
	const SDL_Color blue = { 0, 0, 255, 255 };
	m_pStartLabel = new Label("START SCENE", "Consolas", 60, blue, glm::vec2(400.0f, 40.0f));
	m_pStartLabel->setParent(this);
	addChild(m_pStartLabel);

	m_pInstructionsLabel = new Label("H for debug view, A for showing the blocker ,F for shortest path, M for moving", "Consolas", 17, blue, glm::vec2(400.0f, 500.0f));
	m_pInstructionsLabel->setParent(this);
	addChild(m_pInstructionsLabel);

	//
	m_TextLabels = new Label("name:taojin sha", "Consolas", 20, blue, glm::vec2(90.0f, 560.0f));
	m_TextLabels->setParent(this);
	addChild(m_TextLabels);

	m_TextID = new Label("studentID: 101334639", "Consolas", 20, blue, glm::vec2(115.0f, 580.0f));
	m_TextID->setParent(this);
	addChild(m_TextID);
	//
	// Start Button
	m_pStartButton = new Button();
	m_pStartButton->getTransform()->position = glm::vec2(400.0f, 400.0f);

	m_pStartButton->addEventListener(CLICK, [&]()-> void
		{
			m_pStartButton->setActive(false);
			TheGame::Instance().changeSceneState(PLAY_SCENE);
		});

	m_pStartButton->addEventListener(MOUSE_OVER, [&]()->void
		{
			m_pStartButton->setAlpha(128);
		});

	m_pStartButton->addEventListener(MOUSE_OUT, [&]()->void
		{
			m_pStartButton->setAlpha(255);
		});
	addChild(m_pStartButton);





	SoundManager::Instance().load("../Assets/audio/yay.ogg", "yay", SOUND_SFX);



	m_buildGrid();
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	m_currentHeuristic = MANHATTAN;
	m_pTarget = new Target();
	m_pTarget->getTransform()->position = m_getTile(15, 11)->getTransform()->position + offset;
	m_pTarget->setGridPosition(15.0f, 11.0f);
	m_getTile(15, 11)->setTileStatus(GOAL);
	//addChild(m_pTarget);

	m_pSpaceShip = new SpaceShip();
	m_pSpaceShip->getTransform()->position = m_getTile(1, 3)->getTransform()->position + offset;
	m_pSpaceShip->setGridPosition(1.0f, 3.0f);
	m_getTile(1, 3)->setTileStatus(START);
	//addChild(m_pSpaceShip);
	m_computeTileCosts();
	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&StartScene::GUI_Function, this));
}



void StartScene::m_buildGrid()
{
	const auto tile_size = Config::TILE_SIZE;

	// lay out a grid of tiles
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = new Tile();
			tile->getTransform()->position = glm::vec2(col * tile_size, row * tile_size); // world position
			tile->setGridPosition(col, row); // grid position
			tile->setParent(this);
			addChild(tile);
			tile->addLabels();
			tile->setEnabled(false);
			m_pGrid.push_back(tile);
		}
	}

	// setup the neighbour references for each tile in the grid
	// tiles = nodes in our graph
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			Tile* tile = m_getTile(col, row);

			// TopMost Neighbour
			if (row == 0)
			{
				tile->setNeighbourTile(TOP_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(TOP_TILE, m_getTile(col, row - 1));
			}

			// RightMost Neighbour
			if (col == Config::COL_NUM - 1)
			{
				tile->setNeighbourTile(RIGHT_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(RIGHT_TILE, m_getTile(col + 1, row));
			}

			// BottomMost Neighbour
			if (row == Config::ROW_NUM - 1)
			{
				tile->setNeighbourTile(BOTTOM_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(BOTTOM_TILE, m_getTile(col, row + 1));
			}

			// LeftMost Neighbour
			if (col == 0)
			{
				tile->setNeighbourTile(LEFT_TILE, nullptr);
			}
			else
			{
				tile->setNeighbourTile(LEFT_TILE, m_getTile(col - 1, row));
			}
		}
	}
}

bool StartScene::m_getGridEnabled() const
{
	return m_isGridEnabled;
}

void StartScene::m_setGridEnabled(const bool state)
{
	m_isGridEnabled = state;

	for (auto tile : m_pGrid)
	{
		tile->setEnabled(m_isGridEnabled); // enables each Tile Object
		tile->setLabelsEnabled(m_isGridEnabled); // enables the corresponding Labels
	}
}

void StartScene::m_computeTileCosts()
{
	float distance = 0.0f;
	float dx = 0.0f;
	float dy = 0.0f;

	// for each tile in the grid, loop
	for (auto tile : m_pGrid)
	{
		// compute the distance from each tile to the goal tile
		// distance (f) = tile cost (g) + heuristic estimate (h)
		switch (m_currentHeuristic)
		{
		case MANHATTAN:
			dx = abs(tile->getGridPosition().x - m_pTarget->getGridPosition().x);
			dy = abs(tile->getGridPosition().y - m_pTarget->getGridPosition().y);
			distance = dx + dy;
			break;
		case EUCLIDEAN:
			// computes the euclidean distance ("as the crow flies") for each tile
			distance = Util::distance(tile->getGridPosition(), m_pTarget->getGridPosition());
			break;
		}

		tile->setTileCost(distance);
	}
}

void StartScene::m_findShortestPath()
{
	// check if pathList is empty
	if (m_pPathList.empty())
	{
		// Step 1. initialization
		Tile* start_tile = m_getTile(m_pSpaceShip->getGridPosition());
		start_tile->setTileStatus(OPEN);
		m_pOpenList.push_back(start_tile);
		bool goal_found = false;

		// Step 2. Loop until the OpenList is Empty or the Goal is found
		while (!m_pOpenList.empty() && !goal_found)
		{
			// Step 2a - initialize variables for minimum distance
			auto min_distance = INFINITY;
			Tile* min_tile;
			int min_tile_index = 0;
			int neighbour_count = 0;
			std::vector<Tile*> neighbour_list;

			// Step 2b - Add only Valid Neighbours to the neighbour list
			// loop through each neighbour in right-winding order (Top - Right - Bottom - Left)
			for (int index = 0; index < NUM_OF_NEIGHBOUR_TILES; ++index)
			{
				const auto neighbour = m_pOpenList[0]->getNeighbourTile(static_cast<NeighbourTile>(index));
				if (neighbour == nullptr || neighbour->getTileStatus() == IMPASSABLE)
				{
					continue; // ignore neighbours that are not valid
				}
				neighbour_list.push_back(neighbour);
			}

			// Step 2c - For every valid neighbour in the neighbour list - check if it has the min distance to the goal
			// or -- alternatively -- the neighbour could be the goal
			for (auto neighbour : neighbour_list)
			{
				// step 2c1 - check if the neighbour is not the goal
				if (neighbour->getTileStatus() != GOAL)
				{
					// check if neighbour tile cost is less than the minimum found so far...
					if (neighbour->getTileCost() < min_distance)
					{
						min_distance = neighbour->getTileCost();
						min_tile = neighbour;
						min_tile_index = neighbour_count;
					}
					neighbour_count++;
				}
				else // neighbour is the goal tile
				{
					min_tile = neighbour;
					m_pPathList.push_back(min_tile);
					goal_found = true;
					break;
				}
			}

			// Step 2d - add top tile of the open_list to the path_list
			m_pPathList.push_back(m_pOpenList[0]);
			m_pOpenList.pop_back(); // removes the top tile

			// Step 2e - add the min_tile to the openList
			m_pOpenList.push_back(min_tile);
			min_tile->setTileStatus(OPEN);
			neighbour_list.erase(neighbour_list.begin() + min_tile_index);

			// Step 2f - push all remaining neighbours onto the closed list
			for (auto neighbour : neighbour_list)
			{
				if (neighbour->getTileStatus() == UNVISITED)
				{
					neighbour->setTileStatus(CLOSED);
					m_pClosedList.push_back(neighbour);
				}
			}
		}


		Tile* goal = m_pPathList.at(m_pPathList.size() - 2);
		m_pPathList.erase(m_pPathList.end() - 2);
		m_pPathList.push_back(goal);

		m_displayPathList();
	}
}

void StartScene::m_displayPathList()
{
	for (auto tile : m_pPathList)
	{
		std::cout << "(" << tile->getGridPosition().x << ", " << tile->getGridPosition().y << ")" << std::endl;
	}
	std::cout << "Path Length: " << m_pPathList.size() << std::endl;
}

void StartScene::m_resetPathFinding()
{
	// clear the tile vectors
	m_pPathList.clear();
	m_pPathList.shrink_to_fit();
	m_pOpenList.clear();
	m_pOpenList.shrink_to_fit();
	m_pClosedList.clear();
	m_pClosedList.shrink_to_fit();

	// reset tile statuses
	for (auto tile : m_pGrid)
	{
		tile->setTileStatus(UNVISITED);
	}

	// reset GOAL and START tiles to where the ship and target are currently located
	m_getTile(m_pTarget->getGridPosition())->setTileStatus(GOAL);
	goal_position[0] = m_pTarget->getGridPosition().x;
	goal_position[1] = m_pTarget->getGridPosition().y;
	m_getTile(m_pSpaceShip->getGridPosition())->setTileStatus(START);
	start_position[0] = m_pSpaceShip->getGridPosition().x;
	start_position[1] = m_pSpaceShip->getGridPosition().y;

	m_moveCounter = 0;
	m_shipIsMoving = false;
}

void StartScene::m_resetSimulation()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	m_resetPathFinding();
	// clear current status of  ship and target tiles
	m_getTile(m_pTarget->getGridPosition())->setTileStatus(UNVISITED);
	m_getTile(m_pSpaceShip->getGridPosition())->setTileStatus(UNVISITED);

	m_pTarget->getTransform()->position = m_getTile(15, 11)->getTransform()->position + offset;
	m_pTarget->setGridPosition(15.0f, 11.0f);
	m_getTile(15, 11)->setTileStatus(GOAL);
	goal_position[0] = m_pTarget->getGridPosition().x;
	goal_position[1] = m_pTarget->getGridPosition().y;

	m_pSpaceShip->getTransform()->position = m_getTile(1, 3)->getTransform()->position + offset;
	m_pSpaceShip->setGridPosition(1.0f, 3.0f);
	m_getTile(1, 3)->setTileStatus(START);
	start_position[0] = m_pSpaceShip->getGridPosition().x;
	start_position[1] = m_pSpaceShip->getGridPosition().y;
}

void StartScene::m_moveShip()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);
	if (m_moveCounter < m_pPathList.size())
	{
		auto pathTile_gridPosition = m_pPathList[m_moveCounter]->getGridPosition();
		m_pSpaceShip->getTransform()->position = m_getTile(pathTile_gridPosition)->getTransform()->position + offset;
		m_pSpaceShip->setGridPosition(pathTile_gridPosition.x, pathTile_gridPosition.y);
		if (Game::Instance().getFrames() % 20 == 0)
		{
			m_moveCounter++;
		}
	}
	else
	{
		m_shipIsMoving = false;
	}
}

Tile* StartScene::m_getTile(const int col, const int row)
{
	return m_pGrid[(row * Config::COL_NUM) + col];
}

Tile* StartScene::m_getTile(const glm::vec2 grid_position)
{
	const auto col = grid_position.x;
	const auto row = grid_position.y;

	return m_getTile(col, row);
}


void StartScene::GUI_Function()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();

	ImGui::Begin("assemnt 2", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	ImGui::Separator();

	static bool toggle_grid = false;
	if (ImGui::Checkbox("Toggle Grid", &toggle_grid))
	{
		m_isGridEnabled = toggle_grid;
		m_setGridEnabled(m_isGridEnabled);
		addChild(m_pTarget);
		addChild(m_pSpaceShip);
	}

	ImGui::Separator();

	// Heuristic selection

	static int radio = m_currentHeuristic;
	ImGui::Text("Heuristic Type");
	ImGui::RadioButton("Manhattan", &radio, MANHATTAN);
	ImGui::SameLine();
	ImGui::RadioButton("Euclidean", &radio, EUCLIDEAN);

	// check if current heuristic is not the same as the selection
	if (m_currentHeuristic != radio)
	{
		m_currentHeuristic = static_cast<Heuristic>(radio);
		m_computeTileCosts();
	}

	ImGui::Separator();
	if (ImGui::Button("Find Shortest Path"))
	{
		m_findShortestPath();
	}

	ImGui::Separator();
	if (ImGui::Button("Start"))
	{
		if (!m_shipIsMoving)
		{
			m_shipIsMoving = true;
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset Pathfinding"))
	{
		m_resetPathFinding();
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset Simulation"))
	{
		m_resetSimulation();
	}

	// spaceship properties
	start_position[0] = m_pSpaceShip->getGridPosition().x;
	start_position[1] = m_pSpaceShip->getGridPosition().y;
	if (ImGui::SliderInt2("Start Position", start_position, 0, Config::COL_NUM - 1))
	{
		// constrain the object within max rows
		if (start_position[1] > Config::ROW_NUM - 1)
		{
			start_position[1] = Config::ROW_NUM - 1;
		}
		// converts grid space to world space
		m_getTile(m_pSpaceShip->getGridPosition())->setTileStatus(UNVISITED);
		m_pSpaceShip->getTransform()->position = m_getTile(start_position[0], start_position[1])->getTransform()->position + offset;
		m_pSpaceShip->setGridPosition(start_position[0], start_position[1]); // records the grid position
		m_getTile(m_pSpaceShip->getGridPosition())->setTileStatus(START);
		m_resetPathFinding();
	}

	// Target properties

	goal_position[0] = m_pTarget->getGridPosition().x;
	goal_position[1] = m_pTarget->getGridPosition().y;
	if (ImGui::SliderInt2("Goal Position", goal_position, 0, Config::COL_NUM - 1))
	{
		// constrain the object within max rows
		if (goal_position[1] > Config::ROW_NUM - 1)
		{
			goal_position[1] = Config::ROW_NUM - 1;
		}
		m_getTile(m_pTarget->getGridPosition())->setTileStatus(UNVISITED);
		m_pTarget->getTransform()->position = m_getTile(goal_position[0], goal_position[1])->getTransform()->position + offset;
		m_pTarget->setGridPosition(goal_position[0], goal_position[1]);
		m_getTile(m_pTarget->getGridPosition())->setTileStatus(GOAL);
		m_computeTileCosts();
		m_resetPathFinding();
	}

	ImGui::Separator();



	ImGui::End();
}

// For reset.
int StartScene::start_position[2];
int StartScene::goal_position[2];




