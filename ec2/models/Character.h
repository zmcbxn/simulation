#pragma once
#include <string>
#include <jsoncpp/json/json.h>

struct Character{
    Character() = default;
    ~Character() = default;

    std::string characterId = "";
    std::string characterName = "";
    std::string serverId = "";

    Json::Value characterData;
    Json::Value statusData;
    Json::Value equipmentData;
    Json::Value avatarData;
    Json::Value creatureData;
    Json::Value flagData;
    Json::Value mistData;
    Json::Value skillData;
    Json::Value buffEquipData;
    Json::Value buffAvatarData;
    Json::Value buffCreatureData;
    Json::Value timelineData;

    Json::Value toJson() const{
        Json::Value payload;
        payload["characterId"] = characterId;
        payload["characterName"] = characterName;
        payload["serverId"] = serverId;

        payload["characterData"] = characterData;
        payload["statusData"] = statusData;
        payload["equipmentData"] = equipmentData;
        payload["avatarData"] = avatarData;
        payload["creatureData"] = creatureData;
        payload["flagData"] = flagData;
        payload["mistData"] = mistData;
        payload["skillData"] = skillData;
        payload["timelineData"] = timelineData;

        Json::Value buffData;
        buffData["buffEquipData"] = buffEquipData;
        buffData["buffAvatarData"] = buffAvatarData;
        buffData["buffCreatureData"] = buffCreatureData;
        payload["buffData"] = buffData;

        

        return payload;
    }

    Json::Value getCharacterResponse() const { return characterData.isNull() ? Json::Value(Json::objectValue) : characterData; }

    Json::Value getStatusResponse() const { return statusData.isNull() ? Json::Value(Json::objectValue) : statusData; }

    Json::Value getEquipmentResponse() const { return equipmentData.isNull() ? Json::Value(Json::objectValue) : equipmentData; }

    Json::Value getAvatarAndCreatureResponse() const {
        Json::Value response;
        response["avatar"] = avatarData.isNull() ? Json::Value(Json::objectValue) : avatarData;
        response["creature"] = creatureData.isNull() ? Json::Value(Json::objectValue) : creatureData;
        return response;
    }

    Json::Value getFlagResponse() const { return flagData.isNull() ? Json::Value(Json::objectValue) : flagData; }

    Json::Value getMistResponse() const { return mistData.isNull() ? Json::Value(Json::objectValue) : mistData; }

    Json::Value getBuffResponse() const {
        Json::Value buffData;
        buffData["buffEquipData"] = buffEquipData.isNull() ? Json::Value(Json::objectValue) : buffEquipData;
        buffData["buffAvatarData"] = buffAvatarData.isNull() ? Json::Value(Json::objectValue) : buffAvatarData;
        buffData["buffCreatureData"] = buffCreatureData.isNull() ? Json::Value(Json::objectValue) : buffCreatureData;
        return buffData;
    }

    Json::Value getSkillResponse() const { return skillData.isNull() ? Json::Value(Json::objectValue) : skillData; }

    Json::Value getTimelineResponse() const { return timelineData.isNull() ? Json::Value(Json::objectValue) : timelineData; }
};