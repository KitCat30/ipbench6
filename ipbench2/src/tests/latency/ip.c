#include "latency.h"

/* the socket */
static int s;

static int
create_addr(char *hostname, uint16_t port, struct sockaddr_in6 *addr)
{
	struct addrinfo hints, *res, *p;
	int status;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;

	char port_str[6];
	sprintf(port_str, "%d", port);

	if ((status = getaddrinfo(hostname, port_str, &hints, &res)) != 0){
	    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
	    return -1;
	}

	for (p = res; p != NULL; p = p->ai_next){
	    if((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
	        perror("socket");
		continue;
	    }

	    if(connect(s, p->ai_addr, p->ai_addrlen) == -1){
		close(s);
		perror("connect");
		continue;
	    }
	    break;
	}

	if (p == NULL){
	    fprintf(stderr, "Failed to connect\n");
	    return -1;
	}

	memcpy(addr, p->ai_addr, p->ai_addrlen);

	freeaddrinfo(res);
	return 0;
}


int tcp_setup_socket(char *hostname, int port, char *args)
{
	int c;
	struct sockaddr_in6 addr;
	if (create_addr(hostname, port, &addr) == -1) {
		dbprintf("Can not contact target!\n");
		return -1;
	}

	dbprintf("Socket type is TCP.\n");

	s = socket(AF_INET6, SOCK_STREAM, 0);
	if (s == -1) {
		dbprintf("Can not create socket (%s).\n", strerror(errno));
		return -1;
	}

	c = connect(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in6));
	if (c != 0) {
		dbprintf("Can not connect (%s).\n", strerror(errno));
		return -1;
	}
	return s;
}

int udp_setup_socket(char *hostname, int port, char *args)
{
	int c;
	struct sockaddr_in6 addr;
	if (create_addr(hostname, port, &addr) == -1) {
		dbprintf("Can not contact target!\n");
		return -1;
	}

	dbprintf("Socket type is UDP.\n");

	s = socket(AF_INET6, SOCK_DGRAM, 0);
	if (s == -1) {
		dbprintf("Can not create socket (%s).\n", strerror(errno));
		return -1;
	}

	c = connect(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in6));
	if (c != 0) {
		dbprintf("Can not connect (%s).\n", strerror(errno));
		return -1;
	}
	return s;
}


/* ip based packet sending functions */
int ip_setup_packet(char **buf, int size)
{
	*buf = malloc(size);
	assert (buf != NULL);
	return 0;
}

int ip_fill_packet(void *buf, uint64_t seq)
{
	*((uint64_t*)buf) = seq;
	return 0;
}

int ip_read_packet(void *buf, uint64_t *seq)
{
	*seq = *((uint64_t*)buf);
	return 0;
}

int ip_send_packet(void *buf, int size, int flags)
{
	return send(s, buf, size, flags);
}

int ip_recv_packet(void *buf, int size, int flags)
{
	return recv(s, buf, size, flags);
}
