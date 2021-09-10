# rtmp


rtmp://192.168.0.108:1935/live
rtmp://ivi.bupt.edu.cn:1935/livetv/chcatv
RTMP_LNK
	hostname(与port是一对)
	    av_val: "192.168.0.108:1935/live"
	    av_len: 13(表示192.168.0.108这部分内容)
	    av_val: "192.168.0.108:1935/live"
	    av_len: 13
	port
	    1935

	sockshost(与socksport是一对)
	    av_val:
	    av_len:
	socksport
	    0

	app
	    av_val: "live"
	    av_len: 4(live的长度)
	    av_val: "livetv/chcatv"
	    av_len: 6(livetv的长度)
	protocol
	    16(RTMP_FEATURE_WRITE)
	timeout
	    10
	swfAge
	    30



























#include <netinet/in.h>或#include <arpa/inet.h>
struct in_addr {
    in_addr_t s_addr;// 32位IPv4地址
};
struct sockaddr_in {
	sa_family_t    sin_family; // 地址族
	uint16_t       sin_port;   // 16位TCP/UDP端口号
	struct in_addr sin_addr;   // 32位IP地址
	char           siz_zero[8];// 不使用
}

struct sockaddr_in service;
memset(&service, 0, sizeof(struct sockaddr_in));
service.sin_family = AF_INET;
service->sin_addr.s_addr = inet_addr("ivi.bupt.edu.cn");
service->sin_port = htons(1935);// 36615

r->m_sb.sb_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
if (connect(r->m_sb.sb_socket, service, sizeof(struct sockaddr)) < 0) {}

#define SET_RCVTIMEO(tv,s)	struct timeval tv = {s,0}
if (setsockopt(r->m_sb.sb_socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(tv))) {}
setsockopt(r->m_sb.sb_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &on, sizeof(on));
































































































