#include <iostream>
#include "./Constants.h"
#include "./Game.h"
#include "../lib/glm/glm.hpp"

EntityManager manager;
SDL_Renderer* Game::renderer;

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

	isRunning = true;
	return;
}

void Game::ProcessInput(){
	SDL_Event event;
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

	// call manager.update to update all entities
}

void Game::Render(){
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);
	
	// call manager.render to render all entities
	

	SDL_RenderPresent(renderer);
}

void Game::Destroy(){
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
