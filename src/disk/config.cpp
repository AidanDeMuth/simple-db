#include <vector>
#include <unordered_map>
#include <sys/stat.h>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "./config.hh"
#include "./disk.hh"

#include "../util/error.hh"


namespace config {
    /* stringToConfig()
     *
     * Given a freshly read config file string, will parse to a config file 
     */
    dbconfig stringToConfig(std::string input) {
        std::stringstream stream(input);
        std::string token;

        dbconfig cfg;

        while (std::getline(stream, token, '\n')) {
            size_t pos = token.find(":");
            if (pos == std::string::npos) { continue; }

            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);

            // Map config key to struct value
            if (key == "PROJECT_PATH") { cfg.projectPath = value; }
            else if (key == "PROJECT_NAME") { cfg.projectName = value; }
            else if (key == "PAGE_SIZE") { cfg.pageSize = std::stoi(value); }
            else if (key == "MAX_PAGES") { cfg.maxPages = std::stoi(value); }
        }

        return cfg;
    }


    std::string configToString(dbconfig config) {
        std::string output;

        output += "PROJECT_PATH:" + config.projectPath + "\n";
        output += "PROJECT_NAME:" + config.projectName + "\n";
        output += "PAGE_SIZE:" + std::to_string(config.pageSize) + "\n";
        output += "MAX_PAGES:" + std::to_string(config.maxPages) + "\n";
        output += "INIT_PAGES:" + std::to_string(config.initPages) + "\n";

        // Schemas are listed
        // SCHEMAS:name1,filepath1;name2,filepath2
        // can be empty
        output += "SCHEMAS:";
        for (auto schema : config.schemas) {
            output += schema.first + ","+ schema.second + ";";
        }

        return output;
    }

    void writeConfigFile(dbconfig config) {

    }

    dbconfig readConfigFile(std::string filePath) {0
        if (!disk::checkExistsFile(filePath)) {
            throw std::runtime_error("Config read error: file does not exist!");
        }

        std::ifstream file(filePath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return stringToConfig(buffer.str());
    }
}