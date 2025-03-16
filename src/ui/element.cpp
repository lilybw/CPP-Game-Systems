// Simple hierachical UI component system - based on Entity, because why not

#include <glm/glm.hpp>

#include <entities/entity.h>
#include <entities/components.h>
#include <meta/processing.h>

class UIElement : public IEntity, public IDrawable {

};
