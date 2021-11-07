#include <iostream>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include "librtmp/rtmp.h"
};

#define JNI_OK          (0)         /* no error */
#define JNI_ERR         (-1)        /* generic error */
#define JNI_EDETACHED   (-2)        /* thread detached from the VM */
#define JNI_EVERSION    (-3)        /* JNI version error */

int main() {
    std::cout << "Hello, World!" << std::endl;

    char test1[] = "rtmp://ivi.bupt.edu.cn:1935/livetv/chcatv";
    char test2[] =     "://ivi.bupt.edu.cn:1935/livetv/chcatv";
    int len = (int) (test2 - test1);
    printf("test1-test2: %d\n", (test1 - test2));//  48
    printf("test2-test1: %d\n", (test2 - test1));// -48
    printf("test2-test1: %d\n", len);            // -48

    char *url = "rtmp://ivi.bupt.edu.cn:1935/livetv/chcatv";
    char *p =       "://ivi.bupt.edu.cn:1935/livetv/chcatv";
    len = (int) (p - url);                              //  48
    printf("p - url    : %d\n", len);
    len = (int) (url - p);                              // -48
    printf("url - p    : %d\n", len);
    char *end = p - strlen(p);
    printf("strlen(p)  : %d\n", strlen(p));      //  37
    len = end - p;
    printf("end - p    : %d\n", len);

    char test[] = "helloworld";
    char *ptr = static_cast<char *>(calloc(1, 6 + 5));
    memcpy(ptr, test, 10);
    char *ptr_temp = ptr + 5;
    printf("%s\n", ptr);
    printf("%s\n", ptr_temp);
    free(ptr);
    ptr = NULL;

    p = NULL;
    const char *ppstart = "chcatv";
    int pplen = 6;
    char *destptr = NULL;
    char *streamname = (char *) malloc(11 * sizeof(char));
    destptr = streamname;
    for (p = (char *) ppstart; pplen > 0;) {
        *destptr++ = *p++;// 即 *(p++) 表达的意思是：先运算“p++”，结果是当前的这行代码p=“chcatv”，然后取‘c’给左边的变量，走到下一行代码时p=“hcatv”.
        pplen--;
        printf("---------------------------------\n");
        printf("test() streamname: %s\n", streamname);
        printf("test()    destptr: %s\n", destptr);
        printf("test()          p: %s\n", p);
    }
    *destptr = '\0';
    free(streamname);
    streamname = NULL;

    static RTMP *rtmp = nullptr;
    do {
        rtmp = RTMP_Alloc();
        RTMP_Init(rtmp);
        rtmp->Link.timeout = 10;
        if (!RTMP_SetupURL(rtmp, "rtmp://ivi.bupt.edu.cn:1935/livetv/chcatv")) {
            //if (!RTMP_SetupURL(rtmp, "rtmp://192.168.0.108:1935/live")) {
            //if (!RTMP_SetupURL(rtmp, "rtmp://192.168.43.182/live/stream")) {
            //if (!RTMP_SetupURL(rtmp, "rtmp://192.168.1.104/live/stream")) {
            printf("onTransact_init_rtmp() RTMP_SetupURL");
            return JNI_ERR;
        }
        RTMP_EnableWrite(rtmp);
        if (!RTMP_Connect(rtmp, NULL)) {
            printf("onTransact_init_rtmp() RTMP_Connect");
            return JNI_ERR;
        }
        if (!RTMP_ConnectStream(rtmp, 0)) {
            printf("onTransact_init_rtmp() RTMP_ConnectStream");
            return JNI_ERR;
        }
    } while (0);

    if (rtmp) {
        if (RTMP_IsConnected(rtmp)) {
            printf("onTransact_release() RTMP_Close(rtmp)");
            RTMP_Close(rtmp);
        }
        printf("onTransact_release() RTMP_Free(rtmp)");
        RTMP_Free(rtmp);
        rtmp = nullptr;
    }

    return JNI_OK;
}

/*
 strstr(url, "://");
 strncasecmp(url, "rtmp", 4)

 char *url="rtmp://ivi.bupt.edu.cn:1935/livetv/chcatv";
 char *p=      "://ivi.bupt.edu.cn:1935/livetv/chcatv";
 int len = (int) (p - url);// 4(字符串少的减去字符串多的为正整数)
 p是一个字符串，则*p代表的是一个字符，而且是这个字符串首地址的字符
 char *end = p + strlen(p);// end=“”
 int len = end - p;// 计算p的字符串长度。strlen(p);

 if (strncmp(p, "ondemand/", 9) == 0){...}

 char *p=NULL;
 char *ppstart="chcatv";
 int pplen=6;
 char *destptr=NULL;
 char *streamname = (char *) malloc(11 * sizeof(char));
 destptr=streamname;
 for (p = (char *) ppstart; pplen > 0;) {
     *destptr++ = *p++;// 难理解
     pplen--;
 }
 *destptr = '\0';







 */