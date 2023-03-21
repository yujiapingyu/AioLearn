#pragma once

namespace decorator {

class Component {
public:
  virtual void operation() {};
  virtual ~Component() = default;
};

} // end of decorator