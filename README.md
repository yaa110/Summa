Summa
=====
Daylight Saving Examples in C

## Examples

### [benchmark-server.c](https://github.com/yaa110/Summa/blob/master/src/benchmark-server.c)
<<<<<<< HEAD
Creates a socket to perform download or upload benchmark tests.
=======
Opens a socket to perform download or upload benchmark tests.
>>>>>>> Add benchmark server example

**Compile** `make benchmark-server`

#### Test
- **Download Server** `benchmark-server -d <port>`
- **Download Client** `curl -s -m 10 -w "Speed: %{speed_download} bytes/sec\n" http://<server-ip>:<port> -o /dev/null`
- **Upload Server** `benchmark-server -u <port>`
- **Upload Client** `curl -s -m 10 -w "Speed: %{speed_upload} bytes/sec" -d @"/path/to/file" http://<server-ip>:<port>`

