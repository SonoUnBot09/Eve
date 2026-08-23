#pragma once

#include <vector>
#include <cstdint>

namespace Eve::Entities
{
    class Table;
    class EntityManager;

    struct QueryResult
    {
        public:

            QueryResult() = default;

            uint32_t GetTablesCount();
            Table& GetTable(uint32_t index);

        private:
            std::vector<Table*> tables;

        friend class EntityManager;
    };
}