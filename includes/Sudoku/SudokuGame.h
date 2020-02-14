
#pragma once

#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <functional> // std::ref

#include "SudokuEngine.h"

static std::mutex writting_board_mutex;
static bool is_work_done = false;

namespace wubinboardgames
{
  namespace sudoku
  {
    void DisplayOptionsMenu();

    template<typename GameBoard>
    bool LoadGameFromFile(GameBoard & board)
    {
      std::string path;
      std::cout << "\033[1;32mPlease specify the path to the game file:\033[0m" << std::endl << std::endl;
      std::cin.clear();
      std::cin >> path;
      std::cout << std::endl <<std::endl;
      if(board.loadFromFile(path))
        return true;
      return false;
    }

    template<typename GameBoard>
    bool WriteGameToFile(GameBoard & board)
    {
      std::string path;
      std::cout << "\033[1;32mPlease specify the path to the game file:\033[0m" << std::endl << std::endl;
      std::cin.clear();
      std::cin >> path;
      std::cout << std::endl <<std::endl;
      if(board.writeToFile(path))
        return true;
      return false;
    }

    template<typename GameBoard>
    void RoutineToGenerateBoard(GameBoard & board, const LEVEL & level)
    {
      GameBoard work_board;
      work_board = GenerateSolvableBoard<GameBoard>(level);
      writting_board_mutex.lock();
      if(is_work_done == false)
      {
        board = work_board;
        is_work_done = true;
      }
      writting_board_mutex.unlock();
    }

    template<typename GameBoard>
    bool GenerateNewGame(GameBoard & board)
    {
      std::cout << "\033[1;32mPlease Select The Difficulty Level:\033[0m" <<std::endl << std::endl;
      unsigned int option = 100;
      std::cout << "\033[1;33m0. Easy \033[0m\033[0m" << std::endl << std::endl;
      std::cout << "\033[1;33m1. Medium " << std::endl << std::endl;
      std::cout << "\033[1;33m2. Hard \033[0m" << std::endl << std::endl;
      std::cout << "\033[1;33m3. Samurai \033[0m" << std::endl<< std::endl;
      LEVEL level;
      while(option > 3)
      {
        std::cout << "\033[1;32mPlease Select A Valid Option: \033[0m" << std::endl << std::endl;
        std::cin.clear();
        std::cin >> option;
        std::cout << std::endl <<std::endl;
      }
      switch (option)
      {
        case 0:
          level = LEVEL::EASY;
          break;
        case 1:
          level = LEVEL::MEDIUM;
          break;
        case 2:
          level = LEVEL::HARD;
          break;
        case 3:
          level = LEVEL::SAMURAI;
        default:
          break;
      }
      writting_board_mutex.lock();
      is_work_done = false;
      writting_board_mutex.unlock();

      std::thread work_threads[4];
      for(auto i = 0; i < 4; ++i)
      {
        work_threads[i] = std::thread(RoutineToGenerateBoard<GameBoard>, std::ref(board), std::ref(level));
        work_threads[i].detach();
      }
      std::cout <<"Four threads start..." << std::endl << std::endl;
      unsigned int percent = 0;
      std::cout <<"Generating..." << percent <<" %" << std::endl << std::endl;
      while(!is_work_done)
      {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        percent += 10;
        std::cout <<"Generating..." << percent <<" %" << std::endl << std::endl;
      }
      std::cout << std::endl <<"New Board Is Generated: " << std::endl << std::endl;
      std::cout << board << std::endl << std::endl;
      return true;
    }

    template<typename GameBoard>
    void PlaySudokuGame(const std::string & name_of_game_type = "Regular Soduku")
    {
      std::cout << name_of_game_type <<" Play!" <<std::endl << std::endl;
      unsigned int option = 100;
      GameBoard board;
      GameBoard back_up_baord;
      while(true)
      {
        std::cout << std::endl;
        std::cout << "\033[1;32mOperation Selection: \033[0m" << std::endl << std::endl;
        std::cout << "\033[1;33m0. Load Game From File \033[0m" << std::endl << std::endl;
        std::cout << "\033[1;33m1. Sovle Game \033[0m" << std::endl << std::endl;
        std::cout << "\033[1;33m2. Check Solutions \033[0m" << std::endl << std::endl;
        std::cout << "\033[1;33m3. Evaluate The Game \033[0m" << std::endl << std::endl;
        std::cout << "\033[1;33m4. Genereate A New Game \033[0m" << std::endl<< std::endl;
        std::cout << "\033[1;33m5. Write Game To File \033[0m" << std::endl << std::endl;
        std::cout << "\033[1;33m6. Print Current Game \033[0m" << std::endl << std::endl;
        std::cout << "\033[1;33m7. Reset Current Game \033[0m" << std::endl << std::endl;
        std::cout << "\033[1;33m8. Generate A Random Board(may not be unique) \033[0m" << std::endl << std::endl;
        std::cout << "\033[1;33m9. Return To Main Menu \033[0m" << std::endl << std::endl;

        while(option > 9)
        {
          std::cout << "\033[1;32mPlease Select A Valid Option: \033[0m" << std::endl << std::endl;
          std::cin.clear();
          std::cin >> option;
          std::cout << std::endl <<std::endl;
        }
        switch (option)
        {
          case 0:
          {
            if(LoadGameFromFile<GameBoard>(board))
            {
              std::cout << "\033[1;32mLoading From File Succeeded \033[0m" << std::endl << std::endl;
              std::cout << board;
            }
            else
            {
              std::cout << "\033[1;41mLoading From File Failed \033[0m" << std::endl << std::endl;
            }
            break;
          }
          case 1:
          case 2:
          {
            std::vector<GameBoard> solutions = SearchSolution<GameBoard>(board);
            if(1 == solutions.size())
            {
              std::cout <<"The board has a unique solution: " << std::endl << std::endl;
              if(option == 1)
              {
                back_up_baord = board;
                board = solutions[0];
                std::cout << board <<std::endl << std::endl;
              }
            }
            else if(2 == solutions.size())
            {
              std::cout <<"The board has more than one solution, these are two of them: " << std::endl << std::endl;
              std::cout <<"Solution 1" << std::endl << std::endl;
              std::cout << solutions[0] << std::endl << std::endl;
              std::cout <<"Solution 2" << std::endl << std::endl;
              std::cout << solutions[1] << std::endl << std::endl;
            }
            else
            {
              std::cout <<"The board has no solution: " << std::endl << std::endl;
            }
            break;
          }
          case 3:
          {
            switch (LevelEvaluate<GameBoard>(board))
            {
              case LEVEL::NO_SOLUTION:
                std::cout <<"\033[1;41mNo solution is found. \033[0m "<< std::endl << std::endl;
                break;
              case LEVEL::NO_UNIQUE_SOLUTION:
                std::cout <<"\033[1;41mSolution is not unique. \033[0m "<< std::endl << std::endl;
                break;
              case LEVEL::EASY:
                std::cout <<"\033[1;32mIt is an easy-level game. \033[0m" << std::endl << std::endl;
                break;
              case LEVEL::MEDIUM:
                std::cout <<"\033[1;33mIt is a medium-level game. \033[0m" << std::endl << std::endl;
                break;
              case LEVEL::HARD:
                std::cout <<"\033[1;33mIt is a hard-level game. \033[0m" << std::endl << std::endl;
                break;
              default:
                std::cout << "\033[1;31mIt is a samurai-level game! Have fun...\033[0m" << std::endl << std::endl;
            }
            break;
          }
          case 4:
          {
            back_up_baord = board;
            GenerateNewGame<GameBoard>(board);
            break;
          }
          case 5:
          {
            if(WriteGameToFile<GameBoard>(board))
            {
              std::cout << "\033[1;32mWriting To File Succeeded \033[0m" << std::endl << std::endl;
              std::cout << board;
            }
            else
            {
              std::cout << "\033[1;41mWriting To File Failed \033[0m" << std::endl << std::endl;
            }
            break;
          }
          case 6:
          {
            std::cout << board << std::endl << std::endl;
            break;
          }
          case 7:
          {
            board = back_up_baord;
            break;
          }
          case 8:
          {
            back_up_baord = board;
            board = GenerateRandomBoard<GameBoard>();
            std::cout << board << std::endl << std::endl;
            break;
          }
          case 9:
          default:
            return;
        }
        option = 100;
      }
    }
  }
}