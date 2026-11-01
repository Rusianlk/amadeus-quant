// v054 — add optional commission column to portfolio_transactions.
// SAFE: ALTER TABLE ADD COLUMN is non-destructive.

#include "storage/sqlite/migrations/MigrationRunner.h"

#include <QSqlError>
#include <QSqlQuery>

namespace fincept {
namespace {

Result<void> sql_v054(QSqlDatabase& db, const char* stmt) {
    QSqlQuery q(db);
    if (!q.exec(stmt)) {
        const QString err = q.lastError().text();
        // Column already exists on re-run — ignore.
        if (err.contains("duplicate column", Qt::CaseInsensitive))
            return Result<void>::ok();
        return Result<void>::err(err.toStdString());
    }
    return Result<void>::ok();
}

Result<void> migrate_v054(QSqlDatabase& db) {
    QSqlQuery check(db);
    if (!check.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='portfolio_transactions'"))
        return Result<void>::err(check.lastError().text().toStdString());
    if (!check.next())
        return Result<void>::ok(); // table does not exist yet; nothing to alter
    return sql_v054(db, "ALTER TABLE portfolio_transactions ADD COLUMN commission REAL DEFAULT 0");
}

const bool reg_v054 = [] {
    MigrationRunner::register_migration({54, "portfolio_commission", migrate_v054});
    return true;
}();

} // namespace
} // namespace fincept
