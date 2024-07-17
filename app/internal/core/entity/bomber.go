package entity

type Bomber struct {
	Entity
	ChannelID         int  `gorm:"column:channel_id"`
	Type              int  `gorm:"column:type"`
	MessageInterval   int  `gorm:"column:message_interval"`
	IncrementInterval int  `gorm:"column:increment_interval"`
	Pid               int  `gorm:"column:pid"`
	Byte0             int  `gorm:"column:byte0"`
	Byte1             int  `gorm:"column:byte1"`
	Byte2             int  `gorm:"column:byte2"`
	Byte3             int  `gorm:"column:byte3"`
	Byte4             int  `gorm:"column:byte4"`
	Byte5             int  `gorm:"column:byte5"`
	Byte6             int  `gorm:"column:byte6"`
	Byte7             int  `gorm:"column:byte7"`
	BomberByte0       bool `gorm:"column:bomber_byte0"`
	BomberByte1       bool `gorm:"column:bomber_byte1"`
	BomberByte2       bool `gorm:"column:bomber_byte2"`
	BomberByte3       bool `gorm:"column:bomber_byte3"`
	BomberByte4       bool `gorm:"column:bomber_byte4"`
	BomberByte5       bool `gorm:"column:bomber_byte5"`
	BomberByte6       bool `gorm:"column:bomber_byte6"`
	BomberByte7       bool `gorm:"column:bomber_byte7"`
	Infinity          bool `gorm:"column:infinity"`
	Enabled           bool `gorm:"column:enabled"`
}
