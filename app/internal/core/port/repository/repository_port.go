package repository

import (
	"context"
	"fernandoglatz/can-bus-bridge/internal/core/common/utils/exceptions"
	"fernandoglatz/can-bus-bridge/internal/core/entity"
)

type IRepository[T entity.IEntity] interface {
	Get(ctx context.Context, id int) (T, *exceptions.WrappedError)
	GetAll(ctx context.Context, page int, limit int) ([]T, *exceptions.WrappedError)
	Save(ctx context.Context, entity T) *exceptions.WrappedError
	Remove(ctx context.Context, entity T) *exceptions.WrappedError
}
