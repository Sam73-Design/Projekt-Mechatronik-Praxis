#include "../include/StackLogic.h"
#include "../include/Config.h"
#include "../include/RobotState.h"

void StackLogic::rightTurnLogic() {
    switch (lastPath) {
      case (-2):
        abortRun = true;
        break;
      case (-1):
        path.push(2);
        break;
      case 0:
        path.pop();
        peekLastDirectionPath();
        break;
      case 1:
        path.pop();
        path.push(0);
        lastPath = -1;
        break;
      case 2:
        path.pop();
        path.push(1);
        lastPath = -1;
        break;
    }
  }

void StackLogic::leftTurnLogic() {
    switch (lastPath) {
      case (-2):
        abortRun = true;
        break;
      case (-1):
        path.push(0);
        break;
        /*
          not possible or already covered
        case 0:
          break;
          
        case 1:
          break;
          */
      case 2:
        path.pop();
        peekLastDirectionPath();
        break;
    }
  }

void StackLogic::driveAheadLogic() {
    switch (lastPath) {
      case (-2):
        abortRun = true;
        break;
      case (-1):
        path.push(1);
        break;
        /*
          not possible or already covered
        case 0:
          break;
          */
      case 1:
        path.pop();
        peekLastDirectionPath();
        break;
      case 2:
        path.pop();
        path.push(0);
        lastPath = -1;
        break;
    }
  }

void StackLogic::peekLastDirectionPath() {
    lastPath = (path.length() >= 1) ? path.peek() : -2;
  }

void StackLogic::popLastDirectionSolution() {
    lastPath = (solution.length() >= 1) ? solution.pop() : -2;
  }

boolean StackLogic::checkPathLength() {
    return (path.length() >= 1);
  }

boolean StackLogic::checkSolutionLength() {
    return (solution.length() >= 1);
  }

int StackLogic::getLastPath() {
    return lastPath;
  }

void StackLogic::savePath() {
    if (path.length() < solution.length() || solution.length() == 0) {
      Stack<int> temp;
      temp.concat(path);
      while (temp.length() > 0) {
        // reverse Stack
        int tmp = temp.pop();
        solution.push(tmp);
        backUpSolution.push(tmp);
      }
    }
  }

void StackLogic::restoreSolution() {
    solution.clear();
    solution.concat(backUpSolution);
  }
