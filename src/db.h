#pragma once

#include <QObject>
#include <sqlite3.h>
#include <string>

const auto TABLE_NAME = "sessions";

enum ColumnType {
    time_worked,
    sessions,
    breaks
};

class Database : public QObject {

    Q_OBJECT

    public:
        explicit Database(QObject *parent = nullptr);
        ~Database();
        void initDb(std::string path, bool addNewEntry = false);
        void updateEntry(ColumnType column, int value);
        void newEntry();

        sqlite3* db = nullptr;

};