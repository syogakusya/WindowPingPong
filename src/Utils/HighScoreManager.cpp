#include "HighScoreManager.h"
#include <fstream>
#include <iostream>

HighScoreManager::HighScoreManager(const std::string &filePath)
    : mFilePath(filePath), mHighScore(0)
{
  LoadHighScore();
}

HighScoreManager::~HighScoreManager()
{
  SaveHighScore();
}

int HighScoreManager::GetHighScore() const
{
  return mHighScore;
}

void HighScoreManager::UpdateHighScore(int score)
{
  if (score > mHighScore)
  {
    mHighScore = score;
    SaveHighScore();
  }
}

void HighScoreManager::LoadHighScore()
{
  std::ifstream inFile(mFilePath);
  if (inFile.is_open())
  {
    inFile >> mHighScore;
    inFile.close();
  }
  else
  {
    mHighScore = 0;
  }
}

void HighScoreManager::SaveHighScore() const
{
  std::ofstream outFile(mFilePath, std::ios::trunc);
  if (outFile.is_open())
  {
    outFile << mHighScore;
    outFile.close();
  }
  else
  {
    std::cerr << "ハイスコアの保存に失敗しました。" << std::endl;
  }
}