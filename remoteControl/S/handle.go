package main

import (
	"fmt"
	"net"
	"unsafe"
)

type CMSG struct {
	sign  [10]byte
	mod   int
	msg_l int
}

func listen(port string) {
	fmt.Println("Starting the server ...")
	// 创建 listener
	listener, err := net.Listen("tcp", port)
	if err != nil {
		fmt.Println("Error listening", err.Error())
		return //终止程序
	}
	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Println("Error accepting", err.Error())
			return // 终止程序
		}
		go doServerStuff(conn)
	}
}

// 线程池
func doServerStuff(conn net.Conn) {
	for {
		buf := make([]byte, unsafe.Sizeof(CMSG{}))
		l, err := conn.Read(buf)
		if err != nil {
			fmt.Println("Error reading Code:", l, err.Error())
			return //终止程序
		}
		var msg *CMSG = *(**CMSG)(unsafe.Pointer(&buf))
		fmt.Printf("Recv:\n\tMSG: %s \n\tMOD: %d \n\tLONG: %d \n", msg.sign, msg.mod, msg.msg_l)
		if string(msg.sign[:]) == "customize" {
			if msg.msg_l != 0 {
				buf := make([]byte, msg.msg_l)
				l, err := conn.Read(buf)
				if err != nil {
					fmt.Print("Error reading in inline read.", err.Error())
					return
				}
				fmt.Printf("data: %v", buf[:l])
			}
		}
	}
}
