package entity

type Preset struct {
	Entity
	Description string `gorm:"column:description"`
}
