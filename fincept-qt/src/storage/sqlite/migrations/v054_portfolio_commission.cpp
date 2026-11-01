// v054 — add optional commission column to portfolio_transactions.
// Safe: ALTER TABLE ADD COLUMN is non-destructive.
#include "storage/sqlite/migrations/MigrationRunner.h"
#include "storage/sqlite/Database.h"

namespace fincept::migrations {

static Result<void> migrate_v054(QSqlDatabase& db) {
    auto r = Database::exec(db, "ALTER TABLE portfolio_transactions ADD COLUMN commission REAL DEFAULT 0");
    // Ignore "duplicate column" if re-run
    if (r.is_err() && !QString::fromStdString(r.error()).contains("duplicate column"))
        return r;
    return {};
}

static const bool registered = [] {
    MigrationRunner::register_migration({54, "portfolio_commission", migrate_v054});
    return true;
}();

} // namespace fincept::migrations
