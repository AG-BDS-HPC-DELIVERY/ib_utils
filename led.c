/*
 * How to build:
 * gcc -Wall -g -o led led.c -I $PREFIX/usr/include -L $PREFIX/usr/lib64 -libumad -libmad
 *
 *
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>

#include <infiniband/mad.h>
#include <infiniband/umad.h>

struct ib_mad
{
	uint8_t base_version;  /* 00 */
	uint8_t mgmt_class;    /* 01 */
	uint8_t class_version; /* 02 */
	uint8_t method;	       /* 03 */
	uint16_t status;       /* 04 */
	uint16_t resv1;	       /* 06 */
	uint64_t tid;	       /* 08 */
	uint16_t attr_id;      /* 10 */
	uint16_t resv2;	       /* 12 */
	uint32_t attr_mod;     /* 14 */
	uint64_t vkey;	       /* 18 */
	char data[224];	       /* 20 */
} __attribute__((packed));

typedef struct
{
	uint32_t led_60_63;
	uint32_t led_56_59;
	uint32_t led_52_55;
	uint32_t led_48_51;
	uint32_t led_44_47;
	uint32_t led_40_43;
	uint32_t led_36_39;
	uint32_t led_32_35;
	uint32_t led_28_31;
	uint32_t led_24_27;
	uint32_t led_20_23;
	uint32_t led_16_19;
	uint32_t led_12_15;
	uint32_t led_08_11;
	uint32_t led_04_07;
	uint32_t led_00_03;
} __attribute__((packed)) mlx_led_ctrl_t;

/* Sending:
 * 0000: 010a 0102 0000 0000 a302 947d 0000 0000
 * 0010: 0030 0000 0000 000f 0000 0000 0000 0000
 * 0020: 0000 0000 0000 0000 0000 0000 0000 0000 led[48-63]
 * 0030: 0000 0000 0000 0000 0000 0000 0000 0000 led[32-47]
 * 0040: 0000 0000 0000 0000 0000 0000 0000 0000 led[16-31]
 * 0050: 0000 0000 0000 0000 0000 0000 0000 0002 led[00-15]
 * 0060: 0000 0000 0000 0000 0000 0000 0000 0000
 * 0070: 0000 0000 0000 0000 0000 0000 0000 0000
 * 0080: 0000 0000 0000 0000 0000 0000 0000 0000
 * 0090: 0000 0000 0000 0000 0000 0000 0000 0000
 * 00a0: 0000 0000 0000 0000 0000 0000 0000 0000
 * 00b0: 0000 0000 0000 0000 0000 0000 0000 0000
 * 00c0: 0000 0000 0000 0000 0000 0000 0000 0000
 * 00d0: 0000 0000 0000 0000 0000 0000 0000 0000
 * 00e0: 0000 0000 0000 0000 0000 0000 0000 0000
 * 00f0: 0000 0000 0000 0000 0000 0000 0000 0000
 *
 * MAD status: 0x0 - OK
 * 	Invalid field code:
 * 		No invalid fields
 *
 * Received:
 * 0000: 010a 0181 0000 0000 0000 007e 0000 0000
 * 0010: 0030 0000 0000 000f 0000 0000 0000 0000
 * 0020: 0000 0000 0000 0000 0000 0000 0000 0000
 * 0030: 0000 0000 0000 0000 0000 0000 0000 0000
 * 0040: 0000 0000 0000 0000 0000 0000 0000 0000
 * 0050: 0000 0000 0000 0000 0000 0000 0000 0002
 * 0060: 0000 0000 0000 0000 0000 0000 0000 0000
 * 0070: 0000 0000 0000 0000 0000 0000 0000 0000
 * 0080: 0000 0000 0000 0000 0000 0000 0000 0000
 * 0090: 0000 0000 0000 0000 0000 0000 0000 0000
 * 00a0: 0000 0000 0000 0000 0000 0000 0000 0000
 * 00b0: 0000 0000 0000 0000 0000 0000 0000 0000
 * 00c0: 0000 0000 0000 0000 0000 0000 0000 0000
 * 00d0: 0000 0000 0000 0000 0000 0000 0000 0000
 * 00e0: 0000 0000 0000 0000 0000 0000 0000 0000
 * 00f0: 0000 0000 0000 0000 0000 0000 0000 0000
 */

#define MLX_LED_CTRL_ATTRID 0x30
#define MLX_LED_CTRL_DATA_OFFS 0x20
#define MLX_LED_CTRL_DATA_SZ sizeof(mlx_led_ctrl_t)

typedef enum
{
	HW = 0,
	ON = 1,
	OFF = 2,
	FLASH = 3
} led_mode_t;

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

static void mad_init(struct ib_mad *mad,
		     uint8_t class,
		     uint16_t attr,
		     led_mode_t mode)
{
	mlx_led_ctrl_t *led_ctrl;

	memset(mad, 0, sizeof(struct ib_mad));

	mad->base_version = 1;
	mad->mgmt_class = class;
	mad->class_version = 1;
	mad->method = IB_MAD_METHOD_SET;
	mad->tid = mad_trid();

	mad->attr_id = htons(attr);

	/* 0xf: System Status LEDs (UID, Bad Port) */
	mad->attr_mod = htonl(0xf);

	led_ctrl = (mlx_led_ctrl_t *)&mad->data;

	/* 0x0 - HW controlled LED
	 * 0x1 - LED on
	 * 0x2 - LED off
	 * 0x3 - LED flashing
	 */
	switch (mode)
	{
	case HW:
		/* Already memset to 0 */
		break;

	case ON:
		led_ctrl->led_00_03 = htonl(1);
		break;

	case OFF:
		led_ctrl->led_00_03 = htonl(2);
		break;

	case FLASH:
		led_ctrl->led_00_03 = htonl(3);
		break;

	default:
		assert(0);
		break;
	}
}

static const char *mad_status_str(const struct ib_mad *mad)
{
	uint16_t status = ntohs(mad->status);
	uint16_t err_fields;

	if (status & 0x1)
	{
		/* bit 0 - busy */
		return "EBUSY";
	}

	if (status & 0x2)
	{
		/* bit 1 - redirection required, not an error */
		return "REDIRECTION - not an error";
	}

	/* bits 2,3,4 - invalid fields */
	err_fields = status & 0x1c;

	if (err_fields == 0)
	{
		/* No invalid fields */
		return "Success";
	}
	else
	{
		return "EINVAL";
	}
}

void print_mad_status(uint16_t status)
{
	uint16_t code = (status >> 2) & 0x07;
	uint16_t vendor_code = (status >> 8);

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
	int lid;
	led_mode_t mode;
	int portid;
	int mad_agent;
	void *umad;
	struct ib_mad *mad;
	int length;
	int ibd_timeout = 1000;
	int rv;

	if (argc < 3)
	{
		printf("Usage: %s <lid> <hw|on|off|flash>\n", argv[0]);
		exit(1);
	}

	lid = atoi(argv[1]);

	if (!strcmp(argv[2], "hw"))
		mode = HW;
	else if (!strcmp(argv[2], "on"))
		mode = ON;
	else if (!strcmp(argv[2], "off"))
		mode = OFF;
	else if (!strcmp(argv[2], "flash"))
		mode = FLASH;
	else
	{
		fprintf(stderr, "Invalid LED mode %s, aborting\n", argv[2]);
		return -1;
	}

	if (umad_init() < 0)
		IBPANIC("Failed to init UMAD");

	if ((portid = umad_open_port(NULL, 0)) < 0)
		IBPANIC("Failed to open UMAD port");

	if ((mad_agent = umad_register(portid, 0xa, 1, 0, NULL)) < 0)
		IBPANIC("Couldn't register agent for SMPs");

	if ((umad = umad_alloc(1, umad_size() + IB_MAD_SIZE)) == NULL)
		IBPANIC("can't alloc MAD");

	mad = umad_get_mad(umad);

	mad_init(mad,
		 0xA,
		 MLX_LED_CTRL_ATTRID,
		 mode);

	umad_set_addr(umad, lid, 1, 0, 0);

	length = IB_MAD_SIZE;

#ifdef DEBUG
	printf("Sending:\n");
	dump_data(mad, IB_MAD_SIZE);
#endif
	if (umad_send(portid, mad_agent, umad, length, ibd_timeout, 0) < 0)
		IBPANIC("send failed");

	rv = umad_recv(portid, umad, &length, -1);

	if (rv != mad_agent)
		IBPANIC("recv error: %s", mad_status_str(mad));

	if (!(mad->method & 0x80))
		printf("Not a response MAD\n");

	print_mad_status(ntohs(mad->status));

#ifdef DEBUG
	printf("Received:\n");
	dump_data(mad, IB_MAD_SIZE);
#endif
	umad_free(umad);
	umad_unregister(portid, mad_agent);
	umad_close_port(portid);
	umad_done();

	return 0;
}
