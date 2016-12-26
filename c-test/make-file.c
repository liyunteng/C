/*
 * test-config.c -- test config
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/10/08 12:23:45
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

    if (access("config.xml", F_OK)) {
	const char *p =
	    "<Config>\n\t<SDP ip=\"127.0.0.1\" port=\"30000\"/>\n\t<Cam url=\"rtsp://192.0.0.64:554/1\" bandwidth=\"4000\"/>\n</Config>\n";
	FILE *fp = NULL;
	if ((fp = fopen("config.xml", "w+")) == NULL) {
	    fprintf(stderr, "open config.xml failed");
	} else {
	    if (fputs(p, fp) == EOF) {
		fprintf(stderr, "write config.xml failed");
	    } else {
		fclose(fp);
		fprintf(stderr, "generate config.xml success.");
	    }
	}
    }

    if (access("test.xml", F_OK)) {
	const char *p = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<root>\n\
        <dev_id>beijing-SO</dev_id>\n\
        <user_acount>beijing-SO</user_acount>\n\
        <vsp_url><![CDATA[www.streamoceam.com]]></vsp_url>\n\
        <code>200</code>\n\
        <msg>success</msg>\n\
        <user>\n\
                <user_id>example</user_id>\n\
                <user_acount>example</user_acount>\n\
                <user_name>example</user_name>\n\
                <big_visit_num>4</big_visit_num>\n\
                <vsp_id></vsp_id>\n\
                <vsp_name>test</vsp_name>\n\
        </user>\n\
        <msms>\n\
                <msm>\n\
                        <msm_ip>115.182.35.112</msm_ip>\n\
                        <msm_port>2003</msm_port>\n\
                </msm>\n\
        </msms>\n\
</root>\n";
	FILE *fp = NULL;
	if ((fp = fopen("test.xml", "w+")) == NULL) {
	    fprintf(stderr, "open test.xml failed");
	} else {
	    if (fputs(p, fp) == EOF) {
		fprintf(stderr, "write test.xml failed.");
	    } else {
		fclose(fp);
		fprintf(stderr, "generate test.xml success.");
	    }
	}
    }

    return 0;
}
