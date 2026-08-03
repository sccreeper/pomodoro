#pragma once

#include <QObject>
#include <sqlite3.h>
#include <string>
#include <chrono>
#include <vector>
#include <string_view>

const auto TABLE_NAME = "sessions";

enum ColumnType {
    time_worked,
    sessions,
    breaks
};

constexpr std::array<std::pair<ColumnType, std::string_view>, 3> column_names{{
    {sessions,    "sessions"},
    {breaks,      "breaks"},
    {time_worked, "time_worked"},
}};

std::string_view columnName(ColumnType column);

class Database : public QObject {

    Q_OBJECT

    public:
        explicit Database(QObject *parent = nullptr);
        ~Database();
        void initDb(const std::string &path, bool addNewEntry = false);
        void updateEntry(ColumnType column, int value) const;
        void newEntry() const;
        std::vector<std::array<int64_t, 2>> retrieveEntries(ColumnType column, std::chrono::seconds start, std::chrono::seconds end) const;
    
    private:
        sqlite3* db = nullptr;

};