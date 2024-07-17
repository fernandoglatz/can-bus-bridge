package entity

type Settings struct {
	Entity
	Key   string `json:"key,omitempty" gorm:"column:key"`
	Value string `json:"value,omitempty" gorm:"column:value"`
}
