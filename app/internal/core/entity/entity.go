package entity

import "time"

type Entity struct {
	ID        int       `gorm:"primaryKey"`
	CreatedAt time.Time `gorm:"column:created_at"`
}

func (entity Entity) GetID() int {
	return entity.ID
}

func (entity *Entity) SetID(id int) {
	entity.ID = id
}

func (entity Entity) GetCreatedAt() time.Time {
	return entity.CreatedAt
}

func (entity *Entity) SetCreatedAt(createdAt time.Time) {
	entity.CreatedAt = createdAt
}

type IEntity interface {
	GetID() int
	SetID(int)

	GetCreatedAt() time.Time
	SetCreatedAt(time.Time)
}
