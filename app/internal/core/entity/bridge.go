package entity

type Bridge struct {
	Entity
	PresetID      int  `gorm:"column:preset_id"`
	ChannelA      int  `gorm:"column:channel_a"`
	ChannelB      int  `gorm:"column:channel_b"`
	PidFilterMode int  `gorm:"column:pid_filter_mode"`
	Bidirectional bool `gorm:"column:bidirectional"`
	Enabled       bool `gorm:"column:enabled"`
}
