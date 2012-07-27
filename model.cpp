/*
	game model class
	
	(c) roman filippov, 2012
*/
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include "model.h"
#include "gamer.h"
#include "ai.h"
#include "view.h"
#include "application.h"
#include "gameexception.h"

Model::Model(Application *apl, Options *opt) : AppLayer(apl), board(NULL), options(opt) {
	view = View::getInstance(apl);
	
   if (!gamerImg.LoadFromFile(GAME_RESOURCES(player_game.png)))
   	throw new GameException("no file to load: player_game.png");
 	gamerSpr = sf::Sprite(gamerImg);   	
   
   if (!boxImg.LoadFromFile(GAME_RESOURCES(box_game.png)))
   	throw new GameException("no file to load: box_game.png");
	boxSpr = sf::Sprite(boxImg);   	
   
   if (!cellImg.LoadFromFile(GAME_RESOURCES(cell_game.png)))
   	throw new GameException("no file to load: cell_game.png");
   cellSpr = sf::Sprite(cellImg); 
   
   if (!computerImg.LoadFromFile(GAME_RESOURCES(computer_game.png)))
   	throw new GameException("no file to load: computer_game.png");
   computerSpr = sf::Sprite(computerImg);
   
  	MAP_SIZE = options->getSettings()->size;
  	FIELD_SIZE = MAP_SIZE;
  	unsigned int IMAGE_SIZE = gamerImg.GetWidth();
  	
  	int msize = FIELD_SIZE*IMAGE_SIZE - app->GetHeight();
  	
  	printf("MAP_SIZE=%d\n",MAP_SIZE);
  	
  	if (msize > 0)
  		FIELD_SIZE -= ceil((msize+0.0f)/IMAGE_SIZE);
  	printf("FIELD_SIZE=%d\n",FIELD_SIZE);
}

int Model::step() {

	drawMap();
	int res;
	while (1) {
		for(vector<PlayerPtr>::iterator it=players.begin()+1; it!=players.end(); ++it) {
			//usleep(1000*300);
			if((res = players.begin()->get()->turn()) != GAME_RUNNING) 
				return res;
			
			if((res = it->get()->turn()) != GAME_RUNNING)	 //Each player implements his own turn() method (ai or real)
				if (res != GAME_NO_WAY)
					return res;								
				
			drawMap();
		}
	}
}

void Model::drawMap() {
	
	app->Clear(sf::Color::Color(100,100,100));
	
	unsigned int height = app->GetHeight();
	unsigned int IMAGE_SIZE = gamerImg.GetWidth();
	
   double h = height/2-FIELD_SIZE*IMAGE_SIZE/2;
   double w = app->GetWidth()/2-FIELD_SIZE*IMAGE_SIZE/2;
	
	app->Draw(sf::Shape::Rectangle(w-2, h-2, w+FIELD_SIZE*IMAGE_SIZE+2, h+FIELD_SIZE*IMAGE_SIZE+2, sf::Color::Color(228,228,149)));
	
	app->Draw(sf::Shape::Rectangle(w, h, w+FIELD_SIZE*IMAGE_SIZE, h+FIELD_SIZE*IMAGE_SIZE, sf::Color::Color(100,100,100)));
	
	pair<int,int> pos = getPlayerPosition();
	int i_now = pos.first/FIELD_SIZE*FIELD_SIZE;
	int j_now = pos.second/FIELD_SIZE*FIELD_SIZE;
	int max_value_i = i_now+FIELD_SIZE;
	int max_value_j = j_now+FIELD_SIZE;
	
	if (max_value_i > MAP_SIZE)
		max_value_i = MAP_SIZE;
	if (max_value_j > MAP_SIZE)
		max_value_j = MAP_SIZE;
	
//	printf("i=%d|j=%d|imax=%d|jmax=%d|\n",i_now,j_now,max_value_i,max_value_j);
	
	for (int i=i_now;i<max_value_i;++i) {
		for (int j=j_now;j<max_value_j;++j) {
			if (getState(i,j) == GAME_EMPTY_CELL)
				continue;
			if (getState(i,j) == GAME_WALL) {
				boxSpr.SetPosition(w+(j%FIELD_SIZE)*IMAGE_SIZE,h+(i%FIELD_SIZE)*IMAGE_SIZE);
				app->Draw(boxSpr);
			}
			else if (getState(i,j) == GAME_PLAYER) {
			
				double rot = players.begin()->get()->rotation;
				
				gamerSpr.SetCenter(gamerImg.GetWidth()/2,gamerImg.GetHeight()/2);		
				gamerSpr.SetRotation(rot);
				gamerSpr.SetPosition(w+(j%FIELD_SIZE+0.5)*IMAGE_SIZE,h+(i%FIELD_SIZE+0.5)*IMAGE_SIZE);

				app->Draw(gamerSpr);			
			}			
			else if (getState(i,j) == GAME_ENEMY) {
				for(vector<PlayerPtr>::iterator it = players.begin()+1; it!=players.end(); ++it) {
					if (it->get()->getX() == i && it->get()->getY() == j) {
						double rot = it->get()->rotation;
						
						computerSpr.SetCenter(gamerImg.GetWidth()/2,gamerImg.GetHeight()/2);		
						computerSpr.SetRotation(rot);
						computerSpr.SetPosition(w+(j%FIELD_SIZE+0.5)*IMAGE_SIZE,h+(i%FIELD_SIZE+0.5)*IMAGE_SIZE);
				
						break;
					}
				}
				app->Draw(computerSpr);
			}
		}
	}
	app->Display();
}

bool Model::checkPaths() {
	for(vector<PlayerPtr>::iterator it=players.begin()+1; it!=players.end(); ++it) {
		if (static_cast<Ai*>(it->get())->test_turn() == GAME_NO_WAY)
			return false;
	}
	return true;
}

void Model::createWalls() {
	srand( time (NULL) );	
	printf("Creating walls...");
	
	int chance = options->getSettings()->walls;
	
	switch (chance) {
		case 0:
			chance = 9;
			break;
		case 1:
			chance = 6;
			break;
		case 2:
			chance = 3;
	}
	
	for(int i=0;i<MAP_SIZE;i++)
		for(int j=0;j<MAP_SIZE;j++)		
			board[i][j] = (rand()%chance == 1) ? GAME_WALL : GAME_EMPTY_CELL;
	
	printf("Done\n");
}

void Model::createPlayers(int computers) {
	printf("Creating players...");
	
	players.clear();
	players.reserve(computers+1);

	addPlayer(new Gamer(this,0,0));									

	for (int i=0;i<computers;++i) {
		addPlayer(new Ai(this,FIELD_SIZE-1-i,FIELD_SIZE-1));
	}
					
	printf("Done\n");
}

void Model::createWorld() {
	printf("Creating world...");
	
	board = new char* [MAP_SIZE];							//Creating map array
	for(int i=0;i<MAP_SIZE;++i)
		board[i] = new char [MAP_SIZE]();
		
//	for(int i=0;i<FIELD_SIZE;i++)
//		for(int j=0;j<FIELD_SIZE;j++)		
//			board[i][j] = GAME_EMPTY_CELL;
	printf("Done\n");
}

Model::~Model() {

	for(int i=0;i<MAP_SIZE;++i)
		delete[] board[i];

	delete[] board;
	
	view = NULL;
	options = NULL;
}

bool Model::addPlayer(Player *p) {
	int x = p->getX(),
		 y = p->getY(),
		 ptype = p->player_type;
		 
//	if (getState(x,y) == GAME_EMPTY_CELL) {

	setState(x, y, ptype);
	p->setBoard(this);
	players.push_back(PlayerPtr(p));
	return true;
	
//	}
//	return false;
}
	

