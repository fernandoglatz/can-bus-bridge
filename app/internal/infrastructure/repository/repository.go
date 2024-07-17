package repository

import (
	"context"
	"sync"
	"time"

	"fernandoglatz/can-bus-bridge/internal/core/common/utils"
	"fernandoglatz/can-bus-bridge/internal/core/common/utils/constants"
	"fernandoglatz/can-bus-bridge/internal/core/common/utils/exceptions"
	"fernandoglatz/can-bus-bridge/internal/core/entity"
)

const CREATED_AT = "created_at"

var repositories map[string]any
var repositoryMutex sync.Mutex

type Repository[T entity.IEntity] struct {
}

func GetGenericRepository[T entity.IEntity]() Repository[T] {
	entity := utils.Instance[T]()
	typeName := utils.GetTypeName(entity)

	repositoryMutex.Lock()
	defer repositoryMutex.Unlock()

	if repositories == nil {
		repositories = make(map[string]any)
	}

	repository := repositories[typeName]

	if repository == nil {
		repository = Repository[T]{}

		repositories[typeName] = repository
	}

	return repository.(Repository[T])
}

func (repository Repository[T]) Get(ctx context.Context, id int) (T, *exceptions.WrappedError) {
	entity := utils.Instance[T]()
	filter := utils.Instance[T]()

	filter.SetID(id)

	result := utils.SQLiteDatabase.Where(&filter).First(&entity)
	if result.Error != nil {
		return entity, &exceptions.WrappedError{
			Error: result.Error,
		}
	}

	return entity, nil
}

func (repository Repository[T]) GetAll(ctx context.Context, page int, limit int) ([]T, *exceptions.WrappedError) {
	var entities []T = []T{}
	filter := utils.Instance[T]()

	skip := (page - constants.ONE) * limit

	result := utils.SQLiteDatabase.Where(&filter).Offset(skip).Limit(limit).Order(CREATED_AT + " DESC").Find(&entities)
	if result.Error != nil {
		return entities, &exceptions.WrappedError{
			Error: result.Error,
		}
	}

	return entities, nil
}

func (repository Repository[T]) Save(ctx context.Context, entity T) *exceptions.WrappedError {
	if entity.GetCreatedAt().IsZero() {
		now := time.Now()
		entity.SetCreatedAt(now)
	}

	transaction := utils.SQLiteDatabase.Save(entity)
	err := transaction.Error

	if err != nil {
		return &exceptions.WrappedError{
			Error: err,
		}
	}

	return nil
}

func (repository Repository[T]) Remove(ctx context.Context, entity T) *exceptions.WrappedError {
	transaction := utils.SQLiteDatabase.Delete(entity)
	err := transaction.Error

	if err != nil {
		return &exceptions.WrappedError{
			Error: err,
		}
	}

	return nil
}
