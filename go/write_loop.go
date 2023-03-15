package main

/*
#include <unistd.h>
#include <sys/types.h>
*/
import "C"
import (
    "fmt"
    "os"
    "strconv"
    "unsafe"
)

// a parameters 
type params struct {
    numBytes int
    numWrites int
    filePath string
}
// global variable p
var p params

func setup() (*os.File) {
    if len(os.Args) < 4 {
        fmt.Println("Usage: program <num_bytes> <num_writes> <file_path>")
        os.Exit(1)
    }
    // declare err
    var err error
    p.numBytes, err = strconv.Atoi(os.Args[1])
    if err != nil {
        fmt.Println("Error parsing num_bytes:", err)
        os.Exit(1)
    }

    p.numWrites, err = strconv.Atoi(os.Args[2])
    if err != nil {
        fmt.Println("Error parsing num_writes:", err)
        os.Exit(1)
    }

    p.filePath = os.Args[3]

    if _, err := os.Stat(p.filePath); err == nil {
        if err := os.Remove(p.filePath); err != nil {
            fmt.Println("Error deleting existing file:", err)
            os.Exit(1)
        }
    }

    file, err := os.Create(p.filePath)
    if err != nil {
        fmt.Println("Error creating file:", err)
        os.Exit(1)
    }
    return file
}

func glibc_benchmark() {
    file := setup()

    defer file.Close()

    file_int := C.int(file.Fd())
    buffer := make([]byte, p.numBytes)
    for i := 0; i < p.numWrites; i++ {
        if _, err := C.write(file_int, unsafe.Pointer(&buffer[0]), C.size_t(len(buffer))); err != nil {
            fmt.Println("Error writing to file:", err)
            os.Exit(1)
        }
    }
}

func go_benchmark() {
    file := setup()

    defer file.Close()

    buffer := make([]byte, p.numBytes)
    for i := 0; i < p.numWrites; i++ {
        if _, err := file.Write(buffer); err != nil {
            fmt.Println("Error writing to file:", err)
            os.Exit(1)
        }
    }

}

func print_glibc() {
    fd := C.int(1) // file descriptor for stdout
    buf := []byte("Hello, World!\n")
    _, err := C.write(fd, unsafe.Pointer(&buf[0]), C.size_t(len(buf)))
    if err != nil {
        panic(err)
    }
}

func main() {
    // go_benchmark()
    glibc_benchmark()

    fmt.Println("Done writing to file.")
}
