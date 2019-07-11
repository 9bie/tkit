package main

import (
	"fmt"
	"github.com/gorilla/websocket"
	"github.com/labstack/echo"
	"github.com/labstack/echo/middleware"
	"html/template"
	"io"
	"log"
	"net/http"
)

var upgrader = websocket.Upgrader{}
var wsMap []*websocket.Conn
// TemplateRenderer is a custom html/template renderer for Echo framework
type TemplateRenderer struct {
	templates *template.Template
}

func RemoveWs(i *websocket.Conn) []*websocket.Conn {
	for i2, v := range wsMap {
		if v == i {
			return append(wsMap[:i2], wsMap[i2+1:]...)
		}
	}
	//missing value
	return wsMap
}
func Broadcast(msg string) {
	for _, i := range wsMap {
		err := i.WriteMessage(websocket.TextMessage, []byte(msg))
		if err != nil {
			i.Close()
		}
	}
}

// Render renders a template document
func (t *TemplateRenderer) Render(w io.Writer, name string, data interface{}, c echo.Context) error {
	// Add global methods if data is a map
	if viewContext, isMap := data.(map[string]interface{}); isMap {
		viewContext["reverse"] = c.Echo().Reverse
	}

	return t.templates.ExecuteTemplate(w, name, data)
}

func WsHandle(c echo.Context) error {
	ws, err := upgrader.Upgrade(c.Response(), c.Request(), nil)
	if err != nil {
		return err
	}
	defer func() {
		_ = ws.Close()
	}()
	wsMap = append(wsMap, ws)
	for ; ; {
		_, msg, err := ws.ReadMessage()
		if err != nil {
			return err
		}
		switch string(msg) {
		case "online":
			var n string
			if _, ok := serverMap[nowHandle]; nowHandle != nil && !ok {
				n = ""
			} else {
				n = serverMap[nowHandle].ip
			}
			msg := fmt.Sprintf("config|%s|%s|%s", httpPort, serverPort, n)
			err := ws.WriteMessage(websocket.TextMessage, []byte(msg))
			if err != nil {
				wsMap = RemoveWs(ws)
				return err
			}
			for _,v := range serverMap{
				onlineMsg := fmt.Sprintf("add|%s|%s|%s|%s|%s",v.uuid,v.intIp,v.ip,v.memory,v.OS)
				err := ws.WriteMessage(websocket.TextMessage, []byte(onlineMsg))
				if err != nil {
					wsMap = RemoveWs(ws)
					return err
				}
			}
		case "offline":
			wsMap = RemoveWs(ws)
			return nil
		}
	}
}

func Index(c echo.Context) error {

	return c.Render(http.StatusOK, "Manager.html", nil)
}

func HTTPService(port string) {
	e := echo.New()
	e.Use(middleware.Logger())
	e.Use(middleware.Recover())
	renderer := &TemplateRenderer{
		templates: template.Must(template.ParseGlob("templates/*.html")),
	}
	e.Renderer = renderer
	e.GET("/", Index)
	e.GET("/ws", WsHandle)
	e.Static("/static", "templates/static")
	log.Fatal(e.Start(port))
}
