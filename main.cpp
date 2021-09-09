#include <iostream>
extern "C" {
#include "librtmp/rtmp.h"
};

#define JNI_OK          (0)         /* no error */
#define JNI_ERR         (-1)        /* generic error */
#define JNI_EDETACHED   (-2)        /* thread detached from the VM */
#define JNI_EVERSION    (-3)        /* JNI version error */

int main() {
    std::cout << "Hello, World!" << std::endl;

    static RTMP *rtmp = nullptr;
    do {
        rtmp = RTMP_Alloc();
        RTMP_Init(rtmp);
        rtmp->Link.timeout = 10;
        if (!RTMP_SetupURL(rtmp, "rtmp://192.168.0.108:1935/live")) {
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

    return JNI_OK;
}
