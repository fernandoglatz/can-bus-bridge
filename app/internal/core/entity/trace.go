package entity

import "time"

type Trace struct {
	Entity
	Description string    `gorm:"column:description"`
	StartAt     time.Time `gorm:"column:start_at"`
	EndAt       time.Time `gorm:"column:end_at"`
}
