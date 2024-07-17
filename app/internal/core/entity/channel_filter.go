package entity

type ChannelFilter struct {
	Entity
	ChannelID int  `gorm:"column:channel_id"`
	Extended  bool `gorm:"column:extended"`
	Mask      int  `gorm:"column:mask"`
	Filter    int  `gorm:"column:filter"`
	Enabled   bool `gorm:"column:enabled"`
}
