package main
import (
    "fmt"
    "strings"
    "strconv"
    "errors"
)

func gMap(s string) (map[string]int, error) {
	//fmt.Printf("gMap: [%v] \n", s)
	ss := strings.Split(s, ",")
	m := make(map[string]int)
	for _, pair := range ss {
		z := strings.Split(pair, "=")
		if len(z)<2 {
		    return nil,errors.New("Runtime Error")
		}
		i,e:=strconv.ParseInt(z[1], 0, 32)
		if e != nil {
		    return nil,e
		}
		m[z[0]] = int(i)
	}
	return m,nil
}
func main() {
	//s := "A=1,B=2,C=3"
	s := "A=1,B=2,C=3"
	m,err := gMap(s)
	if err != nil {
	    fmt.Printf("Invalid Agent Options: %v", s)
	}
	fmt.Printf("gMap: [%v] \n", m)
}
