rm -rf $GOPATH/pkg/linux_amd64/github.com/iovisor/gobpf*
unset http_proxy
unset https_proxy
#go get -u -v github.com/iovisor/gobpf/bcc
go install -a github.com/iovisor/gobpf/bcc
