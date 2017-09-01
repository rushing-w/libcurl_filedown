/*************************************************************************
	> File Name: Downloader_Libcurl.c
	> Author: rushing
	> Mail: wangchong_2018@yeah.net 
	> Created Time: Thu 31 Aug 2017 02:13:37 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<gtk/gtk.h>
#include<curl/curl.h>
#include<curl/types.h>
#include<curl/easy.h>
#include<stdlib.h>

GtkWidget *Bar;

//这个函数是为了符合CURLOPT_WRITEFUNCTION而构造
//完成数据保存功能
size_t my_write_func(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	return fwrite(ptr, size, nmemb, stream);
}

//这个函数是为了符合CURLOPT_READFUNCTION而构造
//数据上传时使用
size_t my_read_func(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	return fread(ptr, size, nmemb, stream);
}

//这个函数是为了符合CURLOPT_PROGRESSFUNCTION而构造
//显示文件传输进度，t代表文件大小，d代表文件传输已完成部分
int my_progress_func(GtkWidget* bar, double t, double d, double ultotal, double ulnow)
{
	gdk_threads_enter();
	gtk_progress_set_value(GTK_PROGRESS(bar), d*100.0/t);
	gdk_threads_leave();
	return 0;
}

void* my_thread(void* ptr)
{
	CURL* curl;
	CURLcode res;
	FILE* outfile;
	gchar* url = ptr;
	
	curl = curl_easy_init();
	if(curl)
	{
		outfile = fopen("1223.jpg", "w");

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);//数据存储的对象指针
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write_func);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, my_read_func);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);//设为不验证证书
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, my_progress_func);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, Bar);

		res = curl_easy_perform(curl);

		fclose(outfile);
		curl_easy_cleanup(curl);
	}
	return NULL;
}

int main(int argc, char** argv)
{
	GtkWidget *Window, *Frame, *Frame2;
	GtkAdjustment *adj;

	curl_global_init(CURL_GLOBAL_ALL);

	g_thread_init(NULL);

	gtk_init(&argc, &argv);
	Window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	Frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(Frame), GTK_SHADOW_OUT);
	gtk_container_add(GTK_CONTAINER(Window), Frame);

	Frame2 = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(Frame2), GTK_SHADOW_IN);
	gtk_container_add(GTK_CONTAINER(Frame), Frame2);
	gtk_container_set_border_width(GTK_CONTAINER(Frame2), 5);

	adj = (GtkAdjustment*)gtk_adjustment_new(0, 0, 100, 0, 0, 0);
	Bar = gtk_progress_bar_new_with_adjustment(adj);

	gtk_container_add(GTK_CONTAINER(Frame2), Bar);
	gtk_widget_show_all(Window);

	if(!g_thread_create(&my_thread, argv[1], FALSE, NULL) != 0)
	{
		g_warning("can not create the thread");
	}

	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();

	return 0;
}

