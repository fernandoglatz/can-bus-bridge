package main

import (
	"context"
	"fernandoglatz/can-bus-bridge/internal/core/common/utils/log"
	"fernandoglatz/can-bus-bridge/internal/core/entity"
	"fernandoglatz/can-bus-bridge/internal/core/service"
	"fernandoglatz/can-bus-bridge/internal/gui"
	"fmt"
	"time"

	"fernandoglatz/can-bus-bridge/internal/core/common/utils"
	"fernandoglatz/can-bus-bridge/internal/infrastructure/config"

	"github.com/joho/godotenv"
)

// @title			can-bus-bridge
// @version			1.0

// @license.name	Apache 2.0
// @license.url		http://www.apache.org/licenses/LICENSE-2.0.html

// @BasePath		/api

// @securityDefinitions.basic	BasicAuth
// @securityDefinitions.apikey	Bearer
// @in 				header
// @name 			X-AUTHORIZATION
// @description     Generated by /authentication

func main() {
	ctx := context.Background()
	godotenv.Load()

	err := config.LoadConfig(ctx)
	if err != nil {
		log.Fatal(ctx).Msg(err.Error())
	}

	err = utils.ConnectToSQLite(ctx)
	if err != nil {
		log.Fatal(ctx).Msg(err.Error())
	}

	settingsService := service.GetGenericService[*entity.Settings]()
	settingsArray, errw := settingsService.GetAll(ctx, 0, 10)
	if errw != nil {
		log.Fatal(ctx).Msg(errw.GetMessage())
	}

	for _, setting := range settingsArray {
		log.Info(ctx).Msg(fmt.Sprintf("ID: %d, CreatedAt: %s, Key: %s, Value: %s", setting.ID, setting.CreatedAt.Format(time.RFC3339), setting.Key, setting.Value))
	}

	settings := &entity.Settings{
		Key:   "teste",
		Value: "valor",
	}
	errw = settingsService.Save(ctx, settings)
	if errw != nil {
		log.Fatal(ctx).Msg(errw.GetMessage())
	}

	err = gui.Start(ctx)
	if err != nil {
		log.Fatal(ctx).Msg(err.Error())
	}
}
