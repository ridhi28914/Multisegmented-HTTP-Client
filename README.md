# Multisegmented-HTTP-Client
Multi-segmented HTTP client program with GUI using multithreading in C++ and GTK

It increases the downloading speed of very large files using multithreading(used POSIX threads).
For very large files, multiple threads are created and each thread creates an instruction connection and data connection with the server or to the mirror of the server.Each thread downloads a specific chunk of data and then all are combined together to get the file.

Note:
Use only for very large files because creating and managing threads is also an overhead for CPU. Performance will degrade for smaller files.



Acknowledgements :
Professor Khushil Saini, for the project idea and concepts about multithreading.
Stackoverflow community(http://stackoverflow.com).
The very helpful documentation of HTTP RFC 2616(https://www.ietf.org/rfc/rfc2616.txt).
