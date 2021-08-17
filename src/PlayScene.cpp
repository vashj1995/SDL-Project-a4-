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

}


void PlayScene::update()
{
	updateDisplayList();

	//m_CheckShipLOS(m_pTarget);
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

//	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_W) || EventManager::Instance().isKeyDown(SDL_SCANCODE_S)
//		|| EventManager::Instance().isKeyDown(SDL_SCANCODE_A) || EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
//	{
//		pfootstep--;
//		if (pfootstep <= 0)
//		{
//			pfootstep = 30;
//			SoundManager::Instance().playSound("PWalk", 0, 1);
//		}
//		if (m_pHunt->getAnimationState() != PLAYER_SHOOT && m_pHunt->getAnimationState() != PLAYER_MELEE)
//			m_pHunt->setAnimationState(PLAYER_RUN);
//		m_pHunt->setMoving(true);
//		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_W))
//		{
//			m_pHunt->setYMoving(true);
//			m_pHunt->setCurrentDirection(glm::vec2(m_pHunt->getCurrentDirection().x, -1.0f));
//		}
//		else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_S))
//		{
//			m_pHunt->setYMoving(true);
//			m_pHunt->setCurrentDirection(glm::vec2(m_pHunt->getCurrentDirection().x, 1.0f));
//		}
//		else
//			m_pHunt->setYMoving(false);
//
//		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_A))
//		{
//			m_pHunt->setXMoving(true);
//			m_pHunt->setCurrentDirection(glm::vec2(-1.0f, m_pHunt->getCurrentDirection().y));
//		}
//		else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
//		{
//			m_pHunt->setXMoving(true);
//			m_pHunt->setCurrentDirection(glm::vec2(1.0f, m_pHunt->getCurrentDirection().y));
//		}
//		else
//		{
//			if (m_pHunt->getAnimationState() == PLAYER_RUN)
//				m_pHunt->setAnimationState(PLAYER_IDLE);
//			m_pHunt->setXMoving(false);
//		}
//
//	}
//
//	else
//	{
//		if (m_pHunt->getAnimationState() != PLAYER_SHOOT && m_pHunt->getAnimationState() != PLAYER_MELEE)
//			m_pHunt->setAnimationState(PLAYER_IDLE);
//		m_pHunt->setMoving(false);
//	}
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

	const SDL_Color white = { 255,255,255, 255 };

	m_pInstructions = new Label("(P - Enemy patrol) (H - Debug view) (W,A,S,D - Moves player)", "IMMORTAL", 15, white, glm::vec2(400.0f, 540.0f));
	m_pInstructions->setParent(this);
	addChild(m_pInstructions);

	//She struggles with running animation but it's a start
	//m_pHunt = new Huntress();
	//m_pHunt->getTransform()->position = glm::vec2(740.0f, 550.0f);
	//addChild(m_pHunt, 2);
	//m_huntFacingRight = true;

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

//void PlayScene::m_CheckShipLOS(DisplayObject* target_object)
//{
//	m_pShip->setHasLOS(false);
//	auto ShipToTargetDistance = Util::distance(m_pShip->getTransform()->position, m_pTarget->getTransform()->position);
//	if (ShipToTargetDistance <= m_pShip->getLOSDistance())
//	{
//		std::vector<DisplayObject*> contactList;
//		for (auto object : getDisplayList())
//		{
//
//			auto ShipToObjectDistance = Util::distance(m_pShip->getTransform()->position, object->getTransform()->position);
//			if (ShipToObjectDistance <= ShipToTargetDistance)
//			{
//				if ((object->getType() != m_pShip->getType()) && (object->getType() !=target_object->getType()))
//				{
//					contactList.push_back(object);
//				}
//			}
//		}
//		contactList.push_back(target_object);
//		auto hasLOS = CollisionManager::LOSCheck(m_pShip->getTransform()->position, m_pShip->getTransform()->position + m_pShip->getCurrentDirection() * m_pShip->getLOSDistance(), contactList, target_object);
//		m_pShip->setHasLOS(hasLOS);
//
//	}
//}