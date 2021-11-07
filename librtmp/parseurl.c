/*
 *  Copyright (C) 2009 Andrej Stepanchuk
 *  Copyright (C) 2009-2010 Howard Chu
 *
 *  This file is part of librtmp.
 *
 *  librtmp is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 2.1,
 *  or (at your option) any later version.
 *
 *  librtmp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with librtmp see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *  http://www.gnu.org/copyleft/lgpl.html
 */

#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <ctype.h>

#include "rtmp_sys.h"
#include "log.h"

// rtmp://ivi.bupt.edu.cn:1935/livetv/chcatv
/*
 url代表字符串,而*url代表第一个字符,即'r'
 */
int RTMP_ParseURL(const char *url, int *protocol,
                  AVal *host, unsigned int *port,
                  AVal *playpath, AVal *app) {
    char *p, *end, *col, *ques, *slash;

    RTMP_Log(RTMP_LOGDEBUG, "Parsing...");

    *protocol = RTMP_PROTOCOL_RTMP;// 0
    *port = 0;
    playpath->av_val = NULL;
    playpath->av_len = 0;
    app->av_val = NULL;
    app->av_len = 0;

    /* Old School Parsing */

    /* look for usual :// pattern */
    p = strstr(url, "://");// "://ivi.bupt.edu.cn:1935/livetv/chcatv"
    if (!p) {
        RTMP_Log(RTMP_LOGERROR, "RTMP URL: No :// in url!");
        return FALSE;
    }
    {
        /*
         strncasecmp()用来比较参数s1 和s2 字符串前n个字符，比较时会自动忽略大小写的差异。
         若参数s1 和s2 字符串相同则返回0。s1 若大于s2 则返回大于0 的值，s1 若小于s2 则返回小于0 的值。
         */
        int len = (int) (p - url);// 4
        if (len == 4 && strncasecmp(url, "rtmp", 4) == 0)// 以“rtmp”开头
            *protocol = RTMP_PROTOCOL_RTMP;
        else if (len == 5 && strncasecmp(url, "rtmpt", 5) == 0)
            *protocol = RTMP_PROTOCOL_RTMPT;
        else if (len == 5 && strncasecmp(url, "rtmps", 5) == 0)
            *protocol = RTMP_PROTOCOL_RTMPS;
        else if (len == 5 && strncasecmp(url, "rtmpe", 5) == 0)
            *protocol = RTMP_PROTOCOL_RTMPE;
        else if (len == 5 && strncasecmp(url, "rtmfp", 5) == 0)
            *protocol = RTMP_PROTOCOL_RTMFP;
        else if (len == 6 && strncasecmp(url, "rtmpte", 6) == 0)
            *protocol = RTMP_PROTOCOL_RTMPTE;
        else if (len == 6 && strncasecmp(url, "rtmpts", 6) == 0)
            *protocol = RTMP_PROTOCOL_RTMPTS;
        else {
            RTMP_Log(RTMP_LOGWARNING, "Unknown protocol!\n");
            goto parsehost;
        }
    }

    RTMP_Log(RTMP_LOGDEBUG, "Parsed protocol: %d", *protocol);

    parsehost:

    /* let's get the hostname */
    p += 3;// "ivi.bupt.edu.cn:1935/livetv/chcatv"

    /* check for sudden death */
    if (*p == 0) {// *p = 'i'
        RTMP_Log(RTMP_LOGWARNING, "No hostname in URL!");
        return FALSE;
    }

    end = p + strlen(p);      // ""
    col = strchr(p, ':');  // ":1935/livetv/chcatv"
    ques = strchr(p, '?'); // NULL
    slash = strchr(p, '/');// "/livetv/chcatv"

    {
        int hostlen;
        if (slash) {
            hostlen = slash - p;// 20
        } else {
            hostlen = end - p;
        }
        if (col && (col - p) < hostlen) {
            hostlen = col - p;// 15
        }

        if (hostlen < 256) {
            host->av_val = p;      // "ivi.bupt.edu.cn:1935/livetv/chcatv"
            host->av_len = hostlen;// 15 主机部分的长度,即: ivi.bupt.edu.cn
            RTMP_Log(RTMP_LOGDEBUG, "Parsed host    : %.*s", hostlen, host->av_val);
        } else {
            RTMP_Log(RTMP_LOGWARNING, "Hostname exceeds 255 characters!");
        }

        p += hostlen;// ":1935/livetv/chcatv"
    }

    /* get the port number if available */
    if (*p == ':') {
        unsigned int p2;
        p++;// "1935/livetv/chcatv"
        p2 = atoi(p);// 字符串转化成整数(只转化数字): 1935
        if (p2 > 65535) {
            RTMP_Log(RTMP_LOGWARNING, "Invalid port number!");
        } else {
            *port = p2;// 1935
        }
    }

    if (!slash) {
        RTMP_Log(RTMP_LOGWARNING, "No application or playpath in URL!");
        return TRUE;
    }
    p = slash + 1;// "livetv/chcatv"

    {
        /* parse application
         *
         * rtmp://host[:port]/app[/appinstance][/...]
         * application = app[/appinstance]
         */
        char *slash2 = NULL, *slash3 = NULL;
        int applen, appnamelen;

        slash2 = strchr(p, '/');// "/chcatv"
        if (slash2) {
            slash3 = strchr(slash2 + 1, '/');// 从"chcatv"中查找'/',因为找不到,所以slash3 is NULL
        }

        // 13
        applen = end - p;    /* ondemand, pass all parameters as app */
        appnamelen = applen; /* ondemand length */
        // p="livetv/chcatv"
        if (ques && strstr(p, "slist=")) {
            /* whatever it is, the '?' and slist= means we need to use everything as app and parse plapath from slist= */
            appnamelen = ques - p;
        } else if (strncmp(p, "ondemand/", 9) == 0) {
            /* app = ondemand/foobar, only pass app=ondemand */
            applen = 8;
            appnamelen = 8;
        } else {
            /* app!=ondemand, so app is app[/appinstance] */
            if (slash3) {
                appnamelen = slash3 - p;
            } else if (slash2) {
                appnamelen = slash2 - p;// 6
            }

            applen = appnamelen;
        }

        app->av_val = p;     // "livetv/chcatv"
        app->av_len = applen;// 6
        RTMP_Log(RTMP_LOGDEBUG, "Parsed app     : %.*s", applen, p);

        p += appnamelen;// "/chcatv"
    }

    if (*p == '/') {
        p++;// "chcatv"
    }

    if (end - p) {// 相当于strlen(p)
        AVal av = {p, end - p};// "chcatv" 6
        RTMP_ParsePlaypath(&av, playpath);
    }

    return TRUE;
}

/*
 * Extracts playpath from RTMP URL. playpath is the file part of the
 * URL, i.e. the part that comes after rtmp://host:port/app/
 *
 * Returns the stream name in a format understood by FMS. The name is
 * the playpath part of the URL with formatting depending on the stream
 * type:
 *
 * mp4 streams: prepend "mp4:", remove extension
 * mp3 streams: prepend "mp3:", remove extension
 * flv streams: remove extension
 */
void RTMP_ParsePlaypath(AVal *in, AVal *out) {
    int addMP4 = 0;
    int addMP3 = 0;
    int subExt = 0;
    const char *playpath = in->av_val;// "chcatv"
    const char *ppstart = playpath;
    int pplen = in->av_len;// 6
    const char *temp, *q, *ext = NULL;
    char *streamname, *destptr, *p;


    out->av_val = NULL;
    out->av_len = 0;

    if ((*ppstart == '?') &&
        (temp = strstr(ppstart, "slist=")) != 0) {
        ppstart = temp + 6;
        pplen = strlen(ppstart);

        temp = strchr(ppstart, '&');
        if (temp) {
            pplen = temp - ppstart;
        }
    }

    q = strchr(ppstart, '?');
    if (pplen >= 4) {
        if (q)
            ext = q - 4;
        else
            ext = &(ppstart[pplen - 4]);// "catv"
        if ((strncmp(ext, ".f4v", 4) == 0) ||
            (strncmp(ext, ".mp4", 4) == 0)) {
            addMP4 = 1;
            subExt = 1;
            /* Only remove .flv from rtmp URL, not slist params */
        } else if ((ppstart == playpath) &&
                   (strncmp(ext, ".flv", 4) == 0)) {
            subExt = 1;
        } else if (strncmp(ext, ".mp3", 4) == 0) {
            addMP3 = 1;
            subExt = 1;
        }
    }

    streamname = (char *) malloc((pplen + 4 + 1) * sizeof(char));
    if (!streamname)
        return;

    destptr = streamname;
    if (addMP4) {
        if (strncmp(ppstart, "mp4:", 4)) {
            strcpy(destptr, "mp4:");
            destptr += 4;
        } else {
            subExt = 0;
        }
    } else if (addMP3) {
        if (strncmp(ppstart, "mp3:", 4)) {
            strcpy(destptr, "mp3:");
            destptr += 4;
        } else {
            subExt = 0;
        }
    }

    for (p = (char *) ppstart; pplen > 0;) {
        /* skip extension */
        if (subExt && p == ext) {
            p += 4;
            pplen -= 4;
            continue;
        }
        if (*p == '%') {
            unsigned int c;
            sscanf(p + 1, "%02x", &c);
            *destptr++ = c;
            pplen -= 3;
            p += 3;
        } else {
            *destptr++ = *p++;
            pplen--;
        }
    }
    *destptr = '\0';

    out->av_val = streamname;
    out->av_len = destptr - streamname;
}
