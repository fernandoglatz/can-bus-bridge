package entity

type MessageTransform struct {
	Entity
	BridgePID int `gorm:"column:bridge_pid"`
	FromByte  int `gorm:"column:from_byte"`
	ToByte    int `gorm:"column:to_byte"`
}
