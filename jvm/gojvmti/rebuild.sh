#rm -rf $GOPATH/pkg/linux_amd64/github.com/iovisor/bcc*
rm -rf $GOPATH/pkg/linux_amd64/github.com/weixingsun/gobpf*
#go get -u -v github.com/weixingsun/gobpf/bcc
#go install -a github.com/iovisor/bcc
go install -a github.com/weixingsun/gobpf/bcc
#vi $GOPATH/src/github.com/weixingsun/gobpf/bcc/symbol.go
