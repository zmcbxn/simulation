#pragma once

#include <memory>
#include "CharacterService.h"

class ServiceFactory {
public:
    static std::shared_ptr<CharacterService> getCharacterService();
};