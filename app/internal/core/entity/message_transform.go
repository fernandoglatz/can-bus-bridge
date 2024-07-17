package entity

type MessageTransform struct {
	Entity
	BridgePidId int `gorm:"column:bridge_pid_id"`
	FromByte    int `gorm:"column:from_byte"`
	ToByte      int `gorm:"column:to_byte"`
}
