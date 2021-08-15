#include "StartScene.h"
#include <algorithm>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"
#include "EventManager.h"
#include "SoundManager.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"

StartScene::StartScene()
{
	StartScene::start();

	//Game Music
	SoundManager::Instance().load("../Assets/audio/title.ogg", "title", SOUND_MUSIC);
	SoundManager::Instance().allocateChannels(16);
	SoundManager::Instance().setMusicVolume(15);
	SoundManager::Instance().setSoundVolume(8);
	SoundManager::Instance().playMusic("title", -1, 0);

	//GABE: Sound shenangians to add later
	//Clicking Sound Effect
	SoundManager::Instance().load("../Assets/audio/confirm.wav", "confirm", SOUND_SFX);
}

StartScene::~StartScene()
= default;

void StartScene::draw()
{
	TextureManager::Instance()->draw("title",
		0, getTransform()->position.y, 0, 255, false);

	drawDisplayList();

	SDL_SetRenderDrawColor(Renderer::Instance()->getRenderer(), 255, 255, 255, 255);
}

void StartScene::update()
{
	updateDisplayList();
}

void StartScene::clean()
{
	removeAllChildren();
}

void StartScene::handleEvents()
{
	EventManager::Instance().update();

	// Keyboard Events
	if(EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance()->quit();
	}

	if(EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance()->changeSceneState(PLAY_SCENE);
	}
}

void StartScene::start()
{
	//Dem asset bois. 
	TextureManager::Instance()->load("../Assets/textures/title.png", "title");

	//Sound bois
	SoundManager::Instance().load("../Assets/audio/throw.ogg", "confirm", SOUND_SFX);

	const SDL_Color teal = { 0, 170, 145, 255 };

	m_pInstructionsLabel = new Label("Game 3001 Assignment 3", "IMMORTAL", 60, teal, glm::vec2(400.0f, 80.0f));
	m_pInstructionsLabel->setParent(this);
	addChild(m_pInstructionsLabel);

	m_pStudentLabel = new Label("Vashj Hunter - 100878278", "IMMORTAL", 40, teal, glm::vec2(400.0f, 350.0f));
	m_pStudentLabel->setParent(this);
	addChild(m_pStudentLabel);

	m_pStudentLabel = new Label("Gabriel Kandziorra - 101003585", "IMMORTAL", 40, teal, glm::vec2(400.0f, 400.0f));
	m_pStudentLabel->setParent(this);
	addChild(m_pStudentLabel);

	// Start Button
	m_pStartButton = new Button();
	m_pStartButton->getTransform()->position = glm::vec2(400.0f, 210.0f);

	m_pStartButton->addEventListener(CLICK, [&]()-> void
		{
			m_pStartButton->setActive(false);
			TheGame::Instance()->changeSceneState(PLAY_SCENE);
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

}

