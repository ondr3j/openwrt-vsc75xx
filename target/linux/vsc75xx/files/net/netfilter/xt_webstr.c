/* Kernel module to match a string into a packet.
 *
 * Copyright (C) 2000 Emmanuel Roger  <winfield@freegates.be>
 * 
 * ChangeLog
 *	19.02.2002: Gianni Tedesco <gianni@ecsc.co.uk>
 *		Fixed SMP re-entrancy problem using per-cpu data areas
 *		for the skip/shift tables.
 *	02.05.2001: Gianni Tedesco <gianni@ecsc.co.uk>
 *		Fixed kernel panic, due to overrunning boyer moore string
 *		tables. Also slightly tweaked heuristic for deciding what
 * 		search algo to use.
 * 	27.01.2001: Gianni Tedesco <gianni@ecsc.co.uk>
 * 		Implemented Boyer Moore Sublinear search algorithm
 * 		alongside the existing linear search based on memcmp().
 * 		Also a quick check to decide which method to use on a per
 * 		packet basis.
 */

/* Kernel module to match a http header string into a packet.
 *
 * Copyright (C) 2003, CyberTAN Corporation
 * All Rights Reserved.
 *
 * Description:
 *   This is kernel module for web content inspection. It was derived from 
 *   'string' match module, declared as above.
 *
 *   The module follows the Netfilter framework, called extended packet 
 *   matching modules. 
 */

/* Linux Kernel 2.6 Port ( 2.4 ipt-> 2.6 xt)
 * Copyright (C) 2008, Ralink Technology Corporation. 
 * All Rights Reserved.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/netfilter/x_tables.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <net/sock.h>
#include <linux/file.h> 
#include <linux/fs.h>
#include <net/ip.h>
#include <net/route.h>
#include <net/tcp.h>
#include <linux/workqueue.h>

#define BM_MAX_NLEN 256
#define BM_MAX_HLEN 1024

#define BLK_JAVA        0x01
#define BLK_ACTIVE      0x02
#define BLK_COOKIE      0x04
#define BLK_PROXY       0x08

extern void (*reload_blk_from_file)(void);

typedef char *(*proc_ipt_search) (char *, char *, int, int);

struct ipt_webstr_info {
    char string[BM_MAX_NLEN];
    u_int16_t invert;
    u_int16_t len;
    u_int8_t type;
};

enum xt_webstr_type
{
    IPT_WEBSTR_HOST,
    IPT_WEBSTR_URL,
    IPT_WEBSTR_CONTENT
};


struct my_work_struct{
	struct sk_buff *skb;
	char *target;
	struct work_struct works;
};


#define	isdigit(x) ((x) >= '0' && (x) <= '9')
#define	isupper(x) (((unsigned)(x) >= 'A') && ((unsigned)(x) <= 'Z'))
#define	islower(x) (((unsigned)(x) >= 'a') && ((unsigned)(x) <= 'z'))
#define	isalpha(x) (isupper(x) || islower(x))
#define	toupper(x) (isupper(x) ? (x) : (x) - 'a' + 'A')
#define tolower(x) (isupper(x) ? ((x) - 'A' + 'a') : (x))

#define split(word, wordlist, next, delim) \
    for (next = wordlist, \
	strncpy(word, next, sizeof(word)), \
	word[(next=strstr(next, delim)) ? strstr(word, delim) - word : sizeof(word) - 1] = '\0', \
	next = next ? next + sizeof(delim) - 1 : NULL ; \
	strlen(word); \
	next = next ? : "", \
	strncpy(word, next, sizeof(word)), \
	word[(next=strstr(next, delim)) ? strstr(word, delim) - word : sizeof(word) - 1] = '\0', \
	next = next ? next + sizeof(delim) - 1 : NULL)

#define BUFSIZE 	1024

/* Flags for get_http_info() */
#define HTTP_HOST	0x01
#define HTTP_URL	0x02
/* Flags for mangle_http_header() */
#define HTTP_COOKIE	0x04

#if 0
#define SPARQ_LOG       printk
#else
#define SPARQ_LOG(format, args...)
#endif

typedef struct httpinfo {
    char host[BUFSIZE + 1];
    int hostlen;
    char url[BUFSIZE + 1];
    int urllen;
} httpinfo_t;

/* Return 1 for match, 0 for accept, -1 for partial. */
static int find_pattern2(const char *data, size_t dlen,
	const char *pattern, size_t plen,
	char term,
	unsigned int *numoff,
	unsigned int *numlen)
{
    size_t i, j, k;
    int state = 0;
    *numoff = *numlen = 0;

    SPARQ_LOG("%s: pattern = '%s', dlen = %u\n",__FUNCTION__, pattern, dlen);
    if (dlen == 0)
	return 0;

    if (dlen <= plen) {	/* Short packet: try for partial? */
	if (strnicmp(data, pattern, dlen) == 0)
	    return -1;
	else 
	    return 0;
    }
    for (i = 0; i <= (dlen - plen); i++) {
	/* DFA : \r\n\r\n :: 1234 */
	if (*(data + i) == '\r') {
	    if (!(state % 2)) state++;	/* forwarding move */
	    else state = 0;		/* reset */
	}
	else if (*(data + i) == '\n') {
	    if (state % 2) state++;
	    else state = 0;
	}
	else state = 0;

	if (state >= 4)
	    break;

	/* pattern compare */
	if (memcmp(data + i, pattern, plen ) != 0)
	    continue;

	/* Here, it means patten match!! */
	*numoff=i + plen;
	for (j = *numoff, k = 0; data[j] != term; j++, k++)
	    if (j > dlen) return -1 ;	/* no terminal char */

	*numlen = k;
	return 1;
    }
    return 0;
}

#if 0
static int mangle_http_header(const struct sk_buff *skb, int flags)
{
    struct iphdr *iph = (skb)->nh.iph;
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;
    unsigned char *data = (void *)tcph + tcph->doff*4;
    unsigned int datalen = (skb)->len - (iph->ihl*4) - (tcph->doff*4);

    int found, offset, len;
    int ret = 0;


    SPARQ_LOG("%s: seq=%u\n", __FUNCTION__, ntohl(tcph->seq));

    /* Basic checking, is it HTTP packet? */
    if (datalen < 10)
	return ret;	/* Not enough length, ignore it */
    if (memcmp(data, "GET ", sizeof("GET ") - 1) != 0 &&
        memcmp(data, "POST ", sizeof("POST ") - 1) != 0 &&
        memcmp(data, "HEAD ", sizeof("HEAD ") - 1) != 0) //zg add 2006.09.28 for cdrouter3.3 item 186(cdrouter_urlfilter_15)
	return ret;	/* Pass it */

    /* COOKIE modification */
    if (flags & HTTP_COOKIE) {
	found = find_pattern2(data, datalen, "Cookie: ", 
		sizeof("Cookie: ")-1, '\r', &offset, &len);
	if (found) {
	    char c;
	    offset -= (sizeof("Cookie: ") - 1);
	    /* Swap the 2rd and 4th bit */
	    c = *(data + offset + 2) ;
	    *(data + offset + 2) = *(data + offset + 4) ;
	    *(data + offset + 4) = c ;
	    ret++;
	}
    }

    return ret;
}
#endif

static int get_http_info(const struct sk_buff *skb, int flags, httpinfo_t *info)
{
    //struct iphdr *iph = (skb)->nh.iph;
	struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;
    unsigned char *data = (void *)tcph + tcph->doff*4;
    unsigned int datalen = (skb)->len - (iph->ihl*4) - (tcph->doff*4);

    int found, offset;
    int hostlen, pathlen;
    int ret = 0;


    SPARQ_LOG("%s: seq=%u\n", __FUNCTION__, ntohl(tcph->seq));

    /* Basic checking, is it HTTP packet? */
    if (datalen < 10)
	return ret;	/* Not enough length, ignore it */
    if (memcmp(data, "GET ", sizeof("GET ") - 1) != 0 &&
        memcmp(data, "POST ", sizeof("POST ") - 1) != 0 &&
        memcmp(data, "HEAD ", sizeof("HEAD ") - 1) != 0) //zg add 2006.09.28 for cdrouter3.3 item 186(cdrouter_urlfilter_15)
	return ret;	/* Pass it */

    if (!(flags & (HTTP_HOST | HTTP_URL)))
	return ret;

    /* find the 'Host: ' value */
    found = find_pattern2(data, datalen, "Host: ", 
	    sizeof("Host: ") - 1, '\r', &offset, &hostlen);
    SPARQ_LOG("Host found=%d\n", found);

    if (!found || !hostlen)
	return ret;

    ret++;	/* Host found, increase the return value */
    hostlen = (hostlen < BUFSIZE) ? hostlen : BUFSIZE;
    strncpy(info->host, data + offset, hostlen);
    *(info->host + hostlen) = 0;		/* null-terminated */
    info->hostlen = hostlen;
    SPARQ_LOG("HOST=%s, hostlen=%d\n", info->host, info->hostlen);

    if (!(flags & HTTP_URL))
	return ret;

    /* find the 'GET ' or 'POST ' or 'HEAD ' value */
    found = find_pattern2(data, datalen, "GET ",
	    sizeof("GET ") - 1, '\r', &offset, &pathlen);
    if (!found)
	found = find_pattern2(data, datalen, "POST ",
		sizeof("POST ") - 1, '\r', &offset, &pathlen);
    /******* zg add 2006.09.28 for cdrouter3.3 item 186(cdrouter_urlfilter_15) ******/
    if (!found)
        found = find_pattern2(data, datalen, "HEAD ",
                sizeof("HEAD ") - 1, '\r', &offset, &pathlen);
    /************************* zg end 2006.09.28 ****************************/
    SPARQ_LOG("GET/POST found=%d\n", found);

    if (!found || (pathlen -= (sizeof(" HTTP/x.x") - 1)) <= 0)/* ignor this field */
	return ret;

    ret++;	/* GET/POST/HEAD found, increase the return value */
    pathlen = ((pathlen + hostlen) < BUFSIZE) ? pathlen : BUFSIZE - hostlen;
    strncpy(info->url, info->host, hostlen);
    strncpy(info->url + hostlen, data + offset, pathlen);
    *(info->url + hostlen + pathlen) = 0;	/* null-terminated */
    info->urllen = hostlen + pathlen;
    SPARQ_LOG("URL=%s, urllen=%d\n", info->url, info->urllen);

    return ret;
}

/* Linear string search based on memcmp() */
static char *search_linear (char *needle, char *haystack, int needle_len, int haystack_len) 
{
	char *k = haystack + (haystack_len-needle_len);
	char *t = haystack;
	
	SPARQ_LOG("%s: haystack=%s, needle=%s\n", __FUNCTION__, t, needle);
	for(; t <= k; t++) {
		//SPARQ_LOG("%s: haystack=%s, needle=%s\n", __FUNCTION__, t, needle);
		if (strnicmp(t, needle, needle_len) == 0) return t;
		//if ( memcmp(t, needle, needle_len) == 0 ) return t;
	}

	return NULL;
}


#define MAX_REQUEST_BUFFER_LEN 2048
#define tcp_v4_check(tcph, tcph_sz, s, d, csp) tcp_v4_check((tcph_sz), (s), (d), (csp))
extern int ip_finish_output(struct sk_buff *skb);
static char ct_block[32]="/www/url_block";
char blk_msg_fmt[MAX_REQUEST_BUFFER_LEN];


void
reset_connect(struct sk_buff *oldskb, char *host, char *page, char *category)
{
	struct sk_buff *nskb;
	struct tcphdr *otcph, *tcph;
	struct rtable *rt;
	struct flowi flow_i;
	unsigned int otcplen;
	u_int16_t tmp_port;
	u_int32_t tmp_addr;
	char *msg_info; 
	int msg_len = 0;  //the actual length of msg_info
	u_int8_t tos_value;	
	
	struct file *fp;
	mm_segment_t oldfs;
	
	msg_info = (char *) kmalloc(MAX_REQUEST_BUFFER_LEN,GFP_ATOMIC);
	if (!msg_info) 
		return;


#if 0	// block msg from file every time
	oldfs = get_fs(); 
	set_fs(KERNEL_DS);
	fp=filp_open(ct_block,O_RDONLY,0);

	if (!IS_ERR(fp)) 
	{ 
		char buf[MAX_REQUEST_BUFFER_LEN], buf_tmp[MAX_REQUEST_BUFFER_LEN]; 
		memset(buf,0,MAX_REQUEST_BUFFER_LEN);
		memset(buf_tmp,0,MAX_REQUEST_BUFFER_LEN);
		if (fp->f_op && fp->f_op->read){
			if(fp->f_op->read(fp,buf,MAX_REQUEST_BUFFER_LEN, &fp->f_pos) > 0){
				sprintf(buf_tmp, buf,
	  				NIPQUAD(ip_hdr(oldskb)->saddr),
			  		host, page,
	  				category);
					msg_len = sprintf(msg_info, "HTTP/1.1 %s\r\n\r\n%s","403 Forbidden",buf_tmp);
			}
				
		}
		filp_close(fp,NULL); 
	}
	set_fs(oldfs); 
#else
	if (strcmp(blk_msg_fmt, "")) {
		msg_len = sprintf(msg_info, blk_msg_fmt,
						NIPQUAD(ip_hdr(oldskb)->saddr),
						host, page,
						category);
	}
	else
		msg_len = 0;
#endif
	
	if(msg_len == 0){
		msg_len = sprintf(msg_info, "HTTP/1.1 %s\r\n\r\n<html><head><title>%s</title></head><body><center><h3>%s%u.%u.%u.%u%s%s%s%s</h3></center></body></html>\n\n",
			  		"403 Forbidden","403 Forbidden", "<br><br><br>The requested Web page<br>from ", NIPQUAD(ip_hdr(oldskb)->saddr),
		  			"<br>to ", host, page,
	  				"<br>has been blocked by URL Content Filter.<br><br>Please contact your system administrator for further information.<br><br><br>[Powered by DrayTek]");
	}

       /* IP header checks: fragment, too short. */
       if (ip_hdr(oldskb)->frag_off & htons(IP_OFFSET)
            || oldskb->len < (ip_hdr(oldskb)->ihl<<2) + sizeof(struct tcphdr)){
        //if(ct_info2 > 0){
			//printk(IPFLOG_ALERT "[CSM]reset_connect : return ... fragment, too short \n");
		//}
		kfree(msg_info);
		return;
       }
       otcph = (struct tcphdr *)((u_int32_t*)ip_hdr(oldskb) + ip_hdr(oldskb)->ihl);
       otcplen = oldskb->len - ip_hdr(oldskb)->ihl*4;

       /* No RST for RST. either FIN for FIN*/
       if (otcph->rst || otcph->fin){
	   	//if(ct_info2 > 0){
//			printk(IPFLOG_ALERT "[CSM]reset_connect : return ... No RST for RST \n");
	//	}
		kfree(msg_info);
		return;
		}

       /* Check checksum. */
       if (tcp_v4_check(otcph, otcplen, ip_hdr(oldskb)->saddr,
                         		ip_hdr(oldskb)->daddr,
                         		csum_partial((char *)otcph, otcplen, 0)) != 0){
        //if(ct_info2 > 0){
			//printk(IPFLOG_ALERT "[CSM]reset_connect : return ... checksum \n");
		//}
		kfree(msg_info);
		return;
       	}

	   /* Copy skb (even if skb is about to be dropped, we can't just
           clone it because there may be other things, such as tcpdump,
           interested in it) */
       nskb = skb_copy(oldskb, GFP_ATOMIC);
       if (!nskb){
	   	//if(ct_info2 > 0){
//			printk(IPFLOG_ALERT "[CSM]reset_connect : return ... Copy skb \n");
	//	}
		kfree(msg_info);
		return;
       }
        
       /* This packet will not be the same as the other: clear nf fields */
       nf_conntrack_put(nskb->nfct);
       nskb->nfct = NULL;
       //nskb->nfcache = 0;
       nskb->mark = 0;
       tcph = (struct tcphdr *)((u_int32_t*)ip_hdr(nskb) + ip_hdr(nskb)->ihl);

       /* Swap source and dest */
       tmp_addr = ip_hdr(nskb)->saddr;
       ip_hdr(nskb)->saddr = ip_hdr(nskb)->daddr;
       ip_hdr(nskb)->daddr = tmp_addr;
       tmp_port = tcph->source;
       tcph->source = tcph->dest;
       tcph->dest = tmp_port;

       /* change the total length field of ip header */
       tcph->doff = sizeof(struct tcphdr)/4;
	   if(nskb->len > (ip_hdr(nskb)->ihl*4 + sizeof(struct tcphdr) + msg_len)){	   		
	   		skb_trim(nskb, ip_hdr(nskb)->ihl*4 + sizeof(struct tcphdr) + msg_len);
	   }else{			
			if(skb_tailroom(nskb) > (ip_hdr(nskb)->ihl*4 + sizeof(struct tcphdr) + msg_len)- nskb->len)
				skb_put(nskb,(ip_hdr(nskb)->ihl*4 + sizeof(struct tcphdr) + msg_len)- nskb->len);
			else
				skb_put(nskb,skb_tailroom(nskb));
	   }
       ip_hdr(nskb)->tot_len = htons(nskb->len);   

	   /* Set flags FIN ACK*/
       ((u_int8_t *)tcph)[13] = 0;
       tcph->fin = 1;
       tcph->ack = 1; 
       tcph->seq = otcph->ack_seq ? otcph->ack_seq : 1;
       tcph->ack_seq = otcph->seq ? otcph->seq : 1;
        
       tcph->window = 0;
       tcph->urg_ptr = 0;

       /* Add alert mesg here*/
       strncpy( (char *) tcph + 20 , msg_info, msg_len );
        
        
       /* Adjust TCP checksum */
       tcph->check = 0;        
       tcph->check = tcp_v4_check(tcph, nskb->len - ip_hdr(nskb)->ihl*4,
                                   ip_hdr(nskb)->saddr,
                                   ip_hdr(nskb)->daddr,
                                   csum_partial((char *)tcph,
                                                nskb->len - ip_hdr(nskb)->ihl*4, 0));
       /* Adjust IP TTL, DF */
       ip_hdr(nskb)->ttl = MAXTTL;

	/* Set DF, id = 0 */
       ip_hdr(nskb)->frag_off = htons(IP_DF);
       ip_hdr(nskb)->id = 0;

       /* Adjust IP checksum */
       ip_hdr(nskb)->check = 0;
       ip_hdr(nskb)->check = ip_fast_csum((unsigned char *)ip_hdr(nskb), 
                                           ip_hdr(nskb)->ihl);
       /* Routing: if not headed for us, route won't like source */
/*
	if (ip_route_output(&rt, nskb->nh.iph->daddr,
                            0,
                            RT_TOS(nskb->nh.iph->tos) | RTO_CONN,
                            0) != 0)
*/
	memset(&flow_i, 0, sizeof(flow_i));
	memcpy(&(flow_i.nl_u.ip4_u.daddr), &(ip_hdr(nskb)->daddr), sizeof(flow_i.fl4_dst));
	tos_value = RT_TOS(ip_hdr(nskb)->tos) | RTO_CONN;
	memcpy(&(flow_i.nl_u.ip4_u.tos), &tos_value, sizeof(flow_i.fl4_tos));

	if(ip_route_output_key(&rt, &flow_i) !=0)
		goto free_nskb;
	
       dst_release(nskb->dst);
       nskb->dst = &rt->u.dst;
       /* "Never happens" */
/*
       if (nskb->len > nskb->dst->pmtu)
       	goto free_nskb;
*/
       ip_finish_output(nskb);

	   //if(ct_info2 > 1){
		//	printk(IPFLOG_ALERT "[CSM]reset_connect : return ... done \n");
		//}
		kfree(msg_info);
       	return;

free_nskb:
		//if(ct_info2 > 0){
//			printk(IPFLOG_ALERT "[CSM]reset_connect : return ... kfree_skb \n");
		//}
		kfree(msg_info);
       kfree_skb(nskb);
}

void read_blk_from_file(void)
{
	int msg_len = 0;  //the actual length of msg_info
	struct file *fp;
	mm_segment_t oldfs;
	memset(blk_msg_fmt,0,MAX_REQUEST_BUFFER_LEN);

	oldfs = get_fs(); 
	set_fs(KERNEL_DS);
	fp=filp_open(ct_block,O_RDONLY,0);

	if (!IS_ERR(fp)) 
	{ 
		if (fp->f_op && fp->f_op->read){
			int len = 0;
			len = sprintf(blk_msg_fmt, "HTTP/1.1 %s\r\n\r\n","403 Forbidden");
			fp->f_op->read(fp, &blk_msg_fmt[len], MAX_REQUEST_BUFFER_LEN, &fp->f_pos);
		}
		filp_close(fp,NULL); 
	}
	set_fs(oldfs); 

}

void reset_connect_work(struct work_struct *p_work)
{
	struct my_work_struct *p_test_work = container_of(p_work, struct my_work_struct, works);
	read_blk_from_file();
	reset_connect(p_test_work->skb, p_test_work->target, "", "");
	kfree_skb(p_test_work->skb);
	kfree(p_test_work->target);
	kfree(p_test_work);
}


static int
match(const struct sk_buff *skb,
      const struct net_device *in,
      const struct net_device *out,
      const struct xt_match *match,
      const void *matchinfo,
      int offset,
      unsigned int protoff,
      int *hotdrop)
{
	const struct ipt_webstr_info *info = matchinfo;
	//struct iphdr *ip = skb->nh.iph;
	struct iphdr *ip = ip_hdr(skb);
	proc_ipt_search search=search_linear;

	char token[] = "<&nbsp;>";
	char *wordlist = (char *)&info->string;
	httpinfo_t htinfo;
	int flags = 0;
	int found = 0;
	long int opt = 0;
	char target[BM_MAX_NLEN];


	if (!ip || info->len < 1)
	    return 0;

	SPARQ_LOG("\n************************************************\n"
		"%s: type=%s\n", __FUNCTION__, (info->type == IPT_WEBSTR_URL) 
		? "IPT_WEBSTR_URL"  : (info->type == IPT_WEBSTR_HOST) 
		? "IPT_WEBSTR_HOST" : "IPT_WEBSTR_CONTENT" );
	
	/* Determine the flags value for get_http_info(), and mangle packet 
	 * if needed. */
	switch(info->type)
	{
	    case IPT_WEBSTR_URL:	/* fall through */
		flags |= HTTP_URL;

	    case IPT_WEBSTR_HOST:
		flags |= HTTP_HOST;
		break;

	    case IPT_WEBSTR_CONTENT:
		opt = simple_strtol(wordlist, (char **)NULL, 10);
		SPARQ_LOG("%s: string=%s, opt=%#lx\n", __FUNCTION__, wordlist, opt);

		if (opt & (BLK_JAVA | BLK_ACTIVE | BLK_PROXY))
		    flags |= HTTP_URL;
		if (opt & BLK_PROXY)
		    flags |= HTTP_HOST;
#if 0
		// Could we modify the packet payload in a "match" module?  --YY@Ralink
		if (opt & BLK_COOKIE)
		    mangle_http_header(skb, HTTP_COOKIE);
#endif
		break;

	    default:
		printk("%s: Sorry! Cannot find this match option.\n", __FILE__);
		return 0;
	}

	/* Get the http header info */
	if (get_http_info(skb, flags, &htinfo) < 1)
	    return 0;

	/* Check if the http header content contains the forbidden keyword */
	if (info->type == IPT_WEBSTR_HOST || info->type == IPT_WEBSTR_URL) {
	    int nlen = 0, hlen = 0;
	    char needle[BUFSIZE], *haystack = NULL;
	    char *next;

	    if (info->type == IPT_WEBSTR_HOST) {
		haystack = htinfo.host;
		hlen = htinfo.hostlen;
	    }
	    else {
		haystack = htinfo.url;
		hlen = htinfo.urllen;
	    }
	    split(needle, wordlist, next, token) {
		nlen = strlen(needle);
		SPARQ_LOG("keyword=%s, nlen=%d, hlen=%d\n", needle, nlen, hlen);
		if (!nlen || !hlen || nlen > hlen) continue;
		if (search(needle, haystack, nlen, hlen) != NULL) {
		    found = 1;
		    break;
		}
	    }
		
		strncpy(target, haystack, BM_MAX_NLEN-1);
	}
	else {		/* IPT_WEBSTR_CONTENT */
	    int vicelen;

	    if (opt & BLK_JAVA) {
		vicelen = sizeof(".js") - 1;
		if (strnicmp(htinfo.url + htinfo.urllen - vicelen, ".js", vicelen) == 0) {
		    SPARQ_LOG("%s: MATCH....java\n", __FUNCTION__);
		    found = 1;
			strncpy(target, "Java", BM_MAX_NLEN-1);
		    goto match_ret;
		}
		vicelen = sizeof(".class") - 1;
		if (strnicmp(htinfo.url + htinfo.urllen - vicelen, ".class", vicelen) == 0) {
		    SPARQ_LOG("%s: MATCH....java\n", __FUNCTION__);
		    found = 1;
			strncpy(target, "Java", BM_MAX_NLEN-1);
		    goto match_ret;
		}
	    }
	    if (opt & BLK_ACTIVE){
		vicelen = sizeof(".ocx") - 1;
		if (strnicmp(htinfo.url + htinfo.urllen - vicelen, ".ocx", vicelen) == 0) {
		    SPARQ_LOG("%s: MATCH....activex\n", __FUNCTION__);
		    found = 1;
			strncpy(target, "ActiveX", BM_MAX_NLEN-1);
		    goto match_ret;
		}
		vicelen = sizeof(".cab") - 1;
		if (strnicmp(htinfo.url + htinfo.urllen - vicelen, ".cab", vicelen) == 0) {
		    SPARQ_LOG("%s: MATCH....activex\n", __FUNCTION__);
		    found = 1;
			strncpy(target, "ActiveX", BM_MAX_NLEN-1);
		    goto match_ret;
		}
	    }
	    if (opt & BLK_PROXY){
		if (strnicmp(htinfo.url + htinfo.hostlen, "http://", sizeof("http://") - 1) == 0) {
		    SPARQ_LOG("%s: MATCH....proxy\n", __FUNCTION__);
		    found = 1;
			strncpy(target, "Proxy", BM_MAX_NLEN-1);
		    goto match_ret;
		}
	    }
	}

match_ret:
	SPARQ_LOG("%s: Verdict =======> %s \n",__FUNCTION__
		, found ? "DROP" : "ACCEPT");

	if (found) {
		if (!strcmp(blk_msg_fmt, "")) {
			struct my_work_struct *blk_page_work; 
			blk_page_work = kmalloc(sizeof(struct my_work_struct),GFP_ATOMIC); 
			blk_page_work->skb = skb_copy(skb, GFP_ATOMIC);
			blk_page_work->target = (char *) kmalloc(BM_MAX_NLEN,GFP_ATOMIC);
			if (blk_page_work && blk_page_work->skb && blk_page_work->target) {
				strncpy(blk_page_work->target, target, BM_MAX_NLEN-1);
				INIT_WORK(&(blk_page_work->works), (void*)reset_connect_work);
				schedule_work(&(blk_page_work->works));
			}
			else {
				kfree_skb(blk_page_work->skb);
				kfree(blk_page_work->target);
				kfree(blk_page_work);
			}
		}
		else {
			reset_connect(skb, target, "", "");
		}
		printk(IPFLOG_ALERT "[CSM] URL filter: %s matched.", target);
	}
	return (found ^ info->invert);
}

static int
checkentry(const char *tablename,
	   const void *entry,
	   const struct xt_match *match,
           void *matchinfo,
           unsigned int hook_mask)
{
#if 0
       if (matchsize != IPT_ALIGN(sizeof(struct ipt_webstr_info)))
               return 0;
#endif
       return 1;
}

static struct xt_match xt_webstr_match[] = {
	{
	.name		= "webstr",
	.family		= AF_INET,
	.match		= match,
	.checkentry	= checkentry,
	.matchsize	= sizeof(struct ipt_webstr_info),
	.me		= THIS_MODULE
	},
	{
	.name		= "webstr",
	.family		= AF_INET6,
	.match		= match,
	.checkentry	= checkentry,
	.matchsize	= sizeof(struct ipt_webstr_info),
	.me		= THIS_MODULE
	},

};


static int __init init(void)
{
	//read_blk_from_file();
	reload_blk_from_file = read_blk_from_file;
	return xt_register_matches(xt_webstr_match, ARRAY_SIZE(xt_webstr_match));
}

static void __exit fini(void)
{
	reload_blk_from_file = NULL;
	xt_unregister_matches(xt_webstr_match, ARRAY_SIZE(xt_webstr_match));
}

module_init(init);
module_exit(fini);