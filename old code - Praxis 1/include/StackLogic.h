#pragma once

#include <Arduino.h>
#include <Stack.h>

class StackLogic {
  // left-turn = 0, straight on intersection = 1, right-turn = 2
  Stack<int> path;
  Stack<int> solution;
  Stack<int> backUpSolution;  //used when restarting after an error during solving
  int lastPath = -1;          //re-use lastPath as nextDirection for solution

public:
  //the logic simplifies the path and removes dead ends and associated turns
  void rightTurnLogic();

  void leftTurnLogic();

  void driveAheadLogic();

  void peekLastDirectionPath();

  void popLastDirectionSolution();

  boolean checkPathLength();

  boolean checkSolutionLength();

  int getLastPath();

  void savePath();

  void restoreSolution();
};
