#pragma once
#include <iostream>
#include <functional>

struct TriggerComponent {
    std::function<void()> onTrigger;
};
