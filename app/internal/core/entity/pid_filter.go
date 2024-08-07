package entity

type PidFilter struct {
	Entity
	ChannelID int  `gorm:"column:channel_id"`
	Pid       int  `gorm:"column:pid"`
	Enabled   bool `gorm:"column:enabled"`
}
