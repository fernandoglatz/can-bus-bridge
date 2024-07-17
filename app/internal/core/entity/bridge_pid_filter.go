package entity

type BridgePIDFilter struct {
	Entity
	BridgeID int `gorm:"column:bridge_id"`
	PID      int `gorm:"column:pid"`
}
