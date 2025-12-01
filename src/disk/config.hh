#pragma once

#include <string>
#include <unordered_map>

namespace config {
    typedef struct dbconfig {
        std::string projectPath;
        std::string projectName;
        int pageSize;
        int maxPages;
        int initPages;
        // Map {schema-name, file-paths}
        std::unordered_map<std::string, std::string> schemas;

        dbconfig() {};

        dbconfig(std::string projectPath, std::string projectName) :
            projectPath(projectPath),
            projectName(projectName),
            pageSize(4096),
            maxPages(10000),
            initPages(500),
            schemas()
        {}
    } dbconfig;

    dbconfig stringToConfig(std::string input);
    std::string configToString(dbconfig config);
    dbconfig readConfigFile(std::string filePath);
    void createDB(dbconfig *config);
}