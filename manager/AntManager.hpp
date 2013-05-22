#ifndef ANTMANAGER_HPP
#define ANTMANAGER_HPP

#include "IAntGUI.hpp"
#include "IAntLogic.hpp"
#include "IAntGuiImpl.hpp"
#include "Geom.hpp"

#include <memory>
#include <vector>
#include <set>

using namespace std;

class food_iterator {
    int size;
    vector<vector<int>> mas;
    int pos;
    void gen(int i, int j, int len, int fcnt) {
      if (len < 2)
        return;
      static const vector<int> di = {0, 1, 0, -1};
      static const vector<int> dj = {1, 0, -1, 0};
      for (int k = 0; k < 4; ++k) {
        int ii = i;
        int jj = j;
        for (int s = 0; s < len; ++s, ii += di[k], jj += dj[k])
          mas[ii][jj] = max(mas[ii][jj], static_cast<int>(fcnt * (1 - abs(s - len / 2.0) / (len / 2.0))));
        gen((ii - i) / 2 + i, (jj - j) / 2 + j, len / 2, fcnt / 2);
      }
    }
public:
    food_iterator(int size = 0) : size(size), mas(size, vector<int>(size, 0)), pos(0) {
      gen(size / 2, size / 2, size / 2, 1000);
    }
    food_iterator& operator++()
    {
      ++pos;
      return *this;
    }
    std::shared_ptr<antgui::Food> operator*()
    {
      int i = pos / size;
      int j = pos % size;
      return std::shared_ptr<antgui::Food>(new antgui::ConcreteFood(antgui::Point(i, j), mas[i][j]));
    }
};

class AntManager;

class realAnt : antlogic::Ant
{
    char *_mem;
    bool _hasFood;
    int _teamId;
    int _stun;
    bool _flag;
    point _p;
    static AntManager * pManager;

    friend class AntManager;
public:
    realAnt(const point &p, int teamid)
        :_mem(nullptr), _hasFood(false), _teamId(teamid), _stun(0), _flag(false), _p(p)
    {
        _mem = new char[antlogic::MAX_MEMORY];
        memset(_mem, 0, antlogic::MAX_MEMORY);
    }

    virtual bool hasFood() const{
        return _hasFood;
    }
    bool & chFood()
    {
        return _hasFood;
    }

    virtual int getTeamId() const{
        return _teamId;
    }
    virtual char * getMemory() const{
        return _mem;
    }
    virtual bool isFrozen() const{
        return _stun != 0;
    }
    virtual antgui::Point getPoint() const{
        return antgui::Point(_p.x, _p.y);
    }

    point & getP()
    {
        return _p;
    }

    antlogic::Ant * getLogic()
    {
        return dynamic_cast<antlogic::Ant *>(this);
    }
    std::shared_ptr<antgui::Ant> getGui() const
    {
        return std::shared_ptr<antgui::Ant>(new antgui::ConcreteAnt(hasFood(), isFrozen(), getPoint(), getTeamId()));
    }

    friend bool operator < (const realAnt &a, const realAnt & b)
    {
        return *(a.getGui()) < *(b.getGui());
    }

    bool & getFlag()
    {
        return _flag;
    }

    void stun();
    void update()
    {
        if(_stun > 0)
            _stun--;
        _flag = false;
    }

    static bool go(std::shared_ptr<realAnt > Ant, int dx, int dy);
    static bool bite(std::shared_ptr<realAnt > Ant, int dx, int dy);
};

class AntManager
{
    int maxAntCountPerTeam;
    int teamCount;
    int height;
    int width;
    std::shared_ptr<antgui::IAntGui> gui;
    food_iterator fit;
    vector<vector<int>> food;
    vector<vector<set<shared_ptr<realAnt>>>> ants;
    std::vector<std::vector<std::pair<int,int> > > smell;
    vector<point> anthouse;
    friend class realAnt;
public:
    AntManager(int height,
               int width,
               int teamCount,
               int maxAntCountPerTeam);

    void step(int iRun);
    void setGui(std::shared_ptr<antgui::IAntGui> gui);
    void setFoodGeneretor(std::shared_ptr<food_iterator> it);
    bool inrange(int x, int y) { return x >= 0 && y >= 0 && x < width && y < height; }
};

#endif
