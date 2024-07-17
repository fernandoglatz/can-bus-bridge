package entity

type PidTranslate struct {
	Entity
	PID         int    `gorm:"column:pid"`
	Bytes       int    `gorm:"column:bytes"`
	Description string `gorm:"column:description"`
	Formula     string `gorm:"column:formula"`
}
