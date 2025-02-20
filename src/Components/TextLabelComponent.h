#ifndef TEXTLABELCOMPONENT_H
#define TEXTLABELCOMPONENT_H

#include <glm/glm.hpp>
#include <string>
#include <SDL2/SDL.h>

struct TextLabelComponent {
    glm::vec2 position;
    std::string text;
    std::string assetId;
    SDL_Color color;
    bool isFixed;

    TextLabelComponent() : position(0.0f, 0.0f), text(""), assetId(""), color{0, 0, 0, 255}, isFixed(true) {}
    
    TextLabelComponent(glm::vec2 position, std::string text = "", std::string assetId = "", const SDL_Color& color = {0, 0, 0}, bool isFixed = true) : 
        position(position),
        text(text),
        assetId(assetId),
        color(color),
        isFixed(isFixed) {}
};
#endif