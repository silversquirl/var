# var

var is a super-simple, minimal-to-the-point-of-almost-useless archive
format. It stores files sequentially with a single piece of metadata: the
name.

The format for a single file is as follows:

 - NUL terminated name
 - 64-bit unsigned integer for file size
 - file contents

Multiple files are stored in a single archive by repeating the above block
for each file. This format makes encoding and decoding the archive
incredibly simple and incredibly fast even without optimisation, but makes
it much harder to store metadata (you would need to do so in the name or
the file itself) and makes modifying archives after creation much slower
and more complex.

## Documentation

Read the source. It's 2 functions and less than 50 lines. `example.c`
contains an example usage.
