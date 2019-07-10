package main

import "net"

type S struct {
	memory string
	OS     string
	ip     string
	status int // CONST: SERVER_
}

var tcpConn bool
var httpConn bool
var serverMap = make(map[net.Conn]S)
var sign = "customize\x00"
var shellInChan = make(chan string)
var shellOutChan = make(chan string)
var nowHandle net.Conn
var httpPort string
var serverPort string
func main() {
	httpPort = ":80"
	serverPort = ":81"
	HTTPService(httpPort)
	//listen(serverPort)
}
