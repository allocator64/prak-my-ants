#ifndef ANTMANAGER_HPP
#define ANTMANAGER_HPP

#include "IAntGUI.hpp"
#include "IAntLogic.hpp"
#include "IAntGuiImpl.hpp"

#include <memory>
#include <vector>

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
        gen((ii - i + 1) / 2 + i, (jj - j + 1) / 2 + j, len / 2, fcnt / 2);
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

class AntManager
{
    int maxAntCountPerTeam;
    int teamCount;
    int height;
    int width;
    std::shared_ptr<antgui::IAntGui> gui;
    food_iterator fit;
    vector<vector<int>> food;
public:
    AntManager(int height,
               int width,
               int teamCount,
               int maxAntCountPerTeam);

    void step(int iRun);
    void setGui(std::shared_ptr<antgui::IAntGui> gui);
    void setFoodGeneretor(std::shared_ptr<food_iterator> it);
};

#endif
