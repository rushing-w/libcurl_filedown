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
	
	curl = curl_easy_init();//初始化CURL指针，开始会话，easy_handle用于数据通信，线程间不能共享这个easy_handle
	if(curl)
	{
		outfile = fopen("1223.jpg", "w");

		curl_easy_setopt(curl, CURLOPT_URL, url);//访问url
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);//数据存储的对象指针
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write_func);//通过回调函数完成数据下载
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, my_read_func);//通过回调函数上传数据
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);//设为不验证证书
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, my_progress_func);//每秒调用一次函数
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, Bar);

		res = curl_easy_perform(curl);//完成传输任务

		fclose(outfile);
		curl_easy_cleanup(curl);//结束一个会话

		//只要提供一个url，libcurl会完成所有的下载过程
	}
	return NULL;
}

int main(int argc, char** argv)
{
	GtkWidget *Window, *Frame, *Frame2;//利用gtk库实现界面设计
	GtkAdjustment *adj;

	curl_global_init(CURL_GLOBAL_ALL);//初始化libcurl

	g_thread_init(NULL);

	gtk_init(&argc, &argv);
	Window = gtk_window_new(GTK_WINDOW_TOPLEVEL);//gtk创建窗口，参数表示顶层窗口，有边框
	Frame = gtk_frame_new(NULL);//设置框架
	gtk_frame_set_shadow_type(GTK_FRAME(Frame), GTK_SHADOW_OUT);//设置框架格式，显示框架轮廓
	gtk_container_add(GTK_CONTAINER(Window), Frame);//通过容器添加到窗口中

	Frame2 = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(Frame2), GTK_SHADOW_IN);
	gtk_container_add(GTK_CONTAINER(Frame), Frame2);
	gtk_container_set_border_width(GTK_CONTAINER(Frame2), 5);//设置视窗边框宽度

	adj = (GtkAdjustment*)gtk_adjustment_new(0, 0, 100, 0, 0, 0);//创建调整对象
	Bar = gtk_progress_bar_new_with_adjustment(adj);//建立进度条构件

	gtk_container_add(GTK_CONTAINER(Frame2), Bar);//将进度条加到里面
	gtk_widget_show_all(Window);

	if(!g_thread_create(&my_thread, argv[1], FALSE, NULL) != 0)//线程创建
	{
		g_warning("can not create the thread");
	}

	gdk_threads_enter();//进入多线程互斥区域
	gtk_main();//调用glib的main loop
	gdk_threads_leave();//退出多线程互斥区域

	return 0;
}
