#pragma once
#include <string>
#include <map>
#include <vector>

class KafkaConfig {
public:
    KafkaConfig() = default;

    bool load(const std::string& filename);
    std::string getValue(const std::string& section, const std::string& key) const;
    std::vector<std::string> getVector(const std::string& section, const std::string& key) const;
    std::map<std::string, std::string> getSection(const std::string& section) const;

private:
    std::map<std::string, std::map<std::string, std::string>> configData_;
    std::string trim(const std::string& s) const;
};