Downloader_Libcurl:Downloader_Libcurl.c
	gcc Downloader_Libcurl.c -o Downloader_Libcurl `pkg-config --libs --cflags gtk+-2.0` -lcurl -lgthread-2.0

.PHONY:clean
clean:
	rm -f Downloader_Libcurl
