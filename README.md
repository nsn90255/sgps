# SGPS
A mostly RFC-1436 and RFC-4266 compliant multithreated gopher 
server.

Missing features :
- Type '2' CSO phone-book server requests
- Type '7' index-search server requests
- Type '8' telnet session requests
- Type 'T' tn-3270 session requests

I am not planning on implementing these due to their [lack of popularity](https://sunriseprogrammer.blogspot.com/2019/03/directory-entry-says-what-current.html).

Extra features :
- Support for gophermaps
- Support for widely used types in the gohper community ('d' and 'i')

> [!NOTE]
> This has only been tested on x86_64 GNU/Linux using the gopherus client.
