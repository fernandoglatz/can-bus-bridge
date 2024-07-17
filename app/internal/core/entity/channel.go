package entity

type Channel struct {
	Entity
	PresetID    int    `gorm:"column:preset_id"`
	Description string `gorm:"column:description"`
	Pin         int    `gorm:"column:pin"`
	Channel     int    `gorm:"column:channel"`
	Baudrate    int    `gorm:"column:baudrate"`
	Clock       int    `gorm:"column:clock"`
	Readable    bool   `gorm:"column:readable"`
	ReverseMask bool   `gorm:"column:reverse_mask"`
	Enabled     bool   `gorm:"column:enabled"`
}
