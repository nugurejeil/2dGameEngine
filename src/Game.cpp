#include <iostream>
#include "./Constants.h"
#include "./Game.h"
#include "./AssetManager.h"
#include "./Map.h"
#include "./Components/TransformComponent.h"
#include "./Components/SpriteComponent.h"
#include "./Components/ColliderComponent.h"
#include "./Components/KeyboardControlComponent.h"
#include "./Components/TextLabelComponent.h"
#include "./Components/ProjectileEmitterComponent.h"
#include "../lib/glm/glm.hpp"


EntityManager manager;
AssetManager* Game::assetManager = new AssetManager(&manager);
SDL_Renderer* Game::renderer;
SDL_Event Game::event;
SDL_Rect Game::camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Map* map;

Game::Game(){
	this->isRunning = false;

}

Game::~Game(){

}

bool Game::IsRunning() const{
	return this->isRunning;
}


void Game::Initialize(int width, int height){
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cerr << "Error initializing SDL" << std::endl;
		return;
	}
	if(TTF_Init() != 0){
		std::cerr << "Error initializing SDL TTF" << std::endl;
	}
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_BORDERLESS
	);
	if(!window){
		std::cerr << "Error Creating SDL window" << std::endl;
		return;
	}
	renderer = SDL_CreateRenderer(window,-1,0);
	if (!renderer){
		std::cerr << "Error creating SDL renderer" << std::endl;
		return;
	}
	
	LoadLevel(1);

	isRunning = true;
	return;
}

Entity& player(manager.AddEntity("chopper", PLAYER_LAYER));

void Game::LoadLevel(int levelNumber){
/*
	// Load Assets from lua
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::math);

	std::string levelName = "Level" + std::to_string(levelNumber);
	lua.script_file("./assets/scripts/" + levelName + ".lua");

	sol::table levelData = lua[levelName];
	sol::table levelAssets = levelData["assets"];
	unsigned int assetIndex = 0;

	while(true){
		sol::optional<sol::table> existsAssetIndexNode = levelAssets[assetIndex];
		if(existsAssetIndexNode == sol::nullopt){
			break;
		}else{
			sol::table asset = levelAssets[assetIndex];
			std::string assetType = asset["type"];
			if(assetType.compare("texture") == 0){
				std::string assetId = asset["id"];
				std::string assetFile = asset["file"];
				assetManager->AddTexture(assetId, assetFile.c_str());
			}
		}
		assetIndex++;
	}
	
	sol::table levelMap = levelData["map"];
	std::string mapTextureId = levelMap["textureAssetId"];
	std::string mapFile = levelMap["file"];

	map = new Map(
		mapTextureId,
		static_cast<int>(levelMap["scale"]),
		static_cast<int>(levelMap["tileSize"])
		);
	map->LoadMap(
		mapFile,
		static_cast<int>(levelMap["mapSizeX"]),
		static_cast<int>(levelMap["mapSizeY"])
		);
		*/
	assetManager->AddTexture("tank-image", std::string("./assets/images/tank-big-right.png").c_str());
	assetManager->AddTexture("chopper-image", std::string("./assets/images/chopper-spritesheet.png").c_str());
	assetManager->AddTexture("jungle-tiletexture", std::string("./assets/tilemaps/jungle.png").c_str());
	assetManager->AddTexture("heliport-image", std::string("./assets/images/heliport.png").c_str());
	assetManager->AddTexture("projectile-image", std::string("./assets/images/bullet-enemy.png").c_str());
    assetManager->AddFont("charriot-font", std::string("./assets/fonts/charriot.ttf").c_str(), 14);	
	map = new Map("jungle-tiletexture", 2, 32);
	map->LoadMap("./assets/tilemaps/jungle.map", 25, 20);

	player.AddComponent<TransformComponent>(240, 106, 0,0,32,32,1);
	player.AddComponent<SpriteComponent>("chopper-image", 2, 90, true, false);
	player.AddComponent<KeyboardControlComponent>("up", "right", "down", "left", "space");
	player.AddComponent<ColliderComponent>("PLAYER", 240, 106, 32, 32);

	// add entities and add component
	Entity& tankEntity(manager.AddEntity("tank", ENEMY_LAYER));
	tankEntity.AddComponent<TransformComponent>(150, 495, 0, 0, 32, 32, 1);
	tankEntity.AddComponent<SpriteComponent>("tank-image");
	tankEntity.AddComponent<ColliderComponent>("ENEMY", 150, 495, 32, 32);

	Entity& projectile(manager.AddEntity("projectile", PROJECTILE_LAYER));
	projectile.AddComponent<TransformComponent>(150+16, 495+16, 0, 0, 4, 4, 1);
	projectile.AddComponent<SpriteComponent>("projectile-image");
	projectile.AddComponent<ColliderComponent>("PROJECTILE", 150+16, 495+16, 4, 4);
	projectile.AddComponent<ProjectileEmitterComponent>(50, 270, 200, true);

	Entity& heliport(manager.AddEntity("Heliport", OBSTACLE_LAYER));
    heliport.AddComponent<TransformComponent>(470, 420, 0, 0, 32, 32, 1);
    heliport.AddComponent<SpriteComponent>("heliport-image");
    heliport.AddComponent<ColliderComponent>("LEVEL_COMPLETE", 470, 420, 32, 32);
	
	Entity& labelLevelName(manager.AddEntity("LabelLevelName", UI_LAYER));
	labelLevelName.AddComponent<TextLabelComponent>(10, 10, "Level 1", "charriot-font", WHITE_COLOR);

}

void Game::ProcessInput(){
	SDL_PollEvent(&event);
	switch (event.type){
		case SDL_QUIT: {
			isRunning = false;
			break;
		}
		case SDL_KEYDOWN:{
			if(event.key.keysym.sym == SDLK_ESCAPE){
				isRunning = false;
			}
		}
		default:{
			break;
		}
	}
}

void Game::Update(){
	
	// using SDL_Delay to sleep execution
	//int unsigned timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - ticksLastFrame);

	//if(timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME){
		//SDL_Delay(timeToWait);
	//}

	// wait until 16ms has ellapsed since the last frame
	while(!SDL_TICKS_PASSED(SDL_GetTicks(), ticksLastFrame + FRAME_TARGET_TIME));

	// SDL_GetTicks : number of milliseconds since the SDL library initialized
	float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

	// clamp deltaTime to a maximum value
	deltaTime = (deltaTime > 0.05f) ? 0.05f : deltaTime;
	
	// sets the new ticks for current frame to be used next
	ticksLastFrame = SDL_GetTicks();
     
	manager.Update(deltaTime);

	HandleCameraMovement();
	CheckCollisions();
}

void Game::Render(){
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);
	
	if(manager.HasNoEntities()){
		return;
	}	
	manager.Render();

	SDL_RenderPresent(renderer);
}

void Game::HandleCameraMovement(){
	TransformComponent* mainPlayerTransform = player.GetComponent<TransformComponent>();

	camera.x = mainPlayerTransform->position.x - (WINDOW_WIDTH /2);
	camera.y = mainPlayerTransform->position.y - (WINDOW_HEIGHT /2);

	camera.x = camera.x < 0 ? 0 : camera.x;
	camera.y = camera.y < 0 ? 0 : camera.y;
	camera.x = camera.x > camera.w ? camera.w : camera.x;
	camera.y = camera.y > camera.h ? camera.h : camera.y;
}

void Game::CheckCollisions() {
    CollisionType collisionType = manager.CheckCollisions();
    if (collisionType == PLAYER_ENEMY_COLLISION) {
        ProcessGameOver();
    }
	if (collisionType == PLAYER_PROJECTILE_COLLISION){
		ProcessGameOver();
	}
    if (collisionType == PLAYER_LEVEL_COMPLETE_COLLISION) {
        ProcessNextLevel(1);
    }
}

void Game::ProcessNextLevel(int levelNumber) {
    std::cout << "Next Level" << std::endl;
    isRunning = false;
}

void Game::ProcessGameOver() {
    std::cout << "Game Over" << std::endl;
    isRunning = false;
}

void Game::Destroy(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
