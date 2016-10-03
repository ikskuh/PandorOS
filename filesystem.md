# PandorOS File System

## Structure

The file is structued like this

	Header
	(File Definition, File Data)*
	End Marker

So each file begins with a `Header`, is then followed by an arbitrary number
of `File Definitions` followed by `File Data` and is finally terminated by
an `End Marker`.

### Header
The file has a very simple header that consists only of a single magic number:

| Offset | Size | Type  | Description                          |
|--------|------|-------|--------------------------------------|
|      0 |    4 | u32   | Magic Number, must be 0xD05E4ABC     |

### File Definition

| Offset | Size | Type  | Description                          |
|--------|------|-------|--------------------------------------|
|      0 |    1 | u32   | File Size, must be larger than 0     |
|      4 |   16 | c[16] | The file name with zero termination  |

### File Data
This section contains flat binary data of the previous file definition.

### End Marker
| Offset | Size | Type  | Description                          |
|--------|------|-------|--------------------------------------|
|      0 |    4 | u32   | End Marker Magic Number, must be 0   |
