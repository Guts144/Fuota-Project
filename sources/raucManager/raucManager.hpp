#pragma once

#include "Common/IraucManager.hpp"
namespace raucManager {

class raucManager : public IraucManager {
 public:
    raucManager() = default;
    ~raucManager() = default;
    void getInfo() override;
    void installBundle() override;
    void getEvent() override;
};

}  // namespace raucManager