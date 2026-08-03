#include "db.h"
#include <sqlite3.h>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <cstdint>
#include <format>
#include <QDebug>
#include "utils.h"

std::string_view columnName(ColumnType column)
{
    for (const auto& [type, name] : column_names)
        if (type == column)
            return name;
    throw std::invalid_argument("Unknown ColumnType");
}

Database::Database(QObject *parent) : QObject(parent) {}

/// @brief Initialises the database, and adds a new row for the current date using @ref Database::newEntry().
/// @param path The path to the database file
/// @param addNewEntry
void Database::initDb(const std::string &path, bool addNewEntry)
{

    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK)
    {
        std::string err = sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        throw std::runtime_error(err);
    }

    sqlite3_stmt *raw_tbl_exists_stmt;
    if (sqlite3_prepare_v2(db, "SELECT name FROM sqlite_master WHERE type='table' AND name=?;", -1, &raw_tbl_exists_stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> exists_stmt(raw_tbl_exists_stmt, &sqlite3_finalize);

    if (sqlite3_bind_text(exists_stmt.get(), 1, TABLE_NAME, -1, SQLITE_STATIC) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    int result = sqlite3_step(exists_stmt.get());

    if (result == SQLITE_ERROR)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }
    else if (result == SQLITE_DONE)
    {
        // Create table
        sqlite3_stmt *raw_create_stmt;
        if (sqlite3_prepare_v2(db, "CREATE TABLE sessions (id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER, time_worked INTEGER DEFAULT 0, sessions INTEGER DEFAULT 0, breaks INTEGER DEFAULT 0);", -1, &raw_create_stmt, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db));

        std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> create_stmt(raw_create_stmt, &sqlite3_finalize);

        int result = sqlite3_step(create_stmt.get());
        if (result != SQLITE_DONE)
        {
            throw std::runtime_error(sqlite3_errmsg(db));
        }
    }

    if (addNewEntry)
        newEntry();
}

/// @brief Creates a new row for the current date. Assumes that table already exists.
void Database::newEntry() const
{

    if (db == nullptr)
        throw std::runtime_error("Database not initialised!");

    int64_t unix_time = utils::getUnixTimestamp();

    // See if there is already an entry for the current day
    sqlite3_stmt *raw_exists_stmt;
    if (sqlite3_prepare_v2(db, "SELECT COUNT(id) AS total FROM sessions WHERE date = ?;", -1, &raw_exists_stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> exists_stmt(raw_exists_stmt, &sqlite3_finalize);

    if (sqlite3_bind_int64(exists_stmt.get(), 1, unix_time) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    int result = sqlite3_step(exists_stmt.get());

    // Query worked, but count is zero
    if (result == SQLITE_ROW && !sqlite3_column_int(exists_stmt.get(), 0))
    {
        qInfo() << "No row for today";

        sqlite3_stmt *raw_insert_statement;
        if (sqlite3_prepare_v2(db, "INSERT INTO sessions(date) VALUES (?);", -1, &raw_insert_statement, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg((db)));

        std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> insert_stmt(raw_insert_statement, &sqlite3_finalize);

        if (sqlite3_bind_int64(insert_stmt.get(), 1, unix_time) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db));

        result = sqlite3_step(insert_stmt.get());
        if (result != SQLITE_DONE)
            throw std::runtime_error(sqlite3_errmsg(db));

        qInfo() << "Added row";
    }
    else if (result == SQLITE_ROW)
    {
        // Query worked, exit
        qInfo() << "Already opened today, no need to add new row.";
    }
    else
    {
        // Query did not work
        throw std::runtime_error(sqlite3_errmsg(db));
    }
}

/// @brief Updates an entry for the current day (last row inserted).
/// @param column The column to update
/// @param delta How much to increase/decrease it by
void Database::updateEntry(ColumnType column, int delta) const
{
    if (db == nullptr)
        throw std::runtime_error("Database not initialised!");

    // Get the last row in the table
    sqlite3_stmt *raw_last_row_stmt;
    if (sqlite3_prepare_v2(db, "SELECT id FROM sessions ORDER BY id DESC LIMIT 1;", -1, &raw_last_row_stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> last_row_stmt(raw_last_row_stmt, &sqlite3_finalize);

    int result = sqlite3_step(last_row_stmt.get());

    if (result == SQLITE_DONE)
    {
        throw std::runtime_error("no rows in db");
    }
    else if (result != SQLITE_ROW)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    int64_t row_id = sqlite3_column_int64(last_row_stmt.get(), 0);

    // Update values in last row
    sqlite3_stmt *raw_update_stmt;
    if (sqlite3_prepare_v2(db, std::format("UPDATE {} SET {} = {} + ? WHERE id = ?", TABLE_NAME, columnName(column), columnName(column)).c_str(), -1, &raw_update_stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> update_stmt(raw_update_stmt, &sqlite3_finalize);

    if (sqlite3_bind_int(update_stmt.get(), 1, delta) != SQLITE_OK || sqlite3_bind_int64(update_stmt.get(), 2, row_id) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    if (sqlite3_step(update_stmt.get()) != SQLITE_DONE)
        throw std::runtime_error(sqlite3_errmsg(db));
}

std::vector<std::array<int64_t, 2>> Database::retrieveEntries(ColumnType column, std::chrono::seconds start, std::chrono::seconds end) const
{

    if (db == nullptr)
        throw std::runtime_error("Database not initialised!");

    sqlite3_stmt *raw_stmt;
    if (sqlite3_prepare_v2(db, std::format("SELECT date, {} FROM {} WHERE date >= ? AND date <= ?", columnName(column), TABLE_NAME).c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));

    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> retrieve_stmt(raw_stmt, &sqlite3_finalize);

    if (sqlite3_bind_int64(retrieve_stmt.get(), 1, start.count()) != SQLITE_OK || sqlite3_bind_int64(retrieve_stmt.get(), 2, end.count()) != SQLITE_OK)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    std::vector<std::array<int64_t, 2>> values;
    values.reserve(std::abs((end.count() - start.count()) / (24 * 60 * 60)));

    int result;
    while ((result = sqlite3_step(retrieve_stmt.get())) == SQLITE_ROW)
    {
        values.push_back(
            {sqlite3_column_int64(retrieve_stmt.get(), 0),
             sqlite3_column_int64(retrieve_stmt.get(), 1)});
    }

    if (result != SQLITE_DONE)
        throw std::runtime_error(sqlite3_errmsg(db));

    return values;
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