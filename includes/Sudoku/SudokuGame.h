
#pragma once

#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <memory> // std::shared_ptr for multi-threading

#include "SudokuEngine.h"

/* 
  SudokuGame is a set of template functions to implement UI of Sudoku Game. 
*/

static std::mutex writting_board_mutex; //global mutex for multiple threads

namespace wubinboardgames
{
  namespace sudoku
  {
    // no template, put implementation into a translation unit
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

    /*
      The RoutineToGenerateBoard is run by four threads. Each thread tries to find
      a solvable board with a given level. Each thread has a shared_ptr refering to the
      board to be returned. board is guarded by the mutex lock.
      The first thread completing the job will change the is_work_done and set the board.
      Other threads will discard the result and simply exit.
    */
    template<typename GameBoard>
    void RoutineToGenerateBoard(std::shared_ptr<GameBoard> shared_board, std::shared_ptr<bool> is_work_done, LEVEL level)
    {
      GameBoard work_board{GenerateSolvableBoard<GameBoard>(level)};
      std::lock_guard<std::mutex> mutex_lock{writting_board_mutex};
      if(*is_work_done == false)
      {
        *shared_board = work_board;
        *is_work_done = true;
      }
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
      std::cout << "\033[1;33m4. Extreme \033[0m" << std::endl<< std::endl;
      LEVEL level;
      std::shared_ptr<GameBoard> shared_board = std::make_shared<GameBoard>();
      while(option > 4)
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
          break;
        case 4:
          level = LEVEL::EXTREME;
        default:
          break;
      }
      // set is_work_done back to false
      std::shared_ptr<bool> is_work_done_shared = std::make_shared<bool>(false);

      for(auto i = 0; i < 4; ++i)
      {
        // start 4 threads for 4-core cpu.
        // do not pass any local reference to thread.
        std::thread(RoutineToGenerateBoard<GameBoard>, shared_board, is_work_done_shared, level).detach();
      }
      std::cout <<"Four threads start..." << std::endl << std::endl;
      unsigned int percent = 0;
      std::cout <<"Generating..." << percent <<" %" << std::endl << std::endl;
      while(!(*is_work_done_shared))
      {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        percent += 10;
        // update the progress state
        std::cout <<"Generating..." << percent <<" %" << std::endl << std::endl;
      }
      std::cout << std::endl <<"New Board Is Generated: " << std::endl << std::endl;
      board = *shared_board;
      // shared_board lives longer than this function execution. It is fine as
      // it is shared_ptr.
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
        std::cout << "\033[1;33m1. Solve Game \033[0m" << std::endl << std::endl;
        std::cout << "\033[1;33m2. Check Solutions \033[0m" << std::endl << std::endl;
        std::cout << "\033[1;33m3. Evaluate The Game \033[0m" << std::endl << std::endl;
        std::cout << "\033[1;33m4. Generate A New Game \033[0m" << std::endl<< std::endl;
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
            // check solution does not fill the board.
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
              case LEVEL::SAMURAI:
                std::cout << "\033[1;31mIt is a samurai-level game! \033[0m" << std::endl << std::endl;
                break;
              default:
                std::cout << "\033[1;31mIt is an extrem-level game! Have fun...\033[0m" << std::endl << std::endl;
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
