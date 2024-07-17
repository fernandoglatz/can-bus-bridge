package entity

type BridgePID struct {
	Entity
	PresetID      int  `gorm:"column:preset_id"`
	PID           int  `gorm:"column:pid"`
	ChannelA      int  `gorm:"column:channel_a"`
	ChannelB      int  `gorm:"column:channel_b"`
	Bidirectional bool `gorm:"column:bidirectional"`
	Enabled       bool `gorm:"column:enabled"`
}
