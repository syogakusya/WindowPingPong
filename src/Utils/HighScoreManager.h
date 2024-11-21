#pragma once
#include <string>

class HighScoreManager
{
public:
  HighScoreManager(const std::string &filePath);
  ~HighScoreManager();

  int GetHighScore() const;
  void UpdateHighScore(int score);

private:
  std::string mFilePath;
  int mHighScore;

  void LoadHighScore();
  void SaveHighScore() const;
};
