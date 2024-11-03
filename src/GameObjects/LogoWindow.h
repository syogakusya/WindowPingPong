#include "GameObject.h"

class LogoWindow : public GameObject
{
public:
  LogoWindow(const char *windowName, Vector2 pos, Vector2 size, Uint32 windowFlags);
  ~LogoWindow() = default;
  void Update(float deltaTime) override;
  void Draw(SDL_Renderer *renderer) override;
  const char *GetTitle() const;
};