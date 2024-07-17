package entity

type TraceLog struct {
	Entity
	TraceID  int    `gorm:"column:trace_id"`
	Channel  string `gorm:"column:channel"`
	Interval int    `gorm:"column:interval"`
	Pid      int    `gorm:"column:pid"`
	Bytes    int    `gorm:"column:bytes"`
	Byte0    int    `gorm:"column:byte0"`
	Byte1    int    `gorm:"column:byte1"`
	Byte2    int    `gorm:"column:byte2"`
	Byte3    int    `gorm:"column:byte3"`
	Byte4    int    `gorm:"column:byte4"`
	Byte5    int    `gorm:"column:byte5"`
	Byte6    int    `gorm:"column:byte6"`
	Byte7    int    `gorm:"column:byte7"`
}
