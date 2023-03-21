#pragma once

#include <iostream>
#include <memory>
#include "component.h"

namespace decorator {

class Decorator : public Component{
protected:
  std::shared_ptr<Component> component;

public:
  void set_component(std::shared_ptr<Component> component) {
    this->component = component;
  }

  void operation() override {
    component->operation();
  }
};

} // end of decorator
