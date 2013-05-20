#include "AntManager.hpp"
#include <memory.h>

//
//  AntManager
//

AntManager::AntManager(int height, int width, int teamCount, int maxAntCountPerTeam)
{
	this->height = height;
	this->width = width;
	this->teamCount = teamCount;
	this->maxAntCountPerTeam = maxAntCountPerTeam;
}

void AntManager::step(int step)
{
    gui->BeginPaint();

    // gui->SetAnt(ant);
    // gui->SetFood(food);
    if (food.empty()) {
        food.resize(height, vector<int>(width));
        for (int i = 0; i < height; ++i)
            for (int j = 0; j < width; ++j, ++fit)
                food[i][j] = (*fit)->getCount();
    }

    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
            gui->SetFood(antgui::ConcreteFood(antgui::Point(i, j), food[i][j]));
    
    gui->EndPaint();

    // scoreboard

    for (unsigned int t = 0; t < teamCount; t++)
    {
        gui->SetTeamScore(t, rand() % 100);

    }
}

void AntManager::setGui(std::shared_ptr<antgui::IAntGui> gui)
{
	this->gui = gui;
}

void AntManager::setFoodGeneretor(std::shared_ptr<food_iterator> it)
{
    fit = *it;
}
