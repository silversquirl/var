all: var example
var: var.go
	go build $<
example: example.c
	$(CC) -o $@ $<
