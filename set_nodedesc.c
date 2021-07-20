/*
 * How to build:
 * gcc -Wall -g -o set_nodedesc set_nodedesc.c -I $PREFIX/usr/include/ -L $PREFIX/usr/lib64/ -libumad -libmad
 *
 *
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <infiniband/umad.h>
#include <infiniband/mad.h>

int mad_tid = 0x123;

struct ib_mad_pkt
{
	uint8_t base_version;
	uint8_t mgmt_class;
	uint8_t class_version;
	uint8_t method;
	uint16_t status;
	uint8_t hop_ptr;
	uint8_t hop_cnt;
	uint64_t tid;
	uint16_t attr_id;
	uint16_t resv;
	uint32_t attr_mod;
	uint64_t mkey;
	uint16_t dr_slid;
	uint16_t dr_dlid;
	uint8_t reserved[28];
	uint8_t data[64];
	uint8_t initial_path[64];
	uint8_t return_path[64];
};

void setup_mad_header(void *umad, int lid, int class, int attr, int mod)
{
	struct ib_mad_pkt *mad = umad_get_mad(umad);
	int qp;

	memset(mad, 0, sizeof(struct ib_mad_pkt));

	mad->base_version = 1;
	mad->mgmt_class = class;
	mad->class_version = 1;
	mad->method = IB_MAD_METHOD_SET;
	mad->attr_id = (uint16_t)htons((uint16_t)attr);
	mad->attr_mod = htonl(mod);
	mad->tid = htonll(mad_tid++);

	qp = (class == IB_SMI_CLASS || class == IB_SMI_DIRECT_CLASS) ? 0 : 1;
	umad_set_addr(umad, lid, qp, 0, 0);
}

void dump_data(void *p, int size)
{
#define HEX(x) ((x) < 10 ? '0' + (x) : 'a' + ((x)-10))
	uint8_t *cp = p;
	int i;

	for (i = 0; i < size;)
	{
		if ((i % 16) == 0)
			printf("%.4x: ", i);

		putchar(HEX(*cp >> 4));
		putchar(HEX(*cp & 0xf));

		if (++i >= size)
			break;

		putchar(HEX(cp[1] >> 4));
		putchar(HEX(cp[1] & 0xf));

		if ((++i) % 16)
			putchar(' ');
		else
			putchar('\n');
		cp += 2;
	}
	if (i % 16)
		putchar('\n');
}

char *mad_status_str(struct ib_mad_pkt *mad)
{
	switch (mad->status)
	{
	case 0:
		return "success";
	case ETIMEDOUT:
		return "timeout";
	}
	return "unknown error";
}

void print_mad_status(unsigned short status)
{
	unsigned short code = (status >> 2) & 0x07;
	unsigned short vendor_code = (status >> 8);

	printf("MAD status: 0x%x - %s\n", status, (status == 0) ? "OK" : "ERROR");

	if (status & 0x01)
		printf("\tBusy\n");
	if (status & 0x02)
		printf("\tRedirect required\n");

	printf("\tInvalid field code:\n");

	switch (code)
	{
	case 0:
		printf("\t\tNo invalid fields\n");
		break;
	case 1:
		printf("\t\tBad base/class version\n");
		break;
	case 2:
		printf("\t\tMethod not supported\n");
		break;
	case 3:
		printf("\t\tMethod/attribute combination not supported\n");
		break;
	case 4:
		printf("\t\tReserved4\n");
		break;
	case 5:
		printf("\t\tReserved5\n");
		break;
	case 6:
		printf("\t\tReserved6\n");
		break;
	case 7:
		printf("\t\tInvalid value in attribute or modifier\n");
		break;
	}

	switch (vendor_code)
	{
	case 0:
		break;
	case 1:
		printf("\t\tMAD failed\n");
		break;
	default:
		printf("\t\tVendor error: %d\n", vendor_code);
		break;
	}
}

int main(int argc, char **argv)
{
	int dlid;
	int class = IB_SMI_CLASS;
	int attr = IB_ATTR_NODE_DESC;
	int mod = 0;
	int portid;
	int mad_agent;
	void *umad;
	struct ib_mad_pkt *mad;
	int length;
	int ibd_timeout = 1000;

	if (argc < 2)
	{
		printf("Usage: %s <lid> <hostname>\n", argv[0]);
		exit(1);
	}

	dlid = atoi(argv[1]);

	if (umad_init() < 0)
		IBPANIC("Failed to init UMAD");

	if ((portid = umad_open_port(NULL, 0)) < 0)
		IBPANIC("Failed to open UMAD port");

	if ((mad_agent = umad_register(portid, class, 1, 0, NULL)) < 0)
		IBPANIC("Couldn't register agent for SMPs");

	if ((umad = umad_alloc(1, umad_size() + IB_MAD_SIZE)) == NULL)
		IBPANIC("can't alloc MAD");

	mad = umad_get_mad(umad);

	setup_mad_header(umad, dlid, class, attr, mod);

	sprintf((char *)mad->data, argv[2]);

	length = IB_MAD_SIZE;

#ifdef DEBUG
	printf("Sending:\n");
	dump_data(mad, IB_MAD_SIZE);
#endif
	if (umad_send(portid, mad_agent, umad, length, ibd_timeout, 0) < 0)
		IBPANIC("send failed");

	if (umad_recv(portid, umad, &length, -1) != mad_agent)
		IBPANIC("recv error: %s", mad_status_str(mad));

	if ((mad->method & IB_MAD_RESPONSE) == 0)
	{
		printf("\n");
		printf("Not a response MAD received from LID %d "
		       "for MAD attr 0x%04x on management class 0x%04x\n",
		       dlid, attr, class);
	}

	printf("\n");
	print_mad_status(ntohs(mad->status));
#ifdef DEBUG
	printf("\n");
	printf("Received:\n");
	dump_data(mad, IB_MAD_SIZE);
#endif
	umad_free(umad);
	umad_unregister(portid, mad_agent);
	umad_close_port(portid);
	umad_done();

	return 0;
}
