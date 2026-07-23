#include "db.h"
#include <sqlite3.h>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <cstdint>
#include <format>
#include <QDebug>

Database::Database(QObject *parent) : QObject(parent) {}

/// @brief Initialises the database, and adds a new row for the current date using @ref Database::newEntry().
/// @param path The path to the database file
/// @param addNewEntry
void Database::initDb(std::string path, bool addNewEntry)
{
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    sqlite3_stmt *exists_statement;
    if (sqlite3_prepare_v2(db, "SELECT name FROM sqlite_master WHERE type='table' AND name=?;", -1, &exists_statement, NULL) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    if (sqlite3_bind_text(exists_statement, 1, TABLE_NAME, -1, SQLITE_STATIC) != SQLITE_OK)
    {
        sqlite3_finalize(exists_statement);
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    int result = sqlite3_step(exists_statement);
    sqlite3_finalize(exists_statement);

    if (result == SQLITE_ERROR)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }
    else if (result == SQLITE_DONE)
    {
        // Create table
        sqlite3_stmt *create_statement;
        if (sqlite3_prepare_v2(db, "CREATE TABLE sessions (id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER, time_worked INTEGER DEFAULT 0, sessions INTEGER DEFAULT 0, breaks INTEGER DEFAULT 0);", -1, &create_statement, NULL) != SQLITE_OK)
        {
            sqlite3_finalize(create_statement);
            throw std::runtime_error(sqlite3_errmsg(db));
        }

        int result = sqlite3_step(create_statement);
        sqlite3_finalize(create_statement);
        if (result != SQLITE_DONE)
        {
            throw std::runtime_error(sqlite3_errmsg(db));
        }
    }

    if (addNewEntry)
        newEntry();
}

/// @brief Creates a new row for the current date. Assumes that table already exists.
void Database::newEntry()
{

    if (db == nullptr)
        throw std::runtime_error("Database not initialised!");

    int64_t unix_time;

    {
        using namespace std::chrono;

        auto tz = current_zone();
        auto local_midnight = floor<days>(tz->to_local(system_clock::now()));
        unix_time = duration_cast<seconds>(tz->to_sys(local_midnight).time_since_epoch()).count();
    }

    // See if there is already an entry for the current day
    sqlite3_stmt *exists_statement;
    if (sqlite3_prepare_v2(db, "SELECT COUNT(id) AS total FROM sessions WHERE date = ?;", -1, &exists_statement, NULL) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    if (sqlite3_bind_int64(exists_statement, 1, unix_time) != SQLITE_OK)
    {
        sqlite3_finalize(exists_statement);
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    int result = sqlite3_step(exists_statement);
    if (result != SQLITE_ROW)
    {
        sqlite3_finalize(exists_statement);
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    // Query worked, but count is zero
    if (result == SQLITE_ROW && !sqlite3_column_int(exists_statement, 0))
    {
        qInfo() << "No row for today";
        sqlite3_finalize(exists_statement);

        sqlite3_stmt *insert_statement;
        if (sqlite3_prepare_v2(db, "INSERT INTO sessions(date) VALUES (?);", -1, &insert_statement, NULL) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg((db)));

        if (sqlite3_bind_int64(insert_statement, 1, unix_time) != SQLITE_OK)
        {
            sqlite3_finalize(insert_statement);
            throw std::runtime_error(sqlite3_errmsg(db));
        }

        result = sqlite3_step(insert_statement);
        sqlite3_finalize(insert_statement);
        if (result != SQLITE_DONE)
            throw std::runtime_error(sqlite3_errmsg(db));

    } else if (result != SQLITE_DONE) {
        // Query worked, exit
        qInfo() << "Already opened today, no need to add new row.";
        sqlite3_finalize(exists_statement);
    } else {
        // Query did not work
        sqlite3_finalize(exists_statement);
        throw std::runtime_error("Query did not work.");
    }

}

/// @brief Updates an entry for the current day (last row inserted).
/// @param column The column to update
/// @param delta How much to increase/decrease it by
void Database::updateEntry(ColumnType column, int delta)
{
    if (db == nullptr)
        throw std::runtime_error("Database not initialised!");

    std::string column_name;
    switch (column)
    {
    case sessions:
        column_name = "sessions";
        break;
    case breaks:
        column_name = "breaks";
        break;
    case time_worked:
        column_name = "time_worked";
        break;
    }

    // Get the last row in the table
    sqlite3_stmt *last_row_stmt;
    if (sqlite3_prepare_v2(db, "SELECT id FROM sessions ORDER BY id DESC LIMIT 1;", -1, &last_row_stmt, NULL) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    int result = sqlite3_step(last_row_stmt);

    if (result == SQLITE_DONE)
    {
        sqlite3_finalize(last_row_stmt);
        throw std::runtime_error("no rows in db");
    }
    else if (result != SQLITE_ROW)
    {
        sqlite3_finalize(last_row_stmt);
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    int64_t row_id = sqlite3_column_int64(last_row_stmt, 0);
    sqlite3_finalize(last_row_stmt);

    // Update values in last row
    sqlite3_stmt *update_stmt;
    if (sqlite3_prepare_v2(db, std::format("UPDATE {} SET {} = {} + ? WHERE id = ?", TABLE_NAME, column_name, column_name).c_str(), -1, &update_stmt, NULL) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    if (sqlite3_bind_int(update_stmt, 1, delta) != SQLITE_OK || sqlite3_bind_int64(update_stmt, 2, row_id) != SQLITE_OK)
    {
        sqlite3_finalize(update_stmt);
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    if (sqlite3_step(update_stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(update_stmt);
        throw std::runtime_error(sqlite3_errmsg(db));
    }
    sqlite3_finalize(update_stmt);
}

Database::~Database()
{
    if (this->db)
    {
        qInfo() << "Closing database";
        sqlite3_close_v2(this->db);
        db = nullptr;
    }
}