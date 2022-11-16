/*
 *
 * gcc -Wall -Wno-address-of-packed-member -g -o diag diag.c -I $PREFIX/usr/include -L $PREFIX/usr/lib64 -libumad -libmad -lm
 *
 *
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>	/* CHAR_BIT */
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stddef.h>

#include <infiniband/mad.h>
#include <infiniband/umad.h>

struct ib_mad {
	uint8_t      base_version;   /* 00 */
	uint8_t      mgmt_class;     /* 01 */
	uint8_t      class_version;  /* 02 */
	uint8_t      method;         /* 03 */
	uint16_t     status;         /* 04 */
	uint16_t     resv1;          /* 06 */
	uint64_t     tid;            /* 08 */
	uint16_t     attr_id;        /* 10 */
	uint16_t     resv2;          /* 12 */
	uint32_t     attr_mod;       /* 14 */
	uint64_t     vkey;	     /* 18 */
	char         data[224];      /* 20 */
} __attribute__((packed));

typedef struct {
	uint16_t	unused;
	/* bits [8-11] only */
	uint8_t		page_backward_rev;
	/* bits [0-3] only */
	uint8_t		page_current_rev;
	uint32_t	reserved;
	/* Page 0 */
	uint32_t	rq_num_lle;
	uint32_t	sq_num_lle;
	uint32_t	rq_num_lqpoe;
	uint32_t	sq_num_lqpoe;
	uint32_t	rq_num_leeoe;
	uint32_t	sq_num_leeoe;
	uint32_t	rq_num_lpe;
	uint32_t	sq_num_lpe;
	uint32_t	rq_num_wrfe;
	uint32_t	sq_num_wrfe;
	uint32_t	sq_num_mwbe;
	uint32_t	sq_num_bre;
	uint32_t	rq_num_lae;
	uint32_t	rq_num_rire;
	uint32_t	sq_num_rire;
	uint32_t	rq_num_rae;
	uint32_t	sq_num_rae;
	uint32_t	rq_num_roe;		/* NEW */
	uint32_t	sq_num_roe;
	uint32_t	sq_num_rnr;
	uint32_t	rq_num_oos;
	uint32_t	sq_num_oos;
	uint32_t	rq_num_dup;		/* NEW */
	uint32_t	sq_num_to;		/* NEW */
	uint32_t	sq_num_tree;
	uint32_t	sq_num_rree;
	uint32_t	sq_num_rabrte;
	uint32_t	rq_num_mce;
	uint32_t	rq_num_rsync;
	uint32_t	sq_num_rsync;
	uint32_t	sq_num_ldb_drops;	/* NEW */

} __attribute__((packed)) mlx_ib_transport_counters_t;

typedef struct {
	uint16_t	unused;
	/* bits [8-11] only */
	uint8_t		page_backward_rev;
	/* bits [0-3] only */
	uint8_t		page_current_rev;
	uint32_t	reserved;
	/* Page 0 */
	uint32_t	rq_num_sig_err;
	uint32_t	sq_num_sig_err;
	uint32_t	sq_num_cnak;
	uint32_t	sq_reconnect;
	uint32_t	sq_reconnect_ack;
	uint32_t	rq_open_gb;
	uint32_t	rq_num_no_dcrs;
	uint32_t	rq_num_cnak_sent;
	uint32_t	sq_reconnect_ack_bad;
	uint32_t	rq_open_gb_cnak;
	uint32_t	rq_gb_trap_cnak;
	uint32_t	rq_not_gb_connect;
	uint32_t	rq_not_gb_reconnect;
	uint32_t	rq_curr_gb_connect;
	uint32_t	rq_curr_gb_reconnect;
	uint32_t	rq_close_non_gb_gc;
	uint32_t	rq_dcr_inhale_events;
	uint32_t	rq_state_active_gb;
	uint32_t	rq_state_avail_dcrs;
	uint32_t	rq_dcr_lifo_size;
	uint32_t	sq_cnak_drop;
	uint32_t	minimum_dcrs;
	uint32_t	maximum_dcrs;
	uint32_t	max_cnak_fifo_size;
	uint32_t	rq_num_dc_cacks;
	uint32_t	sq_num_dc_cacks;

} __attribute__((packed)) mlx_hca_ext_flow_counters_t;

typedef struct {
	uint16_t	unused;
	/* bits [8-11] only */
	uint8_t		page_backward_rev;
	/* bits [0-3] only */
	uint8_t		page_current_rev;
	uint32_t	reserved;

	/* Table 915 - PCIe Performance Counters Data Layout */
	uint32_t	life_time_counter_high;
	uint32_t	life_time_counter_low;
	uint32_t	rx_errors;
	uint32_t	tx_errors;
	uint32_t	l0_to_recovery_eieos;
	uint32_t	l0_to_recovery_ts;
	uint32_t	l0_to_recovery_framing;
	uint32_t	l0_to_recovery_retrain;
	uint32_t	crc_error_dllp;
	uint32_t	crc_error_tlp;
	uint64_t	tx_overflow_buffer_pkt;
	uint32_t	outbound_stalled_reads;
	uint32_t	outbound_stalled_writes;
	uint32_t	outbound_stalled_reads_events;
	uint32_t	outbound_stalled_writes_events;
	uint64_t	tx_overflow_buffer_marked_pkt;
	uint16_t	unused2;
	uint8_t		effective_ber_magnitude;/* 0x4a */
	uint8_t		effective_ber_coef;	/* 0x4b bits 0-3 only */
} __attribute__((packed)) mpcnt_counters_t;

typedef struct {
	uint16_t	unused;
	/* bits [8-11] only */
	uint8_t		page_backward_rev;
	/* bits [0-3] only */
	uint8_t		page_current_rev;
	uint32_t	reserved;

	uint32_t	pages_31_00;
	uint32_t	pages_63_32;
	uint32_t	pages_95_64;
	uint32_t	pages_127_96;
	uint32_t	pages_159_128;
	uint32_t	pages_191_160;
	uint32_t	pages_223_192;
	uint32_t	pages_255_224;
} __attribute__((packed)) diag_capabilities_t;

typedef struct {
	uint16_t	unused;
	/* bits [8-11] only */
	uint8_t		page_backward_rev;
	/* bits [0-3] only */
	uint8_t		page_current_rev;
	uint32_t	reserved;

	/* Table 421 - Physical Layer Statistical Counters Data Layout */
	uint32_t	time_since_last_clear_high;
	uint32_t	time_since_last_clear_low;
	uint32_t	phy_received_bits_high;
	uint32_t	phy_received_bits_low;
	uint32_t	phy_symbol_errors_high;
	uint32_t	phy_symbol_errors_low;
	uint32_t	phy_corrected_bits_high;
	uint32_t	phy_corrected_bits_low;
	uint32_t	phy_raw_errors_lane0_high;
	uint32_t	phy_raw_errors_lane0_low;
	uint32_t	phy_raw_errors_lane1_high;
	uint32_t	phy_raw_errors_lane1_low;
	uint32_t	phy_raw_errors_lane2_high;
	uint32_t	phy_raw_errors_lane2_low;
	uint32_t	phy_raw_errors_lane3_high;
	uint32_t	phy_raw_errors_lane3_low;
	uint32_t	phy_raw_errors_lane4_high;
	uint32_t	phy_raw_errors_lane4_low;
	uint32_t	phy_raw_errors_lane5_high;
	uint32_t	phy_raw_errors_lane5_low;
	uint32_t	phy_raw_errors_lane6_high;
	uint32_t	phy_raw_errors_lane6_low;
	uint32_t	phy_raw_errors_lane7_high;
	uint32_t	phy_raw_errors_lane7_low;
	uint16_t	unused1;
	uint8_t		raw_ber_magnitude;	/* 0x62 + 8 */
	uint8_t		raw_ber_coef;		/* bits 0-3 only */
	uint16_t	unused2;
	uint8_t		effective_ber_magnitude;/* 0x66 + 8 */
	uint8_t		effective_ber_coef;	/* bits 0-3 only */
	uint16_t	unused3;
	uint8_t		symbol_ber_magnitude;   /* 0x6A + 8 */
	uint8_t		symbol_fec_ber_coef;	/* bits 0-3 only */
	uint32_t	unused4[4];
	uint32_t	phy_effective_errors_high;
	uint32_t	phy_effective_errors_low;
} __attribute__((packed)) mlx_ppcnt_phy_counters_t;

/* Mellanox Adapters Programmers Reference Manual Table 754 - Module Info Page Layout */
typedef struct {
	uint16_t	unused;
	/* bits [8-11] only */
	uint8_t		page_backward_rev;
	/* bits [0-3] only */
	uint8_t		page_current_rev;
	uint32_t	reserved;

	uint8_t		cable_techno;
	uint8_t		cable_breakout;
	uint8_t		ext_ethernet_compliance_code;
	uint8_t		ethernet_compliance_code;
	uint8_t		cable_type_vendor;
	uint8_t		cable_length;
	uint8_t		cable_identifier;
	uint8_t		cable_power_class;
	uint8_t		unused1;
	uint8_t		cable_rx_amp;
	uint8_t		cable_rx_emphasis;
	uint8_t		cable_tx_equalization;
	uint8_t		unused2;
	uint8_t		cable_attenuation_12g;
	uint8_t		cable_attenuation_7g;
	uint8_t		cable_attenuation_5g;
	uint8_t		unused3;
	uint8_t		rx_tx_cdr_cap;
	uint8_t		rx_cdr_state;
	uint8_t		tx_cdr_state;
	uint8_t		vendor_name[16];
	uint8_t		vendor_pn[16];
	uint32_t	vendor_rev;
	uint32_t	unused4;
	uint8_t		vendor_sn[16];
	uint16_t	temperature;
	uint16_t	voltage;
	uint16_t	rx_power_lane0;
	uint16_t	rx_power_lane1;
	uint16_t	rx_power_lane2;
	uint16_t	rx_power_lane3;
	uint8_t		unused5[8];
	uint16_t	tx_power_lane0;
	uint16_t	tx_power_lane1;
	uint16_t	tx_power_lane2;
	uint16_t	tx_power_lane3;
	uint8_t		unused6[8];
	uint16_t	tx_bias_lane0;
	uint16_t	tx_bias_lane1;
	uint16_t	tx_bias_lane2;
	uint16_t	tx_bias_lane3;
	uint8_t		unused7[8];
	uint16_t	temperature_high_th;
	uint16_t	temperature_low_th;
	uint16_t	voltage_high_th;
	uint16_t	voltage_low_th;
	uint16_t	rx_power_high_th;
	uint16_t	rx_power_low_th;
	uint16_t	tx_power_high_th;
	uint16_t	tx_power_low_th;
	uint8_t		tx_bias_high_th;
	uint16_t	tx_bias_low_th;
	uint16_t	unused8;
	uint16_t	wavelength;
} __attribute__((packed)) pddr_module_info_t;

typedef struct {
	uint16_t	unused;
	/* bits [8-11] only */
	uint8_t		page_backward_rev;
	/* bits [0-3] only */
	uint8_t		page_current_rev;
	uint32_t	reserved;

	uint8_t		unused1;
	uint8_t		proto_negmode_active;
	uint16_t	unused2;
	uint8_t		phy_mngr_fsm_state;
	uint8_t		eth_an_fsm_state;
	uint8_t		ib_phy_fsm_state;
	uint8_t		unused3;
	uint16_t	phy_manager_link_width_enabled;
	uint16_t	phy_manager_link_proto_enabled;
	uint16_t	core_to_phy_link_width_enabled;
	uint16_t	core_to_phy_link_proto_enabled;
	uint32_t	cable_proto_cap;
	uint16_t	link_width_active;
	uint16_t	link_speed_active;
	uint16_t	unused4;
	uint8_t		unused5;
	uint8_t		loopback_mode;
	uint16_t	fec_mode_active;
	uint16_t	fec_mode_request;
	uint16_t	unused6;
	uint8_t		unused7;
	uint8_t		eth_25g_50g_100g_fec_support;
} __attribute__((packed)) pddr_operation_info_t;

/* 22.13.6.13 PLR Counters Group */
typedef struct {
	uint16_t	unused;
	/* bits [8-11] only */
	uint8_t		page_backward_rev;
	/* bits [0-3] only */
	uint8_t		page_current_rev;
	uint32_t	reserved;

	uint32_t	plr_rcv_codes_high;
	uint32_t	plr_rcv_codes_low;
	uint32_t	plr_rcv_code_err_high;
	uint32_t	plr_rcv_code_err_low;
	uint32_t	plr_rcv_uncorrectable_code_high;
	uint32_t	plr_rcv_uncorrectable_code_low;
	uint32_t	plr_xmit_codes_high;
	uint32_t	plr_xmit_codes_low;
	uint32_t	plr_xmit_retry_codes_high;
	uint32_t	plr_xmit_retry_codes_low;
	uint32_t	plr_xmit_retry_events_high;
	uint32_t	plr_xmit_retry_events_low;
	uint32_t	plr_sync_retry_events_high;
	uint32_t	plr_sync_retry_events_low;
	uint32_t	hi_retransmission_rate_high;
	uint32_t	hi_retransmission_rate_low;
	uint32_t	plr_xmit_retry_codes_within_t_sec_max_high;
	uint32_t	plr_xmit_retry_codes_within_t_sec_max_low;
} __attribute__((packed)) plr_counters_t;

/* Mellanox vendor Specific MAD PRM v1.5 / Table 196 - Internal HCA Debug -
 * Page 255 Counters.
 */
typedef struct {
	uint16_t	unused;
	/* bits [8-11] only */
	uint8_t		page_backward_rev;
	/* bits [0-3] only */
	uint8_t		page_current_rev;
	uint32_t	reserved;

	uint32_t	num_cqovf;
	uint32_t	num_eqovf;
	uint32_t	sq_num_ds_ex_pi_retry;
	uint32_t	rq_next_gb_connect;
	uint32_t	rq_next_gb_reconnect;
	uint32_t	rq_psn_close_gb;
	uint32_t	rq_psn_close_gb_cwc;
	uint32_t	rq_close_gb_gc;
	uint32_t	rq_cwc_ghost_discard_connect;
	uint32_t	rq_cwc_ghost_discard_reconnect;
	uint32_t	rq_cwc_reconnect_stale_connection;
	uint32_t	rq_cwc_reconnect_probable_current_connection;
	uint32_t	rq_cwc_uncertainty_discard;
	uint32_t	rq_num_dc_cacks_full_hash;
	uint32_t	rq_num_dc_cacks_collision;
	uint32_t	dc_hash_curr_used_lines;
	uint32_t	dc_has_max_used_entries_in_line;
} __attribute__((packed)) internal_hca_debug_info_t;

#define MLX_DIAG_COUNTERS_ATTRID	0x78
#define MLX_DIAG_COUNTERS_DATA_OFFS	0x20
#define MLX_DIAG_COUNTERS_DATA_SZ	sizeof(mlx_ib_transport_counters_t)
/* PRM: "Counters that are not supported any more by current revision shall
 * hold a constant value 0xffffffff"
 */
#define MLX_DIAG_INVALID_COUNTER_VALUE				0xffffffff

#define MLX_DIAG_COUNTERS_TRANSPORT_ERRORS_PAGE			0
#define MLX_DIAG_COUNTERS_TRANSPORT_ERRORS_CURRENT_REV		2
#define MLX_DIAG_COUNTERS_TRANSPORT_ERRORS_BACKWARD_REV		2

#define MLX_DIAG_COUNTERS_HCA_EXTENDED_FLOWS_PAGE		1
#define MLX_DIAG_COUNTERS_HCA_EXTENDED_FLOWS_CURRENT_REV	4
#define MLX_DIAG_COUNTERS_HCA_EXTENDED_FLOWS_BACKWARD_REV	2

#define MLX_DIAG_COUNTERS_MPCNT_PAGE				2
#define MLX_DIAG_COUNTERS_MPCNT_CURRENT_REV			1
#define MLX_DIAG_COUNTERS_MPCNT_BACKWARD_REV			1

/* Mellanox Vendor Specific MAD PRM v1.5 / Table 190 - DiagnosticData Types of
 * Pages.
 */
#define MLX_DIAG_COUNTERS_CAPABILITIES_PAGE			31
#define MLX_DIAG_COUNTERS_CAPABILITIES_CURRENT_REV		1
#define MLX_DIAG_COUNTERS_CAPABILITIES_BACKWARD_REV		1

#define MLX_DIAG_COUNTERS_PPCNT_PAGE				245
#define MLX_DIAG_COUNTERS_PPCNT_CURRENT_REV			1
#define MLX_DIAG_COUNTERS_PPCNT_BACKWARD_REV			1

#define MLX_DIAG_COUNTERS_PLR_PAGE				246
#define MLX_DIAG_COUNTERS_PLR_CURRENT_REV			1
#define MLX_DIAG_COUNTERS_PLR_BACKWARD_REV			1

#define MLX_DIAG_COUNTERS_PDDR_MODULE_INFO_PAGE			250
#define MLX_DIAG_COUNTERS_PDDR_MODULE_INFO_CURRENT_REV		1
#define MLX_DIAG_COUNTERS_PDDR_MODULE_INFO_BACKWARD_REV		1

#define MLX_DIAG_COUNTERS_PDDR_OPERATION_INFO_PAGE		252
#define MLX_DIAG_COUNTERS_PDDR_OPERATION_INFO_CURRENT_REV	1
#define MLX_DIAG_COUNTERS_PDDR_OPERATION_INFO_BACKWARD_REV	1

#define MLX_DIAG_COUNTERS_INTERNAL_HCA_DEBUG_INFO_PAGE		255
#define MLX_DIAG_COUNTERS_INTERNAL_HCA_DEBUG_INFO_CURRENT_REV	3
#define MLX_DIAG_COUNTERS_INTERNAL_HCA_DEBUG_INFO_BACKWARD_REV	2

enum {
	READ = 1,
	RESET = 2
};

enum {
      CAPABILITIES = 0,
      COUNTER_TYPE_TRANSPORT,
      COUNTER_TYPE_EXT_HCA_FLOW,
      COUNTER_TYPE_MPCNT,
      COUNTER_TYPE_PPCNT,
      COUNTER_TYPE_PLR,
      COUNTER_TYPE_PDDR_MODULE_INFO,
      COUNTER_TYPE_PDDR_OPERATION_INFO,
      COUNTER_TYPE_INTERNAL_HCA_DEBUG_INFO
};

void dump_data8(const void *p, int size)
{
#define HEX(x)  ((x) < 10 ? '0' + (x) : 'a' + ((x) -10))
	const uint8_t *cp = p;
	int i;

	for (i = 0; i < size;) {
		if ((i  % 16) == 0)
			printf("%.4x: ", i);

		putchar(HEX(*cp >> 4));
		putchar(HEX(*cp & 0xf));

		if (++i >= size)
			break;

		putchar(' ');

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

	putchar('\n');
}

static void mad_init(struct ib_mad	*mad,
		     uint8_t		class,
		     uint16_t		attr,
		     uint8_t		portnum,
		     int 		action,
		     int		counter_type)
{
	int				page;
	mlx_ib_transport_counters_t	*diag;

	memset(mad, 0, sizeof(struct ib_mad));

	mad->base_version = 1;
	mad->mgmt_class = class;
	mad->class_version = 1;

	if (action == READ)
		mad->method = IB_MAD_METHOD_GET;
	else
		mad->method = IB_MAD_METHOD_SET;

	mad->attr_id = (uint16_t)htons((uint16_t)attr);
	mad->tid = htonll(mad_trid());

	diag = (mlx_ib_transport_counters_t *)&mad->data;

	switch (counter_type) {

	case COUNTER_TYPE_TRANSPORT:
		page = MLX_DIAG_COUNTERS_TRANSPORT_ERRORS_PAGE;
		diag->page_backward_rev =
			MLX_DIAG_COUNTERS_TRANSPORT_ERRORS_BACKWARD_REV;
		diag->page_current_rev =
			MLX_DIAG_COUNTERS_TRANSPORT_ERRORS_CURRENT_REV;
		break;

	case COUNTER_TYPE_EXT_HCA_FLOW:
		page = MLX_DIAG_COUNTERS_HCA_EXTENDED_FLOWS_PAGE;
		diag->page_backward_rev =
			MLX_DIAG_COUNTERS_HCA_EXTENDED_FLOWS_BACKWARD_REV;
		diag->page_current_rev =
			MLX_DIAG_COUNTERS_HCA_EXTENDED_FLOWS_CURRENT_REV;
		break;

	case COUNTER_TYPE_MPCNT:
		page = MLX_DIAG_COUNTERS_MPCNT_PAGE;
		diag->page_backward_rev =
			MLX_DIAG_COUNTERS_MPCNT_BACKWARD_REV;
		diag->page_current_rev =
			MLX_DIAG_COUNTERS_MPCNT_CURRENT_REV;
		break;

	case CAPABILITIES:
		page = MLX_DIAG_COUNTERS_CAPABILITIES_PAGE;
		diag->page_backward_rev =
			MLX_DIAG_COUNTERS_CAPABILITIES_BACKWARD_REV;
		diag->page_current_rev =
			MLX_DIAG_COUNTERS_CAPABILITIES_CURRENT_REV;
		break;

	case COUNTER_TYPE_PPCNT:
		page = MLX_DIAG_COUNTERS_PPCNT_PAGE;
		diag->page_backward_rev =
			MLX_DIAG_COUNTERS_PPCNT_BACKWARD_REV;
		diag->page_current_rev =
			MLX_DIAG_COUNTERS_PPCNT_CURRENT_REV;
		break;

	case COUNTER_TYPE_PLR:
		page = MLX_DIAG_COUNTERS_PLR_PAGE;
		diag->page_backward_rev =
			MLX_DIAG_COUNTERS_PLR_BACKWARD_REV;
		diag->page_current_rev =
			MLX_DIAG_COUNTERS_PLR_CURRENT_REV;
		break;

	case COUNTER_TYPE_PDDR_MODULE_INFO:
		page = MLX_DIAG_COUNTERS_PDDR_MODULE_INFO_PAGE;
		diag->page_backward_rev =
			MLX_DIAG_COUNTERS_PDDR_MODULE_INFO_BACKWARD_REV;
		diag->page_current_rev =
			MLX_DIAG_COUNTERS_PDDR_MODULE_INFO_CURRENT_REV;
		break;

	case COUNTER_TYPE_PDDR_OPERATION_INFO:
		page = MLX_DIAG_COUNTERS_PDDR_OPERATION_INFO_PAGE;
		diag->page_backward_rev =
			MLX_DIAG_COUNTERS_PDDR_OPERATION_INFO_BACKWARD_REV;
		diag->page_current_rev =
			MLX_DIAG_COUNTERS_PDDR_OPERATION_INFO_CURRENT_REV;
		break;

	case COUNTER_TYPE_INTERNAL_HCA_DEBUG_INFO:
		page = MLX_DIAG_COUNTERS_INTERNAL_HCA_DEBUG_INFO_PAGE;
		diag->page_backward_rev =
			MLX_DIAG_COUNTERS_INTERNAL_HCA_DEBUG_INFO_BACKWARD_REV;
		diag->page_current_rev =
			MLX_DIAG_COUNTERS_INTERNAL_HCA_DEBUG_INFO_CURRENT_REV;
		break;

	default:
		assert(0);
		break;
	}

	/* Attribute modifier bits (Mellanox PRM):
	 *
	 * bits [7:0] 	port number
	 * bits [23:16]	page code
	 * bit 31	ClearAll
	 */
	mad->attr_mod = (portnum & 0xff) | ((page & 0xff) << 16);

#if 0
	if (action == RESET)
		mad->attr_mod |= (1 << 31);
#endif

	mad->attr_mod = htonl(mad->attr_mod);
}

static const char *mad_status_str(const struct ib_mad *mad)
{
	uint16_t status = ntohs(mad->status);
	uint16_t err_fields;

	if (status & 0x1)
		/* bit 0 - busy */
		return "EBUSY";

	if (status & 0x2)
		/* bit 1 - redirection required, not an error */
		return "REDIRECTION - not an error";

	/* bits 2,3,4 - invalid fields */
	err_fields = status & 0x1c;

	if (err_fields == 0)
		/* No invalid fields */
		return "Success";
	else
		return "EINVAL";
}

static void dump_transport_counters(const mlx_ib_transport_counters_t *stats)
{
	static const char *counter_names[] = {
		"rq_num_lle",
		"sq_num_lle",
		"rq_num_lqpoe",
		"sq_num_lqpoe",
		"rq_num_leeoe",
		"sq_num_leeoe",
		"rq_num_lpe",
		"sq_num_lpe",
		"rq_num_wrfe",
		"sq_num_wrfe",
		"sq_num_mwbe",
		"sq_num_bre",
		"rq_num_lae",
		"rq_num_rire",
		"sq_num_rire",
		"rq_num_rae",
		"sq_num_rae",
		"rq_num_roe",
		"sq_num_roe",
		"sq_num_rnr",
		"rq_num_oos",
		"sq_num_oos",
		"rq_num_dup",
		"sq_num_to",
		"sq_num_tree",
		"sq_num_rree",
		"sq_num_rabrte",
		"rq_num_mce",
		"rq_num_rsync",
		"sq_num_rsync",
		"sq_num_ldb_drops"
	};

	/* Counters in host endian format */
	mlx_ib_transport_counters_t host_stats = { 0 };

	const uint32_t *counter = &stats->rq_num_lle;
	uint32_t *host_counter = &host_stats.rq_num_lle;

	int i;

	for (i = 0; i < sizeof(counter_names) / sizeof(counter_names[0]); i++) {

		if (*counter == 0xffffffff)
			/* Unsupported */
			*host_counter = 0;
		else
			*host_counter = ntohl(*counter);

		printf("%-16s: %" PRIuLEAST32 "\n", counter_names[i], *host_counter);
		counter++;
		host_counter++;
	}
}

static void dump_hca_ext_flow_counters(const mlx_hca_ext_flow_counters_t *stats)
{
	static const char *counter_names[] = {
		"rq_num_sig_err",
		"sq_num_sig_err",
		"sq_num_cnak",
		"sq_reconnect",
		"sq_reconnect_ack",
		"rq_open_gb",
		"rq_num_no_dcrs",
		"rq_num_cnak_sent",
		"sq_reconnect_ack_bad",
		"rq_open_gb_cnak",
		"rq_gb_trap_cnak",
		"rq_not_gb_connect",
		"rq_not_gb_reconnect",
		"rq_curr_gb_connect",
		"rq_curr_gb_reconnect",
		"rq_close_non_gb_gc",
		"rq_dcr_inhale_events",
		"rq_state_active_gb",
		"rq_state_avail_dcrs",
		"rq_dcr_lifo_size",
		"sq_cnak_drop",
		"minimum_dcrs",
		"maximum_dcrs",
		"max_cnak_fifo_size",
		"rq_num_dc_cacks",
		"sq_num_dc_cacks"
	};

	/* Counters in host endian format */
	mlx_hca_ext_flow_counters_t host_stats = { 0 };

	const uint32_t *counter = &stats->rq_num_sig_err;
	uint32_t *host_counter = &host_stats.rq_num_sig_err;

	int i;

	for (i = 0; i < sizeof(counter_names) / sizeof(counter_names[0]); i++) {

		if (*counter == 0xffffffff)
			/* Unsupported */
			*host_counter = 0;
		else
			*host_counter = ntohl(*counter);

		printf("%-24s: %" PRIuLEAST32 "\n", counter_names[i], *host_counter);
		counter++;
		host_counter++;
	}
}

static void dump_mpcnt_counters(const mpcnt_counters_t *mpcnt)
{
#ifdef DEBUG
	int page_offset = offsetof(mpcnt_counters_t, life_time_counter_high);

	printf("MPCNT page %d data:\n", MLX_DIAG_COUNTERS_MPCNT_PAGE);
	dump_data8((char *)mpcnt + page_offset, sizeof(mpcnt_counters_t) - page_offset);
#endif
	printf("%-40s: %u (0x%08x)\n", "life_time_counter_high",
	       ntohl(mpcnt->life_time_counter_high), ntohl(mpcnt->life_time_counter_high));
	printf("%-40s: %u (0x%08x)\n", "life_time_counter_low",
	       ntohl(mpcnt->life_time_counter_low), ntohl(mpcnt->life_time_counter_low));
	printf("%-40s: %u\n", "RX errors", ntohl(mpcnt->rx_errors));
	printf("%-40s: %u\n", "TX errors", ntohl(mpcnt->tx_errors));
	printf("%-40s: %u\n", "l0_to_recovery_eieos", ntohl(mpcnt->l0_to_recovery_eieos));
	printf("%-40s: %u\n", "l0_to_recovery_ts", ntohl(mpcnt->l0_to_recovery_ts));
	printf("%-40s: %u\n", "l0_to_recovery_framing", ntohl(mpcnt->l0_to_recovery_framing));
	printf("%-40s: %u\n", "l0_to_recovery_retrain", ntohl(mpcnt->l0_to_recovery_retrain));
	printf("%-40s: %u\n", "crc_error_dllp", ntohl(mpcnt->crc_error_dllp));
	printf("%-40s: %u\n", "crc_error_tlp", ntohl(mpcnt->crc_error_tlp));
	printf("%-40s: %lu\n", "TX overflow packets", ntohll(mpcnt->tx_overflow_buffer_pkt));
	printf("%-40s: %u\n", "Outbound stalled reads", ntohl(mpcnt->outbound_stalled_reads));
	printf("%-40s: %u\n", "Outbound stalled writes", ntohl(mpcnt->outbound_stalled_writes));
	printf("%-40s: %u\n", "Outbound stalled read events", ntohl(mpcnt->outbound_stalled_reads_events));
	printf("%-40s: %u\n", "Outbound stalled write events", ntohl(mpcnt->outbound_stalled_writes_events));
	printf("%-40s: %lu\n", "TX overflow buffer marked packets", ntohll(mpcnt->tx_overflow_buffer_marked_pkt));

	printf("%-40s: %.2e\n", "phy_effective_ber",
	       (double)mpcnt->effective_ber_coef *
	       pow((double)10.0,
		   (double)-1.0 * (double)mpcnt->effective_ber_magnitude));
}

static void dump_ppcnt_phy_counters(const mlx_ppcnt_phy_counters_t *ppcnt)
{
	uint64_t		time_since_last_clear;
	uint64_t		phy_received_bits;
	uint64_t		phy_symbol_errors;
	uint64_t		phy_corrected_bits;
	uint64_t		phy_effective_errors;

	const uint32_t		*phy_raw_errors_lane_high;
	const uint32_t		*phy_raw_errors_lane_low;
	uint64_t		phy_raw_errors_lane;
	unsigned int		lane;

	time_since_last_clear =
		(((uint64_t)ntohl(ppcnt->time_since_last_clear_high)) << 32) |
		(uint64_t)ntohl(ppcnt->time_since_last_clear_low);

	/* Convert to ms */
	printf("Time since last clear: %lu seconds\n", time_since_last_clear / 1000);

	phy_received_bits =
		(((uint64_t)ntohl(ppcnt->phy_received_bits_high)) << 32) |
		(uint64_t)ntohl(ppcnt->phy_received_bits_low);
	printf("Received bits        : %lu\n", phy_received_bits);

	phy_symbol_errors =
		(((uint64_t)ntohl(ppcnt->phy_symbol_errors_high)) << 32) |
		(uint64_t)ntohl(ppcnt->phy_symbol_errors_low);

	printf("Symbol errors        : %lu\n", phy_symbol_errors);

	phy_corrected_bits =
		(((uint64_t)ntohl(ppcnt->phy_corrected_bits_high)) << 32) |
		(uint64_t)ntohl(ppcnt->phy_corrected_bits_low);

	printf("Corrected bits       : %lu\n", phy_corrected_bits);

	for (phy_raw_errors_lane_high = &ppcnt->phy_raw_errors_lane0_high,
		     phy_raw_errors_lane_low = &ppcnt->phy_raw_errors_lane0_low,
		     lane = 0;
	     lane < 8;
	     phy_raw_errors_lane_high += 2, phy_raw_errors_lane_low += 2,
		     lane++) {

		phy_raw_errors_lane =
			(((uint64_t)ntohl(*phy_raw_errors_lane_high)) << 32) |
			(uint64_t)ntohl(*phy_raw_errors_lane_low);
		printf("raw errors (lane %u)  : %lu\n", lane, phy_raw_errors_lane);
	}

	printf("Raw BER              : %.2e\n",
	       (double)ppcnt->raw_ber_coef *
	       pow(10.0, (-1.0) * (double)ppcnt->raw_ber_magnitude));

	printf("Effective BER        : %.2e\n",
	       (double)ppcnt->effective_ber_coef *
	       pow(10.0, (-1.0) * (double)ppcnt->effective_ber_magnitude));

	printf("FEC BER              : %.2e\n",
	       (double)ppcnt->symbol_fec_ber_coef *
	       pow(10.0, (-1.0) * (double)ppcnt->symbol_ber_magnitude));

	phy_effective_errors =
		(((uint64_t)ntohl(ppcnt->phy_effective_errors_high)) << 32) |
		(uint64_t)ntohl(ppcnt->phy_effective_errors_low);

	printf("Phy effective errors : %lu\n", phy_effective_errors);
}

static void dump_plr_counters(const plr_counters_t *stats)
{
	/* Counters in host endian format */
	struct {
		uint64_t	plr_rcv_codes;
		uint64_t	plr_rcv_code_err;
		uint64_t	plr_rcv_uncorrectable_code;
		uint64_t	plr_xmit_codes;
		uint64_t	plr_xmit_retry_codes;
		uint64_t	plr_xmit_retry_events;
		uint64_t	plr_sync_retry_events;
		uint64_t	hi_retransmission_rate;
		uint64_t	plr_xmit_retry_codes_within_t_sec_max;
	} host_stats = { 0 };

	static const char *counter_names[] = {
		"plr_rcv_codes",
		"plr_rcv_code_err",
		"plr_rcv_uncorrectable_code",
		"plr_xmit_codes",
		"plr_xmit_retry_codes",
		"plr_xmit_retry_events",
		"plr_sync_retry_events",
		"hi_retransmission_rate",
		"plr_xmit_retry_codes_within_t_sec_max",
	};

	uint64_t	*host_counter;
	const uint32_t	*high;
	const uint32_t	*low;
	int i;

	for (i = 0, host_counter = &host_stats.plr_rcv_codes,
		     high = &stats->plr_rcv_codes_high, low = &stats->plr_rcv_codes_low;
	     i < sizeof(counter_names) / sizeof(counter_names[0]);
	     i++, low += 2, high += 2, host_counter++) {

		*host_counter = (((uint64_t)ntohl(*high)) << 32) | (uint64_t)ntohl(*low);

		if (*host_counter == 0xffffffffffffffff)
			/* Unsupported */
			*host_counter = 0;

		printf("%-24s: %" PRIuLEAST64 "\n", counter_names[i], *host_counter);
	}

	if (host_stats.plr_rcv_codes) {
		printf("\n%-36s: %.2e\n", "raw recv error rate",
		       (double)host_stats.plr_rcv_code_err /
		       (double)host_stats.plr_rcv_codes);

		printf("%-36s: %.2e\n", "uncorrectable recv error rate",
		       (double)host_stats.plr_rcv_uncorrectable_code /
		       (double)host_stats.plr_rcv_codes);
	}

	if (host_stats.plr_xmit_codes)
		printf("%-36s: %.2e\n", "xmit retry rate",
		       (double)host_stats.plr_xmit_retry_codes /
		       (double)host_stats.plr_xmit_codes);

	if (host_stats.plr_rcv_code_err)
		printf("\n%-36s: %.2f%%\n", "PLR efficiency (percentage of "
		       "invalid packets successfully corrected)",
		       (double)(host_stats.plr_rcv_code_err -
				host_stats.plr_rcv_uncorrectable_code) /
		       (double)(host_stats.plr_rcv_code_err) * 100.0);
}

static void dump_pddr_module_info(const pddr_module_info_t *pddr)
{
	const char	*str;
	uint8_t		val;

#ifdef DEBUG
	int		page_offset = offsetof(pddr_module_info_t, cable_techno);

	printf("Module Info Page %d data:\n", MLX_DIAG_COUNTERS_PDDR_MODULE_INFO_PAGE);
	dump_data8((char *)pddr + page_offset, sizeof(pddr_module_info_t) - page_offset);
#endif
	/* SFF 8636 / TABLE 6-20 TRANSMITTER TECHNOLOGY (PAGE 00H BYTE 147 BITS 7-4)*/
	static const char *cable_techno[] = {
		 [0]	= "850 nm VCSEL",
		 [1]	= "1310 nm VCSEL",
		 [2]	= "1550 nm VCSEL",
		 [3]	= "1310 nm FP",
		 [4]	= "1310 nm DFB",
		 [5]	= "1550 nm DFB",
		 [6]	= "1310 nm EML",
		 [7]	= "1550 nm EML",
		 [8]	= "Others",
		 [9]	= "1490 nm DFB",
		 [10]	= "Copper cable unequalized",
		 [11]	= "Copper cable passive equalized",
		 [12]	= "Copper cable, near and far end limiting active equalizers",
		 [13]	= "Copper cable, far end limiting active equalizers",
		 [14]	= "Copper cable, near end limiting active equalizers",
		 [15]	= "Copper cable, linear active equalizers"
	};

	printf("Cable technology:                                %s (0x%x)\n",
	       cable_techno[pddr->cable_techno >> 4],
	       pddr->cable_techno);

	printf("Cable breakout:                                  %u (0x%x)\n",
	       pddr->cable_breakout, pddr->cable_breakout);

	printf("Extended Ethernet specification compliance code: %u (0x%x)\n",
	       pddr->ext_ethernet_compliance_code,
	       pddr->ext_ethernet_compliance_code);

	printf("Ethernet specification compliance code:          %u (0x%x)\n",
	       pddr->ethernet_compliance_code,
	       pddr->ethernet_compliance_code);

	val = (pddr->cable_type_vendor & 0xf0) >> 4;

	switch (val) {

	case 1:
		str = "Active cable (active copper / optics)";
		break;
	case 2:
		str = "Optical Module (separated)";
		break;
	case 3:
		str = "Passive copper cable";
		break;
	case 4:
		str = "Cable unplugged";
		break;
	case 0:
	default:
		str = "Unidentified";
		break;
	}

	printf("Cable type:                                      %s (%d)\n",
	       str, val);

	val = pddr->cable_type_vendor & 0xf;

	switch (val) {
	case 1:
		str = "Mellanox";
		break;
	case 2:
		str = "Known OUI";
		break;
	case 0:
	default:
		str = "Other";
		break;
	}

	printf("Cable vendor:                                    %s (%d)\n",
	       str, val);
	printf("Cable length:                                    %dm\n",
	       pddr->cable_length);

	switch (pddr->cable_identifier) {

	case 0:
		str = "QSFP28";
		break;
	case 1:
		str = "QSFP+";
		break;
	case 2:
		str = "SFP28/SFP+";
		break;
	case 3:
		str = "QSA (QSFP->SFP+)";
		break;
	case 4:
		str = "Backplane";
		break;
	default:
		str = "Unknown";
		break;
	}

	printf("Cable identifier:                                %s (%d)\n",
	       str, pddr->cable_identifier);

	switch (pddr->cable_power_class) {

	case 0:
		str = "Power Class 0 (1.0 W max)";
		break;
	case 1:
		str = "Power Class 1 (1.5 W max)";
		break;
	case 2:
		str = "Power Class 2 (2 W max)";
		break;
	case 3:
		str = "Power Class 3 (2.5 W max)";
		break;
	case 4:
		str = "Power Class 4 (3.5 W max)";
		break;
	case 5:
		str = "Power Class 5 (4.0 W max)";
		break;
	case 6:
		str = "Power Class 6 (4.5 W max)";
		break;
	case 7:
		str = "Power Class 7 (5.0 W max)";
		break;
	default:
		str = "Unknown power class";
		break;
	}

	printf("Power class:                                     %s (%d)\n",
	       str, pddr->cable_power_class);

	printf("Rx amplitude:                                    %d\n",
	       pddr->cable_rx_amp);

	printf("Rx emphasis:                                     %d\n",
	       pddr->cable_rx_emphasis);

	printf("Tx equalization:                                 %d\n",
	       pddr->cable_tx_equalization);

	printf("Cable attenuation 12g:                           %d\n",
	       pddr->cable_attenuation_12g);

	printf("Cable attenuation 7g:                            %d\n",
	       pddr->cable_attenuation_7g);

	printf("Cable attenuation 5g:                            %d\n",
	       pddr->cable_attenuation_5g);

	printf("Rx CDR cap:                                      %d\n",
	       pddr->rx_tx_cdr_cap >> 4);

	printf("Tx CDR cap:                                      %d\n",
	       pddr->rx_tx_cdr_cap & 0xf);

	printf("Rx CDR state:                          	         %d\n",
	       pddr->rx_cdr_state & 0xf);

	printf("Tx CDR state:                          	         %d\n",
	       pddr->tx_cdr_state & 0xf);

	/* Not NULL terminated, will include vendor_pn  */
	printf("Vendor name:                                     %s\n",
	       pddr->vendor_name);

	printf("Vendor part number:                              %s\n",
	       pddr->vendor_pn);

	/* Hack: Extract letter + digit (e.g. A4) from 32b integer */
	printf("Vendor revision:                                 %c%c\n",
	       (ntohl(pddr->vendor_rev) >> 8) & 0xff,
	       ntohl(pddr->vendor_rev) & 0xff);

	printf("Vendor serial number:                            %s\n",
	       pddr->vendor_sn);

	printf("Temperature:                                     %.2f C\n",
	       (float)ntohs(pddr->temperature) / 256.0);

	printf("Voltage:                                         %.2f V\n",
	       (float)ntohs(pddr->voltage) * 0.0001);

	printf("Rx power lane 0:                                 %d dBm\n",
	       ntohs(pddr->rx_power_lane0));

	printf("Rx power lane 1:                                 %d dBm\n",
	       ntohs(pddr->rx_power_lane1));

	printf("Rx power lane 2:                                 %d dBm\n",
	       ntohs(pddr->rx_power_lane2));

	printf("Rx power lane 3:                                 %d dBm\n",
	       ntohs(pddr->rx_power_lane3));

	printf("Tx power lane 0:                                 %d dBm\n",
	       ntohs(pddr->tx_power_lane0));

	printf("Tx power lane 1:                                 %d dBm\n",
	       ntohs(pddr->tx_power_lane1));

	printf("Tx power lane 2:                                 %d dBm\n",
	       ntohs(pddr->tx_power_lane2));

	printf("Tx power lane 3:                                 %d dBm\n",
	       ntohs(pddr->tx_power_lane3));

	printf("Tx bias lane 0:                                  %d uA\n",
	       ntohs(pddr->tx_bias_lane0) * 2);

	printf("Tx bias lane 1:                                  %d uA\n",
	       ntohs(pddr->tx_bias_lane1) * 2);

	printf("Tx bias lane 2:                                  %d uA\n",
	       ntohs(pddr->tx_bias_lane2) * 2);

	printf("Tx bias lane 3:                                  %d uA\n",
	       ntohs(pddr->tx_bias_lane3) * 2);

	printf("Temperature high threshold:                      %.2f C\n",
	       (float)ntohs(pddr->temperature_high_th) / 256.0);

	printf("Temperature low threshold:                       %.2f C\n",
	       (float)ntohs(pddr->temperature_low_th) / 256.0);

	printf("Voltage high threshold:                          %.2f V\n",
	       (float)ntohs(pddr->voltage_high_th) * 0.0001);

	printf("Voltage low threshold:                           %.2f V\n",
	       (float)ntohs(pddr->voltage_low_th) * 0.0001);

	printf("Rx power high threshold:                         %d dBm\n",
	       ntohs(pddr->rx_power_high_th));

	printf("Rx power low threshold:                          %d dBm\n",
	       ntohs(pddr->rx_power_low_th));

	printf("Tx power high threshold:                         %d dBm\n",
	       ntohs(pddr->tx_power_high_th));

	printf("Tx power low threshold:                          %d dBm\n",
	       ntohs(pddr->tx_power_low_th));

	printf("Tx bias high threshold:                          %d uA\n",
	       ntohs(pddr->tx_bias_high_th) * 2);

	printf("Tx bias low threshold:                           %d uA\n",
	       ntohs(pddr->tx_bias_low_th) * 2);

	printf("Wavelength:                                      %d nm\n",
	       ntohs(pddr->wavelength));
}

static void dump_pddr_operation_info(const pddr_operation_info_t *pddr)
{
	const char	*str;
	uint8_t		val;

#ifdef DEBUG
	int		page_offset = offsetof(pddr_operation_info_t, unused1);

	printf("Module Info Page %d data:\n", MLX_DIAG_COUNTERS_PDDR_OPERATION_INFO_PAGE);
	dump_data8((char *)pddr + page_offset, sizeof(pddr_operation_info_t) - page_offset);
#endif

	printf("Prototype: %s\n",
	       (pddr->proto_negmode_active >> 4) & 0x1 ? "Infiniband" : "Ethernet");

	switch (pddr->phy_mngr_fsm_state) {
	case 0:
		str = "Disabled";
		break;
	case 1:
		str = "Open port";
		break;
	case 2:
		str = "Polling";
		break;
	case 3:
		str = "Active / Linkup";
		break;
	case 4:
		str = "Close port";
		break;
	default:
		str = "Invalid";
		break;
	}

	printf("%-50s: %s\n", "FW Phy Manager FSM state:", str);

	switch (pddr->eth_an_fsm_state) {
	case 0:
		str = "ETH_AN_FSM_ENABLE";
		break;
	case 1:
		str = "ETH_AN_FSM_XMIT_DISABLE";
		break;
	case 2:
		str = "ETH_AN_FSM_ABILITY_DETECT";
		break;
	case 3:
		str = "ETH_AN_FSM_ACK_DETECT";
		break;
	case 4:
		str = "ETH_AN_FSM_COMPLETE_ACK";
		break;
	case 5:
		str = "ETH_AN_FSM_AN_GOOD_CHECK";
		break;
	case 6:
		str = "ETH_AN_FSM_AN_GOOD";
		break;
	case 7:
		str = "ETH_AN_FSM_NEXT_PAGE_WAIT";
		break;
	default:
		str = "Invalid";
		break;
	}

	printf("%-50s: %s\n", "Ethernet (CL73) Auto-negotiation FSM state", str);

	switch (pddr->ib_phy_fsm_state) {
	case 0:
		str = "IB_AN_FSM_DISABLED";
		break;
	case 1:
		str = "IB_AN_FSM_INITIALY";
		break;
	case 2:
		str = "IB_AN_FSM_RCVR_CFG";
		break;
	case 3:
		str = "IB_AN_FSM_CFG_TEST";
		break;
	case 4:
		str = "IB_AN_FSM_WAIT_RMT_TEST";
		break;
	case 5:
		str = "IB_AN_FSM_WAIT_CFG_ENHANCED";
		break;
	case 6:
		str = "IB_AN_FSM_CFG_IDLE";
		break;
	case 7:
		str = "IB_AN_FSM_LINK_UP";
		break;
	default:
		str = "Invalid";
		break;
	}

	printf("%-50s: %s\n", "FW IB state machine", str);

	switch (ntohs(pddr->phy_manager_link_width_enabled)) {
	case 1 << 0:
		val = 1;
		break;
	case 1 << 1:
		val = 2;
		break;
	case 1 << 2:
		val = 4;
		break;
	default:
		val = -1;
		break;
	}

	printf("%-50s: %uX (0x%x)\n", "Phy manager link width enabled",
	       val, ntohs(pddr->phy_manager_link_width_enabled));

	switch (ntohs(pddr->phy_manager_link_proto_enabled)) {
	case 1 << 0:
		str = "SDR";
		break;
	case 1 << 1:
		str = "DDR";
		break;
	case 1 << 2:
		str = "QDR";
		break;
	case 1 << 3:
		str = "FDR10";
		break;
	case 1 << 4:
		str = "FDR";
		break;
	case 1 << 5:
		str = "EDR";
		break;
	default:
		str = "Invalid";
		break;
	}

	printf("%-50s: %s (0x%x)\n", "Phy manager link proto enabled",
	       str, ntohs(pddr->phy_manager_link_proto_enabled));

	switch (ntohs(pddr->core_to_phy_link_width_enabled)) {
	case 1 << 0:
		val = 1;
		break;
	case 1 << 1:
		val = 2;
		break;
	case 1 << 2:
		val = 4;
		break;
	default:
		val = -1;
		break;
	}

	printf("%-50s: %uX (0x%x)\n", "Core to Phy link width enabled",
	       val, ntohs(pddr->core_to_phy_link_width_enabled));

	switch (ntohs(pddr->core_to_phy_link_proto_enabled)) {
	case 1 << 0:
		str = "SDR";
		break;
	case 1 << 1:
		str = "DDR";
		break;
	case 1 << 2:
		str = "QDR";
		break;
	case 1 << 3:
		str = "FDR10";
		break;
	case 1 << 4:
		str = "FDR";
		break;
	case 1 << 5:
		str = "EDR";
		break;
	default:
		str = "Invalid";
		break;
	}

	printf("%-50s: %s (0x%x)\n", "Core to Phy link proto enabled",
	       str, ntohs(pddr->core_to_phy_link_proto_enabled));

	switch (ntohs(pddr->fec_mode_active)) {
	case 0:
		str = "No FEC";
		break;
	case 1:
		str = "Firecode FEC";
		break;
	case 2:
		str = "Standard RS-FEC - RS(528,514)";
		break;
	case 3:
		str = "Standard LL RS-FEC - RS(271,257)";
		break;
#if 0
	case 1 << 7:
#else /* PRM says Bit 7 but it looks like value 7 in real life */
	case 7:
#endif
		str = "Standard RS-FEC (544,514)";
		break;
	default:
		str = "Invalid";
		break;
	}

	printf("%-50s: %s (0x%x)\n", "Fec mode active",
	       str, ntohs(pddr->fec_mode_active));
}

static void dump_capabilities(const diag_capabilities_t *caps)
{
	uint32_t	val;
	const char	*str;
	const uint32_t	*word;
	int		start_bit;
	int		bit;

#ifdef DEBUG
	int		page_offset = offsetof(diag_capabilities_t, pages_31_00);

	printf("Capabilities Page %d data:\n", MLX_DIAG_COUNTERS_CAPABILITIES_PAGE);
	dump_data8((char *)caps + page_offset, sizeof(diag_capabilities_t) - page_offset);
#endif

	printf("Capabilities:\n");

	for (start_bit = 0, word = &caps->pages_31_00;
	     word <= &caps->pages_255_224;
	     start_bit +=32, word++) {

		val = ntohl(*word);

		for (bit = 0; bit < 32; bit++) {

			if (!(val & (1 << bit)))
				continue;

			switch (start_bit + bit) {

			case 0:
				str = "Transport errors and flows";
				break;
			case 1:
				str = "HCA extended flows";
				break;
			case 2:
				str = "PCIe Performance Counters";
				break;
			case 31:
				str = "Page Identification";
				break;
			case 244:
				str = "ZL FEC Counters";
				break;
			case 245:
				str = "Physical Layer Statistics";
				break;
			case 246:
				str = "PLR Counters";
				break;
			case 247:
				str = "RS-FEC Histogram";
				break;
			case 248:
				str = "Link down info";
				break;
			case 249:
				str = "Physical Layer Events";
				break;
			case 250:
				str = "Module Info";
				break;
			case 251:
				str = "Physical Layer Debug Info";
				break;
			case 252:
				str = "Physical Layer Operational Info";
				break;
			case 253:
				str = "Troubleshooting Info";
				break;
			case 254:
				str = "Physical Layer Counters";
				break;
			case 255:
				str = "Internal HCA Diagnostics";
				break;
			default:
				str = NULL;
				break;
			}

			if (str)
				printf("Page %u: %s\n", start_bit + bit, str);
		}
	}
}

static void dump_hca_debug_info(const internal_hca_debug_info_t *stats)
{
	static const char *counter_names[] = {
		"num_cqovf",
		"num_eqovf",
		"sq_num_ds_ex_pi_retry",
		"rq_next_gb_connect",
		"rq_next_gb_reconnect",
		"rq_psn_close_gb",
		"rq_psn_close_gb_cwc",
		"rq_close_gb_gc",
		"rq_cwc_ghost_discard_connect",
		"rq_cwc_ghost_discard_reconnect",
		"rq_cwc_reconnect_stale_connection",
		"rq_cwc_reconnect_probable_current_connection",
		"rq_cwc_uncertainty_discard",
		"rq_num_dc_cacks_full_hash",
		"rq_num_dc_cacks_collision",
		"dc_hash_curr_used_lines",
		"dc_has_max_used_entries_in_line"
	};

	/* Counters in host endian format */
	internal_hca_debug_info_t host_stats = { 0 };

	const uint32_t *counter = &stats->num_cqovf;
	uint32_t *host_counter = &host_stats.num_cqovf;

	int i;

#ifdef DEBUG
	int page_offset = offsetof(internal_hca_debug_info_t, num_cqovf);

	printf("MPCNT page %d data:\n", MLX_DIAG_COUNTERS_INTERNAL_HCA_DEBUG_INFO_PAGE);
	dump_data8((char *)stats + page_offset, sizeof(internal_hca_debug_info_t) - page_offset);
#endif

	for (i = 0; i < sizeof(counter_names) / sizeof(counter_names[0]); i++) {

		if (*counter == 0xffffffff)
			/* Unsupported */
			*host_counter = 0;
		else
			*host_counter = ntohl(*counter);

		printf("%-50s: %" PRIuLEAST32 "\n", counter_names[i], *host_counter);
		counter++;
		host_counter++;
	}
}

void print_mad_status(uint16_t status)
{
	uint16_t code = (status >> 2) & 0x07;
	uint16_t vendor_code = (status >> 8);

	printf("MAD status: 0x%x - %s\n", status, (status == 0)? "OK": "ERROR");

	if (status & 0x01)
		printf("\tBusy\n");
	if (status & 0x02)
		printf("\tRedirect required\n");

	printf("\tInvalid field code:\n");

	switch (code) {
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

	switch (vendor_code) {
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
	int				lid;
	int				portnum;
	int				portid;
	int				mad_agent;
	void				*umad;
	struct ib_mad			*mad;
	int				length;
	int				ibd_timeout = 1000;
	int				rv;
	int				action;
	int				counter_type;

	mlx_ib_transport_counters_t	transport_counters;
	mlx_hca_ext_flow_counters_t	hca_ext_flow_counters;
	mpcnt_counters_t		mpcnt_counters;
	diag_capabilities_t		capabilities;
	mlx_ppcnt_phy_counters_t	ppcnt_phy_counters;
	plr_counters_t			plr_counters;
	pddr_module_info_t		pddr_module_info;
	pddr_operation_info_t		pddr_operation_info;
	internal_hca_debug_info_t	hca_debug_info;

	if (argc < 5) {
		printf("Usage: %s <read|reset> <caps|transport|flow|pci|ppcnt|plr|"
		       "pddr_module_info|pddr_operation_info|hca_debug> <lid> <portnum>\n", argv[0]);
		exit(1);
	}

	if (strcmp(argv[1], "read") == 0)
		action = READ;
	else if (strcmp(argv[1], "reset") == 0)
		action = RESET;
	else {
		printf("Error: Invalid action '%s', aborting\n", argv[1]);
		printf("Usage: %s <read|reset> <caps|transport|flow|pci|ppcnt|plr|"
		       "pddr_module_info|pddr_operation_info|hca_debug> <lid> <portnum>\n", argv[0]);
		exit(1);
	}

	if (strcmp(argv[2], "transport") == 0)
		counter_type = COUNTER_TYPE_TRANSPORT;
	else if (strcmp(argv[2], "flow") == 0)
		counter_type = COUNTER_TYPE_EXT_HCA_FLOW;
	else if (strcmp(argv[2], "pci") == 0)
		counter_type = COUNTER_TYPE_MPCNT;
	else if (strcmp(argv[2], "caps") == 0)
		counter_type = CAPABILITIES;
	else if (strcmp(argv[2], "ppcnt") == 0)
		counter_type = COUNTER_TYPE_PPCNT;
	else if (strcmp(argv[2], "plr") == 0)
		counter_type = COUNTER_TYPE_PLR;
	else if (strcmp(argv[2], "pddr_module_info") == 0)
		counter_type = COUNTER_TYPE_PDDR_MODULE_INFO;
	else if (strcmp(argv[2], "pddr_operation_info") == 0)
		counter_type = COUNTER_TYPE_PDDR_OPERATION_INFO;
	else if (strcmp(argv[2], "hca_debug") == 0)
		counter_type = COUNTER_TYPE_INTERNAL_HCA_DEBUG_INFO;
	else {
		printf("Error: Invalid counter type '%s', aborting\n", argv[2]);
		printf("Usage: %s <read|reset> <caps|transport|flow|pci|ppcnt|plr|"
		       "pddr_module_info|pddr_operation_info|hca_debug> <lid> <portnum>\n", argv[0]);
		exit(1);
	}

	lid = atoi(argv[3]);
	portnum = atoi(argv[4]);

	if ((portnum < 0) || (portnum > 80)) {
		printf("Invalid port number specified: %d, "
		       "must be 1 <= p <= 80\n", portnum);
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
		 MLX_DIAG_COUNTERS_ATTRID,
		 portnum,
		 action,
		 counter_type);

	umad_set_addr(umad, lid, 1, 0, 0);

	length = IB_MAD_SIZE;

#ifdef DEBUG
	printf("\nSending MAD:\n");
	umad_dump(umad);
	dump_data8(mad, IB_MAD_SIZE);
#endif

	if (umad_send(portid, mad_agent, umad, length, ibd_timeout, 0) < 0)
		IBPANIC("send failed");

	rv = umad_recv(portid, umad, &length, -1);

#ifdef DEBUG
	printf("Received MAD:\n");
	dump_data8(mad, IB_MAD_SIZE);
#endif

	if (rv != mad_agent)
		IBPANIC("recv error: %s", mad_status_str(mad));

	if (!(mad->method & 0x80))
		printf("Not a response MAD\n");

	switch (counter_type) {

	case COUNTER_TYPE_TRANSPORT:
		memcpy(&transport_counters, &mad->data,
		       sizeof(mlx_ib_transport_counters_t));
		break;

	case COUNTER_TYPE_EXT_HCA_FLOW:
		memcpy(&hca_ext_flow_counters, &mad->data,
		       sizeof(mlx_hca_ext_flow_counters_t));
		break;

	case COUNTER_TYPE_MPCNT:
		memcpy(&mpcnt_counters, &mad->data,
		       sizeof(mpcnt_counters));
		break;

	case CAPABILITIES:
		memcpy(&capabilities, &mad->data,
		       sizeof(capabilities));
		break;

	case COUNTER_TYPE_PPCNT:
		memcpy(&ppcnt_phy_counters, &mad->data,
		       sizeof(mlx_ppcnt_phy_counters_t));
		break;

	case COUNTER_TYPE_PLR:
		memcpy(&plr_counters, &mad->data, sizeof(plr_counters_t));
		break;

	case COUNTER_TYPE_PDDR_MODULE_INFO:
		memcpy(&pddr_module_info, &mad->data,
		       sizeof(pddr_module_info_t));
		break;

	case COUNTER_TYPE_PDDR_OPERATION_INFO:
		memcpy(&pddr_operation_info, &mad->data,
		       sizeof(pddr_operation_info_t));
		break;

	case COUNTER_TYPE_INTERNAL_HCA_DEBUG_INFO:
		memcpy(&hca_debug_info, &mad->data,
		       sizeof(internal_hca_debug_info_t));
		break;

	default:
		/* Should have been checked above */
		assert(0);
		/* Turn off gcc warnings */
		break;
	}

	umad_free(umad);
	umad_unregister(portid, mad_agent);
	umad_close_port(portid);
	umad_done();

	if (action == READ) {

		switch (counter_type) {

		case COUNTER_TYPE_TRANSPORT:
			dump_transport_counters(&transport_counters);
			break;

		case COUNTER_TYPE_EXT_HCA_FLOW:
			dump_hca_ext_flow_counters(&hca_ext_flow_counters);
			break;

		case COUNTER_TYPE_MPCNT:
			dump_mpcnt_counters(&mpcnt_counters);
			break;

		case CAPABILITIES:
			dump_capabilities(&capabilities);
			break;

		case COUNTER_TYPE_PPCNT:
			dump_ppcnt_phy_counters(&ppcnt_phy_counters);
			break;

		case COUNTER_TYPE_PLR:
			dump_plr_counters(&plr_counters);
			break;

		case COUNTER_TYPE_PDDR_MODULE_INFO:
			dump_pddr_module_info(&pddr_module_info);
			break;

		case COUNTER_TYPE_PDDR_OPERATION_INFO:
			dump_pddr_operation_info(&pddr_operation_info);
			break;

		case COUNTER_TYPE_INTERNAL_HCA_DEBUG_INFO:
			dump_hca_debug_info(&hca_debug_info);
			break;

		default:
			assert(0);
			break;
		}
	} else
		printf("Counters cleared on LID %d / port %d\n", lid, portnum);

	return 0;
}
