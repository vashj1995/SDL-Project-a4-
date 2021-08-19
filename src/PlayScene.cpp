#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include <sstream>
#include <iomanip>

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"
#include "time.h"

PlayScene::PlayScene()
{
	PlayScene::start();
	SoundManager::Instance().load("../Assets/audio/forest.ogg", "forest", SOUND_MUSIC);
	SoundManager::Instance().allocateChannels(16);
	SoundManager::Instance().setMusicVolume(10);
	SoundManager::Instance().setSoundVolume(25);
	SoundManager::Instance().playMusic("forest", -1, 0);
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	drawDisplayList();
	SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);

	TextureManager::Instance()->draw("Background", 0, getTransform()->position.y, 0, 255, false);

	for (int i = 0; i < m_pPlayerBullets.size(); i++)
		addChild(m_pPlayerBullets[i]);

	for (int i = 0; i < m_pEnemyBullets.size(); i++)
		addChild(m_pEnemyBullets[i]);
}


void PlayScene::update()
{
	updateDisplayList();


	for (auto enemy : m_pEnemy)
		enemy->getDecisionTree()->MakeDecision();

	for (auto enemy : m_pEnemy)
	{
		if (enemy->getHealth() == 1)
			enemy->setHealthState(true);
	}

	EnemyFireCoolDown--;
	meleeCoolDown--;
	m_setUIScore();
	CollisionsUpdate();
	updateDisplayList();
	for (int i = 0; i < m_pEnemy.size(); i++)
	{
		if (m_pEnemy[i] != nullptr)
		{
			m_DecisionMaking(m_pEnemy[i]);
			if (m_pEnemy[i]->attackCooldown > 0)
				m_pEnemy[i]->attackCooldown--;
		}
	}

	for (int i = 0; i < m_pObstacle.size(); i++)
	{
		if (m_pObstacle[i]->getHealth() != -10)
		{
			if (m_pObstacle[i] != nullptr)
			{
				if (m_pObstacle[i]->getHealth() == 0)
				{
					removeChild(m_pObstacle[i]);
					m_pObstacle[i] = nullptr;
					m_pObstacle.erase(m_pObstacle.begin() + i);
					m_pObstacle.shrink_to_fit();
				}
			}
		}
	}
	for (int i = 0; i < m_pNode.size(); i++)
		m_CheckNodeLOS(m_pNode[i]);

	//m_DecisionMaking();
	for (int i = 0; i < m_pEnemy.size(); i++)
	{
		m_CheckHuntLOS(m_pEnemy[i]);
		m_CheckEnemyDetection(m_pEnemy[i]);
		m_CheckEnemyLOS(m_pEnemy[i]);
		m_CheckEnemyFireDetection(m_pEnemy[i]);
		m_CheckTooClose(m_pEnemy[i]);
		m_CheckBehindCover(m_pEnemy[i]);
		if (m_pEnemy[i]->getHitState())
		{
			m_pEnemy[i]->SetHitTimer(m_pEnemy[i]->getHitTimer() - 1);
			if (m_pEnemy[i]->getHitTimer() <= 0)
			{
				m_pEnemy[i]->SetHitTimer(0);
				m_pEnemy[i]->setHitState(false);
			}
		}
		if (m_pEnemy[i]->getAgentType() == CLOSE_COMBAT_ENEMY)
			m_CheckCloseCombatRange(m_pEnemy[i]);
	}

	if (m_getPatrolMode())
	{
		footstepCooldown--;
		if (footstepCooldown <= 0)
		{
			footstepCooldown = 60;
			SoundManager::Instance().playSound("EWalk", 0, 2);
		}
		//decisionTree->Update();
		for (int i = 0; i < m_pEnemy.size(); i++)
		{
			if (m_pEnemy[i]->getAnimationState() != ENEMY_DAMAGE && m_pEnemy[i]->getAnimationState() != ENEMY_DEATH && m_pEnemy[i]->getAgentType() != CLOSE_COMBAT_ENEMY)
				m_pEnemy[i]->setAnimationState(ENEMY_RUN);
		}

	}

	else
	{
		for (int i = 0; i < m_pEnemy.size(); i++)
		{
			if (m_pEnemy[i]->getAnimationState() != ENEMY_DAMAGE && m_pEnemy[i]->getAnimationState() != ENEMY_DEATH && m_pEnemy[i]->getAgentType() != CLOSE_COMBAT_ENEMY)
				m_pEnemy[i]->setAnimationState(ENEMY_IDLE);
		}
	}

	if (m_meleeActtack != nullptr)
	{
		m_meleeActtack->getTransform()->position = m_pHunt->getTransform()->position - glm::vec2(-10.0f, 10.f);
		m_meleeActtack->setDirection(m_pHunt->getCurrentHeading() + 90);
	}

	for (int i = 0; i < m_pEnemy.size(); i++)
	{
		if (m_pEnemy[i]->getHealth() == 0)
		{
			m_pEnemy[i]->setAnimationState(ENEMY_DEATH);
			m_setPatrolMode(false);

			break;
		}
	}

	for (int i = 0; i < m_pEnemy.size(); i++)
	{
		if (m_pEnemy[i]->getAnimationState() == ENEMY_DEATH)
			m_pEnemy[i]->deathCooldown--;

		if (m_pEnemy[i]->deathCooldown <= 0)
		{
			m_enemysKilled++;
			SoundManager::Instance().playSound("Death", 0, -1);
			m_pEnemy[i]->deathCooldown = 60;
			removeChild(m_pEnemy[i]);
			m_pEnemy[i] = nullptr;
			m_pEnemy.erase(m_pEnemy.begin() + i);
			m_pEnemy.shrink_to_fit();
			if (m_pEnemy.empty())
				TheGame::Instance()->changeSceneState(END_SCENE);

			break;
		}
	}


	for (int i = 0; i < m_pEnemy.size(); i++)
	{
		if (m_pEnemy[i]->getAnimationState() == ENEMY_DAMAGE && m_pEnemy[i]->getAgentType() != CLOSE_COMBAT_ENEMY)
			damageCooldown--;

		if (damageCooldown <= 0 && m_pEnemy[i]->getAgentType() != CLOSE_COMBAT_ENEMY)
		{
			damageCooldown = 60;
			m_pEnemy[i]->setAnimationState(ENEMY_IDLE);
		}
	}

	for (int i = 0; i < m_pEnemy.size(); i++)
	{
		if (m_pEnemy[i]->getAnimationState() == ENEMY_DAMAGE && m_pEnemy[i]->getAgentType() != RANGED_ENEMY)
			meleeDamageCooldown--;

		if (meleeDamageCooldown <= 0 && m_pEnemy[i]->getAgentType() != RANGED_ENEMY)
		{
			meleeDamageCooldown = 60;
			m_pEnemy[i]->setAnimationState(ENEMY_RUN);
		}
	}

	if (m_pHunt->getAnimationState() == PLAYER_IDLE && m_pHunt->getMoving() == true)
		m_pHunt->setAnimationState(PLAYER_RUN);
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
		TheGame::Instance()->quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_H))
	{
		if (cooldown <= -20)
		{
			m_pHunt->setDebug(!m_pHunt->getDebugState());
			for (auto node : m_pNode)
				node->setDebug(!node->getDebugState());

			for (int i = 0; i < m_pEnemy.size(); i++)
				m_pEnemy[i]->setDebug(!m_pEnemy[i]->getDebugState());

			for (auto obstacle : m_pObstacle)
				obstacle->setDebug(!obstacle->getDebug());

			cooldown = 20;
		}
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_M))
	{
		if (cooldown <= -20)
		{
			if (m_pHunt != nullptr)
			{
				if (m_pHunt->getHealth() != 3)
					m_pHunt->setHealth(m_pHunt->getHealth() + 1);
				cooldown = 20;
			}
		}
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_K))
	{
		if (cooldown <= -20)
		{
			for (int i = 0; i < m_pEnemy.size(); i++)
			{
				if (m_pEnemy[i]->getHealth() != 0)
					m_pEnemy[i]->setHealth(m_pEnemy[i]->getHealth() - 1);
				m_pEnemy[i]->setAnimationState(ENEMY_DAMAGE);
			}
			cooldown = 20;
		}
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_G))
	{
		if (cooldown <= -20)
		{
			if (m_pHunt != nullptr)
			{
				m_pHunt->setHealth(m_pHunt->getHealth() - 1);
				cooldown = 20;
			}
		}
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_P))
	{
		if (cooldown <= -20)
		{
			m_setPatrolMode(!m_getPatrolMode());
			if (m_getPatrolMode())
			{
				for (int i = 0; i < m_pEnemy.size(); i++)
					m_pEnemy[i]->setAnimationState(ENEMY_RUN);
			}
			else
			{
				for (int i = 0; i < m_pEnemy.size(); i++)
					m_pEnemy[i]->setAnimationState(ENEMY_IDLE);
			}
			cooldown = 20;
		}
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_W) || EventManager::Instance().isKeyDown(SDL_SCANCODE_S)
		|| EventManager::Instance().isKeyDown(SDL_SCANCODE_A) || EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
	{
		pfootstep--;
		if (pfootstep <= 0)
		{
			pfootstep = 30;
			SoundManager::Instance().playSound("PWalk", 0, 1);
		}
		if (m_pHunt->getAnimationState() != PLAYER_SHOOT && m_pHunt->getAnimationState() != PLAYER_MELEE)
			m_pHunt->setAnimationState(PLAYER_RUN);
		m_pHunt->setMoving(true);
		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_W))
		{
			m_pHunt->setYMoving(true);
			m_pHunt->setCurrentDirection(glm::vec2(m_pHunt->getCurrentDirection().x, -1.0f));
		}
		else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_S))
		{
			m_pHunt->setYMoving(true);
			m_pHunt->setCurrentDirection(glm::vec2(m_pHunt->getCurrentDirection().x, 1.0f));
		}
		else
			m_pHunt->setYMoving(false);

		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_A))
		{
			m_pHunt->setXMoving(true);
			m_pHunt->setCurrentDirection(glm::vec2(-1.0f, m_pHunt->getCurrentDirection().y));
		}
		else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
		{
			m_pHunt->setXMoving(true);
			m_pHunt->setCurrentDirection(glm::vec2(1.0f, m_pHunt->getCurrentDirection().y));
		}
		else
		{
			if (m_pHunt->getAnimationState() == PLAYER_RUN)
				m_pHunt->setAnimationState(PLAYER_IDLE);
			m_pHunt->setXMoving(false);
		}

	}

	else
	{
		if (m_pHunt->getAnimationState() != PLAYER_SHOOT && m_pHunt->getAnimationState() != PLAYER_MELEE)
			m_pHunt->setAnimationState(PLAYER_IDLE);
		m_pHunt->setMoving(false);
	}

	if (EventManager::Instance().getMouseButton(1))
	{
		if (meleeCoolDown <= 0)
		{
			m_pHunt->setAnimationState(PLAYER_MELEE);
			addChild(m_meleeActtack);
			m_meleeActtack->setEnabled(true);
			meleeCoolDown = 30;
		}
	}
	else
	{
		m_meleeActtack->setEnabled(false);
		m_pHunt->setAnimationState(PLAYER_IDLE);
	}
	if (cooldown <= 0 && m_pHunt->getMoving() == false && m_pHunt->getAnimationState() != PLAYER_MELEE)
		m_pHunt->setAnimationState(PLAYER_IDLE);

	if (EventManager::Instance().getMouseButton(2))
	{
		if (cooldown <= -20)
		{
			m_pHunt->setAnimationState(PLAYER_SHOOT);
			m_pPlayerBullets.push_back(new FeatherBullet(m_pHunt->getTransform()->position.x + m_pHunt->getWidth() / 2, m_pHunt->getTransform()->position.y + m_pHunt->getHeight() / 2, m_pHunt->getCurrentHeading()));
			m_pPlayerBullets[m_pPlayerBullets.size() - 1]->setRotation(m_pHunt->getCurrentHeading());
			SoundManager::Instance().playSound("Fire", 0, -1);
			cooldown = 20;
		}

	}
}

void PlayScene::start()
{
	TextureManager::Instance()->load("../Assets/textures/playScene.jpg", "Background");

	//Sound stuff
	SoundManager::Instance().load("../Assets/audio/chickenChase.ogg", "eChase", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/chickenDie.ogg", "eDie", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/chickenIdle.ogg", "eIdle", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/melee.ogg", "melee", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/ranged.ogg", "range", SOUND_SFX);
	SoundManager::Instance().load("../Assets/audio/walking.ogg", "pWalk", SOUND_SFX);

	// Set GUI Title
	m_guiTitle = "Play Scene";

	TextureManager::Instance()->load("../Assets/textures/BackgroundFramework_1.png", "Background");
	const SDL_Color colour = { 255, 0, 0, 255 };
	m_UIScore = new Label("--", "Consolas", 20, colour, glm::vec2(625.0f, 15.0f));
	m_UIScore->setParent(this);
	addChild(m_UIScore);

	m_pInstructions = new Label("--", "Consolas", 20, colour, glm::vec2(550.0f, 585.0f));
	m_pInstructions->setParent(this);
	addChild(m_pInstructions);

	std::stringstream stream;

	stream << " (H - Debug view)" << " (W,A,S,D - Moves player)";
	const std::string Score_string = stream.str();
	m_pInstructions->setText(Score_string);

	//m_pObstacle.push_back(new Obstacle(79.0f, 79.0f, "../Assets/textures/Obstacle_1.png", "Obstacle_1")); //top left obstacle

	//m_pObstacle.push_back(new Obstacle(134, 47, 0, 441));

	//m_pObstacle.push_back(new Obstacle(61, 160, 229, 440));

	//m_pObstacle.push_back(new Obstacle(536, 292, "../Assets/textures/Obstacle_3.png", "Obstacle_3"));


	//m_pObstacle.push_back(new Obstacle(503, 52, "../Assets/textures/Obstacle_2.png", "Obstacle_2"));

	for (auto obstacle : m_pObstacle)
		addChild(obstacle);

	//Node Example
	m_pNode.push_back(new Node(40, 20)); //Top Nodes

	for (int i = 0; i < m_pNode.size(); i++)
		m_pNode[i]->m_label->setText(std::to_string(i));

	for (auto node : m_pNode)
		addChild(node);

	m_pHunt = new Huntress();
	m_pHunt->getTransform()->position = glm::vec2(50.0f, 550.0f);
	addChild(m_pHunt);

	//m_pEnemy.push_back(new RCE());
	//m_pEnemy[0]->getTransform()->position = glm::vec2(10.0f, 15.0f);
	//m_pEnemy[0]->setTargetPosition(m_pNode[0]->getTransform()->position);
	//m_pEnemy[0]->setPatrol(0, 19);
	//addChild(m_pEnemy[0]);

	//m_pEnemy.push_back(new CCE());
	//m_pEnemy[1]->getTransform()->position = m_pNode[40]->getTransform()->position;
	//m_pEnemy[1]->setTargetPosition(m_pNode[41]->getTransform()->position);
	//m_pEnemy[1]->setPatrol(40, 55);
	//addChild(m_pEnemy[1]);

	// Create Decision Tree
	m_meleeActtack = new Melee(m_pHunt->getCurrentHeading());



	std::cout << "------------------------" << std::endl;
	for (auto enemy : m_pEnemy)
		enemy->getDecisionTree()->MakeDecision();
	std::cout << "------------------------\n" << std::endl;

	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::GUI_Function()
{
	// TODO:
	// Toggle Visibility for the StarShip and the Target

	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();

	ImGui::Begin("GAME3001 - M2021 - Lab 7", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

	// allow ship rotation
	static int angle;
	if (ImGui::SliderInt("Ship Direction", &angle, -360, 360))
	{
		//m_pShip->setCurrentHeading(angle);
	}

	ImGui::Separator();

	//static int shipPosition[] = { m_pShip->getTransform()->position.x, m_pShip->getTransform()->position.y };
	//if (ImGui::SliderInt2("Ship Position", shipPosition, 0, 800))
	//{
	//	m_pShip->getTransform()->position.x = shipPosition[0];
	//	m_pShip->getTransform()->position.y = shipPosition[1];
	//}

	//static int targetPosition[] = { m_pTarget->getTransform()->position.x, m_pTarget->getTransform()->position.y };
	//if (ImGui::SliderInt2("Target Position", targetPosition, 0, 800))
	//{
	//	m_pTarget->getTransform()->position.x = targetPosition[0];
	//	m_pTarget->getTransform()->position.y = targetPosition[1];
	//}

	ImGui::Separator();

	ImGui::End();
}

void PlayScene::CollisionsUpdate()
{
	srand((unsigned)time(NULL));
	for (auto& obj : m_pObstacle)
	{
		if (CollisionManager::AABBCheck(m_pHunt, obj))
		{
			if (int(m_pHunt->getTransform()->position.x + m_pHunt->getWidth() - m_pHunt->getRigidBody()->velocity.x) <= (obj->getTransform()->position.x))
				m_pHunt->getTransform()->position.x -= m_playerSpeed * 2;

			else if (int(m_pHunt->getTransform()->position.x - m_pHunt->getRigidBody()->velocity.x) >= (obj->getTransform()->position.x + obj->getWidth()))
				m_pHunt->getTransform()->position.x += m_playerSpeed * 2;

			else if ((m_pHunt->getTransform()->position.y + m_pHunt->getHeight() - m_playerSpeed) <= (obj->getTransform()->position.y))
				m_pHunt->getTransform()->position.y -= m_playerSpeed * 2;

			else if ((m_pHunt->getTransform()->position.y + m_playerSpeed) >= (obj->getTransform()->position.y + obj->getHeight()))
				m_pHunt->getTransform()->position.y += m_playerSpeed * 2;
		}
	}

	for (auto& obj : m_pObstacle)
	{
		for (int i = 0; i < m_pEnemy.size(); i++)
		{
			if (CollisionManager::AABBCheck(m_pEnemy[i], obj))
			{
				if (int(m_pEnemy[i]->getTransform()->position.x + m_pEnemy[i]->getWidth() - m_pEnemy[i]->getRigidBody()->velocity.x) <= (obj->getTransform()->position.x))
					m_pEnemy[i]->getTransform()->position.x -= m_playerSpeed * 2;

				else if (int(m_pEnemy[i]->getTransform()->position.x - m_pEnemy[i]->getRigidBody()->velocity.x) >= (obj->getTransform()->position.x + obj->getWidth()))
					m_pEnemy[i]->getTransform()->position.x += m_playerSpeed * 2;

				else if ((m_pEnemy[i]->getTransform()->position.y + m_pEnemy[i]->getHeight() - m_playerSpeed) <= (obj->getTransform()->position.y))
					m_pEnemy[i]->getTransform()->position.y -= m_playerSpeed * 2;

				else if ((m_pEnemy[i]->getTransform()->position.y + m_playerSpeed) >= (obj->getTransform()->position.y + obj->getHeight()))
					m_pEnemy[i]->getTransform()->position.y += m_playerSpeed * 2;
			}
		}

	}
	for (int i = 0; i < m_pEnemy.size(); i++)
	{
		if (m_pEnemy[i]->getTargetDistance() < 50)
			m_pEnemy[i]->setMaxSpeed(1.0f);

		else m_pEnemy[i]->setMaxSpeed(3.0f);
	}


	for (int i = 0; i < m_pPlayerBullets.size(); i++)
	{
		if (m_pPlayerBullets[i] != nullptr)
		{
			if (m_pPlayerBullets[i]->getTransform()->position.x >= 900)
			{
				removeChild(m_pPlayerBullets[i]);
				m_pPlayerBullets[i] = nullptr;
				m_pPlayerBullets.erase(m_pPlayerBullets.begin() + i);
				m_pPlayerBullets.shrink_to_fit();
				break;
			}
			if (m_pPlayerBullets[i]->getTransform()->position.x <= -100)
			{
				removeChild(m_pPlayerBullets[i]);
				m_pPlayerBullets[i] = nullptr;
				m_pPlayerBullets.erase(m_pPlayerBullets.begin() + i);
				m_pPlayerBullets.shrink_to_fit();
				break;
			}
			if (m_pPlayerBullets[i]->getTransform()->position.y <= -100)
			{
				removeChild(m_pPlayerBullets[i]);
				m_pPlayerBullets[i] = nullptr;
				m_pPlayerBullets.erase(m_pPlayerBullets.begin() + i);
				m_pPlayerBullets.shrink_to_fit();
				break;
			}
			if (m_pPlayerBullets[i]->getTransform()->position.y >= 700)
			{
				removeChild(m_pPlayerBullets[i]);
				m_pPlayerBullets[i] = nullptr;
				m_pPlayerBullets.erase(m_pPlayerBullets.begin() + i);
				m_pPlayerBullets.shrink_to_fit();
				break;
			}

			for (int i = 0; i < m_pPlayerBullets.size(); i++)
			{
				for (int j = 0; j < m_pEnemy.size(); j++)
				{
					if (CollisionManager::AABBCheck(m_pPlayerBullets[i], m_pEnemy[j]))
					{
						SoundManager::Instance().playSound("Hit", 0, -1);
						damageCooldown = 60;
						m_pEnemy[j]->setAnimationState(ENEMY_DAMAGE);
						m_pEnemy[j]->setHealth(m_pEnemy[j]->getHealth() - 1);
						m_pEnemy[j]->setHitState(true);
						m_pEnemy[i]->SetHitTimer(600);
					}
				}

			}
			for (auto obstacle : m_pObstacle)
			{
				if (CollisionManager::AABBCheck(m_pPlayerBullets[i], obstacle))
				{
					if (obstacle->getHealth() != -10)
						obstacle->setHealth(obstacle->getHealth() - 1);

					removeChild(m_pPlayerBullets[i]);
					m_pPlayerBullets[i] = nullptr;
					m_pPlayerBullets.erase(m_pPlayerBullets.begin() + i);
					m_pPlayerBullets.shrink_to_fit();
					break;
				}
			}
		}
	}

	for (int i = 0; i < m_pEnemyBullets.size(); i++)
	{
		if (m_pEnemyBullets[i] != nullptr)
		{
			if (m_pEnemyBullets[i]->getTransform()->position.x >= 900)
			{
				removeChild(m_pEnemyBullets[i]);
				m_pEnemyBullets[i] = nullptr;
				m_pEnemyBullets.erase(m_pEnemyBullets.begin() + i);
				m_pEnemyBullets.shrink_to_fit();
				break;
			}
			if (m_pEnemyBullets[i]->getTransform()->position.x <= -100)
			{
				removeChild(m_pEnemyBullets[i]);
				m_pEnemyBullets[i] = nullptr;
				m_pEnemyBullets.erase(m_pEnemyBullets.begin() + i);
				m_pEnemyBullets.shrink_to_fit();
				break;
			}
			if (m_pEnemyBullets[i]->getTransform()->position.y <= -100)
			{
				removeChild(m_pEnemyBullets[i]);
				m_pEnemyBullets[i] = nullptr;
				m_pEnemyBullets.erase(m_pEnemyBullets.begin() + i);
				m_pEnemyBullets.shrink_to_fit();
				break;
			}
			if (m_pEnemyBullets[i]->getTransform()->position.y >= 700)
			{
				removeChild(m_pEnemyBullets[i]);
				m_pEnemyBullets[i] = nullptr;
				m_pEnemyBullets.erase(m_pEnemyBullets.begin() + i);
				m_pEnemyBullets.shrink_to_fit();
				break;
			}

			for (int i = 0; i < m_pEnemyBullets.size(); i++)
			{

				if (CollisionManager::AABBCheck(m_pEnemyBullets[i], m_pHunt))
				{
					if (m_pHunt != nullptr)
						m_pHunt->setHealth(m_pHunt->getHealth() - 1);
				}
			}
			for (auto obstacle : m_pObstacle)
			{
				if (CollisionManager::AABBCheck(m_pEnemyBullets[i], obstacle))
				{
					removeChild(m_pEnemyBullets[i]);
					m_pEnemyBullets[i] = nullptr;
					m_pEnemyBullets.erase(m_pEnemyBullets.begin() + i);
					m_pEnemyBullets.shrink_to_fit();
					break;
				}
			}
		}
	}
	for (int i = 0; i < m_pEnemy.size(); i++)
	{
		if (m_pEnemy[i] != nullptr)
		{
			int randNumb = 1 + rand() % 2;
			if (m_pEnemy[i]->getTransform()->position.x >= 1000)
			{
				removeChild(m_pEnemy[i]);
				m_pEnemy[i] = nullptr;
				m_pEnemy.erase(m_pEnemy.begin() + i);
				m_pEnemy.shrink_to_fit();
				m_RespawnEnemy(randNumb);
				break;
			}
			if (m_pEnemy[i]->getTransform()->position.x <= -200)
			{
				removeChild(m_pEnemy[i]);
				m_pEnemy[i] = nullptr;
				m_pEnemy.erase(m_pEnemy.begin() + i);
				m_pEnemy.shrink_to_fit();
				m_RespawnEnemy(randNumb);
				break;
			}
			if (m_pEnemy[i]->getTransform()->position.y <= -200)
			{
				removeChild(m_pEnemy[i]);
				m_pEnemy[i] = nullptr;
				m_pEnemy.erase(m_pEnemy.begin() + i);
				m_pEnemy.shrink_to_fit();
				m_RespawnEnemy(randNumb);
				break;
			}
			if (m_pEnemy[i]->getTransform()->position.y >= 800)
			{
				removeChild(m_pEnemy[i]);
				m_pEnemy[i] = nullptr;
				m_pEnemy.erase(m_pEnemy.begin() + i);
				m_pEnemy.shrink_to_fit();
				m_RespawnEnemy(randNumb);
				break;
			}
		}
	}
}

void PlayScene::m_RespawnEnemy(int numb)
{
	switch (numb)
	{
	default:
		break;
	case 1:
		m_pEnemy.push_back(new RCE());
		m_pEnemy[m_pEnemy.size() - 1]->getTransform()->position = glm::vec2(10.0f, 15.0f);
		m_pEnemy[m_pEnemy.size() - 1]->setTargetPosition(m_pNode[0]->getTransform()->position);
		m_pEnemy[m_pEnemy.size() - 1]->setPatrol(0, 19);
		m_pEnemy[m_pEnemy.size() - 1]->setDebug(!m_getDebugMode());
		addChild(m_pEnemy[m_pEnemy.size() - 1]);
		for (auto enemy : m_pEnemy)
			enemy->getDecisionTree()->MakeDecision();
		break;

	case 2:
		m_pEnemy.push_back(new CCE());
		m_pEnemy[m_pEnemy.size() - 1]->getTransform()->position = m_pNode[40]->getTransform()->position;
		m_pEnemy[m_pEnemy.size() - 1]->setTargetPosition(m_pNode[41]->getTransform()->position);
		m_pEnemy[m_pEnemy.size() - 1]->setPatrol(40, 55);
		m_pEnemy[m_pEnemy.size() - 1]->setDebug(!m_getDebugMode());
		addChild(m_pEnemy[m_pEnemy.size() - 1]);
		for (auto enemy : m_pEnemy)
			enemy->getDecisionTree()->MakeDecision();
		break;
	}
}

void PlayScene::m_DecisionMaking(Enemy * m_agent)
{
	if (m_agent == nullptr)
	{
		return;
	}
	if (m_agent->getAgentType() == RANGED_ENEMY)
	{
		if (m_agent->getDecisionTree()->getCurrentNode()->name == "Patrol Action")
		{
			m_agent->getDecisionTree()->setCurrentAction(new PatrolAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
			if (CollisionManager::AABBCheck(m_agent, m_pNode[m_agent->getPatrolCurrent()]))
			{
				if (m_agent->getPatrolCurrent() == m_agent->getPatrolE())
					m_agent->setPatrol(m_agent->getPatrolS(), m_agent->getPatrolE());
				else
					m_agent->setPatrolCurrent(m_agent->getPatrolCurrent() + 1);
			}
			m_agent->setTargetPosition(glm::vec2(m_pNode[m_agent->getPatrolCurrent()]->getTransform()->position.x - (m_pNode[m_agent->getPatrolCurrent()]->getWidth() / 2),
				m_pNode[m_agent->getPatrolCurrent()]->getTransform()->position.y - (m_pNode[m_agent->getPatrolCurrent()]->getHeight() / 2)));
		}
		else if (m_agent->getDecisionTree()->getCurrentNode()->name == "Move To Player Action")
		{
			m_agent->getDecisionTree()->setCurrentAction(new MoveToPlayerAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
			m_agent->setTargetPosition(m_pHunt->getTransform()->position);
		}
		else if (m_agent->getDecisionTree()->getCurrentNode()->name == "Ranged Attack Action")
		{
			m_agent->getDecisionTree()->setCurrentAction(new RangeAttackAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
			if (EnemyFireCoolDown <= -20)
			{
				m_pEnemyBullets.push_back(new FeatherBullet(m_agent->getTransform()->position.x + m_agent->getWidth() / 2, m_agent->getTransform()->position.y + m_agent->getHeight() / 2, m_agent->getCurrentHeading()));
				m_pEnemyBullets[m_pEnemyBullets.size() - 1]->setRotation(m_agent->getCurrentHeading());
				EnemyFireCoolDown = 20;
			}
			m_agent->setTargetPosition(m_pHunt->getTransform()->position);
		}
		else if (m_agent->getDecisionTree()->getCurrentNode()->name == "Move to LOS Action")
		{
			m_agent->getDecisionTree()->setCurrentAction(new MoveToLOSAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
			for (auto node : m_pNode)
			{
				if (m_CheckNodeEnemyLOS(node, m_agent))
				{
					if (node->getHasLOS())
						m_agent->setTargetPosition(node->getTransform()->position);
				}
			}

		}
		else if (m_agent->getDecisionTree()->getCurrentNode()->name == "Flee Action")
		{
			m_agent->getDecisionTree()->setCurrentAction(new FleeAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
			m_agent->setTargetPosition(m_pHunt->getTransform()->position);
		}
		else if (m_agent->getDecisionTree()->getCurrentNode()->name == "Move To Range Action")
		{
			Node* tempNode = m_pNode[0];
			m_agent->getDecisionTree()->setCurrentAction(new MoveToRangeAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
			for (auto node : m_pNode)
			{
				if (m_CheckNodeEnemyLOS(node, m_agent) && node->getHasLOS())
				{
					if (Util::distance(node->getTransform()->position, m_pHunt->getTransform()->position) > m_agent->getMinFireDistance())
					{
						if (Util::distance(node->getTransform()->position, m_agent->getTransform()->position) <= Util::distance(tempNode->getTransform()->position, m_agent->getTransform()->position))
						{
							tempNode = node;
							m_agent->setTargetPosition(tempNode->getTransform()->position);
						}
					}
				}
			}
		}
		else if (m_agent->getDecisionTree()->getCurrentNode()->name == "Move Behind Cover Action") //this
		{
			m_agent->getDecisionTree()->setCurrentAction(new MoveBehindCoverAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
			for (auto node : m_pNode)
			{
				if (!node->getHasLOS())
				{
					if (!m_CheckNodeEnemyLOS(node, m_agent))
					{
						for (auto node1 : m_pNode)
						{
							if (m_CheckNodeEnemyLOS(node, node1) && !(node == node1))
							{
								m_agent->setTargetPosition(node1->getTransform()->position);
								break;
							}
							else
							{
								m_agent->setTargetPosition(node->getTransform()->position);
								break;
							}
						}
					}
					else
					{
						m_agent->setTargetPosition(node->getTransform()->position);
						break;
					}
				}
			}
		}
		else if (m_agent->getDecisionTree()->getCurrentNode()->name == "Wait Behind Cover Action")
		{
			m_agent->getDecisionTree()->setCurrentAction(new WaitBehindCoverAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
		}
	}
	else if (m_agent->getAgentType() == CLOSE_COMBAT_ENEMY)
	{
		if (m_agent->getDecisionTree()->getCurrentNode()->name == "Patrol Action")
		{
			m_agent->getDecisionTree()->setCurrentAction(new PatrolAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
			if (CollisionManager::AABBCheck(m_agent, m_pNode[m_agent->getPatrolCurrent()]))
			{
				if (m_agent->getPatrolCurrent() == m_agent->getPatrolE())
					m_agent->setPatrol(m_agent->getPatrolS(), m_agent->getPatrolE());
				else
					m_agent->setPatrolCurrent(m_agent->getPatrolCurrent() + 1);
			}
			m_agent->setTargetPosition(glm::vec2(m_pNode[m_agent->getPatrolCurrent()]->getTransform()->position.x - (m_pNode[m_agent->getPatrolCurrent()]->getWidth() / 2),
				m_pNode[m_agent->getPatrolCurrent()]->getTransform()->position.y - (m_pNode[m_agent->getPatrolCurrent()]->getHeight() / 2)));
		}
		else if (m_agent->getDecisionTree()->getCurrentNode()->name == "Move To Player Action")
		{
			m_agent->getDecisionTree()->setCurrentAction(new MoveToPlayerAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
			m_agent->setTargetPosition(m_pHunt->getTransform()->position);
		}
		else if (m_agent->getDecisionTree()->getCurrentNode()->name == "Move to LOS Action") //this
		{
			m_agent->getDecisionTree()->setCurrentAction(new MoveToLOSAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
			for (auto node : m_pNode)
			{
				if (m_CheckNodeEnemyLOS(node, m_agent))
				{
					if (node->getHasLOS())
						m_agent->setTargetPosition(node->getTransform()->position);
				}
			}
		}
		else if (m_agent->getDecisionTree()->getCurrentNode()->name == "Flee Action")
		{
			m_agent->getDecisionTree()->setCurrentAction(new FleeAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
			m_agent->setTargetPosition(m_pHunt->getTransform()->position);
		}
		//Close Combat Action
		else if (m_agent->getDecisionTree()->getCurrentNode()->name == "Close Combat Action")
		{
			m_agent->getDecisionTree()->setCurrentAction(new CloseCombatAction());
			m_agent->getDecisionTree()->getCurrentAction()->Action(m_agent);
			m_agent->setAnimationState(ENEMY_MELEE);
			m_agent->Attack();
			if ((CollisionManager::AABBCheck(m_agent, m_pHunt) && m_agent->attackCooldown <= 0))
			{
				SoundManager::Instance().playSound("CCattack", 0, -1);
				m_agent->attackCooldown = 80;
				if (m_pHunt != nullptr)
					m_pHunt->setHealth(m_pHunt->getHealth() - 1);
			}
		}
	}
}

void PlayScene::m_CheckEnemyBehindCover(Enemy * enemy)
{
}

void PlayScene::m_setUIScore()
{
	std::stringstream stream;

	stream << "Enemys alive:" << m_pEnemy.size() << " Enemeys killed:" << m_enemysKilled;
	const std::string Score_string = stream.str();
	m_UIScore->setText(Score_string);
}

void PlayScene::m_CheckHuntLOS(DisplayObject * target_object)
{
	bool collidingObstacle;
	// if Huntress to target distance is less than or equal to LOS Distance
	auto HuntToTargetDistance = Util::distance(m_pHunt->getTransform()->position, target_object->getTransform()->position);
	if (HuntToTargetDistance <= m_pHunt->getLOSDistance())
	{
		std::vector<DisplayObject*> contactList;
		for (auto object : getDisplayList())
		{
			if (object->getType() == NODE || object->getType() == NONE || object->getType() == BULLET)
				continue;
			// check if object is farther than than the target
			auto HuntToObjectDistance = Util::distance(m_pHunt->getTransform()->position, object->getTransform()->position);
			bool collidingObstacle = (CollisionManager::lineRectCheck(glm::vec2(m_pHunt->getTransform()->position.x + getWidth() / 2, m_pHunt->getTransform()->position.y + getHeight() / 2),
				(glm::vec2(m_pHunt->getTransform()->position.x + getWidth() / 2, m_pHunt->getTransform()->position.y + getHeight() / 2) + m_pHunt->getCurrentDirection() * m_pHunt->getLOSDistance()),
				object->getTransform()->position, object->getWidth(), object->getHeight()));

			if (HuntToObjectDistance <= HuntToTargetDistance && collidingObstacle)
			{
				if ((object->getType() != m_pHunt->getType()) && (object->getType() != target_object->getType()))
					contactList.push_back(object);
			}
		}
		contactList.push_back(target_object); // add the target to the end of the list
		auto hasLOS = CollisionManager::LOSCheck(m_pHunt->getTransform()->position,
			m_pHunt->getTransform()->position + m_pHunt->getCurrentDirection() * m_pHunt->getLOSDistance(), contactList, target_object);

		m_pHunt->setHasLOS(hasLOS);
	}
}

void PlayScene::m_CheckEnemyDetection(Enemy * enemy)
{
	// if Huntress to target distance is less than or equal to detection Distance
	if (enemy->getHasDetected())
	{
		enemy->setHasDetection(true);
		return;
	}
	auto HuntToTargetDistance = Util::distance(enemy->getTransform()->position, m_pHunt->getTransform()->position);
	if (HuntToTargetDistance <= enemy->getDetectionDistance())
	{
		enemy->setHasDetection(true);
		enemy->setHasDetected(true);
	}
	else
		enemy->setHasDetection(false);
}

void PlayScene::m_CheckEnemyLOS(Enemy * enemy)
{
	for (auto obj : getDisplayList())
	{
		if (obj->getType() == OBSTACLE)
		{
			if (CollisionManager::lineRectCheck(enemy->getTransform()->position, m_pHunt->getTransform()->position, obj->getTransform()->position, obj->getWidth(), obj->getHeight()))
			{
				enemy->setHasLOS(false);
				return;
			}
			else
			{
				//std::cout << "i see you";
				enemy->setHasLOS(true);
			}
		}
	}
}

void PlayScene::m_setDebugMode(bool state)
{
	m_isGridEnabled = state;
}

bool PlayScene::m_getDebugMode() const
{
	return m_isGridEnabled;
}

void PlayScene::m_setPatrolMode(bool state)
{
	m_isPatrolling = state;
}

bool PlayScene::m_getPatrolMode() const
{
	return m_isPatrolling;
}

void PlayScene::m_CheckEnemyHealth(Enemy * enemy)
{
	if (enemy->getHealth() == 1)
		enemy->setHealthState(true);

	else
		enemy->setHealthState(false);
}

void PlayScene::m_CheckCloseCombatRange(Enemy * enemy)
{
	auto HuntToTargetDistance = Util::distance(enemy->getTransform()->position, m_pHunt->getTransform()->position);
	if (HuntToTargetDistance <= 50)
		enemy->setCloseCombat(true);

	else
	{
		enemy->setCloseCombat(false);
		if (enemy->getAnimationState() != ENEMY_DAMAGE && enemy->getAnimationState() != ENEMY_DEATH)
			enemy->setAnimationState(ENEMY_RUN);
	}
}

void PlayScene::m_CheckEnemyFireDetection(Enemy * enemy)
{
	auto HuntToTargetDistance = Util::distance(enemy->getTransform()->position, m_pHunt->getTransform()->position);
	if (HuntToTargetDistance <= enemy->getFireDistance())
	{
		enemy->setCloseCombat(true);
		enemy->setIsInFireDetection(true);
		enemy->setRangedAttackState(true);
	}
	else
	{
		enemy->setCloseCombat(false);
		enemy->setIsInFireDetection(false);
		enemy->setRangedAttackState(false);
	}
}

void PlayScene::m_CheckNodeLOS(Node * node)
{
	for (auto obj : getDisplayList())
	{
		if (obj->getType() == OBSTACLE)
		{
			if (CollisionManager::lineRectCheck(node->getTransform()->position, m_pHunt->getTransform()->position, obj->getTransform()->position, obj->getWidth(), obj->getHeight()))
			{
				node->setHasLOS(false);
				return;
			}
			else
				node->setHasLOS(true);
		}
	}
}

bool PlayScene::m_CheckNodeEnemyLOS(Node * node, Agent * enemy)
{
	for (auto obj : getDisplayList())
	{
		if (obj->getType() == OBSTACLE)
		{
			if (CollisionManager::lineRectCheck(node->getTransform()->position, enemy->getTransform()->position, obj->getTransform()->position, obj->getWidth(), obj->getHeight()))
				return false;
			else
				return true;
		}
	}
	return false;
}



void PlayScene::m_CheckTooClose(Enemy * enemy)
{
	auto HuntToTargetDistance = Util::distance(enemy->getTransform()->position, m_pHunt->getTransform()->position);
	if (HuntToTargetDistance <= enemy->getMinFireDistance())
		enemy->setInrange(true);

	else
		enemy->setInrange(false);
}

void PlayScene::m_CheckBehindCover(Enemy * enemy)
{
	auto Distance = Util::distance(enemy->getTransform()->position, m_pHunt->getTransform()->position); //Distance from enemy to Huntress
	for (auto object : getDisplayList())
	{
		if (object->getType() != OBSTACLE)
			continue;

		auto DistanceToObject = Util::distance(enemy->getTransform()->position, object->getTransform()->position); //Distance from enemy to object
		auto Collision = CollisionManager::lineRectCheck(enemy->getTransform()->position, m_pHunt->getTransform()->position, object->getTransform()->position,
			object->getWidth(), object->getHeight()); //if line between Huntress and enemy hit an obstacle
		if (Collision) //if line to player hits object
		{
			enemy->setBehindCoverState(true);
			return;
		}
		else
			enemy->setBehindCoverState(false);
	}
}