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
        ON CONFLICT (character_id) DO UPDATE SET
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

void CharacterDAO::upsertEquipmentData(const std::string& characterId, const Json::Value& equipmentData){

    std::string sql = R"(
        INSERT INTO character.equipment_data
        (character_id, slot_id, slot_name, item_id, item_name, item_rarity, item_grade_name,
        reinforce_value, is_amplified, amplification_name, refine_value, enchant_data, fusion_option, tune_level, set_point)
        VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15)
        ON CONFLICT (character_id, slot_id) DO UPDATE SET
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
            tune_level = EXCLUDED.tune_level,
            set_point = EXCLUDED.set_point,
            updated_date = CURRENT_TIMESTAMP
    )";

    for (const auto& equipment : equipmentData) {
        bool isAmplified = !equipment["amplificationName"].isNull() && !equipment["amplificationName"].asString().empty();
        std::string ampName = isAmplified ? equipment["amplificationName"].asString() : "";

        int tuneLevel = 0;
        int setPoint = 0;
        if (equipment.isMember("tune") && equipment["tune"].isArray() && !equipment["tune"].empty()){
            tuneLevel = equipment["tune"][0]["level"].asInt();
            setPoint = equipment["tune"][0]["setPoint"].asInt();
        }

        Json::Value fusion;
        if (equipment.isMember("upgradeInfo")) {
            fusion["baseInfo"] = equipment["upgradeInfo"];
        } if (equipment.isMember("fusionOption")) {
            fusion["options"] = equipment["fusionOption"];
        }

        dbClient_->execSqlAsync(
            sql,
            [](const drogon::orm::Result& result) {
                // 성공 처리 (필요시)
            },
            [](const drogon::orm::DrogonDbException& e) {
                LOG_ERROR << "Failed to upsert character equipment data: " << e.base().what();
            },
            characterId,
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
            fusion,
            tuneLevel,
            setPoint
        );
    }
}