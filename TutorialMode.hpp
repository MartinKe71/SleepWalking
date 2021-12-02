#pragma once
#include "Mode.hpp"
#include "Scene.hpp"
#include "PlayMode.hpp"
#include "Inivar.hpp"
#include "UIBGObject.hpp"

#include "Scene.hpp"

#define PI_F 3.1415926f

struct TutorialMode : Mode {
    TutorialMode();
    virtual ~TutorialMode();


    //functions called by main loop:
    virtual bool handle_event(SDL_Event const&, glm::uvec2 const& window_size) override;
    virtual void update(float elapsed) override;
    virtual void draw(glm::uvec2 const& drawable_size) override;

    float elpasedTime = 0.f;

    Scene::Camera* camera = new Scene::Camera(new Scene::Transform());

    UIBGObject* tutorial;
};