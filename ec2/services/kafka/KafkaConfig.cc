#include "KafkaConfig.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <drogon/utils/Utilities.h>

bool KafkaConfig::load(const std::string& filename){
    std::ifstream file(filename);
    if(!file.is_open()) return false;

    std::string line, currentSection;
    while(std::getline(file, line)){
        line = trim(line);
        if(line.empty() || line[0] == ';' || line[0] == '#') continue;  // 주석 제외
        if(line[0] == '[' && line.back() == ']'){
            currentSection = line.substr(1, line.size() - 2);
        } else{
            auto pos = line.find('=');
            if(pos != std::string::npos){
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                configData_[currentSection][key] = value;
            }
        }
    }
    return true;
}

std::string KafkaConfig::getValue(const std::string& section, const std::string& key) const{
    auto secIt = configData_.find(section);
    if(secIt != configData_.end()){
        auto keyIt = secIt->second.find(key);
        if(keyIt != secIt->second.end()) return keyIt->second;
    }
    return "";
}

std::vector<std::string> KafkaConfig::getVector(const std::string& section, const std::string& key) const{
    std::vector<std::string> result;
    std::string fullStr = getValue(section, key);
    if(fullStr.empty()) return result;

    std::stringstream ss(fullStr);
    std::string item;
    while(std::getline(ss, item, ',')){
        result.push_back(trim(item));
    }
    return result;
}

std::map<std::string, std::string> KafkaConfig::getSection(const std::string& section) const{
    auto it = configData_.find(section);
    if (it != configData_.end()) return it->second;
    return {};
}

std::string KafkaConfig::trim(const std::string& s) const {
    auto start = s.find_first_not_of(" \t\r\n");
    auto end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}