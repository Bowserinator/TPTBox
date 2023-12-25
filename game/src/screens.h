#ifndef SCREENS_H
#define SCREENS_H

#define SCREEN_METHODS \
    void init() override;\
    void update() override;\
    void draw() override;\
    void unload() override;\

class Screen {
public:
    virtual ~Screen() {};
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void unload() = 0;
};

class ScreenGameplay : public Screen { SCREEN_METHODS };

#endif // SCREENS_H