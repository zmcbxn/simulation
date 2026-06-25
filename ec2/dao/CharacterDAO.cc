#include "CharacterDAO.h"

void CharacterDAO::upsertBaseData(const Json::Value& characterData)
{
    LOG_DEBUG << "[upsertBaseData] Full JSON: " << characterData.toStyledString();
    LOG_DEBUG << "Saving BaseData - Adventure: " << characterData["adventureName"].asString() 
              << ", Guild: " << characterData["guildName"].asString();

    std::string sql = R"(
        INSERT INTO character.base_data
        (character_id, server_id, character_name, adventure_name, job_name, job_grow_name, level, guild_name, fame)
        VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)
        ON CONFLICT (character_id, server_id) DO UPDATE SET
            character_name = EXCLUDED.character_name,
            adventure_name = EXCLUDED.adventure_name,
            job_name = EXCLUDED.job_name,
            job_grow_name = EXCLUDED.job_grow_name,
            level = EXCLUDED.level,
            guild_name = EXCLUDED.guild_name,
            fame = EXCLUDED.fame,
            updated_date = CURRENT_TIMESTAMP
    )";

    dbClient_->execSqlAsync(
        sql,
        [](const drogon::orm::Result& result) {
            // 성공 처리 (필요시)
        },
        [](const drogon::orm::DrogonDbException& e) {
            LOG_ERROR << "Failed to upsert character base data: " << e.base().what();
        },
        characterData["characterId"].asString(),
        characterData["serverId"].asString(),
        characterData["characterName"].asString(),
        characterData["adventureName"].asString(),
        characterData["jobName"].asString(),
        characterData["jobGrowName"].asString(),
        characterData["level"].asInt(),
        characterData["guildName"].asString(),
        characterData["fame"].asInt()
    );
}

void CharacterDAO::upsertStatusData(const std::string& characterId, const std::string& serverId, const Json::Value& statusData){

    int level = statusData["level"].asInt();
    std::string jobName = statusData["jobName"].asString();
    std::string adventureName = statusData["adventureName"].asString();
    int fame = 0;
    for (const auto& fameEntry : statusData["status"]){
        if (fameEntry["name"].asString() == "모험가 명성"){
            fame = fameEntry["value"].asInt();
            break;
        }
    }

    std::string sql = R"(
        INSERT INTO character.status_data
        (character_id, server_id, level, job_name, adventure_name, fame, raw_data)
        VALUES ($1, $2, $3, $4, $5, $6, $7)
        ON CONFLICT (character_id, server_id) DO UPDATE SET
            level = EXCLUDED.level,
            job_name = EXCLUDED.job_name,
            adventure_name = EXCLUDED.adventure_name,
            fame = EXCLUDED.fame,
            raw_data = EXCLUDED.raw_data,
            updated_date = CURRENT_TIMESTAMP
    )";

    dbClient_->execSqlAsync(
        sql,
        [](const drogon::orm::Result& result) {
            // 성공 처리 (필요시)
        },
        [](const drogon::orm::DrogonDbException& e) {
            LOG_ERROR << "Failed to upsert character status data: " << e.base().what();
        },
        characterId,
        serverId,
        level,
        jobName,
        adventureName,
        fame,
        statusData
    );
}

void CharacterDAO::upsertEquipmentData(const std::string& characterId, const std::string& serverId, const Json::Value& equipmentData){

    std::string sql = R"(
        INSERT INTO character.equipment_data
        (character_id, server_id, slot_id, slot_name, item_id, item_name, item_rarity, item_grade_name,
        reinforce_value, is_amplified, amplification_name, refine_value, enchant_data, fusion_option, upgrade_info)
        VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15)
        ON CONFLICT (character_id, server_id, slot_id) DO UPDATE SET
            item_id = EXCLUDED.item_id,
            item_name = EXCLUDED.item_name,
            item_rarity = EXCLUDED.item_rarity,
            item_grade_name = EXCLUDED.item_grade_name,
            reinforce_value = EXCLUDED.reinforce_value,
            is_amplified = EXCLUDED.is_amplified,
            amplification_name = EXCLUDED.amplification_name,
            refine_value = EXCLUDED.refine_value,
            enchant_data = EXCLUDED.enchant_data,
            fusion_option = EXCLUDED.fusion_option,
            upgrade_info = EXCLUDED.upgrade_info,
            updated_date = CURRENT_TIMESTAMP
    )";

    for (const auto& equipment : equipmentData) {
        bool isAmplified = !equipment["amplificationName"].isNull() && !equipment["amplificationName"].asString().empty();
        std::string ampName = isAmplified ? equipment["amplificationName"].asString() : "";

        Json::Value upgradeInfo = Json::objectValue;
        if (equipment.isMember("upgradeInfo"))
            upgradeInfo["upgradeInfo"] = equipment["upgradeInfo"];
        if (equipment.isMember("tune") && equipment["tune"].isArray() && !equipment["tune"].empty())
            upgradeInfo["tune"] = equipment["tune"];

        Json::Value fusionOption = Json::objectValue;
        if (equipment.isMember("fusionOption"))
            fusionOption = equipment["fusionOption"];

        dbClient_->execSqlAsync(
            sql,
            [](const drogon::orm::Result& result) {},
            [](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "Failed to upsert character equipment data: " << e.base().what();
            },
            characterId,
            serverId,
            equipment["slotId"].asString(),
            equipment["slotName"].asString(),
            equipment["itemId"].asString(),
            equipment["itemName"].asString(),
            equipment["itemRarity"].asString(),
            equipment["itemGradeName"].asString(),
            equipment["reinforce"].asInt(),
            isAmplified,
            ampName,
            equipment["refine"].asInt(),
            equipment["enchantData"],
            fusionOption,
            upgradeInfo
        );
    }
}

void CharacterDAO::upsertOathData(const std::string& characterId, const std::string& serverId, const Json::Value& oathData) {
    const Json::Value& oath = oathData["oath"];
    if (oath.isNull() || !oath.isObject()) return;

    const Json::Value& info = oath["info"];
    std::string itemId    = info["itemId"].asString();
    std::string itemName  = info["itemName"].asString();
    std::string itemRarity = info["itemRarity"].asString();
    int setPoint = info["setPoint"].asInt();

    int upgradeLevel = 0;
    if (info.isMember("oathUpgrade") && info["oathUpgrade"]["options"].isArray())
        upgradeLevel = static_cast<int>(info["oathUpgrade"]["options"].size());

    std::string sql = R"(
        INSERT INTO character.oath_data
        (character_id, server_id, item_id, item_name, item_rarity, set_point, upgrade_level, raw_data)
        VALUES ($1, $2, $3, $4, $5, $6, $7, $8)
        ON CONFLICT (character_id, server_id) DO UPDATE SET
            item_id = EXCLUDED.item_id,
            item_name = EXCLUDED.item_name,
            item_rarity = EXCLUDED.item_rarity,
            set_point = EXCLUDED.set_point,
            upgrade_level = EXCLUDED.upgrade_level,
            raw_data = EXCLUDED.raw_data,
            updated_date = CURRENT_TIMESTAMP
    )";

    dbClient_->execSqlAsync(sql,
        [](const drogon::orm::Result&) {},
        [](const drogon::orm::DrogonDbException& e) {
            LOG_ERROR << "Failed to upsert oath data: " << e.base().what();
        },
        characterId, serverId, itemId, itemName, itemRarity, setPoint, upgradeLevel, oath
    );
}

void CharacterDAO::upsertAvatarData(const std::string& characterId, const std::string& serverId, const Json::Value& avatarData) {
    const Json::Value& avatarArr = avatarData["avatar"];
    if (!avatarArr.isArray()) return;

    std::string sql = R"(
        INSERT INTO character.avatar_data
        (character_id, server_id, slot_id, slot_name, item_id, item_name, item_rarity, option_ability, clone_avatar, emblems)
        VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)
        ON CONFLICT (character_id, server_id, slot_id) DO UPDATE SET
            item_id = EXCLUDED.item_id,
            item_name = EXCLUDED.item_name,
            item_rarity = EXCLUDED.item_rarity,
            option_ability = EXCLUDED.option_ability,
            clone_avatar = EXCLUDED.clone_avatar,
            emblems = EXCLUDED.emblems,
            updated_date = CURRENT_TIMESTAMP
    )";

    for (const auto& slot : avatarArr) {
        dbClient_->execSqlAsync(sql,
            [](const drogon::orm::Result&) {},
            [](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "Failed to upsert avatar data: " << e.base().what();
            },
            characterId, serverId,
            slot["slotId"].asString(),
            slot["slotName"].asString(),
            slot["itemId"].asString(),
            slot["itemName"].asString(),
            slot["itemRarity"].asString(),
            slot["optionAbility"].asString(),
            slot["clone"],
            slot["emblems"]
        );
    }
}

void CharacterDAO::upsertCreatureData(const std::string& characterId, const std::string& serverId, const Json::Value& creatureData) {
    const Json::Value& creature = creatureData["creature"];
    if (!creature.isObject()) return;

    std::string sql = R"(
        INSERT INTO character.creature_data
        (character_id, server_id, item_id, item_name, item_rarity, clone, artifact)
        VALUES ($1, $2, $3, $4, $5, $6, $7)
        ON CONFLICT (character_id, server_id) DO UPDATE SET
            item_id = EXCLUDED.item_id,
            item_name = EXCLUDED.item_name,
            item_rarity = EXCLUDED.item_rarity,
            clone = EXCLUDED.clone,
            artifact = EXCLUDED.artifact,
            updated_date = CURRENT_TIMESTAMP
    )";

    dbClient_->execSqlAsync(sql,
        [](const drogon::orm::Result&) {},
        [](const drogon::orm::DrogonDbException& e) {
            LOG_ERROR << "Failed to upsert creature data: " << e.base().what();
        },
        characterId, serverId,
        creature["itemId"].asString(),
        creature["itemName"].asString(),
        creature["itemRarity"].asString(),
        creature["clone"],
        creature["artifact"]
    );
}

void CharacterDAO::upsertMistData(const std::string& characterId, const std::string& serverId, const Json::Value& mistData) {
    const Json::Value& mist = mistData["mistAssimilation"];
    if (!mist.isObject()) return;

    std::string sql = R"(
        INSERT INTO character.mist_data
        (character_id, server_id, mist_level, exp_rate, raw_data)
        VALUES ($1, $2, $3, $4, $5)
        ON CONFLICT (character_id, server_id) DO UPDATE SET
            mist_level = EXCLUDED.mist_level,
            exp_rate = EXCLUDED.exp_rate,
            raw_data = EXCLUDED.raw_data,
            updated_date = CURRENT_TIMESTAMP
    )";

    dbClient_->execSqlAsync(sql,
        [](const drogon::orm::Result&) {},
        [](const drogon::orm::DrogonDbException& e) {
            LOG_ERROR << "Failed to upsert mist data: " << e.base().what();
        },
        characterId, serverId,
        mist["level"].asInt(),
        mist["expRate"].asString(),
        mist
    );
}

void CharacterDAO::upsertSkillData(const std::string& characterId, const std::string& serverId, const Json::Value& skillData) {
    const Json::Value& skill = skillData["skill"];
    if (!skill.isObject()) return;

    const Json::Value& style = skill["style"];

    std::string sql = R"(
        INSERT INTO character.skill_data
        (character_id, server_id, skill_hash, active_skills, passive_skills, raw_data)
        VALUES ($1, $2, $3, $4, $5, $6)
        ON CONFLICT (character_id, server_id) DO UPDATE SET
            skill_hash = EXCLUDED.skill_hash,
            active_skills = EXCLUDED.active_skills,
            passive_skills = EXCLUDED.passive_skills,
            raw_data = EXCLUDED.raw_data,
            updated_date = CURRENT_TIMESTAMP
    )";

    dbClient_->execSqlAsync(sql,
        [](const drogon::orm::Result&) {},
        [](const drogon::orm::DrogonDbException& e) {
            LOG_ERROR << "Failed to upsert skill data: " << e.base().what();
        },
        characterId, serverId,
        skill["hash"].asString(),
        style["active"],
        style["passive"],
        style
    );
}

void CharacterDAO::upsertBuffEquipData(const std::string& characterId, const std::string& serverId, const Json::Value& buffEquipData) {
    const Json::Value& equipment = buffEquipData["skill"]["buff"]["equipment"];
    if (!equipment.isArray()) return;

    std::string sql = R"(
        INSERT INTO character.buff_equip_data
        (character_id, server_id, slot_id, item_id, item_name, item_rarity)
        VALUES ($1, $2, $3, $4, $5, $6)
        ON CONFLICT (character_id, server_id, slot_id) DO UPDATE SET
            item_id = EXCLUDED.item_id,
            item_name = EXCLUDED.item_name,
            item_rarity = EXCLUDED.item_rarity,
            updated_date = CURRENT_TIMESTAMP
    )";

    for (const auto& item : equipment) {
        dbClient_->execSqlAsync(sql,
            [](const drogon::orm::Result&) {},
            [](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "Failed to upsert buff equip data: " << e.base().what();
            },
            characterId, serverId,
            item["slotId"].asString(),
            item["itemId"].asString(),
            item["itemName"].asString(),
            item["itemRarity"].asString()
        );
    }
}

void CharacterDAO::upsertBuffAvatarData(const std::string& characterId, const std::string& serverId, const Json::Value& buffAvatarData) {
    const Json::Value& avatar = buffAvatarData["skill"]["buff"]["avatar"];
    if (!avatar.isArray()) return;

    std::string sql = R"(
        INSERT INTO character.buff_avatar_data
        (character_id, server_id, slot_id, item_id, item_name, item_rarity, has_emblems)
        VALUES ($1, $2, $3, $4, $5, $6, $7)
        ON CONFLICT (character_id, server_id, slot_id) DO UPDATE SET
            item_id = EXCLUDED.item_id,
            item_name = EXCLUDED.item_name,
            item_rarity = EXCLUDED.item_rarity,
            has_emblems = EXCLUDED.has_emblems,
            updated_date = CURRENT_TIMESTAMP
    )";

    for (const auto& slot : avatar) {
        bool hasEmblems = slot["emblems"].isArray() && !slot["emblems"].empty();
        dbClient_->execSqlAsync(sql,
            [](const drogon::orm::Result&) {},
            [](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "Failed to upsert buff avatar data: " << e.base().what();
            },
            characterId, serverId,
            slot["slotId"].asString(),
            slot["itemId"].asString(),
            slot["itemName"].asString(),
            slot["itemRarity"].asString(),
            hasEmblems
        );
    }
}

void CharacterDAO::upsertBuffCreatureData(const std::string& characterId, const std::string& serverId, const Json::Value& buffCreatureData) {
    const Json::Value& creature = buffCreatureData["skill"]["buff"]["creature"];
    if (!creature.isArray() || creature.empty()) return;

    const Json::Value& c = creature[0];
    bool buffSkill = !c["enchant"].isNull();

    std::string sql = R"(
        INSERT INTO character.buff_creature_data
        (character_id, server_id, item_id, item_name, item_rarity, buff_skill)
        VALUES ($1, $2, $3, $4, $5, $6)
        ON CONFLICT (character_id, server_id) DO UPDATE SET
            item_id = EXCLUDED.item_id,
            item_name = EXCLUDED.item_name,
            item_rarity = EXCLUDED.item_rarity,
            buff_skill = EXCLUDED.buff_skill,
            updated_date = CURRENT_TIMESTAMP
    )";

    dbClient_->execSqlAsync(sql,
        [](const drogon::orm::Result&) {},
        [](const drogon::orm::DrogonDbException& e) {
            LOG_ERROR << "Failed to upsert buff creature data: " << e.base().what();
        },
        characterId, serverId,
        c["itemId"].asString(),
        c["itemName"].asString(),
        c["itemRarity"].asString(),
        buffSkill
    );
}

void CharacterDAO::upsertTimelineData(const std::string& characterId, const std::string& serverId, const Json::Value& timelineData) {
    const Json::Value& rows = timelineData["timeline"]["rows"];
    if (!rows.isArray()) return;

    std::string sql = R"(
        INSERT INTO character.timeline_data
        (character_id, server_id, event_date, event_code, event_name, data_detail)
        VALUES ($1, $2, $3::timestamp, $4, $5, $6)
        ON CONFLICT (character_id, server_id, event_date, event_code) DO NOTHING
    )";

    for (const auto& row : rows) {
        dbClient_->execSqlAsync(sql,
            [](const drogon::orm::Result&) {},
            [](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "Failed to upsert timeline data: " << e.base().what();
            },
            characterId, serverId,
            row["date"].asString(),
            row["code"].asInt(),
            row["name"].asString(),
            row["data"]
        );
    }
}