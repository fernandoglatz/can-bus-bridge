package config

import (
	"context"
	"fernandoglatz/can-bus-bridge/internal/core/common/utils/constants"
	"fernandoglatz/can-bus-bridge/internal/core/common/utils/log"
	"os"
)

func LoadConfig(ctx context.Context) error {
	loadProfile(ctx)

	loggingLevel := os.Getenv(constants.LOGGING_LEVEL)
	if len(loggingLevel) == constants.ZERO {
		loggingLevel = "INFO"
	}

	log.ReconfigureLogger(ctx, loggingLevel, true)

	return nil
}

func IsDevProfile() bool {
	profile := os.Getenv(constants.PROFILE)
	return constants.DEV_PROFILE == profile
}

func loadProfile(ctx context.Context) {
	profile := os.Getenv(constants.PROFILE)
	if len(profile) == constants.ZERO {
		profile = constants.DEV_PROFILE
		os.Setenv(constants.PROFILE, profile)
	}

	log.SetupLogger(profile) //after setup profile
	log.Info(ctx).Msg("Profile loaded: " + profile)
}
