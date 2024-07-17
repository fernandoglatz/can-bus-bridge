package utils

import (
	"context"
	"errors"
	"fernandoglatz/can-bus-bridge/internal/core/common/utils/log"
	"fmt"
	"path/filepath"
	"time"

	"github.com/golang-migrate/migrate/v4"
	sqlitemg "github.com/golang-migrate/migrate/v4/database/sqlite3"
	_ "github.com/golang-migrate/migrate/v4/source/file"
	"gorm.io/driver/sqlite"

	"gorm.io/gorm"
	"gorm.io/gorm/logger"
	"gorm.io/gorm/schema"
	"gorm.io/gorm/utils"
)

var SQLiteDatabase *gorm.DB

const (
	FORMAT_TRACE_STR       = "[%.3fms] [rows:%d] %s"
	FORMAT_TRACE_ERROR_STR = "%s [%.3fms] [rows:%d] %s"
	DB_FILE_NAME           = "database.db"
	DB_NAME                = "database"
)

func ConnectToSQLite(ctx context.Context) error {
	log.Info(ctx).Msg("Connecting to SQLite")

	db, err := gorm.Open(sqlite.Open(DB_FILE_NAME), &gorm.Config{
		NamingStrategy: schema.NamingStrategy{
			SingularTable: true,
		},
		Logger: &gormLogger{}})
	if err != nil {
		return err
	}

	log.Info(ctx).Msg("SQLite connected")

	err = migrateSQLite(ctx, db)
	if err != nil {
		return err
	}

	SQLiteDatabase = db
	return nil
}

func migrateSQLite(ctx context.Context, db *gorm.DB) error {
	log.Info(ctx).Msg("Migrating SQLite")

	conn, err := db.DB()
	if err != nil {
		return err
	}

	sqliteDriver, err := sqlitemg.WithInstance(conn, &sqlitemg.Config{})
	if err != nil {
		return err
	}

	migrations, err := migrate.NewWithDatabaseInstance("file://scripts/sqlite/migrations", DB_NAME, sqliteDriver)
	if err != nil {
		return err
	}

	log.Info(ctx).Msg("Checking migrations")
	err = migrations.Up()

	if err != nil && err != migrate.ErrNoChange {
		return err
	}

	log.Info(ctx).Msg("Migrations done")
	return nil
}

type gormLogger struct {
}

func (gormLogger *gormLogger) LogMode(level logger.LogLevel) logger.Interface {
	return gormLogger
}

func (gormLogger *gormLogger) Info(ctx context.Context, msg string, data ...interface{}) {
	if log.IsLevelEnabled(log.INFO) {
		file := utils.FileWithLineNum()
		caller := filepath.Base(file)
		message := gormLogger.getMessage(msg, data...)
		log.Trace(ctx).Caller(caller).Msg(message)
	}
}

func (gormLogger *gormLogger) Warn(ctx context.Context, msg string, data ...interface{}) {
	if log.IsLevelEnabled(log.WARN) {
		file := utils.FileWithLineNum()
		caller := filepath.Base(file)
		message := gormLogger.getMessage(msg, data...)
		log.Warn(ctx).Caller(caller).Msg(message)
	}
}

func (gormLogger *gormLogger) Error(ctx context.Context, msg string, data ...interface{}) {
	if log.IsLevelEnabled(log.ERROR) {
		file := utils.FileWithLineNum()
		caller := filepath.Base(file)
		message := gormLogger.getMessage(msg, data...)
		log.Error(ctx).Caller(caller).Msg(message)
	}
}

func (gormLogger *gormLogger) Trace(ctx context.Context, begin time.Time, fc func() (sql string, rowsAffected int64), err error) {

	if err != nil && log.IsLevelEnabled(log.ERROR) && !errors.Is(err, logger.ErrRecordNotFound) {
		sql, rows := fc()
		elapsed := time.Since(begin)
		duration := float64(elapsed.Nanoseconds()) / 1e6

		file := utils.FileWithLineNum()
		caller := filepath.Base(file)
		message := gormLogger.getMessage(sql)

		formattedMessage := fmt.Sprintf(FORMAT_TRACE_ERROR_STR, err, duration, rows, message)
		log.Error(ctx).Caller(caller).Msg(formattedMessage)

	} else if log.IsLevelEnabled(log.INFO) {
		sql, rows := fc()
		elapsed := time.Since(begin)
		duration := float64(elapsed.Nanoseconds()) / 1e6

		file := utils.FileWithLineNum()
		caller := filepath.Base(file)
		message := gormLogger.getMessage(sql)

		formattedMessage := fmt.Sprintf(FORMAT_TRACE_STR, duration, rows, message)
		log.Trace(ctx).Caller(caller).Msg(formattedMessage)
	}
}

func (gormLogger *gormLogger) getMessage(msg string, data ...interface{}) string {
	return fmt.Sprintf(msg, data...)
}
