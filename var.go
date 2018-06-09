// CLI for creating or extracting var archives
// This does not use the standard var API and instead uses a streaming
// variant written in Go, which may eventually be moved into its own file
// for public usage
package main

import (
	"flag"
	"fmt"
	"io"
	"os"
)

func errln(args... interface{}) {
	fmt.Fprintln(os.Stderr, args...)
}

func extractArchive(archive io.Reader) int {
	for {
		var fn []byte
		buf := make([]byte, 1)
		if _, err := archive.Read(buf); err == io.EOF {
			break
		} else if err != nil {
			errln("Error reading from archive:", err)
			return 1
		}

		for {
			if buf[0] == 0 {
				break
			}
			fn = append(fn, buf[0])
			if _, err := archive.Read(buf); err != nil {
				errln("Error reading from archive:", err)
				return 1
			}
		}

		sb := make([]byte, 8) // Size buffer
		if _, err := io.ReadFull(archive, sb); err != nil {
			errln("Error reading from archive:", err)
			return 1
		}
		size :=
			uint64(sb[0]) << (8*7) |
			uint64(sb[1]) << (8*6) |
			uint64(sb[2]) << (8*5) |
			uint64(sb[3]) << (8*4) |
			uint64(sb[4]) << (8*3) |
			uint64(sb[5]) << (8*2) |
			uint64(sb[6]) << (8*1) |
			uint64(sb[7]) << (8*0)

		f, err := os.Create(string(fn))
		if err != nil {
			errln("Open failed for", string(fn))
			return 1
		}
		if _, err := io.CopyN(f, archive, int64(size)); err != nil {
			errln("Error copying data out of archive:", err)
			return 1
		}
	}
	return 0
}

func extract(targets []string) int {
	if len(targets) == 0 {
		return extractArchive(os.Stdin)
	}

	for _, fn := range targets {
		archive, err := os.Open(fn)
		if err != nil {
			errln("Open failed for", fn)
			return 1
		}
		if code := extractArchive(archive); code != 0 {
			return code
		}
		archive.Close()
	}
	return 0
}

func create(targets []string) int {
	for _, fn := range targets {
		f, err := os.Open(fn)
		if err != nil {
			errln("Open failed for", fn)
			return 1
		}
		stat, err := f.Stat()
		if err != nil {
			errln("Stat failed for", fn)
			return 1
		}
		size := uint64(stat.Size())

		os.Stdout.WriteString(fn)
		os.Stdout.Write([]byte{
			0, // NUL terminator
			byte((size >> (8*7)) & 0xff),
			byte((size >> (8*6)) & 0xff),
			byte((size >> (8*5)) & 0xff),
			byte((size >> (8*4)) & 0xff),
			byte((size >> (8*3)) & 0xff),
			byte((size >> (8*2)) & 0xff),
			byte((size >> (8*1)) & 0xff),
			byte((size >> (8*0)) & 0xff),
		})
		if _, err := io.Copy(os.Stdout, f); err != nil {
			errln("Error copying data to archive:", err)
			return 1
		}
		f.Close()
	}
	return 0
}

func main() {
	xMode := flag.Bool("x", false, "Extract mode")
	flag.Parse()

	if *xMode {
		os.Exit(extract(flag.Args()))
	} else {
		os.Exit(create(flag.Args()))
	}
}
