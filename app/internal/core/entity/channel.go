package entity

type Channel struct {
	Entity
	Pin      int  `json:"pin,omitempty" gorm:"column:pin"`
	Channel  int  `json:"channel,omitempty" gorm:"column:channel"`
	Baudrate int  `json:"baudrate,omitempty" gorm:"column:baudrate"`
	Clock    int  `json:"clock,omitempty" gorm:"column:clock"`
	Readable bool `json:"readable,omitempty" gorm:"column:readable"`
}
