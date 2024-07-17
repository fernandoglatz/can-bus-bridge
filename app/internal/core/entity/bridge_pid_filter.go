package entity

type BridgePidFilter struct {
	Entity
	BridgeID int `gorm:"column:bridge_id"`
	Pid      int `gorm:"column:pid"`
}
