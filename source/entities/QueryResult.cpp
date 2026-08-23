#include <eve/entities/QueryResult.hpp>

using namespace Eve::Entities;

uint32_t QueryResult::GetTablesCount()
{
    return tables.size();
}

Table& QueryResult::GetTable(uint32_t index)
{
    return *tables[index];
}