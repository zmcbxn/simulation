#include "ServiceFactory.h"

std::shared_ptr<CharacterService> ServiceFactory::getCharacterService() {
    static std::shared_ptr<CharacterService> instance = std::make_shared<CharacterService>();
    return instance;
}