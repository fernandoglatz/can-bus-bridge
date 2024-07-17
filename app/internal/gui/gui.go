package gui

import (
	"context"
	"fernandoglatz/can-bus-bridge/internal/core/common/utils/log"
	"image/color"
	"net/url"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/widget"
)

var myApp fyne.App
var window fyne.Window

type MyTheme struct{}

func Start(ctx context.Context) error {
	log.Info(ctx).Msg("Starting GUI")

	myApp = app.New()
	window = myApp.NewWindow("can-bus-bridge")

	menuItemQuit := fyne.NewMenuItem("Quit", func() {
		Quit(ctx)
	})
	menuItemAbout := fyne.NewMenuItem("About", showAboutDialog)
	fileMenu := fyne.NewMenu("File", menuItemQuit)
	helpMenu := fyne.NewMenu("Help", menuItemAbout)
	menu := fyne.NewMainMenu(fileMenu, helpMenu)
	window.SetMainMenu(menu)

	label := widget.NewLabel("can-bus-bridge")
	window.SetContent(container.NewVBox(label))

	window.Resize(fyne.NewSize(1024, 640))
	window.CenterOnScreen()

	myTheme := MyTheme{}
	myApp.Settings().SetTheme(myTheme)

	window.Show()
	myApp.Run()

	return nil
}

func showAboutDialog() {
	githubUrl, _ := url.Parse("https://github.com/fernandoglatz/can-bus-bridge")

	content := container.NewVBox(
		widget.NewLabel("can-bus-bridge"),
		widget.NewLabel("Version: 0.1"),
		widget.NewHyperlink("https://github.com/fernandoglatz/can-bus-bridge", githubUrl),
		widget.NewLabel("Copyright (c) 2024 Fernando Glatz"),
	)

	aboutDialog := dialog.NewCustom("About", "Close", content, window)
	aboutDialog.Show()
}

func Quit(ctx context.Context) {
	log.Info(ctx).Msg("Stopping GUI")
	myApp.Quit()
}

func (myTheme MyTheme) Color(name fyne.ThemeColorName, variant fyne.ThemeVariant) color.Color {
	return theme.DefaultTheme().Color(name, variant)
}

func (myTheme MyTheme) Icon(name fyne.ThemeIconName) fyne.Resource {
	return theme.DefaultTheme().Icon(name)
}

func (myTheme MyTheme) Font(style fyne.TextStyle) fyne.Resource {
	return theme.DefaultTheme().Font(style)
}

func (myTheme MyTheme) Size(name fyne.ThemeSizeName) float32 {
	return theme.DefaultTheme().Size(name)
}
