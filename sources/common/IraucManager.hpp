#pragma once

namespace raucManager {

class IraucManager {
 public:
    virtual void getInfo() = 0;
    virtual void installBundle() = 0;
    virtual void getEvent() = 0;
};

}  // namespace raucManager