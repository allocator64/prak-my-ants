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
	this->ants.resize(height, vector<set<shared_ptr<realAnt>>>(width));
	this->anthouse = vector<point>({{0, 0}, {width - 1, 0}, {0, height - 1}, {width - 1, height - 1}});
	this->smell.resize(height, vector<pair<int,int> >(width));
	realAnt::pManager = this;
}

void AntManager::step(int step)
{
	if (step <= 100 && step % 10 == 0) {
		for (int i = 0; i < 4; ++i)
			ants[anthouse[i].y][anthouse[i].x].insert(shared_ptr<realAnt>(new realAnt(anthouse[i], i)));
	}
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
	
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
			for (auto &ant : ants[i][j])
				gui->SetAnt(*ant->getGui());

	gui->EndPaint();

	for(int team = 0; team < teamCount; ++team)
	{
		for(int y = 0; y < height; ++y)
		{
			for(int x = 0; x < width; ++x)
			{
				auto safeIter = ants[y][x];
				for(auto & AntPtr : safeIter)
				{
					if(AntPtr->getTeamId() == team && AntPtr->getFlag() == false && !AntPtr->isFrozen())
					{
						std::cout << "Processing ant : team = " << team << std::endl;
						std::shared_ptr<antlogic::IAntLogic> logic = antlogic::IAntLogic::GetAntLogic(AntPtr->getTeamId());
						antlogic::AntSensor sensor[3][3];

						for(int dy = -1; dy <= 1; ++dy)
						{
							for(int dx = -1; dx <= 1; ++dx)
							{
								int cx = x + dx;
								int cy = y + dy;
								point pos(cx, cy);

#define SENSOR sensor[dx+1][dy+1]

								if(!inrange(cx, cy))
									SENSOR.isWall = true;
								else
								{
									if(food[cy][cx] > 0)
										SENSOR.isFood = true;
									auto it = std::find(anthouse.begin(), anthouse.end(), pos);
									if(it != anthouse.end())
									{
										if(it - anthouse.begin() == team)
										   SENSOR.isMyHill = true;
										else
											SENSOR.isEnemyHill = true;
									}
									std::set<std::shared_ptr<realAnt> > Ants = ants[cy][cx];
									for(auto &Ant : Ants)
									{
										if(Ant->getTeamId() == AntPtr->getTeamId())
											SENSOR.isFriend = true;
										else
											SENSOR.isEnemy = true;
									}
									SENSOR.smell = smell[cy][cx].first;
									SENSOR.smellIntensity = smell[cy][cx].second;
								}
							}
						}

						antlogic::AntAction res = logic->GetAction(*AntPtr->getLogic(), sensor);
						if(res.putSmell)
						{
							smell[y][x].first = res.smell;
							smell[y][x].second = 100;
						}
						for(int i = 0; i < antlogic::MAX_MEMORY; ++i)
							std::cout << (int)AntPtr->getMemory()[i] << " ";
						std::cout << std::endl;
						switch(res.actionType)
						{
						case antlogic::AntActionType::MOVE_UP:
							if(!realAnt::go(AntPtr, 0, -1))
								std::cout << "MOVE_UP failed" << std::endl;
							break;
						case antlogic::AntActionType::MOVE_LEFT:
							if(!realAnt::go(AntPtr, -1, 0))
								std::cout << "MOVE_LEFT failed" << std::endl;
							break;
						case antlogic::AntActionType::MOVE_DOWN:
							if(!realAnt::go(AntPtr, 0, 1))
								std::cout << "MOVE_DOWN failed" << std::endl;
							break;
						case antlogic::AntActionType::MOVE_RIGHT:
							if(!realAnt::go(AntPtr, 1, 0))
								std::cout << "MOVE_RIGHT failed" << std::endl;
							break;
						case antlogic::AntActionType::BITE_UP:
							if(!realAnt::bite(AntPtr, 0, -1))
								std::cout << "BITE_UP failed" << std::endl;
							break;
						case antlogic::AntActionType::BITE_LEFT:
							if(!realAnt::bite(AntPtr, -1, 0))
								std::cout << "BITE_LEFT failed" << std::endl;
							break;
						case antlogic::AntActionType::BITE_DOWN:
							if(!realAnt::bite(AntPtr, 0, 1))
								std::cout << "BITE_DOWN failed" << std::endl;
							break;
						case antlogic::AntActionType::BITE_RIGHT:
							if(!realAnt::bite(AntPtr, 1, 0))
								std::cout << "BITE_RIGHT failed" << std::endl;
							break;
						case antlogic::AntActionType::GET:
							AntPtr->chFood() = true;
							food[y][x]--;
							break;
						case antlogic::AntActionType::PUT:
							food[y][x]++;
							AntPtr->chFood() = false;
							break;
						}

						AntPtr->getFlag() = true;
					}
				}
			}
		}
	}



    for(int i = 0; i < height; ++i)
    {
        for(int j = 0; j < width; ++j)
        {
            for(auto & AntPtr : ants[i][j])
            {
                AntPtr->update();
            }
        }
    }

    // scoreboard

    for (unsigned int t = 0; t < teamCount; t++)
    {
        gui->SetTeamScore(t, food[anthouse[t].y][anthouse[t].x]);
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

void realAnt::stun()
{
    _stun = 8;
    if(hasFood())
        pManager->food[getP().y][getP().x]++;
    chFood() = false;
}

bool realAnt::go(std::shared_ptr<realAnt> Ant, int dx, int dy)
{
    int x = Ant->getP().x;
    int y = Ant->getP().y;
    if(!pManager->inrange(x + dx, y + dy))
        return false;

    Ant->getP().x += dx;
    Ant->getP().y += dy;

    pManager->ants[y + dy][x + dx].insert(Ant);
    pManager->ants[y][x].erase(Ant);

    return true;
}

bool realAnt::bite(std::shared_ptr<realAnt> Ant, int dx, int dy)
{
    int x = Ant->getP().x;
    int y = Ant->getP().y;
    if(!pManager->inrange(x + dx, y + dy))
        return false;

    if(pManager->ants[x + dx][y + dy].empty())
        return true;

    (*pManager->ants[x + dx][y + dy].begin())->stun();

    return true;
}

AntManager * realAnt::pManager = nullptr;

