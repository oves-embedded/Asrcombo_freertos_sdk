
/**
 ****************************************************************************************
 *
 * @file (app_patch.h)
 *
 * @brief
 *
 * Copyright (C) ASR 2020 - 2029
 *
 *
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "app_patch.h"
#include <stdio.h>
#include <stdlib.h>        // standard library
#include "rom.h"

#include "arch.h"
#include "ke.h"
#include "ke_msg.h"
#include "ke_task.h"
#include "co_list.h"
#include "rwip.h"
#include "co_bt_defines.h"
#include "aes_int.h"
#include "sch_arb.h"
#include "sch_alarm.h"
#include "sch_prog.h"
#include "lld_int.h"
#include "lld_scan_int.h"
#include "sch_plan.h"
#include "llc_llcp.h"
#include "llc_int.h"
#include "llm.h"
#include "lld.h"
#include "llc.h"
#include "ecc_p256.h"
#include "nvds.h"
#include "l2cc_lecb.h"
#include "l2cc_pdu.h"
#include "l2cc_task.h"
#include "l2cc.h"
#include "rom_patch.h"
#include "aes_int.h"
#include "sch_arb.h"
#include "sch_alarm.h"
#include "sch_prog.h"
#include "lld_int.h"
#include "lld_scan_int.h"
#include "sch_plan.h"
#include "llc_llcp.h"
#include "llc_int.h"
#include "llm.h"
#include "lld.h"
#include "llc.h"
#include "ecc_p256.h"
#include "nvds.h"
#include "l2cc_lecb.h"
#include "l2cc_pdu.h"
#include "l2cc_task.h"
#include "l2cc.h"
#include "ke_event.h"

/*
 * MACRO DEFINES
 ****************************************************************************************
 */
struct aes_rpa_gen_env;
struct aes_rpa_resolve_env;
struct llc_op_clk_acc_ind;
struct llc_op_con_upd_ind;
struct llc_op_disconnect_ind;
struct llc_op_past_ind;
struct llc_op_dl_upd_ind;
struct llc_op_encrypt_ind;
struct llc_op_feats_exch_ind;
struct llc_op_phy_upd_ind;
struct llc_op_ver_exch_ind;
struct llc_op_ch_map_upd_ind;
struct nvds_tag_header;
struct gapc_smp_pair_info;
struct lld_conless_cte_rx_ind;
/*
 * VARIABLE DECLARATIONS
 ****************************************************************************************
 */
#define  APP_PATCH_KE           0  //(Num:56  Status:PASS)
#define  APP_PATCH_CO           0  //(Num:21  Status:PASS)
#define  APP_PATCH_AES          0  //(Num:13  Status:PASS)
#define  APP_PATCH_SCH          0  //(Num:28  Status:PASS)
#define  APP_PATCH_LLD          0  //(Num:99  Status:PASS)
#define  APP_PATCH_RPA          0  //(Num:4  Status:PASS)
#define  APP_PATCH_SCH_PLAN     0  //(Num:10  Status:PASS)
#define  APP_PATCH_HCI          0  //(Num:38  Status:PASS)
#define  APP_PATCH_LLC          0  //(Num:127  Status:PASS)
#define  APP_PATCH_LLM          0  //(Num:123  Status:PASS)
#define  APP_PATCH_LLC_CHMAP    0  //(Num:24  Status:PASS)
#define  APP_PATCH_NVDS         0  //(Num:14  Status:PASS)
#define  APP_PATCH_L2CC         0  //(Num:  Status:PASS)
#define  APP_PATCH_GAP          0  //(Num:99  Status:PASS)


//////////////////////////////////// P1: Externed patch /////////////////////////////////////////////////////////////////////////////
#if APP_PATCH_KE
extern uint8_t (*pf_patch_ke_init)(void);
extern uint8_t (*pf_patch_ke_flush)(void);
extern bool (*pf_patch_ke_sleep_check)(void);
extern enum KE_STATUS (*pf_patch_ke_stats_get)(uint8_t* max_msg_sent,uint8_t* max_msg_saved,uint8_t* max_timer_used,uint16_t* max_heap_used);
extern uint8_t (*pf_patch_ke_event_init)(void);
extern uint8_t (*pf_patch_ke_event_callback_set)(uint8_t event_type, void (*p_callback)(void));
extern uint8_t (*pf_patch_ke_event_set)(uint8_t event_type);
extern uint8_t (*pf_patch_ke_event_clear)(uint8_t event_type);
extern uint8_t (*pf_patch_ke_event_get)(uint8_t event_type);
extern uint32_t (*pf_patch_ke_event_get_all)(void);
extern uint8_t (*pf_patch_ke_event_flush)(void);
extern uint8_t (*pf_patch_ke_event_schedule)(void);
extern bool (*pf_patch_cmp_dest_id)(struct co_list_hdr const *msg, uint32_t dest_id);
extern int (*pf_patch_ke_msg_discard)(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_patch_ke_msg_save)(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern ke_msg_func_t (*pf_patch_ke_handler_search)(ke_msg_id_t const msg_id, struct ke_msg_handler const *msg_handler_tab, uint16_t msg_cnt);
extern ke_msg_func_t (*pf_patch_ke_task_handler_get)(ke_msg_id_t const msg_id, ke_task_id_t const task_id);
extern ke_state_t (*pf_patch_ke_state_get)(ke_task_id_t const id);
extern ke_task_id_t (*pf_patch_ke_task_check)(ke_task_id_t task);
extern uint8_t (*pf_patch_ke_task_create)(uint8_t task_type, struct ke_task_desc const *p_task_desc);
extern uint8_t (*pf_patch_ke_task_delete)(uint8_t task_type);
extern uint8_t (*pf_patch_ke_state_set)(ke_task_id_t const id, ke_state_t const state_id);
extern uint8_t (*pf_patch_ke_task_init)(void);
extern uint8_t (*pf_patch_ke_task_msg_flush)(ke_task_id_t task);
extern uint8_t (*pf_patch_ke_task_saved_update)(ke_task_id_t const ke_task_id);
extern uint8_t (*pf_patch_ke_task_schedule)(void);
extern uint8_t *(*pf_patch_ke_msg_alloc)(ke_msg_id_t const id, ke_task_id_t const dest_id, ke_task_id_t const src_id, uint16_t const param_len);
extern uint8_t (*pf_patch_ke_msg_send)(void const *param_ptr);
extern uint8_t (*pf_patch_ke_msg_send_basic)(ke_msg_id_t const id, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_patch_ke_msg_forward)(void const *param_ptr, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_patch_ke_msg_forward_new_id)(void const *param_ptr, ke_msg_id_t const msg_id, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_patch_ke_msg_free)(struct ke_msg const *msg);
extern ke_msg_id_t (*pf_patch_ke_msg_dest_id_get)(void const *param_ptr);
extern ke_msg_id_t (*pf_patch_ke_msg_src_id_get)(void const *param_ptr);
extern bool (*pf_patch_ke_msg_in_queue)(void const *param_ptr);
extern bool (*pf_patch_ke_mem_is_in_heap)(uint8_t type, void* mem_ptr);
extern uint8_t (*pf_patch_ke_mem_init)(uint8_t type, uint8_t* heap, uint16_t heap_size);
extern bool (*pf_patch_ke_mem_is_empty)(uint8_t type);
extern bool (*pf_patch_ke_check_malloc)(uint32_t size, uint8_t type);
extern void *(*pf_patch_ke_malloc)(uint32_t size, uint8_t type);
extern void (*pf_patch_ke_free)(void* mem_ptr);
extern bool (*pf_patch_ke_is_free)(void* mem_ptr);
extern uint16_t (*pf_patch_ke_get_mem_usage)(uint8_t type);
extern uint32_t (*pf_patch_ke_get_max_mem_usage)(void);
extern struct co_list_hdr *(*pf_patch_ke_queue_extract)(struct co_list * const queue,bool (*func)(struct co_list_hdr const * elmt, uint32_t arg), uint32_t arg);
extern uint8_t (*pf_patch_ke_queue_insert)(struct co_list * const queue, struct co_list_hdr * const element, bool (*cmp)(struct co_list_hdr const *elementA, struct co_list_hdr const *elementB));
extern rwip_time_t (*pf_patch_ke_time)(void);
extern bool (*pf_patch_cmp_abs_time)(struct co_list_hdr const * timerA, struct co_list_hdr const * timerB);
extern bool (*pf_patch_cmp_timer_id)(struct co_list_hdr const * timer, uint32_t timer_task);
extern bool (*pf_patch_ke_time_past)(rwip_time_t time);
extern bool (*pf_patch_ke_timer_active)(ke_msg_id_t const timer_id, ke_task_id_t const task_id);
extern uint8_t (*pf_patch_ke_timer_adjust_all)(uint32_t delay);
extern uint8_t (*pf_patch_ke_timer_clear)(ke_msg_id_t const timer_id, ke_task_id_t const task_id);
extern uint8_t (*pf_patch_ke_timer_init)(void);
extern uint8_t (*pf_patch_ke_timer_schedule)(void);
extern uint8_t (*pf_patch_ke_timer_set)(ke_msg_id_t const timer_id, ke_task_id_t const task_id, uint32_t delay);
#endif  //APP_PATCH_KE

#if APP_PATCH_CO
extern bool (*pf_patch_co_list_extract)(struct co_list *list, struct co_list_hdr *list_hdr);
extern bool (*pf_patch_co_list_find)(struct co_list *list, struct co_list_hdr *list_hdr);
extern struct co_list_hdr *(*pf_patch_co_list_pop_front)(struct co_list *list);
extern uint16_t (*pf_patch_co_list_size)(struct co_list *list);
extern uint8_t (*pf_patch_co_list_extract_after)(struct co_list *list, struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_rem_hdr);
extern uint8_t (*pf_patch_co_list_extract_sublist)(struct co_list *list, struct co_list_hdr *ref_hdr, struct co_list_hdr *last_hdr);
extern uint8_t (*pf_patch_co_list_init)(struct co_list *list);
extern uint8_t (*pf_patch_co_list_insert_after)(struct co_list *list, struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_add_hdr);
extern uint8_t (*pf_patch_co_list_insert_before)(struct co_list *list, struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_add_hdr);
extern uint8_t (*pf_patch_co_list_merge)(struct co_list *list1, struct co_list *list2);
extern uint8_t (*pf_patch_co_list_pool_init)(struct co_list *list, void *pool, size_t elmt_size, uint32_t elmt_cnt);
extern uint8_t (*pf_patch_co_list_push_back)(struct co_list *list, struct co_list_hdr *list_hdr);
extern uint8_t (*pf_patch_co_list_push_back_sublist)(struct co_list *list, struct co_list_hdr *first_hdr, struct co_list_hdr *last_hdr);
extern uint8_t (*pf_patch_co_list_push_front)(struct co_list *list, struct co_list_hdr *list_hdr);
extern uint16_t (*pf_patch_co_util_read_array_size)(char **fmt_cursor);
extern void (*pf_patch_co_bytes_to_string)(char* dest, uint8_t* src, uint8_t nb_bytes);
extern bool (*pf_patch_co_bdaddr_compare)(struct bd_addr const *bd_address1, struct bd_addr const *bd_address2);
extern uint8_t (*pf_patch_co_nb_good_le_channels)(const struct le_chnl_map* map);
extern uint8_t (*pf_patch_co_util_pack)(uint8_t* out, uint8_t* in, uint16_t* out_len, uint16_t in_len, const char* format);
extern uint8_t (*pf_patch_co_util_unpack)(uint8_t* out, uint8_t* in, uint16_t* out_len, uint16_t in_len, const char* format);
extern uint16_t (*pf_patch_co_ble_pkt_dur_in_us)(uint8_t len, uint8_t rate);
#endif

#if APP_PATCH_AES
extern struct aes_func_env* (*pf_patch_aes_alloc)(uint16_t size, aes_func_continue_cb aes_continue_cb, aes_func_result_cb res_cb, uint32_t src_info);
extern uint8_t (*pf_patch_aes_encrypt)(const uint8_t* key, const uint8_t *val, bool copy, aes_func_result_cb res_cb, uint32_t src_info);
extern uint8_t (*pf_patch_aes_init)(uint8_t init_type);
extern uint8_t (*pf_patch_aes_start)(struct aes_func_env* env, const uint8_t* key, const uint8_t *val);
extern void (*pf_patch_aes_app_encrypt)(const uint8_t* key, const uint8_t *val, aes_func_result_cb res_cb, uint32_t src_info);
extern void (*pf_patch_aes_app_decrypt)(const uint8_t* key, const uint8_t *val, aes_func_result_cb res_cb, uint32_t src_info);
extern uint8_t (*pf_patch_aes_rand)(aes_func_result_cb res_cb, uint32_t src_info);
extern uint8_t (*pf_patch_aes_result_handler)(uint8_t status, uint8_t* result);
extern uint8_t (*pf_patch_aes_xor_128)(uint8_t* result, const uint8_t* a, const uint8_t* b, uint8_t size);
extern uint8_t (*pf_patch_aes_shift_left_128)(const uint8_t* input,uint8_t* output);
extern uint8_t (*pf_patch_aes_cmac)(const uint8_t *key, const uint8_t *message, uint16_t message_len, aes_func_result_cb res_cb, uint32_t src_info);
extern uint8_t (*pf_patch_aes_cmac_start)(struct aes_cmac_env *env, const uint8_t *key, const uint8_t *message, uint16_t message_len);
extern bool (*pf_patch_aes_cmac_continue)(struct aes_cmac_env *env, uint8_t *aes_res);
#endif

#if APP_PATCH_SCH
extern uint8_t (*pf_patch_sch_arb_conflict_check)(struct sch_arb_elt_tag * evt_a, struct sch_arb_elt_tag * evt_b);
extern uint8_t (*pf_patch_sch_arb_insert)(struct sch_arb_elt_tag *elt);
extern uint8_t (*pf_patch_sch_arb_remove)(struct sch_arb_elt_tag *elt, bool not_waiting);
extern uint8_t (*pf_patch_sch_arb_elt_cancel)(struct sch_arb_elt_tag *new_elt);
extern uint8_t (*pf_patch_sch_arb_event_start_isr)(void);
extern uint8_t (*pf_patch_sch_arb_init)(uint8_t init_type);
extern uint8_t (*pf_patch_sch_arb_prog_timer)(void);
extern uint8_t (*pf_patch_sch_arb_sw_isr)(void);
extern uint8_t (*pf_patch_sch_alarm_prog)(void);
extern uint8_t (*pf_patch_sch_alarm_init)(uint8_t init_type);
extern uint8_t (*pf_patch_sch_alarm_timer_isr)(void);
extern uint8_t (*pf_patch_sch_alarm_set)(struct sch_alarm_tag* elt);
extern uint8_t (*pf_patch_sch_alarm_clear)(struct sch_alarm_tag* elt);
extern uint8_t (*pf_patch_sch_prog_rx_isr)(uint8_t et_idx);
extern uint8_t (*pf_patch_sch_prog_tx_isr)(uint8_t et_idx);
extern uint8_t (*pf_patch_sch_prog_skip_isr)(uint8_t et_idx);
extern uint8_t (*pf_patch_sch_prog_end_isr)(uint8_t et_idx);
extern uint8_t (*pf_patch_sch_prog_init)(uint8_t init_type);
extern uint8_t (*pf_patch_sch_prog_fifo_isr)(void);
extern uint8_t (*pf_patch_sch_prog_push)(struct sch_prog_params* params);
extern uint8_t (*pf_patch_sch_slice_compute)(void);
extern uint8_t (*pf_patch_sch_slice_init)(uint8_t init_type);
extern uint8_t (*pf_patch_sch_slice_bg_add)(uint8_t type);
extern uint8_t (*pf_patch_sch_slice_bg_remove)(uint8_t type);
extern uint8_t (*pf_patch_sch_slice_fg_add)(uint8_t type, uint32_t end_ts);
extern uint8_t (*pf_patch_sch_slice_fg_remove)(uint8_t type);
extern uint8_t (*pf_patch_sch_slice_per_add)(uint8_t type, uint8_t id, uint32_t intv, uint16_t duration, bool retx);
extern uint8_t (*pf_patch_sch_slice_per_remove)(uint8_t type, uint8_t id);
#endif//

#if APP_PATCH_LLD
extern bool (*pf_patch_lld_calc_aux_rx)(struct lld_calc_aux_rx_out* aux_rx_out, uint8_t index_pkt, uint32_t aux_data);
extern bool (*pf_patch_lld_rxdesc_check)(uint8_t label);
extern uint32_t (*pf_patch_lld_read_clock)(void);
extern uint8_t (*pf_patch_lld_ch_idx_get)(void);
extern uint8_t (*pf_patch_lld_local_sca_get)(void);
extern uint8_t (*pf_patch_lld_channel_assess)(uint8_t channel, bool rx_ok, uint32_t timestamp);
extern uint8_t (*pf_patch_lld_rpa_renew_evt_canceled_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_rpa_renew_evt_start_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_rpa_renew_instant_cbk)(struct sch_alarm_tag* elt);
extern uint8_t (*pf_patch_lld_rxdesc_free)(void);
extern uint32_t (*pf_patch_lld_adv_ext_pkt_prepare)(uint8_t act_id, uint8_t txdesc_idx,
        uint8_t type, uint8_t mode, bool adva, bool targeta, bool adi,bool aux_ptr, bool sync_info, bool tx_power,
        uint16_t* ext_header_txbuf_offset, uint16_t* data_txbuf_offset, uint16_t* data_len);
extern bool (*pf_patch_lld_adv_pkt_rx)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_adv_end)(uint8_t act_id, bool indicate, uint8_t status);
extern uint8_t (*pf_patch_lld_adv_sync_info_set)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_adv_aux_ch_idx_set)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_adv_ext_chain_construct)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_adv_adv_data_set)(uint8_t act_id, uint8_t len, uint16_t data, bool release_old_buf);
extern uint8_t (*pf_patch_lld_adv_scan_rsp_data_set)(uint8_t act_id, uint8_t len, uint16_t data, bool release_old_buf);
extern uint8_t (*pf_patch_lld_adv_evt_start_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_adv_aux_evt_start_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_adv_evt_canceled_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_adv_aux_evt_canceled_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_adv_frm_isr)(uint8_t act_id, uint32_t timestamp, bool abort);
extern uint8_t (*pf_patch_lld_adv_frm_skip_isr)(uint8_t act_id, uint32_t timestamp);
extern uint8_t (*pf_patch_lld_adv_frm_cbk)(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern bool (*pf_patch_lld_scan_sync_accept)(uint8_t scan_id, uint8_t index_pkt, uint8_t addr_type, struct bd_addr rx_adva, uint8_t rx_adv_sid);
extern bool (*pf_patch_lld_scan_sync_info_unpack)(struct sync_info* p_syncinfo, uint16_t em_addr);
extern uint8_t (*pf_patch_lld_scan_restart)(void);
extern uint8_t (*pf_patch_lld_scan_aux_evt_canceled_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_scan_aux_evt_start_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_scan_aux_sched)(uint8_t scan_id);
extern uint8_t (*pf_patch_lld_scan_end)(void);
extern uint8_t (*pf_patch_lld_scan_evt_canceled_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_scan_evt_start_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_scan_frm_cbk)(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern uint8_t (*pf_patch_lld_scan_frm_eof_isr)(uint8_t scan_id, uint32_t timestamp, bool abort);
extern uint8_t (*pf_patch_lld_scan_frm_rx_isr)(uint8_t scan_id);
extern uint8_t (*pf_patch_lld_scan_frm_skip_isr)(uint8_t scan_id);
extern uint8_t (*pf_patch_lld_scan_process_pkt_rx)(uint8_t scan_id);
extern uint8_t (*pf_patch_lld_scan_sched)(uint8_t phy_idx, uint32_t timestamp, bool resched);
extern uint8_t (*pf_patch_lld_scan_trunc_ind)(struct lld_scan_evt_params* scan_evt);
extern uint16_t (*pf_patch_lld_init_compute_winoffset)(uint16_t con_intv, uint16_t con_offset, uint8_t txwin_delay, uint32_t timestamp);
extern uint8_t (*pf_patch_lld_init_connect_req_pack)(uint8_t* p_data_pack, struct pdu_con_req_lldata* p_data);
extern uint8_t (*pf_patch_lld_init_end)(void);
extern uint8_t (*pf_patch_lld_init_evt_canceled_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_init_evt_start_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_init_frm_cbk)(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern uint8_t (*pf_patch_lld_init_frm_eof_isr)(uint8_t env_idx, uint32_t timestamp, bool abort);
extern uint8_t (*pf_patch_lld_init_frm_skip_isr)(uint8_t env_idx);
extern uint8_t (*pf_patch_lld_init_process_pkt_rx)(uint8_t env_idx);
extern uint8_t (*pf_patch_lld_init_process_pkt_tx)(uint8_t env_idx);
extern uint8_t (*pf_patch_lld_init_sched)(uint8_t env_idx, uint32_t timestamp, bool resched);
extern void (*pf_patch_lld_con_max_lat_calc)(uint8_t link_id);
extern void (*pf_patch_lld_con_tx_len_update)(uint8_t link_id, uint8_t tx_rate, uint32_t con_intv);
extern void (*pf_patch_lld_con_evt_time_update)(uint8_t link_id);
extern void (*pf_patch_lld_con_cleanup)(uint8_t link_id, bool indicate, uint8_t reason);
extern void (*pf_patch_lld_instant_proc_end)(uint8_t link_id);
extern void (*pf_patch_lld_con_sched)(uint8_t link_id, uint32_t clock, bool sync);
extern void (*pf_patch_lld_con_rx)(uint8_t link_id, uint32_t timestamp);
extern void (*pf_patch_lld_con_tx)(uint8_t link_id);
extern void (*pf_patch_lld_con_tx_prog)(uint8_t link_id);
extern void (*pf_patch_lld_con_evt_start_cbk)(struct sch_arb_elt_tag* evt);
extern void (*pf_patch_lld_con_evt_canceled_cbk)(struct sch_arb_elt_tag* evt);
extern void (*pf_patch_lld_con_frm_isr)(uint8_t link_id, uint32_t timestamp, bool abort);
extern void (*pf_patch_lld_con_rx_isr)(uint8_t link_id, uint32_t timestamp);
extern void (*pf_patch_lld_con_tx_isr)(uint8_t link_id, uint32_t timestamp);
extern void (*pf_patch_lld_con_frm_skip_isr)(uint8_t act_id);
extern void (*pf_patch_lld_con_frm_cbk)(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern uint8_t (*pf_patch_lld_sync_cleanup)(uint8_t act_id, uint8_t status);
extern uint8_t (*pf_patch_lld_sync_trunc_ind)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_sync_process_pkt_rx)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_sync_sched)(uint8_t act_id, bool resched);
extern uint8_t (*pf_patch_lld_sync_frm_eof_isr)(uint8_t act_id, uint32_t timestamp, bool abort);
extern uint8_t (*pf_patch_lld_sync_frm_rx_isr)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_sync_frm_skip_isr)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_sync_frm_cbk)(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern uint8_t (*pf_patch_lld_sync_evt_start_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_sync_evt_canceled_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_sync_ant_switch_config)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_per_adv_cleanup)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_per_adv_sched)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_per_adv_chain_construct)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_per_adv_data_set)(uint8_t act_id, uint8_t len, uint16_t data, bool release_old_buf);
extern uint8_t (*pf_patch_lld_per_adv_evt_start_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_per_adv_evt_canceled_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_per_adv_frm_isr)(uint8_t act_id, uint32_t timestamp, bool abort);
extern uint8_t (*pf_patch_lld_per_adv_frm_skip_isr)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_per_adv_frm_cbk)(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern uint8_t (*pf_patch_lld_per_adv_ant_switch_config)(uint8_t act_id);
extern uint8_t (*pf_patch_lld_per_adv_sync_info_get)(uint8_t act_id, uint32_t* sync_ind_ts, uint16_t* pa_evt_cnt, struct le_chnl_map *map);
extern uint8_t (*pf_patch_lld_per_adv_init_info_get)(uint8_t act_id, struct access_addr* aa, struct crc_init* crcinit, struct le_chnl_map* chm);
extern uint32_t (*pf_patch_lld_per_adv_ext_pkt_prepare)(uint8_t act_id, uint8_t txdesc_idx,
                                                 uint8_t type, uint8_t mode, bool tx_power,
                                                 uint16_t* ext_header_txbuf_offset, uint16_t* data_txbuf_offset,
                                                 uint16_t* data_len, bool force_aux_ptr);
extern uint8_t (*pf_patch_lld_test_freq2chnl)(uint8_t freq);
extern uint8_t (*pf_patch_lld_test_cleanup)(void);
extern uint8_t (*pf_patch_lld_test_evt_canceled_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_test_evt_start_cbk)(struct sch_arb_elt_tag* evt);
extern uint8_t (*pf_patch_lld_test_frm_cbk)(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern uint8_t (*pf_patch_lld_test_frm_isr)(uint32_t timestamp, bool abort);
extern uint8_t (*pf_patch_lld_test_rx_isr)(uint32_t timestamp);
#endif

#if APP_PATCH_RPA
extern uint8_t (*pf_aes_rpa_gen_continue_patch)(struct aes_rpa_gen_env *env, uint8_t *aes_res);
extern uint8_t (*pf_aes_rpa_resolve_continue_patch)(struct aes_rpa_resolve_env *env, uint8_t *aes_res);
extern uint8_t (*pf_aes_rpa_gen_patch)(struct irk *irk, aes_func_result_cb res_cb, uint32_t src_info);
extern uint8_t (*pf_aes_rpa_resolve_patch)(uint8_t nb_irk, struct irk *irk, struct bd_addr *addr, aes_rpa_func_result_cb res_cb, uint32_t src_info);
#endif

#if APP_PATCH_SCH_PLAN
extern uint8_t (*pf_sch_plan_init_patch)(uint8_t init_type);
extern uint32_t (*pf_sch_plan_offset_max_calc_patch)(uint32_t interval, uint32_t duration, uint32_t offset_min, uint16_t conhdl);
extern uint8_t (*pf_sch_plan_offset_req_patch)(uint8_t action, bool dur_max, struct sch_plan_req_param *req_param, struct sch_plan_chk_param *chk_param);
extern uint8_t (*pf_sch_plan_interval_req_patch)(struct sch_plan_req_param *req_param);
extern uint8_t (*pf_sch_plan_position_range_compute_patch)(uint8_t link_id, uint8_t role, uint16_t interval, uint16_t duration, uint16_t *min_offset, uint16_t *max_offset);
extern uint8_t (*pf_sch_plan_clock_wrap_offset_update_patch)(void);
extern uint8_t (*pf_sch_plan_set_patch)(struct sch_plan_elt_tag *elt_to_add);
extern uint8_t (*pf_sch_plan_rem_patch)(struct sch_plan_elt_tag *elt_to_remove);
extern uint8_t (*pf_sch_plan_req_patch)(struct sch_plan_req_param* req_param);
extern uint8_t (*pf_sch_plan_chk_patch)(struct sch_plan_chk_param* chk_param);
#endif

#if APP_PATCH_HCI
extern uint8_t (*pf_hci_send_2_host_patch)(void *param);
extern uint8_t (*pf_hci_send_2_controller_patch)(void *param);
extern bool (*pf_hci_evt_mask_check_patch)(struct ke_msg *msg);
extern uint8_t (*pf_hci_init_patch)(uint8_t init_type);
extern uint8_t (*pf_hci_ble_conhdl_register_patch)(uint8_t link_id);
extern uint8_t (*pf_hci_ble_conhdl_unregister_patch)(uint8_t link_id);
extern uint8_t (*pf_hci_evt_mask_set_patch)(struct evt_mask const *evt_msk, uint8_t page);
extern uint8_t (*pf_hci_evt_filter_add_patch)(struct hci_set_evt_filter_cmd const *param);
extern struct hci_cmd_desc_tag* (*pf_hci_look_for_cmd_desc_patch)(uint16_t opcode);
extern struct hci_evt_desc_tag* (*pf_hci_look_for_evt_desc_patch)(uint8_t code);
extern struct hci_evt_desc_tag* (*pf_hci_look_for_le_evt_desc_patch)(uint8_t subcode);
extern uint8_t (*pf_hci_pack_bytes_patch)(uint8_t** pp_in, uint8_t** pp_out, uint8_t* p_in_end, uint8_t* p_out_end, uint8_t len);
extern uint8_t (*pf_hci_host_nb_cmp_pkts_cmd_pkupk_patch)(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t (*pf_hci_le_set_ext_scan_param_cmd_upk_patch)(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t (*pf_hci_le_ext_create_con_cmd_upk_patch)(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t (*pf_hci_le_set_ext_adv_en_cmd_upk_patch)(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t (*pf_hci_le_adv_report_evt_pkupk_patch)(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t (*pf_hci_le_dir_adv_report_evt_pkupk_patch)(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t (*pf_hci_le_ext_adv_report_evt_pkupk_patch)(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t (*pf_hci_le_conless_iq_report_evt_pkupk_patch)(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t (*pf_hci_le_con_iq_report_evt_pkupk_patch)(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern bool (*pf_hci_ble_adv_report_filter_check_patch)(struct ke_msg * msg);
extern uint8_t (*pf_hci_ble_adv_report_tx_check_patch)(struct ke_msg * msg);
extern uint8_t (*pf_hci_cmd_reject_patch)(const struct hci_cmd_desc_tag* cmd_desc, uint16_t opcode, uint8_t error, uint8_t * payload);
extern uint8_t (*pf_hci_cmd_received_patch)(uint16_t opcode, uint8_t length, uint8_t *payload);
extern uint8_t (*pf_hci_acl_tx_data_received_patch)(uint16_t hdl_flags, uint16_t datalen, uint8_t * payload);
extern uint8_t* (*pf_hci_build_cs_evt_patch)(struct ke_msg * msg);
extern uint8_t* (*pf_hci_build_cc_evt_patch)(struct ke_msg * msg);
extern uint8_t* (*pf_hci_build_evt_patch)(struct ke_msg * msg);
extern uint8_t* (*pf_hci_build_le_evt_patch)(struct ke_msg * msg);
extern uint8_t* (*pf_hci_build_acl_data_patch)(struct ke_msg * msg);
extern uint8_t* (*pf_hci_acl_tx_data_alloc_patch)(uint16_t hdl_flags, uint16_t len);
extern uint8_t (*pf_hci_tx_start_patch)(void);
extern uint8_t (*pf_hci_tx_done_patch)(void);
extern uint8_t (*pf_hci_tx_trigger_patch)(void);
extern uint8_t (*pf_hci_tl_send_patch)(struct ke_msg *msg);
extern uint8_t (*pf_hci_tl_init_patch)(uint8_t init_type);
extern uint8_t (*pf_hci_cmd_get_max_param_size_patch)(uint16_t opcode);
#endif

#if APP_PATCH_LLC
extern uint8_t (*pf_llc_loc_clk_acc_proc_continue_patch)(uint8_t link_id, uint8_t status);
extern uint8_t (*pf_llc_ll_clk_acc_req_pdu_send_patch)(uint8_t link_id, uint8_t sca);
extern uint8_t (*pf_llc_ll_clk_acc_rsp_pdu_send_patch)(uint8_t link_id, uint8_t sca);
extern uint8_t (*pf_llc_clk_acc_proc_err_cb_patch)(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t (*pf_ll_clk_acc_req_handler_patch)(uint8_t link_id, struct ll_clk_acc_req *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_clk_acc_rsp_handler_patch)(uint8_t link_id, struct ll_clk_acc_rsp *pdu, uint16_t event_cnt);
extern int (*pf_llc_op_clk_acc_ind_handler_patch)(ke_msg_id_t const msgid, struct llc_op_clk_acc_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_llc_clk_acc_modify_patch)(uint8_t link_id, uint8_t action);
extern uint8_t (*pf_llc_loc_con_upd_proc_continue_patch)(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t (*pf_llc_rem_con_upd_proc_continue_patch)(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t (*pf_llc_pref_param_compute_patch)(uint8_t link_id, struct llc_op_con_upd_ind* param);
extern uint8_t (*pf_llc_hci_con_upd_info_send_patch)(uint8_t link_id, uint8_t status, struct llc_op_con_upd_ind *param);
extern bool (*pf_llc_con_upd_param_in_range_patch)(uint8_t link_id, uint16_t interval_max, uint16_t interval_min, uint16_t latency, uint16_t timeout);
extern uint8_t (*pf_llc_ll_connection_update_ind_pdu_send_patch)(uint8_t link_id, struct llc_op_con_upd_ind *param);
extern uint8_t (*pf_llc_ll_connection_param_req_pdu_send_patch)(uint8_t link_id, struct llc_op_con_upd_ind *param);
extern uint8_t (*pf_llc_ll_connection_param_rsp_pdu_send_patch)(uint8_t link_id, struct llc_op_con_upd_ind *param);
extern uint8_t (*pf_llc_hci_con_param_req_evt_send_patch)(uint8_t link_id, uint16_t con_intv_min, uint16_t con_intv_max, uint16_t con_latency, uint16_t superv_to);
extern uint8_t (*pf_llc_loc_con_upd_proc_err_cb_patch)(uint8_t link_id, uint8_t error_type, void *param);
extern uint8_t (*pf_llc_rem_con_upd_proc_err_cb_patch)(uint8_t link_id, uint8_t error_type, void *param);
extern uint8_t (*pf_ll_connection_update_ind_handler_patch)(uint8_t link_id, struct ll_connection_update_ind *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_connection_param_req_handler_patch)(uint8_t link_id, struct ll_connection_param_req *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_connection_param_rsp_handler_patch)(uint8_t link_id, struct ll_connection_param_rsp *param, uint16_t event_cnt);
extern int (*pf_hci_le_con_upd_cmd_handler_patch)(uint8_t link_id, struct hci_le_con_update_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rem_con_param_req_reply_cmd_handler_patch)(uint8_t link_id, struct hci_le_rem_con_param_req_reply_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rem_con_param_req_neg_reply_cmd_handler_patch)(uint8_t link_id, struct hci_le_rem_con_param_req_neg_reply_cmd const *param, uint16_t opcode);
extern int (*pf_llc_op_con_upd_ind_handler_patch)(ke_msg_id_t const msgid, struct llc_op_con_upd_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_con_param_upd_cfm_handler_patch)(ke_msg_id_t const msgid, void *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_con_estab_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_con_estab_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_con_offset_upd_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_con_offset_upd_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_llc_con_move_cbk_patch)(uint16_t id);
extern uint8_t (*pf_llc_disconnect_proc_continue_patch)(uint8_t link_id, uint8_t status);
extern int (*pf_llc_op_disconnect_ind_handler_patch)(ke_msg_id_t const msgid, struct llc_op_disconnect_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_hci_disconnect_cmd_handler_patch)(uint8_t link_id, struct hci_disconnect_cmd const *param, uint16_t opcode);
extern uint8_t (*pf_llc_disconnect_end_patch)(uint8_t link_id, uint8_t status, uint8_t reason);
extern uint8_t (*pf_llc_ll_terminate_ind_pdu_send_patch)(uint16_t link_id, uint8_t reason);
extern uint8_t (*pf_llc_disconnect_proc_err_cb_patch)(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t (*pf_ll_terminate_ind_handler_patch)(uint8_t link_id, struct ll_terminate_ind *pdu, uint16_t event_cnt);
extern int (*pf_lld_disc_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_disc_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_llc_stopped_ind_handler_patch)(ke_msg_id_t const msgid, void *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_llc_disconnect_patch)(uint8_t link_id, uint8_t reason, bool immediate);
extern uint8_t (*pf_llc_init_term_proc_patch)(uint8_t link_id, uint8_t reason);
extern uint8_t (*pf_llc_llcp_send_patch)(uint8_t link_id, union llcp_pdu *pdu, llc_llcp_tx_cfm_cb tx_cfm_cb);
extern uint8_t (*pf_llc_llcp_tx_check_patch)(uint8_t link_id);
extern uint8_t (*pf_llc_ll_reject_ind_pdu_send_patch)(uint8_t link_id, uint8_t rej_op_code, uint8_t reason, llc_llcp_tx_cfm_cb cfm_cb);
extern int (*pf_lld_llcp_rx_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_llcp_rx_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_llcp_tx_cfm_handler_patch)(ke_msg_id_t const msgid, void *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_llc_past_sync_info_unpack_patch)(struct sync_info* p_syncinfo_struct, uint8_t * p_syncinfo_tab);
extern uint8_t (*pf_llc_past_rx_patch)(uint8_t link_id);
extern uint8_t (*pf_llc_past_rpa_res_cb_patch)(uint8_t index, uint32_t src_info) ;
extern int (*pf_llc_op_past_ind_handler_patch)(ke_msg_id_t const msgid, struct llc_op_past_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_ll_per_sync_ind_handler_patch)(uint8_t link_id, struct ll_per_sync_ind *pdu, uint16_t event_cnt);
extern int (*pf_hci_le_per_adv_sync_transf_cmd_handler_patch)(uint8_t link_id, struct hci_le_per_adv_sync_transf_cmd const *param, uint16_t opcode) ;
extern int (*pf_hci_le_per_adv_set_info_transf_cmd_handler_patch)(uint8_t link_id, struct hci_le_per_adv_set_info_transf_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_per_adv_sync_transf_param_cmd_handler_patch)(uint8_t link_id, struct hci_le_set_per_adv_sync_transf_param_cmd const *param, uint16_t opcode) ;
extern uint8_t (*pf_llc_loc_dl_upd_proc_continue_patch)(uint8_t link_id, uint8_t status);
extern uint8_t (*pf_llc_rem_dl_upd_proc_patch)(uint8_t link_id, uint16_t max_tx_time, uint16_t max_tx_octets, uint16_t max_rx_time, uint16_t max_rx_octets);
extern int (*pf_ll_length_req_handler_patch)(uint8_t link_id, struct ll_length_req *pdu, uint16_t event_cnt);
extern int (*pf_hci_le_set_data_len_cmd_handler_patch)(uint8_t link_id, struct hci_le_set_data_len_cmd const *param, uint16_t opcode);
extern uint8_t (*pf_dl_upd_proc_start_patch)(uint8_t link_id);
extern uint8_t (*pf_llc_ll_length_req_pdu_send_patch)(uint8_t link_id, uint16_t eff_tx_time, uint16_t eff_tx_octets, uint16_t eff_rx_time, uint16_t eff_rx_octets);
extern uint8_t (*pf_llc_ll_length_rsp_pdu_send_patch)(uint8_t link_id, uint16_t eff_tx_time, uint16_t eff_tx_octets, uint16_t eff_rx_time, uint16_t eff_rx_octets);
extern uint8_t (*pf_llc_hci_dl_upd_info_send_patch)(uint8_t link_id, uint8_t status, uint16_t max_tx_time, uint16_t max_tx_octets, uint16_t max_rx_time, uint16_t max_rx_octets);
extern uint8_t (*pf_llc_dle_proc_err_cb_patch)(uint8_t link_id, uint8_t error_type, void* param);
extern int (*pf_ll_length_rsp_handler_patch)(uint8_t link_id, struct ll_length_rsp *pdu, uint16_t event_cnt);
extern int (*pf_llc_op_dl_upd_ind_handler_patch)(ke_msg_id_t const msgid, struct llc_op_dl_upd_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_llc_loc_encrypt_proc_continue_patch)(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t (*pf_llc_rem_encrypt_proc_continue_patch)(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t (*pf_llc_aes_res_cb_patch)(uint8_t status, const uint8_t* aes_res, uint32_t link_id);
extern uint8_t (*pf_llc_ll_pause_enc_req_pdu_send_patch)(uint8_t link_id);
extern uint8_t (*pf_llc_ll_pause_enc_rsp_pdu_send_patch)(uint8_t link_id, llc_llcp_tx_cfm_cb cfm_cb);
extern uint8_t (*pf_llc_ll_enc_req_pdu_send_patch)(uint8_t link_id, uint16_t ediv, uint8_t* rand, uint8_t* skdm, uint8_t* ivm);
extern uint8_t (*pf_llc_ll_enc_rsp_pdu_send_patch)(uint8_t link_id, uint8_t* skds, uint8_t* ivs);
extern uint8_t (*pf_llc_ll_start_enc_req_pdu_send_patch)(uint8_t link_id);
extern uint8_t (*pf_llc_ll_start_enc_rsp_pdu_send_patch)(uint8_t link_id, llc_llcp_tx_cfm_cb cfm_cb);
extern uint8_t (*pf_llc_ll_pause_enc_rsp_ack_handler_patch)(uint8_t link_id, uint8_t op_code);
extern uint8_t (*pf_llc_ll_start_enc_rsp_ack_handler_patch)(uint8_t link_id, uint8_t op_code);
extern uint8_t (*pf_llc_ll_reject_ind_ack_handler_patch)(uint8_t link_id, uint8_t op_code);
extern uint8_t (*pf_llc_hci_ltk_request_evt_send_patch)(uint8_t link_id, uint16_t enc_div, uint8_t* randnb);
extern uint8_t (*pf_llc_hci_enc_evt_send_patch)(uint8_t link_id, uint8_t status, bool re_encrypt);
extern uint8_t (*pf_llc_loc_encrypt_proc_err_cb_patch)(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t (*pf_llc_rem_encrypt_proc_err_cb_patch)(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t (*pf_ll_pause_enc_req_handler_patch)(uint8_t link_id, struct ll_pause_enc_req *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_pause_enc_rsp_handler_patch)(uint8_t link_id, struct ll_pause_enc_rsp *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_enc_req_handler_patch)(uint8_t link_id, struct ll_enc_req *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_enc_rsp_handler_patch)(uint8_t link_id, struct ll_enc_rsp *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_start_enc_req_handler_patch)(uint8_t link_id, struct ll_start_enc_req *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_start_enc_rsp_handler_patch)(uint8_t link_id, struct ll_start_enc_rsp *pdu, uint16_t event_cnt);
extern int (*pf_hci_le_start_enc_cmd_handler_patch)(uint8_t link_id, struct hci_le_start_enc_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_ltk_req_reply_cmd_handler_patch)(uint8_t link_id, struct hci_le_ltk_req_reply_cmd const *cmd, uint16_t opcode);
extern int (*pf_hci_le_ltk_req_neg_reply_cmd_handler_patch)(uint8_t link_id, struct hci_le_ltk_req_neg_reply_cmd const *param, uint16_t opcode);
extern int (*pf_llc_encrypt_ind_handler_patch)(ke_msg_id_t const msgid, struct llc_encrypt_ind const *ind,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_llc_op_encrypt_ind_handler_patch)(ke_msg_id_t const msgid, struct llc_op_encrypt_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_llc_loc_feats_exch_proc_continue_patch)(uint8_t link_id, uint8_t status);
extern uint8_t (*pf_llc_ll_feature_req_pdu_send_patch)(uint8_t link_id);
extern uint8_t (*pf_llc_ll_feature_rsp_pdu_send_patch)(uint8_t link_id);
extern uint8_t (*pf_llc_hci_feats_info_send_patch)(uint8_t link_id, uint8_t status, struct features * feats);
extern uint8_t (*pf_llc_feats_exch_proc_err_cb_patch)(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t (*pf_ll_feature_req_handler_patch)(uint8_t link_id, struct ll_feature_req *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_slave_feature_req_handler_patch)(uint8_t link_id, struct ll_slave_feature_req *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_feature_rsp_handler_patch)(uint8_t link_id, struct ll_feature_rsp *pdu, uint16_t event_cnt);
extern int (*pf_hci_le_rd_rem_feats_cmd_handler_patch)(uint8_t link_id, struct hci_le_rd_rem_feats_cmd const *param, uint16_t opcode);
extern int (*pf_llc_op_feats_exch_ind_handler_patch)(ke_msg_id_t const msgid, struct llc_op_feats_exch_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_hci_acl_data_handler_patch)(ke_msg_id_t const msgid, struct hci_acl_data *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_acl_rx_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_acl_rx_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_llc_hci_nb_cmp_pkts_evt_send_patch)(uint8_t link_id, uint8_t nb_of_pkt);
extern int (*pf_hci_rd_tx_pwr_lvl_cmd_handler_patch)(uint8_t link_id, struct hci_rd_tx_pwr_lvl_cmd const *param, uint16_t opcode);
extern int (*pf_hci_rd_rssi_cmd_handler_patch)(uint8_t link_id, struct hci_basic_conhdl_cmd const *param, uint16_t opcode);
extern int (*pf_hci_vs_set_pref_slave_latency_cmd_handler_patch)(uint8_t link_id, struct hci_vs_set_pref_slave_latency_cmd const *param, uint16_t opcode);
extern int (*pf_hci_vs_set_pref_slave_evt_dur_cmd_handler_patch)(uint8_t link_id, struct hci_vs_set_pref_slave_evt_dur_cmd const *param, uint16_t opcode);
extern int (*pf_hci_command_handler_patch)(ke_msg_id_t const msgid, uint16_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_llc_loc_phy_upd_proc_continue_patch)(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t (*pf_llc_rem_phy_upd_proc_continue_patch)(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t (*pf_llc_dl_chg_check_patch)(uint8_t link_id, uint8_t old_tx_phy, uint8_t old_rx_phy);
extern uint8_t (*pf_llc_loc_phy_upd_proc_err_cb_patch)(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t (*pf_llc_rem_phy_upd_proc_err_cb_patch)(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t (*pf_ll_phy_req_handler_patch)(uint8_t link_id, struct ll_phy_req *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_phy_rsp_handler_patch)(uint8_t link_id, struct ll_phy_rsp *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_phy_update_ind_handler_patch)(uint8_t link_id, struct ll_phy_update_ind *pdu, uint16_t event_cnt);
extern int (*pf_hci_le_rd_phy_cmd_handler_patch)(uint8_t link_id, struct hci_le_rd_phy_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_phy_cmd_handler_patch)(uint8_t link_id, struct hci_le_set_phy_cmd const *param, uint16_t opcode);
extern int (*pf_llc_op_phy_upd_ind_handler_patch)(ke_msg_id_t const msgid, struct llc_op_phy_upd_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_phy_upd_cfm_handler_patch)(ke_msg_id_t const msgid, void *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_phy_upd_proc_start_patch)(uint8_t link_id);
extern uint8_t (*pf_llc_ver_exch_loc_proc_continue_patch)(uint8_t link_id, uint8_t status);
extern int (*pf_llc_op_ver_exch_ind_handler_patch)(ke_msg_id_t const msgid, struct llc_op_ver_exch_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_ll_version_ind_handler_patch)(uint8_t link_id, struct ll_version_ind *pdu, uint16_t event_cnt);
extern int (*pf_hci_rd_rem_ver_info_cmd_handler_patch)(uint8_t link_id, struct hci_rd_rem_ver_info_cmd const *param, uint16_t opcode);
#endif //#if APP_PATCH_LLC

#if (APP_PATCH_LLM)
extern uint8_t (*pf_llm_adv_set_release_patch)(uint8_t act_id);
extern uint8_t (*pf_llm_adv_rpa_gen_cb_patch)(uint8_t status, const uint8_t* aes_res, uint32_t src_info);
extern uint8_t (*pf_llm_adv_con_len_check_patch)(struct hci_le_set_ext_adv_param_cmd* ext_param, uint16_t adv_data_len);
extern uint8_t (*pf_llm_adv_set_dft_params_patch)(uint8_t act_id);
extern uint8_t (*pf_llm_adv_hdl_to_id_patch)(uint16_t adv_hdl, struct hci_le_set_ext_adv_param_cmd** adv_param);
extern int (*pf_hci_le_set_adv_param_cmd_handler_patch)(struct hci_le_set_adv_param_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_adv_data_cmd_handler_patch)(struct hci_le_set_adv_data_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_scan_rsp_data_cmd_handler_patch)(struct hci_le_set_scan_rsp_data_cmd const *param, uint16_t opcode) ;
extern int (*pf_hci_le_set_adv_en_cmd_handler_patch)(struct hci_le_set_adv_en_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_ext_adv_param_cmd_handler_patch)(struct hci_le_set_ext_adv_param_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_adv_set_rand_addr_cmd_handler_patch)(struct hci_le_set_adv_set_rand_addr_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_ext_adv_data_cmd_handler_patch)(struct hci_le_set_ext_adv_data_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_ext_scan_rsp_data_cmd_handler_patch)(struct hci_le_set_ext_scan_rsp_data_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_ext_adv_en_cmd_handler_patch)(struct hci_le_set_ext_adv_en_cmd *param, uint16_t opcode);
extern int (*pf_hci_le_set_per_adv_param_cmd_handler_patch)(struct hci_le_set_per_adv_param_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_per_adv_data_cmd_handler_patch)(struct hci_le_set_per_adv_data_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_per_adv_en_cmd_handler_patch)(struct hci_le_set_per_adv_en_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_max_adv_data_len_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_nb_supp_adv_sets_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_rmv_adv_set_cmd_handler_patch)(struct hci_le_rem_adv_set_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_clear_adv_sets_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_set_conless_cte_tx_param_cmd_handler_patch)(struct hci_le_set_conless_cte_tx_param_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_conless_cte_tx_en_cmd_handler_patch)(struct hci_le_set_conless_cte_tx_en_cmd const *param, uint16_t opcode);
extern uint8_t (*pf_llm_adv_act_id_get_patch)(uint8_t adv_hdl, uint8_t* ext_adv_id, uint8_t* per_adv_id);
extern uint8_t (*pf_llm_adv_set_id_get_patch)(uint8_t act_id, uint8_t* sid, uint8_t* atype, struct bd_addr* adva);
extern int (*pf_hci_le_rd_adv_ch_tx_pw_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_lld_scan_req_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_scan_req_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_adv_end_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_adv_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_per_adv_end_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_per_adv_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern bool (*pf_llm_is_dev_connected_patch)(struct bd_addr const *peer_addr, uint8_t peer_addr_type);
extern bool (*pf_llm_is_dev_synced_patch)(struct bd_addr const *peer_addr, uint8_t peer_addr_type, uint8_t adv_sid);
extern uint8_t (*pf_llm_link_disc_patch)(uint8_t link_id);
extern uint8_t (*pf_llm_ch_map_update_patch)(void);
extern uint8_t (*pf_llm_dev_list_empty_entry_patch)(void);
extern uint8_t (*pf_llm_dev_list_search_patch)(const struct bd_addr *bd_addr, uint8_t bd_addr_type);
extern uint8_t (*pf_llm_ral_search_patch)(const struct bd_addr *bd_addr, uint8_t bd_addr_type);
extern bool (*pf_llm_ral_is_empty_patch)(void);
extern uint8_t (*pf_llm_init_patch)(uint8_t init_type);
extern uint8_t (*pf_llm_clk_acc_set_patch)(uint8_t act_id, bool clk_acc);
extern uint8_t (*pf_llm_activity_free_get_patch)(uint8_t* act_id);
extern uint8_t (*pf_llm_activity_free_set_patch)(uint8_t act_id);
extern bool (*pf_llm_is_wl_accessible_patch)(void);
extern bool (*pf_llm_is_non_con_act_ongoing_check_patch)(void);
extern bool (*pf_llm_is_rand_addr_in_use_check_patch)(void);
extern int (*pf_hci_reset_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_rd_local_ver_info_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_rd_bd_addr_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_rd_local_supp_cmds_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_rd_local_supp_feats_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_set_evt_mask_cmd_handler_patch)(struct hci_set_evt_mask_cmd const *param, uint16_t opcode);
extern int (*pf_hci_set_evt_mask_page_2_cmd_handler_patch)(struct hci_set_evt_mask_cmd const *param, uint16_t opcode);
extern int (*pf_hci_set_ctrl_to_host_flow_ctrl_cmd_handler_patch)(struct hci_set_ctrl_to_host_flow_ctrl_cmd const *param, uint16_t opcode);
extern int (*pf_hci_host_buf_size_cmd_handler_patch)(struct hci_host_buf_size_cmd const *param, uint16_t opcode);
extern int (*pf_hci_host_nb_cmp_pkts_cmd_handler_patch)(struct hci_host_nb_cmp_pkts_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_evt_mask_cmd_handler_patch)(struct hci_le_set_evt_mask_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_buff_size_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_local_supp_feats_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_set_rand_addr_cmd_handler_patch)(struct hci_le_set_rand_addr_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_host_ch_class_cmd_handler_patch)(struct hci_le_set_host_ch_class_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_wl_size_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_clear_wlst_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_add_dev_to_wlst_cmd_handler_patch)(struct hci_le_add_dev_to_wlst_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rmv_dev_from_wlst_cmd_handler_patch)(struct hci_le_rmv_dev_from_wlst_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_rslv_list_size_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_clear_rslv_list_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_add_dev_to_rslv_list_cmd_handler_patch)(struct hci_le_add_dev_to_rslv_list_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rmv_dev_from_rslv_list_cmd_handler_patch)(struct hci_le_rmv_dev_from_rslv_list_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_rslv_priv_addr_to_cmd_handler_patch)(struct hci_le_set_rslv_priv_addr_to_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_addr_resol_en_cmd_handler_patch)(struct hci_le_set_addr_resol_en_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_peer_rslv_addr_cmd_handler_patch)(struct hci_le_rd_peer_rslv_addr_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_loc_rslv_addr_cmd_handler_patch)(struct hci_le_rd_loc_rslv_addr_cmd const *param, uint16_t opcode);
extern uint8_t (*pf_llm_aes_res_cb_patch)(uint8_t status, const uint8_t* aes_res, uint32_t dummy);
extern int (*pf_hci_le_enc_cmd_handler_patch)(struct hci_le_enc_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rand_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_supp_states_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_tx_pwr_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_rf_path_comp_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_wr_rf_path_comp_cmd_handler_patch)(struct hci_le_wr_rf_path_comp_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_priv_mode_cmd_handler_patch)(struct hci_le_set_priv_mode_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_mod_sleep_clk_acc_cmd_handler_patch)(struct hci_le_mod_sleep_clk_acc_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_antenna_inf_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_suggted_dft_data_len_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_wr_suggted_dft_data_len_cmd_handler_patch)(struct hci_le_wr_suggted_dft_data_len_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_max_data_len_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_local_p256_public_key_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_gen_dhkey_v1_cmd_handler_patch)(struct hci_le_gen_dhkey_v1_cmd *param, uint16_t opcode);
extern int (*pf_hci_le_gen_dhkey_v2_cmd_handler_patch)(struct hci_le_gen_dhkey_v2_cmd *param, uint16_t opcode);
extern int (*pf_hci_le_set_dft_phy_cmd_handler_patch)(struct hci_le_set_dft_phy_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_dft_per_adv_sync_transf_param_cmd_handler_patch)(struct hci_le_set_dft_per_adv_sync_transf_param_cmd const *param, uint16_t opcode);
extern int (*pf_llm_hci_command_handler_patch)(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_hci_le_create_con_cmd_handler_patch)(struct hci_le_create_con_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_ext_create_con_cmd_handler_patch)(struct hci_le_ext_create_con_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_create_con_cancel_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_lld_init_end_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_init_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_llm_scan_start_patch)(uint8_t act_id);
extern bool (*pf_llm_adv_reports_list_check_patch)(struct bd_addr const *adv_bd_addr, uint8_t addr_type, uint8_t adv_evt_type, uint8_t sid, uint16_t did, uint16_t per_adv_intv);
extern uint8_t (*pf_llm_scan_sync_acad_attach_patch)(uint8_t sync_act_id, uint8_t ad_type, uint16_t task);
extern uint8_t (*pf_llm_scan_sync_info_get_patch)(uint8_t sync_act_id, uint8_t* sid, uint8_t* atype, struct bd_addr* adva);
extern uint8_t (*pf_llm_per_adv_sync_patch)(struct llm_per_adv_sync_params * params);
extern int (*pf_hci_le_set_scan_param_cmd_handler_patch)(struct hci_le_set_scan_param_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_scan_en_cmd_handler_patch)(struct hci_le_set_scan_en_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_ext_scan_param_cmd_handler_patch)(struct hci_le_set_ext_scan_param_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_ext_scan_en_cmd_handler_patch)(struct hci_le_set_ext_scan_en_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_per_adv_create_sync_cmd_handler_patch)(struct hci_le_per_adv_create_sync_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_per_adv_create_sync_cancel_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_per_adv_term_sync_cmd_handler_patch)(struct hci_le_per_adv_term_sync_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_add_dev_to_per_adv_list_cmd_handler_patch)(struct hci_le_add_dev_to_per_adv_list_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_rmv_dev_from_per_adv_list_cmd_handler_patch)(struct hci_le_rmv_dev_from_per_adv_list_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_clear_per_adv_list_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_rd_per_adv_list_size_cmd_handler_patch)(void const *param, uint16_t opcode);
extern int (*pf_hci_le_set_conless_iq_sampl_en_cmd_handler_patch)(struct hci_le_set_conless_iq_sampl_en_cmd const *param, uint16_t opcode);
extern int (*pf_hci_le_set_per_adv_rec_en_cmd_handler_patch)(struct hci_le_set_per_adv_rec_en_cmd const *param, uint16_t opcode);
extern int (*pf_llm_scan_period_to_handler_patch)(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_adv_rep_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_adv_rep_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_sync_start_req_handler_patch)(ke_msg_id_t const msgid, struct lld_sync_start_req const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_per_adv_rep_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_per_adv_rep_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_per_adv_rx_end_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_per_adv_rx_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_scan_end_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_scan_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_conless_cte_rx_ind_handler_patch)(ke_msg_id_t const msgid, struct lld_conless_cte_rx_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_llm_rpa_renew_to_handler_patch)(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_llm_pub_key_gen_ind_handler_patch)(ke_msg_id_t const msgid, struct ecc_result_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_llm_dh_key_gen_ind_handler_patch)(ke_msg_id_t const msgid, struct ecc_result_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_llm_encrypt_ind_handler_patch)(ke_msg_id_t const msgid, struct llm_encrypt_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
#endif//APP_PATCH_LLM

#if APP_PATCH_LLC_CHMAP
extern uint8_t (*pf_llc_loc_ch_map_proc_continue_patch)(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t (*pf_llc_rem_ch_map_proc_continue_patch)(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t (*pf_llc_ch_map_up_proc_err_cb_patch)(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t (*pf_ll_channel_map_ind_handler_patch)(uint8_t link_id, struct ll_channel_map_ind *pdu, uint16_t event_cnt);
extern uint8_t (*pf_ll_min_used_channels_ind_handler_patch)(uint8_t link_id, struct ll_min_used_channels_ind *pdu, uint16_t event_cnt);
extern int (*pf_hci_le_rd_chnl_map_cmd_handler_patch)(uint8_t link_id, struct hci_basic_conhdl_cmd const *param, uint16_t opcode);
extern int (*pf_llc_op_ch_map_upd_ind_handler_patch)(ke_msg_id_t const msgid, struct llc_op_ch_map_upd_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_llm_ch_map_update_ind_handler_patch)(ke_msg_id_t const msgid, void *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_lld_ch_map_upd_cfm_handler_patch)(ke_msg_id_t const msgid, void *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t (*pf_llc_cleanup_patch)(uint8_t link_id, bool reset);
extern uint8_t (*pf_llc_start_patch)(uint8_t link_id, struct llc_init_parameters *con_params);
extern uint8_t (*pf_llc_llcp_trans_timer_set_patch)(uint8_t link_id, uint8_t proc_type, bool enable);
extern uint8_t (*pf_llc_init_patch)(uint8_t init_type);
extern uint8_t (*pf_llc_stop_patch)(uint8_t link_id);
extern uint8_t (*pf_llc_llcp_state_set_patch)(uint8_t link_id, uint8_t dir, uint8_t state);
extern uint8_t (*pf_llc_proc_reg_patch)(uint8_t link_id, uint8_t proc_type, llc_procedure_t *params);
extern uint8_t (*pf_llc_proc_unreg_patch)(uint8_t link_id, uint8_t proc_type);
extern uint8_t (*pf_llc_proc_id_get_patch)(uint8_t link_id, uint8_t proc_type);
extern uint8_t (*pf_llc_proc_state_set_patch)(llc_procedure_t *proc, uint8_t link_id, uint8_t proc_state);
extern llc_procedure_t * (*pf_llc_proc_get_patch)(uint8_t link_id, uint8_t proc_type);
extern uint8_t (*pf_llc_proc_err_ind_patch)(uint8_t link_id, uint8_t proc_type, uint8_t error_type, void* param);
extern uint8_t (*pf_llc_proc_timer_set_patch)(uint8_t link_id, uint8_t proc_type, bool enable);
extern uint8_t (*pf_llc_proc_timer_pause_set_patch)(uint8_t link_id, uint8_t proc_type, bool enable);
extern uint8_t (*pf_llc_proc_collision_check_patch)(uint8_t link_id, uint8_t proc_id);
#endif

#if APP_PATCH_NVDS
extern bool (*pf_nvds_is_magic_number_ok_patch)(void);
extern uint8_t (*pf_nvds_walk_tag_patch) (uint32_t cur_tag_addr,struct nvds_tag_header *nvds_tag_header_ptr,uint32_t *nxt_tag_addr_ptr);
extern uint8_t (*pf_nvds_browse_tag_patch) (uint8_t tag,struct nvds_tag_header *nvds_tag_header_ptr,uint32_t *tag_address_ptr);
extern uint8_t (*pf_nvds_purge_patch)(uint32_t length, uint8_t* buf);
extern uint8_t (*pf_nvds_init_patch)(uint8_t *base, uint32_t len);
extern uint8_t (*pf_nvds_get_patch)(uint8_t tag, nvds_tag_len_t * lengthPtr, uint8_t *buf);
extern uint8_t (*pf_nvds_lock_patch)(uint8_t tag);
extern uint8_t (*pf_nvds_put_patch)(uint8_t tag, nvds_tag_len_t length, uint8_t *buf);
extern uint8_t (*pf_nvds_del_patch)(uint8_t tag);
extern uint8_t (*pf_nvds_null_init_patch)(void);
extern uint8_t (*pf_nvds_read_patch)(uint32_t address, uint32_t length, uint8_t *buf);
extern uint8_t (*pf_nvds_write_patch)(uint32_t address, uint32_t length, uint8_t *buf);
extern uint8_t (*pf_nvds_erase_patch)(uint32_t address, uint32_t length);
extern uint8_t (*pf_nvds_init_memory_patch)(void);
#endif

#if APP_PATCH_L2CC
extern uint8_t (*pf_l2cc_init_patch)(uint8_t init_type);
extern uint8_t (*pf_l2cc_create_patch)(uint8_t conidx);
extern uint8_t (*pf_l2cc_cleanup_patch)(uint8_t conidx, bool reset);
extern uint8_t (*pf_l2cc_update_state_patch)(uint8_t conidx, ke_state_t state, bool busy);
extern uint8_t (*pf_l2cc_op_complete_patch)(uint8_t conidx, uint8_t op_type, uint8_t status);
extern uint8_t (*pf_l2cc_send_cmp_evt_patch)(uint8_t conidx, uint8_t operation, const ke_task_id_t requester, uint8_t status, uint16_t cid,uint16_t credit);
extern uint8_t (*pf_l2cc_set_operation_ptr_patch)(uint8_t conidx, uint8_t op_type, void* op);
extern uint8_t (*pf_l2cc_data_send_patch)(uint8_t conidx, uint8_t nb_buffer);
extern uint8_t (*pf_l2cc_get_operation_patch)(uint8_t conidx, uint8_t op_type);
extern void* (*pf_l2cc_get_operation_ptr_patch)(uint8_t conidx, uint8_t op_type);
extern uint8_t (*pf_l2cc_lecb_free_patch)(uint8_t conidx, struct l2cc_lecb_info* lecb, bool disconnect_ind);
extern uint8_t (*pf_l2cc_lecb_send_con_req_patch)(uint8_t conidx, uint8_t pkt_id, uint16_t le_psm, uint16_t scid, uint16_t credits,uint16_t mps, uint16_t mtu);
extern uint8_t (*pf_l2cc_lecb_send_con_rsp_patch)(uint8_t conidx, uint16_t status, uint8_t pkt_id,uint16_t dcid, uint16_t credits, uint16_t mps, uint16_t mtu);
extern uint8_t (*pf_l2cc_lecb_send_disc_req_patch)(uint8_t conidx, uint8_t pkt_id, uint16_t scid, uint16_t dcid);
extern uint8_t (*pf_l2cc_lecb_send_disc_rsp_patch)(uint8_t conidx, uint8_t pkt_id, uint16_t dcid, uint16_t scid);
extern uint8_t (*pf_l2cc_lecb_send_credit_add_patch)(uint8_t conidx, uint8_t pkt_id, uint16_t cid, uint16_t credits);
extern uint8_t (*pf_l2cc_lecb_init_disconnect_patch)(uint8_t conidx, struct l2cc_lecb_info* lecb, uint8_t disc_reason);
extern uint16_t (*pf_l2cc_lecb_h2l_err_patch)(uint8_t h_err);
extern struct l2cc_lecb_info* (*pf_l2cc_lecb_find_patch)(uint8_t conidx, uint8_t field, uint16_t value);
extern uint8_t (*pf_l2cc_pdu_pack_patch)(struct l2cc_pdu *p_pdu, uint16_t *p_offset, uint16_t *p_length, uint8_t *p_buffer, uint8_t *pb_flag);
extern uint8_t (*pf_l2cc_pdu_unpack_patch)(uint8_t conidx, struct l2cc_pdu *p_pdu, uint16_t *p_offset, uint16_t *p_rem_len,const uint8_t *p_buffer, uint16_t pkt_length, uint8_t pb_flag);
extern uint8_t (*pf_l2cc_lecb_pdu_pack_patch)(uint8_t conidx, struct l2cc_sdu *sdu, uint16_t *length, uint8_t *buffer, uint16_t* offset,uint16_t pdu_len, uint8_t pb_flag);
extern uint8_t (*pf_l2cc_lecb_pdu_unpack_patch)(uint8_t conidx, struct l2cc_sdu *sdu, uint8_t *buffer, uint16_t length, uint16_t* offset,uint16_t* pdu_remain_len, uint8_t pb_flag);
extern uint8_t (*pf_l2cc_pdu_rx_init_patch)(uint8_t conidx);
extern uint8_t (*pf_l2cc_pdu_header_check_patch)(uint8_t conidx, uint8_t** pp_buffer, uint16_t* p_rx_length);
extern uint8_t (*pf_l2cc_sig_send_cmd_reject_patch)(uint8_t conidx, uint8_t pkt_id, uint16_t reason,uint16_t opt1, uint16_t opt2);
extern uint8_t (*pf_l2cc_sig_lecb_l2h_err_patch)(uint16_t l_err);
extern int (*pf_l2c_code_reject_handler_patch)(uint8_t conidx, struct l2cc_reject *pdu);
extern int (*pf_l2c_code_conn_param_upd_req_handler_patch)(uint8_t conidx, struct l2cc_conn_param_upd_req *pdu);
extern int (*pf_l2c_code_conn_param_upd_resp_handler_patch)(uint8_t conidx, struct l2cc_conn_param_upd_resp *pdu);
extern int (*pf_l2c_code_le_cb_conn_req_handler_patch)(uint8_t conidx, struct l2cc_le_cb_conn_req *pdu);
extern int (*pf_l2c_code_le_cb_conn_resp_handler_patch)(uint8_t conidx, struct l2cc_le_cb_conn_resp *pdu);
extern int (*pf_l2c_code_disconnection_req_handler_patch)(uint8_t conidx, struct l2cc_disconnection_req *pdu);
extern int (*pf_l2c_code_disconnection_resp_handler_patch)(uint8_t conidx, struct l2cc_disconnection_resp *pdu);
extern int (*pf_l2c_code_le_flow_control_credit_handler_patch)(uint8_t conidx, struct l2cc_le_flow_control_credit *pdu);
extern int (*pf_l2cc_sig_pdu_recv_handler_patch)(uint8_t conidx, struct l2cc_pdu *pdu);
extern int (*pf_l2cc_process_op_patch)(uint8_t conidx, uint8_t op_type, void *op_msg, enum l2cc_operation *supp_ops);
extern int (*pf_l2cc_pdu_send_cmd_handler_patch)(ke_msg_id_t const msgid, struct l2cc_pdu_send_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_l2cc_lecb_sdu_send_cmd_handler_patch)(ke_msg_id_t const msgid, struct l2cc_lecb_sdu_send_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_l2cc_dbg_pdu_send_cmd_handler_patch)(ke_msg_id_t const msgid, struct l2cc_dbg_pdu_send_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_hci_nb_cmp_pkts_evt_handler_patch)(ke_msg_id_t const msgid, struct hci_nb_cmp_pkts_evt const *event, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_l2cc_task_hci_acl_data_handler_patch)(ke_msg_id_t const msgid, struct hci_acl_data const *param, ke_task_id_t const _dest_id, ke_task_id_t const src_id);
extern int (*pf_l2cc_lecb_connect_cmd_handler_patch)(ke_msg_id_t const msgid, struct l2cc_lecb_connect_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_l2cc_lecb_connect_cfm_handler_patch)(ke_msg_id_t const msgid, struct l2cc_lecb_connect_cfm *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_l2cc_lecb_disconnect_cmd_handler_patch)(ke_msg_id_t const msgid, struct l2cc_lecb_disconnect_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_l2cc_signaling_trans_to_ind_handler_patch)(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_l2cc_lecb_add_cmd_handler_patch)(ke_msg_id_t const msgid, struct l2cc_lecb_add_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_l2cc_pdu_recv_ind_handler_patch)(ke_msg_id_t const msgid, struct l2cc_pdu_recv_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_l2cc_cmp_evt_handler_patch)(ke_msg_id_t const msgid, void *p_paran, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_l2cc_default_msg_handler_patch)(ke_msg_id_t const msgid, void *p_paran, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern void *(*pf_l2cc_pdu_alloc_patch)(uint8_t conidx, uint16_t cid, uint8_t code, ke_task_id_t src_id, uint16_t length);
extern uint8_t (*pf_l2cm_init_patch)(uint8_t init_type);
extern uint8_t (*pf_l2cm_cleanup_patch)(uint8_t conidx);
extern uint8_t (*pf_l2cm_set_link_layer_buff_size_patch)(uint16_t pkt_len, uint16_t nb_acl);
extern uint8_t (*pf_l2cm_tx_status_patch)(uint8_t conidx, bool busy);
#endif //APP_PATCH_L2CC

#if APP_PATCH_GAP
extern bool (*pf_gapc_smp_check_key_distrib_patch)(uint8_t conidx, uint8_t sec_level);
extern bool (*pf_gapc_smp_check_max_key_size_patch)(uint8_t conidx);
extern bool (*pf_gapc_smp_check_pairing_feat_patch)(struct gapc_pairing *pair_feat);
extern bool (*pf_gapc_smp_is_sec_mode_reached_patch)(uint8_t conidx, uint8_t role);
extern int (*pf_hci_enc_key_refr_evt_handler_patch)(ke_msg_id_t const msgid, struct hci_enc_key_ref_cmp_evt const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapc_bond_cfm_handler_patch)(ke_msg_id_t const msgid, struct gapc_bond_cfm *cfm, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapc_bond_cmd_handler_patch)(ke_msg_id_t const msgid, struct gapc_bond_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapc_encrypt_cfm_handler_patch)(ke_msg_id_t const msgid, struct gapc_encrypt_cfm *cfm, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapc_encrypt_cmd_handler_patch)(ke_msg_id_t const msgid, struct gapc_encrypt_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapc_gen_dh_key_ind_handler_patch)(ke_msg_id_t const msgid, struct gapm_gen_dh_key_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapc_key_press_notification_cmd_handler_patch)(ke_msg_id_t const msgid, struct gapc_key_press_notif_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapc_security_cmd_handler_patch)(ke_msg_id_t const msgid, struct gapc_security_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapc_sign_cmd_handler_patch)(ke_msg_id_t const msgid, struct gapc_sign_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapc_smp_gapm_cmp_evt_handler_patch)(ke_msg_id_t const msgid, struct gapm_cmp_evt *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapc_smp_rep_attempts_timer_handler_patch)(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapc_smp_timeout_timer_ind_handler_patch)(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapm_smpc_pub_key_ind_handler_patch)(ke_msg_id_t const msg_id, struct gapm_pub_key_ind *p_param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_hci_enc_chg_evt_handler_patch)(ke_msg_id_t const msgid, struct hci_enc_change_evt const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_hci_le_ltk_request_evt_handler_patch)(ke_msg_id_t const msgid, struct hci_le_ltk_request_evt const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_hci_le_start_enc_stat_evt_handler_patch)(ke_msg_id_t const msgid, struct hci_cmd_stat_event const *event, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapc_smp_l2cc_cmp_evt_handler_patch)(ke_msg_id_t const msgid, struct l2cc_cmp_evt *evt, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int8_t (*pf_gapc_smp_pairing_irk_exch_patch)(uint8_t conidx, struct gap_sec_key *irk, struct gap_bdaddr *identity);
extern uint8_t (*pf_gapc_smp_aes_res_cb_patch)(uint8_t status, const uint8_t *aes_res, uint32_t aes_info);
extern uint8_t (*pf_gapc_smp_aes_sign_res_cb_patch)(uint8_t status, const uint8_t *aes_res, uint32_t aes_info);
extern uint8_t (*pf_gapc_smp_check_repeated_attempts_patch)(uint8_t conidx);
extern uint8_t (*pf_gapc_smp_cleanup_patch)(uint8_t conidx);
extern uint8_t (*pf_gapc_smp_clear_timeout_timer_patch)(uint8_t conidx);
extern uint8_t (*pf_gapc_smp_comp_cnf_val_patch)(uint8_t conidx, uint8_t role, bool local);
extern uint8_t (*pf_gapc_smp_comp_f4_patch)(uint8_t conidx, const uint8_t *u, const uint8_t *v, const uint8_t *x, uint8_t z, uint8_t state);
extern uint8_t (*pf_gapc_smp_comp_f5_patch)(uint8_t conidx, const uint8_t *w, const uint8_t *n1, const uint8_t *n2, const uint8_t *a1, const uint8_t *a2, uint8_t state);
extern uint8_t (*pf_gapc_smp_comp_f6_patch)(uint8_t conidx, const uint8_t *w, const uint8_t *n1, const uint8_t *n2, const uint8_t *r, const uint8_t *iocap, const uint8_t *a1, const uint8_t *a2, uint8_t state);
extern uint8_t (*pf_gapc_smp_comp_g2_patch)(uint8_t conidx, const uint8_t *u, const uint8_t *v, const uint8_t *x, const uint8_t *y, uint8_t state);
extern uint8_t (*pf_gapc_smp_dhkey_check_send_patch)(uint8_t conidx, const uint8_t *dh_key_chk);
extern uint8_t (*pf_gapc_smp_encrypt_cfm_patch)(uint8_t conidx, bool accept, struct gap_sec_key *ltk, uint8_t key_size);
extern uint8_t (*pf_gapc_smp_encrypt_start_handler_patch)(uint8_t conidx, struct gapc_ltk *ltk);
extern uint8_t (*pf_gapc_smp_encrypt_start_patch)(uint8_t conidx, struct gapc_ltk *ltk);
extern uint8_t (*pf_gapc_smp_generate_rand_patch)(uint8_t conidx, uint8_t state);
extern uint8_t (*pf_gapc_smp_generate_stk_patch)(uint8_t conidx, uint8_t role);
extern uint8_t (*pf_gapc_smp_get_key_sec_prop_patch)(uint8_t conidx);
extern uint8_t (*pf_gapc_smp_get_next_passkey_bit_patch)(struct gapc_smp_pair_info *p_pair);
extern uint8_t (*pf_gapc_smp_handle_dh_key_check_complete_patch)(uint8_t conidx, const uint8_t *dh_key);
extern uint8_t (*pf_gapc_smp_handle_enc_change_evt_patch)(uint8_t conidx, uint8_t role, uint8_t status);
extern uint8_t (*pf_gapc_smp_id_addr_info_send_patch)(uint8_t conidx);
extern uint8_t (*pf_gapc_smp_id_info_send_patch)(uint8_t conidx, const uint8_t *irk);
extern uint8_t (*pf_gapc_smp_init_mac_key_calculation_patch)(uint8_t conidx);
extern uint8_t (*pf_gapc_smp_initiate_dhkey_check_patch)(uint8_t conidx);
extern uint8_t (*pf_gapc_smp_initiate_dhkey_verification_patch)(uint8_t conidx);
extern uint8_t (*pf_gapc_smp_key_press_notification_ind_patch)(uint8_t conidx, uint8_t notification_type);
extern uint8_t (*pf_gapc_smp_launch_rep_att_timer_patch)(uint8_t conidx);
extern uint8_t (*pf_gapc_smp_pair_cfm_send_patch)(uint8_t conidx, const uint8_t *conf_val);
extern uint8_t (*pf_gapc_smp_pair_fail_send_patch)(uint8_t conidx, uint8_t status);
extern uint8_t (*pf_gapc_smp_pair_rand_send_patch)(uint8_t conidx, const uint8_t *rand_nb);
extern uint8_t (*pf_gapc_smp_pairing_csrk_exch_patch)(uint8_t conidx, struct gap_sec_key *csrk);
extern uint8_t (*pf_gapc_smp_pairing_end_patch)(uint8_t conidx, uint8_t role, uint8_t status, bool start_ra_timer);
extern uint8_t (*pf_gapc_smp_pairing_ltk_exch_patch)(uint8_t conidx, struct gapc_ltk *ltk);
extern uint8_t (*pf_gapc_smp_pairing_nc_exch_patch)(uint8_t conidx, uint8_t accept);
extern uint8_t (*pf_gapc_smp_pairing_oob_exch_patch)(uint8_t conidx, bool accept, struct gapc_oob *oob);
extern uint8_t (*pf_gapc_smp_pairing_req_handler_patch)(uint8_t conidx, struct gapc_pairing *feat);
extern uint8_t (*pf_gapc_smp_pairing_rsp_patch)(uint8_t conidx, bool accept, struct gapc_pairing *feat);
extern uint8_t (*pf_gapc_smp_pairing_start_patch)(uint8_t conidx, struct gapc_pairing *pairing);
extern uint8_t (*pf_gapc_smp_pairing_tk_exch_patch)(uint8_t conidx, bool accept, struct gap_sec_key *tk);
extern uint8_t (*pf_gapc_smp_pdu_recv_patch)(uint8_t conidx, struct l2cc_pdu *pdu, uint8_t status);
extern uint8_t (*pf_gapc_smp_pdu_send_patch)(uint8_t conidx, struct l2cc_pdu_data_t *pdu);
extern uint8_t (*pf_gapc_smp_recv_dhkey_check_pdu_patch)(uint8_t conidx, struct l2cc_dhkey_check *pdu);
extern uint8_t (*pf_gapc_smp_recv_enc_info_pdu_patch)(uint8_t conidx, struct l2cc_encryption_inf *pdu);
extern uint8_t (*pf_gapc_smp_recv_id_addr_info_pdu_patch)(uint8_t conidx, struct l2cc_id_addr_inf *pdu);
extern uint8_t (*pf_gapc_smp_recv_id_info_pdu_patch)(uint8_t conidx, struct l2cc_identity_inf *pdu);
extern uint8_t (*pf_gapc_smp_recv_keypress_notification_pdu_patch)(uint8_t conidx, struct l2cc_keypress_notification *pdu);
extern uint8_t (*pf_gapc_smp_recv_mst_id_pdu_patch)(uint8_t conidx, struct l2cc_master_id *pdu);
extern uint8_t (*pf_gapc_smp_recv_pair_cfm_pdu_patch)(uint8_t conidx, struct l2cc_pairing_cfm *pdu);
extern uint8_t (*pf_gapc_smp_recv_pair_fail_pdu_patch)(uint8_t conidx, struct l2cc_pairing_failed *pdu);
extern uint8_t (*pf_gapc_smp_recv_pair_rand_pdu_patch)(uint8_t conidx, struct l2cc_pairing_random *pdu);
extern uint8_t (*pf_gapc_smp_recv_pair_req_pdu_patch)(uint8_t conidx, struct l2cc_pairing_req *pdu);
extern uint8_t (*pf_gapc_smp_recv_pair_rsp_pdu_patch)(uint8_t conidx, struct l2cc_pairing_resp *pdu);
extern uint8_t (*pf_gapc_smp_recv_public_key_pdu_patch)(uint8_t conidx, struct l2cc_public_key *pdu);
extern uint8_t (*pf_gapc_smp_recv_sec_req_pdu_patch)(uint8_t conidx, struct l2cc_security_req *pdu);
extern uint8_t (*pf_gapc_smp_recv_sign_info_pdu_patch)(uint8_t conidx, struct l2cc_signing_inf *pdu);
extern uint8_t (*pf_gapc_smp_security_req_send_patch)(uint8_t conidx, uint8_t auth);
extern uint8_t (*pf_gapc_smp_send_gen_dh_key_cmd_patch)(uint8_t conidx, uint8_t *operand_1, uint8_t *operand_2);
extern uint8_t (*pf_gapc_smp_send_get_pub_key_cmd_patch)(uint8_t conidx);
extern uint8_t (*pf_gapc_smp_send_ltk_req_rsp_patch)(uint8_t conidx, bool found, const uint8_t *key);
extern uint8_t (*pf_gapc_smp_send_pairing_ind_patch)(uint8_t conidx, uint8_t ind_type, void *value);
extern uint8_t (*pf_gapc_smp_send_pairing_req_ind_patch)(uint8_t conidx, uint8_t req_type);
extern uint8_t (*pf_gapc_smp_send_start_enc_cmd_patch)(uint8_t conidx, uint8_t key_type, const uint8_t *key, const uint8_t *randnb, uint16_t ediv);
extern uint8_t (*pf_gapc_smp_sign_command_patch)(uint8_t conidx, struct gapc_sign_cmd *param);
extern uint8_t (*pf_gapc_smp_tkdp_rcp_continue_patch)(uint8_t conidx, uint8_t role);
extern uint8_t (*pf_gapc_smp_tkdp_rcp_start_patch)(uint8_t conidx, uint8_t role);
extern uint8_t (*pf_gapc_smp_tkdp_send_continue_patch)(uint8_t conidx, uint8_t role);
extern uint8_t (*pf_gapc_smp_tkdp_send_start_patch)(uint8_t conidx, uint8_t role);
extern int (*pf_gapm_gen_dh_key_cmd_handler_patch)(ke_msg_id_t const msgid, struct gapm_gen_dh_key_cmd * param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapm_gen_rand_addr_cmd_handler_patch)(ke_msg_id_t const msgid, struct gapm_gen_rand_addr_cmd *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapm_get_pub_key_cmd_handler_patch)(ke_msg_id_t const msgid, struct gapm_get_pub_key_cmd* param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapm_resolv_addr_cmd_handler_patch)(ke_msg_id_t const msgid, struct gapm_resolv_addr_cmd *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_gapm_use_enc_block_cmd_handler_patch)(ke_msg_id_t const msgid, struct gapm_use_enc_block_cmd * param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int (*pf_hci_le_enc_cmd_cmp_evt_handler_patch)(uint16_t opcode, struct hci_le_enc_cmd_cmp_evt *param);
extern int (*pf_hci_le_gen_dhkey_cmp_evt_handler_patch)(uint16_t opcode, struct hci_le_gen_dhkey_cmp_evt const *param);
extern int (*pf_hci_le_gen_dhkey_v2_stat_evt_handler_patch)(uint16_t opcode, struct hci_basic_cmd_cmp_evt const *p_event);
extern uint8_t (*pf_gapm_smp_gen_rand_addr_cb_patch)(uint8_t status, const uint8_t* aes_res, uint32_t src_info);
extern uint8_t (*pf_gapm_smp_gen_rand_nb_cb_patch)(uint8_t status, const uint8_t* aes_res, uint32_t src_info);
extern uint8_t (*pf_gapm_smp_resolv_res_cb_patch)(uint8_t index, uint32_t src_info);
extern uint8_t (*pf_gapm_smp_send_hci_encrypt_patch)(const uint8_t *key, const uint8_t* val);
extern uint8_t (*pf_gapm_smp_use_enc_block_cb_patch)(uint8_t status, const uint8_t* aes_res, uint32_t src_info);
#endif //APP_PATCH_GAP




///////////////////////////////////////////P2:  Externed ROM Functions     ////////////////////////////////////////////////////
#if APP_PATCH_KE
extern bool cmp_dest_id(struct co_list_hdr const * msg, uint32_t dest_id);
extern ke_msg_func_t ke_handler_search(ke_msg_id_t const msg_id, struct ke_msg_handler const *msg_handler_tab, uint16_t msg_cnt);
extern ke_msg_func_t ke_task_handler_get(ke_msg_id_t const msg_id, ke_task_id_t const task_id);
extern void ke_task_saved_update(ke_task_id_t const ke_task_id);
extern void ke_task_schedule(void);
extern bool ke_mem_is_in_heap(uint8_t type, void* mem_ptr);
extern struct co_list_hdr *ke_queue_extract(struct co_list * const queue,bool (*func)(struct co_list_hdr const * elmt, uint32_t arg), uint32_t arg);
extern void ke_queue_insert(struct co_list * const queue, struct co_list_hdr * const element,bool (*cmp)(struct co_list_hdr const *elementA, struct co_list_hdr const *elementB));
extern rwip_time_t ke_time(void);
extern bool cmp_abs_time(struct co_list_hdr const * timerA, struct co_list_hdr const * timerB);
extern bool cmp_timer_id(struct co_list_hdr const * timer, uint32_t timer_task);
extern bool ke_time_past(rwip_time_t time);
extern bool ke_timer_active(ke_msg_id_t const timer_id, ke_task_id_t const task_id);
extern uint8_t ke_timer_adjust_all(uint32_t delay);
extern uint8_t ke_timer_clear(ke_msg_id_t const timer_id, ke_task_id_t const task_id);
extern uint8_t ke_timer_init(void);
extern uint8_t ke_timer_schedule(void);
extern uint8_t ke_timer_set(ke_msg_id_t const timer_id, ke_task_id_t const task_id, uint32_t delay);
rwip_time_t testTimer = {0};
#endif //#if APP_PATCH_KE

#if APP_PATCH_CO
extern bool co_list_extract(struct co_list *list, struct co_list_hdr *list_hdr);
extern bool co_list_find(struct co_list *list, struct co_list_hdr *list_hdr);
extern struct co_list_hdr *co_list_pop_front(struct co_list *list);
extern uint16_t co_list_size(struct co_list *list);
extern void co_list_extract_after(struct co_list *list, struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_rem_hdr);
extern void co_list_extract_sublist(struct co_list *list, struct co_list_hdr *ref_hdr, struct co_list_hdr *last_hdr);
extern void co_list_init(struct co_list *list);
extern void co_list_insert_after(struct co_list *list, struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_add_hdr);
extern void co_list_insert_before(struct co_list *list, struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_add_hdr);
extern void co_list_merge(struct co_list *list1, struct co_list *list2);
extern void co_list_pool_init(struct co_list *list, void *pool, size_t elmt_size, uint32_t elmt_cnt);
extern void co_list_push_back(struct co_list *list, struct co_list_hdr *list_hdr);
extern void co_list_push_back_sublist(struct co_list *list, struct co_list_hdr *first_hdr, struct co_list_hdr *last_hdr);
extern void co_list_push_front(struct co_list *list, struct co_list_hdr *list_hdr);
extern uint16_t co_util_read_array_size(char **fmt_cursor);
extern void co_bytes_to_string(char* dest, uint8_t* src, uint8_t nb_bytes);
extern bool co_bdaddr_compare(struct bd_addr const *bd_address1, struct bd_addr const *bd_address2);
extern uint8_t co_nb_good_le_channels(const struct le_chnl_map* map);
extern uint8_t co_util_pack(uint8_t* out, uint8_t* in, uint16_t* out_len, uint16_t in_len, const char* format);
extern uint8_t co_util_unpack(uint8_t* out, uint8_t* in, uint16_t* out_len, uint16_t in_len, const char* format);
extern uint16_t co_ble_pkt_dur_in_us(uint8_t len, uint8_t rate);
#endif

#if APP_PATCH_AES
extern struct aes_func_env* aes_alloc(uint16_t size, aes_func_continue_cb aes_continue_cb, aes_func_result_cb res_cb, uint32_t src_info);
extern void aes_encrypt(const uint8_t* key, const uint8_t *val, bool copy, aes_func_result_cb res_cb, uint32_t src_info);
extern void aes_init(uint8_t init_type);
extern void aes_start(struct aes_func_env* env, const uint8_t* key, const uint8_t *val);
extern void aes_app_encrypt(uint8_t* key, uint8_t *val, aes_func_result_cb res_cb, uint32_t src_info);
extern void aes_app_decrypt(uint8_t* key, uint8_t *val, aes_func_result_cb res_cb, uint32_t src_info);
extern void aes_rand(aes_func_result_cb res_cb, uint32_t src_info);
extern void aes_result_handler(uint8_t status, uint8_t* result);
extern void aes_xor_128(uint8_t* result, const uint8_t* a, const uint8_t* b, uint8_t size);
extern void aes_shift_left_128(const uint8_t* input,uint8_t* output);
extern void aes_cmac(const uint8_t *key, const uint8_t *message, uint16_t message_len, aes_func_result_cb res_cb, uint32_t src_info);
extern void aes_cmac_start(struct aes_cmac_env *env, const uint8_t *key, const uint8_t *message, uint16_t message_len);
extern bool aes_cmac_continue(struct aes_cmac_env *env, uint8_t *aes_res);
#endif //APP_PATCH_AES

#if APP_PATCH_SCH
extern uint8_t sch_arb_conflict_check(struct sch_arb_elt_tag * evt_a, struct sch_arb_elt_tag * evt_b);
extern uint8_t sch_arb_insert(struct sch_arb_elt_tag *elt);
extern uint8_t sch_arb_remove(struct sch_arb_elt_tag *elt, bool not_waiting);
extern uint8_t sch_arb_elt_cancel(struct sch_arb_elt_tag *new_elt);
extern void sch_arb_event_start_isr(void);
extern void sch_arb_init(uint8_t init_type);
extern uint8_t sch_arb_prog_timer(void);
extern void sch_arb_sw_isr(void);
extern uint8_t sch_alarm_prog(void);
extern void sch_alarm_init(uint8_t init_type);
extern void sch_alarm_timer_isr(void);
extern void sch_alarm_set(struct sch_alarm_tag* elt);
extern uint8_t sch_alarm_clear(struct sch_alarm_tag* elt);
extern void sch_prog_rx_isr(uint8_t et_idx);
extern uint8_t sch_prog_tx_isr(uint8_t et_idx);
extern uint8_t sch_prog_skip_isr(uint8_t et_idx);
extern uint8_t sch_prog_end_isr(uint8_t et_idx);
extern void sch_prog_init(uint8_t init_type);
extern void sch_prog_fifo_isr(void);
extern void sch_prog_push(struct sch_prog_params* params);
extern uint8_t sch_slice_compute(void);
extern void sch_slice_init(uint8_t init_type);
extern void sch_slice_bg_add(uint8_t type);
extern void sch_slice_bg_remove(uint8_t type);
extern void sch_slice_fg_add(uint8_t type, uint32_t end_ts);
extern void sch_slice_fg_remove(uint8_t type);
extern void sch_slice_per_add(uint8_t type, uint8_t id, uint32_t intv, uint16_t duration, bool retx);
extern void sch_slice_per_remove(uint8_t type, uint8_t id);
#endif

#if (APP_PATCH_LLD)
extern bool lld_calc_aux_rx(struct lld_calc_aux_rx_out* aux_rx_out, uint8_t index_pkt, uint32_t aux_data);
extern bool lld_rxdesc_check(uint8_t label);
extern uint32_t lld_read_clock(void);
extern uint8_t lld_ch_idx_get(void);
extern uint8_t lld_local_sca_get(void);
extern void lld_channel_assess(uint8_t channel, bool rx_ok, uint32_t timestamp);
extern uint8_t lld_rpa_renew_evt_canceled_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_rpa_renew_evt_start_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_rpa_renew_instant_cbk(struct sch_alarm_tag* elt);
extern void lld_rxdesc_free(void);
extern uint32_t lld_adv_ext_pkt_prepare(uint8_t act_id, uint8_t txdesc_idx,
                                        uint8_t type, uint8_t mode, bool adva, bool targeta, bool adi,bool aux_ptr, bool sync_info, bool tx_power,
                                        uint16_t* ext_header_txbuf_offset, uint16_t* data_txbuf_offset, uint16_t* data_len);
extern uint8_t lld_adv_end(uint8_t act_id, bool indicate, uint8_t status);
extern uint8_t lld_adv_sync_info_set(uint8_t act_id);
extern uint8_t lld_adv_aux_ch_idx_set(uint8_t act_id);
extern uint8_t lld_adv_ext_chain_construct(uint8_t act_id);
extern uint8_t lld_adv_adv_data_set(uint8_t act_id, uint8_t len, uint16_t data, bool release_old_buf);
extern uint8_t lld_adv_scan_rsp_data_set(uint8_t act_id, uint8_t len, uint16_t data, bool release_old_buf);
extern uint8_t lld_adv_evt_start_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_adv_aux_evt_start_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_adv_evt_canceled_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_adv_aux_evt_canceled_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_adv_frm_isr(uint8_t act_id, uint32_t timestamp, bool abort);
extern uint8_t lld_adv_frm_skip_isr(uint8_t act_id, uint32_t timestamp);
extern uint8_t lld_adv_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern bool lld_scan_sync_accept(uint8_t scan_id, uint8_t index_pkt, uint8_t addr_type, struct bd_addr rx_adva, uint8_t rx_adv_sid);
extern bool lld_scan_sync_info_unpack(struct sync_info* p_syncinfo, uint16_t em_addr);
extern uint8_t lld_scan_restart(void);
extern uint8_t lld_scan_aux_evt_canceled_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_scan_aux_evt_start_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_scan_aux_sched(uint8_t scan_id);
extern uint8_t lld_scan_end(void);
extern uint8_t lld_scan_evt_canceled_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_scan_evt_start_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_scan_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern uint8_t lld_scan_frm_eof_isr(uint8_t scan_id, uint32_t timestamp, bool abort);
extern uint8_t lld_scan_frm_rx_isr(uint8_t scan_id);
extern uint8_t lld_scan_frm_skip_isr(uint8_t scan_id);
extern uint8_t lld_scan_process_pkt_rx(uint8_t scan_id);
extern uint8_t lld_scan_sched(uint8_t phy_idx, uint32_t timestamp, bool resched);
extern uint8_t lld_scan_trunc_ind(struct lld_scan_evt_params* scan_evt);
extern uint16_t lld_init_compute_winoffset(uint16_t con_intv, uint16_t con_offset, uint8_t txwin_delay, uint32_t timestamp);
extern uint8_t lld_init_connect_req_pack(uint8_t* p_data_pack, struct pdu_con_req_lldata* p_data);
extern uint8_t lld_init_end(void);
extern uint8_t lld_init_evt_canceled_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_init_evt_start_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_init_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern uint8_t lld_init_frm_eof_isr(uint8_t env_idx, uint32_t timestamp, bool abort);
extern uint8_t lld_init_frm_skip_isr(uint8_t env_idx);
extern uint8_t lld_init_process_pkt_rx(uint8_t env_idx);
extern uint8_t lld_init_process_pkt_tx(uint8_t env_idx);
extern uint8_t lld_init_sched(uint8_t env_idx, uint32_t timestamp, bool resched);
extern void lld_con_max_lat_calc(uint8_t link_id);
extern void lld_con_tx_len_update(uint8_t link_id, uint8_t tx_rate, uint32_t con_intv);
extern void lld_con_evt_time_update(uint8_t link_id);
extern void lld_con_cleanup(uint8_t link_id, bool indicate, uint8_t reason);
extern void lld_instant_proc_end(uint8_t link_id);
extern void lld_con_sched(uint8_t link_id, uint32_t clock, bool sync);
extern void lld_con_rx(uint8_t link_id, uint32_t timestamp);
extern void lld_con_tx(uint8_t link_id);
extern void lld_con_tx_prog(uint8_t link_id);
extern void lld_con_evt_start_cbk(struct sch_arb_elt_tag* evt);
extern void lld_con_evt_canceled_cbk(struct sch_arb_elt_tag* evt);
extern void lld_con_frm_isr(uint8_t link_id, uint32_t timestamp, bool abort);
extern void lld_con_rx_isr(uint8_t link_id, uint32_t timestamp);
extern void lld_con_tx_isr(uint8_t link_id, uint32_t timestamp);
extern void lld_con_frm_skip_isr(uint8_t act_id);
extern void lld_con_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern uint8_t lld_sync_cleanup(uint8_t act_id, uint8_t status);
extern uint8_t lld_sync_trunc_ind(uint8_t act_id);
extern uint8_t lld_sync_process_pkt_rx(uint8_t act_id);
extern uint8_t lld_sync_sched(uint8_t act_id, bool resched);
extern uint8_t lld_sync_frm_eof_isr(uint8_t act_id, uint32_t timestamp, bool abort);
extern uint8_t lld_sync_frm_rx_isr(uint8_t act_id);
extern uint8_t lld_sync_frm_skip_isr(uint8_t act_id);
extern uint8_t lld_sync_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern uint8_t lld_sync_evt_start_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_sync_evt_canceled_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_sync_ant_switch_config(uint8_t act_id);
extern uint8_t lld_per_adv_cleanup(uint8_t act_id);
extern uint8_t lld_per_adv_sched(uint8_t act_id);
extern uint8_t lld_per_adv_chain_construct(uint8_t act_id);
extern uint8_t lld_per_adv_data_set(uint8_t act_id, uint8_t len, uint16_t data, bool release_old_buf);
extern uint8_t lld_per_adv_evt_start_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_per_adv_evt_canceled_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_per_adv_frm_isr(uint8_t act_id, uint32_t timestamp, bool abort);
extern uint8_t lld_per_adv_frm_skip_isr(uint8_t act_id);
extern uint8_t lld_per_adv_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern uint8_t lld_per_adv_ant_switch_config(uint8_t act_id);
extern uint8_t lld_per_adv_sync_info_get(uint8_t act_id, uint32_t* sync_ind_ts, uint16_t* pa_evt_cnt, struct le_chnl_map *map);
extern uint8_t lld_per_adv_init_info_get(uint8_t act_id, struct access_addr* aa, struct crc_init* crcinit, struct le_chnl_map* chm);
extern uint32_t lld_per_adv_ext_pkt_prepare(uint8_t act_id, uint8_t txdesc_idx,
                                            uint8_t type, uint8_t mode, bool tx_power,
                                            uint16_t* ext_header_txbuf_offset, uint16_t* data_txbuf_offset,
                                            uint16_t* data_len, bool force_aux_ptr);
extern uint8_t lld_test_cleanup(void);
extern uint8_t lld_test_evt_canceled_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_test_evt_start_cbk(struct sch_arb_elt_tag* evt);
extern uint8_t lld_test_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type);
extern uint8_t lld_test_frm_isr(uint32_t timestamp, bool abort);
extern uint8_t lld_test_rx_isr(uint32_t timestamp);
extern bool lld_adv_pkt_rx(uint8_t act_id);
extern uint8_t lld_test_freq2chnl(uint8_t freq);
struct bd_addr testAddr = {0};
#endif

#if (APP_PATCH_RPA)
extern uint8_t aes_rpa_gen_continue(struct aes_rpa_gen_env *env, uint8_t *aes_res);
extern uint8_t aes_rpa_resolve_continue(struct aes_rpa_resolve_env *env, uint8_t *aes_res);
extern void aes_rpa_gen(struct irk *irk, aes_func_result_cb res_cb, uint32_t src_info);
extern void aes_rpa_resolve(uint8_t nb_irk, struct irk *irk, struct bd_addr *addr, aes_rpa_func_result_cb res_cb, uint32_t src_info);
#endif

#if (APP_PATCH_SCH_PLAN)
extern void sch_plan_init(uint8_t init_type);
extern uint32_t sch_plan_offset_max_calc(uint32_t interval, uint32_t duration, uint32_t offset_min, uint16_t conhdl);
extern uint8_t sch_plan_offset_req(uint8_t action, bool dur_max, struct sch_plan_req_param *req_param, struct sch_plan_chk_param *chk_param);
extern uint8_t sch_plan_interval_req(struct sch_plan_req_param *req_param);
extern uint8_t sch_plan_position_range_compute(uint8_t link_id, uint8_t role, uint16_t interval, uint16_t duration, uint16_t *min_offset, uint16_t *max_offset);
extern uint8_t sch_plan_clock_wrap_offset_update(void);
extern void sch_plan_set(struct sch_plan_elt_tag *elt_to_add);
extern void sch_plan_rem(struct sch_plan_elt_tag *elt_to_remove);
extern uint8_t sch_plan_req(struct sch_plan_req_param* req_param);
extern uint8_t sch_plan_chk(struct sch_plan_chk_param* chk_param);
#endif

#if (APP_PATCH_HCI)
extern uint8_t hci_send_2_host(void *param);
extern uint8_t hci_send_2_controller(void *param);
extern bool hci_evt_mask_check(struct ke_msg *msg);
extern uint8_t hci_init(uint8_t init_type);
extern uint8_t hci_ble_conhdl_register(uint8_t link_id);
extern uint8_t hci_ble_conhdl_unregister(uint8_t link_id);
extern uint8_t hci_evt_mask_set(struct evt_mask const *evt_msk, uint8_t page);
extern uint8_t hci_evt_filter_add(struct hci_set_evt_filter_cmd const *param);
extern struct hci_cmd_desc_tag* hci_look_for_cmd_desc(uint16_t opcode);
extern struct hci_evt_desc_tag* hci_look_for_evt_desc(uint8_t code);
extern struct hci_evt_desc_tag* hci_look_for_le_evt_desc(uint8_t subcode);
extern uint8_t hci_pack_bytes(uint8_t** pp_in, uint8_t** pp_out, uint8_t* p_in_end, uint8_t* p_out_end, uint8_t len);
extern uint8_t hci_host_nb_cmp_pkts_cmd_pkupk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t hci_le_set_ext_scan_param_cmd_upk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t hci_le_ext_create_con_cmd_upk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t hci_le_set_ext_adv_en_cmd_upk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t hci_le_adv_report_evt_pkupk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t hci_le_dir_adv_report_evt_pkupk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t hci_le_ext_adv_report_evt_pkupk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t hci_le_conless_iq_report_evt_pkupk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern uint8_t hci_le_con_iq_report_evt_pkupk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
extern bool hci_ble_adv_report_filter_check(struct ke_msg * msg);
extern uint8_t hci_ble_adv_report_tx_check(struct ke_msg * msg);
extern uint8_t hci_cmd_reject(const struct hci_cmd_desc_tag* cmd_desc, uint16_t opcode, uint8_t error, uint8_t * payload);
extern uint8_t hci_cmd_received(uint16_t opcode, uint8_t length, uint8_t *payload);
extern uint8_t hci_acl_tx_data_received(uint16_t hdl_flags, uint16_t datalen, uint8_t * payload);
extern uint8_t* hci_build_cs_evt(struct ke_msg * msg);
extern uint8_t* hci_build_cc_evt(struct ke_msg * msg);
extern uint8_t* hci_build_evt(struct ke_msg * msg);
extern uint8_t* hci_build_le_evt(struct ke_msg * msg);
extern uint8_t* hci_build_acl_data(struct ke_msg * msg);
extern uint8_t* hci_acl_tx_data_alloc(uint16_t hdl_flags, uint16_t len);
extern uint8_t hci_tx_start(void);
extern uint8_t hci_tx_done(void);
extern uint8_t hci_tx_trigger(void);
extern uint8_t hci_tl_send(struct ke_msg *msg);
extern uint8_t hci_tl_init(uint8_t init_type);
extern uint8_t hci_cmd_get_max_param_size(uint16_t opcode);
#endif

#if (APP_PATCH_LLC)
extern uint8_t llc_loc_clk_acc_proc_continue(uint8_t link_id, uint8_t status);
extern uint8_t llc_ll_clk_acc_req_pdu_send(uint8_t link_id, uint8_t sca);
extern uint8_t llc_ll_clk_acc_rsp_pdu_send(uint8_t link_id, uint8_t sca);
extern uint8_t llc_clk_acc_proc_err_cb(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t ll_clk_acc_req_handler(uint8_t link_id, struct ll_clk_acc_req *pdu, uint16_t event_cnt);
extern uint8_t ll_clk_acc_rsp_handler(uint8_t link_id, struct ll_clk_acc_rsp *pdu, uint16_t event_cnt);
extern int llc_op_clk_acc_ind_handler(ke_msg_id_t const msgid, struct llc_op_clk_acc_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern void llc_clk_acc_modify(uint8_t link_id, uint8_t action);
extern uint8_t llc_loc_con_upd_proc_continue(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t llc_rem_con_upd_proc_continue(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t llc_pref_param_compute(uint8_t link_id, struct llc_op_con_upd_ind* param);
extern uint8_t llc_hci_con_upd_info_send(uint8_t link_id, uint8_t status, struct llc_op_con_upd_ind *param);
extern bool llc_con_upd_param_in_range(uint8_t link_id, uint16_t interval_max, uint16_t interval_min, uint16_t latency, uint16_t timeout);
extern uint8_t llc_ll_connection_update_ind_pdu_send(uint8_t link_id, struct llc_op_con_upd_ind *param);
extern uint8_t llc_ll_connection_param_req_pdu_send(uint8_t link_id, struct llc_op_con_upd_ind *param);
extern uint8_t llc_ll_connection_param_rsp_pdu_send(uint8_t link_id, struct llc_op_con_upd_ind *param);
extern uint8_t llc_hci_con_param_req_evt_send(uint8_t link_id, uint16_t con_intv_min, uint16_t con_intv_max, uint16_t con_latency, uint16_t superv_to);
extern uint8_t llc_loc_con_upd_proc_err_cb(uint8_t link_id, uint8_t error_type, void *param);
extern uint8_t llc_rem_con_upd_proc_err_cb(uint8_t link_id, uint8_t error_type, void *param);
extern uint8_t ll_connection_update_ind_handler(uint8_t link_id, struct ll_connection_update_ind *pdu, uint16_t event_cnt);
extern uint8_t ll_connection_param_req_handler(uint8_t link_id, struct ll_connection_param_req *pdu, uint16_t event_cnt);
extern uint8_t ll_connection_param_rsp_handler(uint8_t link_id, struct ll_connection_param_rsp *param, uint16_t event_cnt);
extern int hci_le_con_upd_cmd_handler(uint8_t link_id, struct hci_le_con_update_cmd const *param, uint16_t opcode);
extern int hci_le_rem_con_param_req_reply_cmd_handler(uint8_t link_id, struct hci_le_rem_con_param_req_reply_cmd const *param, uint16_t opcode);
extern int hci_le_rem_con_param_req_neg_reply_cmd_handler(uint8_t link_id, struct hci_le_rem_con_param_req_neg_reply_cmd const *param, uint16_t opcode);
extern int llc_op_con_upd_ind_handler(ke_msg_id_t const msgid, struct llc_op_con_upd_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_con_param_upd_cfm_handler(ke_msg_id_t const msgid, void *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_con_estab_ind_handler(ke_msg_id_t const msgid, struct lld_con_estab_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_con_offset_upd_ind_handler(ke_msg_id_t const msgid, struct lld_con_offset_upd_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern void llc_con_move_cbk(uint16_t id);
extern uint8_t llc_disconnect_proc_continue(uint8_t link_id, uint8_t status);
extern int llc_op_disconnect_ind_handler(ke_msg_id_t const msgid, struct llc_op_disconnect_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int hci_disconnect_cmd_handler(uint8_t link_id, struct hci_disconnect_cmd const *param, uint16_t opcode);
extern uint8_t llc_disconnect_end(uint8_t link_id, uint8_t status, uint8_t reason);
extern uint8_t llc_ll_terminate_ind_pdu_send(uint16_t link_id, uint8_t reason);
extern uint8_t llc_disconnect_proc_err_cb(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t ll_terminate_ind_handler(uint8_t link_id, struct ll_terminate_ind *pdu, uint16_t event_cnt);
extern int lld_disc_ind_handler(ke_msg_id_t const msgid, struct lld_disc_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int llc_stopped_ind_handler(ke_msg_id_t const msgid, void *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern void llc_disconnect(uint8_t link_id, uint8_t reason, bool immediate);
extern void llc_init_term_proc(uint8_t link_id, uint8_t reason);
extern void llc_llcp_send(uint8_t link_id, union llcp_pdu *pdu, llc_llcp_tx_cfm_cb tx_cfm_cb);
extern void llc_llcp_tx_check(uint8_t link_id);
extern void llc_ll_reject_ind_pdu_send(uint8_t link_id, uint8_t rej_op_code, uint8_t reason, llc_llcp_tx_cfm_cb cfm_cb);
extern int lld_llcp_rx_ind_handler(ke_msg_id_t const msgid, struct lld_llcp_rx_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_llcp_tx_cfm_handler(ke_msg_id_t const msgid, void *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t llc_past_sync_info_unpack(struct sync_info* p_syncinfo_struct, uint8_t * p_syncinfo_tab);
extern uint8_t (*llc_past_)(int8_t link_id);
extern uint8_t llc_past_rpa_res_cb(uint8_t index, uint32_t src_info) ;
extern int llc_op_past_ind_handler(ke_msg_id_t const msgid, struct llc_op_past_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int ll_per_sync_ind_handler(uint8_t link_id, struct ll_per_sync_ind *pdu, uint16_t event_cnt);
extern int hci_le_per_adv_sync_transf_cmd_handler(uint8_t link_id, struct hci_le_per_adv_sync_transf_cmd const *param, uint16_t opcode) ;
extern int hci_le_per_adv_set_info_transf_cmd_handler(uint8_t link_id, struct hci_le_per_adv_set_info_transf_cmd const *param, uint16_t opcode);
extern int hci_le_set_per_adv_sync_transf_param_cmd_handler(uint8_t link_id, struct hci_le_set_per_adv_sync_transf_param_cmd const *param, uint16_t opcode) ;
extern uint8_t llc_loc_dl_upd_proc_continue(uint8_t link_id, uint8_t status);
extern uint8_t llc_rem_dl_upd_proc(uint8_t link_id, uint16_t max_tx_time, uint16_t max_tx_octets, uint16_t max_rx_time, uint16_t max_rx_octets);
extern int ll_length_req_handler(uint8_t link_id, struct ll_length_req *pdu, uint16_t event_cnt);
extern int hci_le_set_data_len_cmd_handler(uint8_t link_id, struct hci_le_set_data_len_cmd const *param, uint16_t opcode);
extern void dl_upd_proc_start(uint8_t link_id);
extern uint8_t llc_ll_length_req_pdu_send(uint8_t link_id, uint16_t eff_tx_time, uint16_t eff_tx_octets, uint16_t eff_rx_time, uint16_t eff_rx_octets);
extern uint8_t llc_ll_length_rsp_pdu_send(uint8_t link_id, uint16_t eff_tx_time, uint16_t eff_tx_octets, uint16_t eff_rx_time, uint16_t eff_rx_octets);
extern uint8_t llc_hci_dl_upd_info_send(uint8_t link_id, uint8_t status, uint16_t max_tx_time, uint16_t max_tx_octets, uint16_t max_rx_time, uint16_t max_rx_octets);
extern uint8_t llc_dle_proc_err_cb(uint8_t link_id, uint8_t error_type, void* param);
extern int ll_length_rsp_handler(uint8_t link_id, struct ll_length_rsp *pdu, uint16_t event_cnt);
extern int llc_op_dl_upd_ind_handler(ke_msg_id_t const msgid, struct llc_op_dl_upd_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t llc_loc_encrypt_proc_continue(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t llc_rem_encrypt_proc_continue(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t llc_aes_res_cb(uint8_t status, const uint8_t* aes_res, uint32_t link_id);
extern uint8_t llc_ll_pause_enc_req_pdu_send(uint8_t link_id);
extern uint8_t llc_ll_pause_enc_rsp_pdu_send(uint8_t link_id, llc_llcp_tx_cfm_cb cfm_cb);
extern uint8_t llc_ll_enc_req_pdu_send(uint8_t link_id, uint16_t ediv, uint8_t* rand, uint8_t* skdm, uint8_t* ivm);
extern uint8_t llc_ll_enc_rsp_pdu_send(uint8_t link_id, uint8_t* skds, uint8_t* ivs);
extern uint8_t llc_ll_start_enc_req_pdu_send(uint8_t link_id);
extern uint8_t llc_ll_start_enc_rsp_pdu_send(uint8_t link_id, llc_llcp_tx_cfm_cb cfm_cb);
extern uint8_t llc_ll_pause_enc_rsp_ack_handler(uint8_t link_id, uint8_t op_code);
extern uint8_t llc_ll_start_enc_rsp_ack_handler(uint8_t link_id, uint8_t op_code);
extern uint8_t llc_ll_reject_ind_ack_handler(uint8_t link_id, uint8_t op_code);
extern uint8_t llc_hci_ltk_request_evt_send(uint8_t link_id, uint16_t enc_div, uint8_t* randnb);
extern uint8_t llc_hci_enc_evt_send(uint8_t link_id, uint8_t status, bool re_encrypt);
extern uint8_t llc_loc_encrypt_proc_err_cb(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t llc_rem_encrypt_proc_err_cb(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t ll_pause_enc_req_handler(uint8_t link_id, struct ll_pause_enc_req *pdu, uint16_t event_cnt);
extern uint8_t ll_pause_enc_rsp_handler(uint8_t link_id, struct ll_pause_enc_rsp *pdu, uint16_t event_cnt);
extern uint8_t ll_enc_req_handler(uint8_t link_id, struct ll_enc_req *pdu, uint16_t event_cnt);
extern uint8_t ll_enc_rsp_handler(uint8_t link_id, struct ll_enc_rsp *pdu, uint16_t event_cnt);
extern uint8_t ll_start_enc_req_handler(uint8_t link_id, struct ll_start_enc_req *pdu, uint16_t event_cnt);
extern uint8_t ll_start_enc_rsp_handler(uint8_t link_id, struct ll_start_enc_rsp *pdu, uint16_t event_cnt);
extern int hci_le_start_enc_cmd_handler(uint8_t link_id, struct hci_le_start_enc_cmd const *param, uint16_t opcode);
extern int hci_le_ltk_req_reply_cmd_handler(uint8_t link_id, struct hci_le_ltk_req_reply_cmd const *cmd, uint16_t opcode);
extern int hci_le_ltk_req_neg_reply_cmd_handler(uint8_t link_id, struct hci_le_ltk_req_neg_reply_cmd const *param, uint16_t opcode);
extern int llc_encrypt_ind_handler(ke_msg_id_t const msgid, struct llc_encrypt_ind const *ind,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int llc_op_encrypt_ind_handler(ke_msg_id_t const msgid, struct llc_op_encrypt_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t llc_loc_feats_exch_proc_continue(uint8_t link_id, uint8_t status);
extern uint8_t llc_ll_feature_req_pdu_send(uint8_t link_id);
extern uint8_t llc_ll_feature_rsp_pdu_send(uint8_t link_id);
extern uint8_t llc_hci_feats_info_send(uint8_t link_id, uint8_t status, struct features * feats);
extern uint8_t llc_feats_exch_proc_err_cb(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t ll_feature_req_handler(uint8_t link_id, struct ll_feature_req *pdu, uint16_t event_cnt);
extern uint8_t ll_slave_feature_req_handler(uint8_t link_id, struct ll_slave_feature_req *pdu, uint16_t event_cnt);
extern uint8_t ll_feature_rsp_handler(uint8_t link_id, struct ll_feature_rsp *pdu, uint16_t event_cnt);
extern int hci_le_rd_rem_feats_cmd_handler(uint8_t link_id, struct hci_le_rd_rem_feats_cmd const *param, uint16_t opcode);
extern int llc_op_feats_exch_ind_handler(ke_msg_id_t const msgid, struct llc_op_feats_exch_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int hci_acl_data_handler(ke_msg_id_t const msgid, struct hci_acl_data *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_acl_rx_ind_handler(ke_msg_id_t const msgid, struct lld_acl_rx_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t llc_hci_nb_cmp_pkts_evt_send(uint8_t link_id, uint8_t nb_of_pkt);
extern int hci_rd_tx_pwr_lvl_cmd_handler(uint8_t link_id, struct hci_rd_tx_pwr_lvl_cmd const *param, uint16_t opcode);
extern int hci_rd_rssi_cmd_handler(uint8_t link_id, struct hci_basic_conhdl_cmd const *param, uint16_t opcode);
extern int hci_vs_set_pref_slave_latency_cmd_handler(uint8_t link_id, struct hci_vs_set_pref_slave_latency_cmd const *param, uint16_t opcode);
extern int hci_vs_set_pref_slave_evt_dur_cmd_handler(uint8_t link_id, struct hci_vs_set_pref_slave_evt_dur_cmd const *param, uint16_t opcode);
extern int hci_command_handler(ke_msg_id_t const msgid, uint16_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t llc_loc_phy_upd_proc_continue(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t llc_rem_phy_upd_proc_continue(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t llc_dl_chg_check(uint8_t link_id, uint8_t old_tx_phy, uint8_t old_rx_phy);
extern uint8_t llc_loc_phy_upd_proc_err_cb(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t llc_rem_phy_upd_proc_err_cb(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t ll_phy_req_handler(uint8_t link_id, struct ll_phy_req *pdu, uint16_t event_cnt);
extern uint8_t ll_phy_rsp_handler(uint8_t link_id, struct ll_phy_rsp *pdu, uint16_t event_cnt);
extern uint8_t ll_phy_update_ind_handler(uint8_t link_id, struct ll_phy_update_ind *pdu, uint16_t event_cnt);
extern int hci_le_rd_phy_cmd_handler(uint8_t link_id, struct hci_le_rd_phy_cmd const *param, uint16_t opcode);
extern int hci_le_set_phy_cmd_handler(uint8_t link_id, struct hci_le_set_phy_cmd const *param, uint16_t opcode);
extern int llc_op_phy_upd_ind_handler(ke_msg_id_t const msgid, struct llc_op_phy_upd_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_phy_upd_cfm_handler(ke_msg_id_t const msgid, void *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern void phy_upd_proc_start(uint8_t link_id);
extern uint8_t llc_ver_exch_loc_proc_continue(uint8_t link_id, uint8_t status);
extern int llc_op_ver_exch_ind_handler(ke_msg_id_t const msgid, struct llc_op_ver_exch_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t ll_version_ind_handler(uint8_t link_id, struct ll_version_ind *pdu, uint16_t event_cnt);
extern int hci_rd_rem_ver_info_cmd_handler(uint8_t link_id, struct hci_rd_rem_ver_info_cmd const *param, uint16_t opcode);
void llc_past_rx(uint8_t link_id);
#endif

#if (APP_PATCH_LLM)
extern uint8_t llm_adv_set_release(uint8_t act_id);
extern uint8_t llm_adv_rpa_gen_cb(uint8_t status, const uint8_t* aes_res, uint32_t src_info);
extern uint8_t llm_adv_con_len_check(struct hci_le_set_ext_adv_param_cmd* ext_param, uint16_t adv_data_len);
extern uint8_t llm_adv_set_dft_params(uint8_t act_id);
extern uint8_t llm_adv_hdl_to_id(uint16_t adv_hdl, struct hci_le_set_ext_adv_param_cmd** adv_param);
extern int hci_le_set_adv_param_cmd_handler(struct hci_le_set_adv_param_cmd const *param, uint16_t opcode);
extern int hci_le_set_adv_data_cmd_handler(struct hci_le_set_adv_data_cmd const *param, uint16_t opcode);
extern int hci_le_set_scan_rsp_data_cmd_handler(struct hci_le_set_scan_rsp_data_cmd const *param, uint16_t opcode) ;
extern int hci_le_set_adv_en_cmd_handler(struct hci_le_set_adv_en_cmd const *param, uint16_t opcode);
extern int hci_le_set_ext_adv_param_cmd_handler(struct hci_le_set_ext_adv_param_cmd const *param, uint16_t opcode);
extern int hci_le_set_adv_set_rand_addr_cmd_handler(struct hci_le_set_adv_set_rand_addr_cmd const *param, uint16_t opcode);
extern int hci_le_set_ext_adv_data_cmd_handler(struct hci_le_set_ext_adv_data_cmd const *param, uint16_t opcode);
extern int hci_le_set_ext_scan_rsp_data_cmd_handler(struct hci_le_set_ext_scan_rsp_data_cmd const *param, uint16_t opcode);
extern int hci_le_set_ext_adv_en_cmd_handler(struct hci_le_set_ext_adv_en_cmd *param, uint16_t opcode);
extern int hci_le_set_per_adv_param_cmd_handler(struct hci_le_set_per_adv_param_cmd const *param, uint16_t opcode);
extern int hci_le_set_per_adv_data_cmd_handler(struct hci_le_set_per_adv_data_cmd const *param, uint16_t opcode);
extern int hci_le_set_per_adv_en_cmd_handler(struct hci_le_set_per_adv_en_cmd const *param, uint16_t opcode);
extern int hci_le_rd_max_adv_data_len_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_rd_nb_supp_adv_sets_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_rmv_adv_set_cmd_handler(struct hci_le_rem_adv_set_cmd const *param, uint16_t opcode);
extern int hci_le_clear_adv_sets_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_set_conless_cte_tx_param_cmd_handler(struct hci_le_set_conless_cte_tx_param_cmd const *param, uint16_t opcode);
extern int hci_le_set_conless_cte_tx_en_cmd_handler(struct hci_le_set_conless_cte_tx_en_cmd const *param, uint16_t opcode);
extern uint8_t llm_adv_act_id_get(uint8_t adv_hdl, uint8_t* ext_adv_id, uint8_t* per_adv_id);
extern uint8_t llm_adv_set_id_get(uint8_t act_id, uint8_t* sid, uint8_t* atype, struct bd_addr* adva);
extern int hci_le_rd_adv_ch_tx_pw_cmd_handler(void const *param, uint16_t opcode);
extern int lld_scan_req_ind_handler(ke_msg_id_t const msgid, struct lld_scan_req_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_adv_end_ind_handler(ke_msg_id_t const msgid, struct lld_adv_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_per_adv_end_ind_handler(ke_msg_id_t const msgid, struct lld_per_adv_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern bool llm_is_dev_connected(struct bd_addr const *peer_addr, uint8_t peer_addr_type);
extern bool llm_is_dev_synced(struct bd_addr const *peer_addr, uint8_t peer_addr_type, uint8_t adv_sid);
extern void llm_link_disc(uint8_t link_id);
extern uint8_t llm_ch_map_update(void);
extern uint8_t llm_dev_list_empty_entry(void);
extern uint8_t llm_dev_list_search(const struct bd_addr *bd_addr, uint8_t bd_addr_type);
extern uint8_t llm_ral_search(const struct bd_addr *bd_addr, uint8_t bd_addr_type);
extern bool llm_ral_is_empty(void);
extern void llm_init(uint8_t init_type);
extern void llm_clk_acc_set(uint8_t act_id, bool clk_acc);
extern uint8_t llm_activity_free_get(uint8_t* act_id);
extern void llm_activity_free_set(uint8_t act_id);
extern bool llm_is_wl_accessible(void);
extern bool llm_is_non_con_act_ongoing_check(void);
extern bool llm_is_rand_addr_in_use_check(void);
extern int hci_reset_cmd_handler(void const *param, uint16_t opcode);
extern int hci_rd_local_ver_info_cmd_handler(void const *param, uint16_t opcode);
extern int hci_rd_bd_addr_cmd_handler(void const *param, uint16_t opcode);
extern int hci_rd_local_supp_cmds_cmd_handler(void const *param, uint16_t opcode);
extern int hci_rd_local_supp_feats_cmd_handler(void const *param, uint16_t opcode);
extern int hci_set_evt_mask_cmd_handler(struct hci_set_evt_mask_cmd const *param, uint16_t opcode);
extern int hci_set_evt_mask_page_2_cmd_handler(struct hci_set_evt_mask_cmd const *param, uint16_t opcode);
extern int hci_set_ctrl_to_host_flow_ctrl_cmd_handler(struct hci_set_ctrl_to_host_flow_ctrl_cmd const *param, uint16_t opcode);
extern int hci_host_buf_size_cmd_handler(struct hci_host_buf_size_cmd const *param, uint16_t opcode);
extern int hci_host_nb_cmp_pkts_cmd_handler(struct hci_host_nb_cmp_pkts_cmd const *param, uint16_t opcode);
extern int hci_le_set_evt_mask_cmd_handler(struct hci_le_set_evt_mask_cmd const *param, uint16_t opcode);
extern int hci_le_rd_buff_size_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_rd_local_supp_feats_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_set_rand_addr_cmd_handler(struct hci_le_set_rand_addr_cmd const *param, uint16_t opcode);
extern int hci_le_set_host_ch_class_cmd_handler(struct hci_le_set_host_ch_class_cmd const *param, uint16_t opcode);
extern int hci_le_rd_wl_size_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_clear_wlst_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_add_dev_to_wlst_cmd_handler(struct hci_le_add_dev_to_wlst_cmd const *param, uint16_t opcode);
extern int hci_le_rmv_dev_from_wlst_cmd_handler(struct hci_le_rmv_dev_from_wlst_cmd const *param, uint16_t opcode);
extern int hci_le_rd_rslv_list_size_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_clear_rslv_list_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_add_dev_to_rslv_list_cmd_handler(struct hci_le_add_dev_to_rslv_list_cmd const *param, uint16_t opcode);
extern int hci_le_rmv_dev_from_rslv_list_cmd_handler(struct hci_le_rmv_dev_from_rslv_list_cmd const *param, uint16_t opcode);
extern int hci_le_set_rslv_priv_addr_to_cmd_handler(struct hci_le_set_rslv_priv_addr_to_cmd const *param, uint16_t opcode);
extern int hci_le_set_addr_resol_en_cmd_handler(struct hci_le_set_addr_resol_en_cmd const *param, uint16_t opcode);
extern int hci_le_rd_peer_rslv_addr_cmd_handler(struct hci_le_rd_peer_rslv_addr_cmd const *param, uint16_t opcode);
extern int hci_le_rd_loc_rslv_addr_cmd_handler(struct hci_le_rd_loc_rslv_addr_cmd const *param, uint16_t opcode);
extern uint8_t llm_aes_res_cb(uint8_t status, const uint8_t* aes_res, uint32_t dummy);
extern int hci_le_enc_cmd_handler(struct hci_le_enc_cmd const *param, uint16_t opcode);
extern int hci_le_rand_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_rd_supp_states_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_rd_tx_pwr_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_rd_rf_path_comp_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_wr_rf_path_comp_cmd_handler(struct hci_le_wr_rf_path_comp_cmd const *param, uint16_t opcode);
extern int hci_le_set_priv_mode_cmd_handler(struct hci_le_set_priv_mode_cmd const *param, uint16_t opcode);
extern int hci_le_mod_sleep_clk_acc_cmd_handler(struct hci_le_mod_sleep_clk_acc_cmd const *param, uint16_t opcode);
extern int hci_le_rd_antenna_inf_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_rd_suggted_dft_data_len_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_wr_suggted_dft_data_len_cmd_handler(struct hci_le_wr_suggted_dft_data_len_cmd const *param, uint16_t opcode);
extern int hci_le_rd_max_data_len_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_rd_local_p256_public_key_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_gen_dhkey_v1_cmd_handler(struct hci_le_gen_dhkey_v1_cmd *param, uint16_t opcode);
extern int hci_le_gen_dhkey_v2_cmd_handler(struct hci_le_gen_dhkey_v2_cmd *param, uint16_t opcode);
extern int hci_le_set_dft_phy_cmd_handler(struct hci_le_set_dft_phy_cmd const *param, uint16_t opcode);
extern int hci_le_set_dft_per_adv_sync_transf_param_cmd_handler(struct hci_le_set_dft_per_adv_sync_transf_param_cmd const *param, uint16_t opcode);
extern int llm_hci_command_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int hci_le_create_con_cmd_handler(struct hci_le_create_con_cmd const *param, uint16_t opcode);
extern int hci_le_ext_create_con_cmd_handler(struct hci_le_ext_create_con_cmd const *param, uint16_t opcode);
extern int hci_le_create_con_cancel_cmd_handler(void const *param, uint16_t opcode);
extern int lld_init_end_ind_handler(ke_msg_id_t const msgid, struct lld_init_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern uint8_t llm_scan_start(uint8_t act_id);
extern bool llm_adv_reports_list_check(struct bd_addr const *adv_bd_addr, uint8_t addr_type, uint8_t adv_evt_type, uint8_t sid, uint16_t did, uint16_t per_adv_intv);
extern uint8_t llm_scan_sync_acad_attach(uint8_t sync_act_id, uint8_t ad_type, uint16_t task);
extern uint8_t llm_scan_sync_info_get(uint8_t sync_act_id, uint8_t* sid, uint8_t* atype, struct bd_addr* adva);
extern uint8_t llm_per_adv_sync(struct llm_per_adv_sync_params * params);
extern int hci_le_set_scan_param_cmd_handler(struct hci_le_set_scan_param_cmd const *param, uint16_t opcode);
extern int hci_le_set_scan_en_cmd_handler(struct hci_le_set_scan_en_cmd const *param, uint16_t opcode);
extern int hci_le_set_ext_scan_param_cmd_handler(struct hci_le_set_ext_scan_param_cmd const *param, uint16_t opcode);
extern int hci_le_set_ext_scan_en_cmd_handler(struct hci_le_set_ext_scan_en_cmd const *param, uint16_t opcode);
extern int hci_le_per_adv_create_sync_cmd_handler(struct hci_le_per_adv_create_sync_cmd const *param, uint16_t opcode);
extern int hci_le_per_adv_create_sync_cancel_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_per_adv_term_sync_cmd_handler(struct hci_le_per_adv_term_sync_cmd const *param, uint16_t opcode);
extern int hci_le_add_dev_to_per_adv_list_cmd_handler(struct hci_le_add_dev_to_per_adv_list_cmd const *param, uint16_t opcode);
extern int hci_le_rmv_dev_from_per_adv_list_cmd_handler(struct hci_le_rmv_dev_from_per_adv_list_cmd const *param, uint16_t opcode);
extern int hci_le_clear_per_adv_list_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_rd_per_adv_list_size_cmd_handler(void const *param, uint16_t opcode);
extern int hci_le_set_conless_iq_sampl_en_cmd_handler(struct hci_le_set_conless_iq_sampl_en_cmd const *param, uint16_t opcode);
extern int hci_le_set_per_adv_rec_en_cmd_handler(struct hci_le_set_per_adv_rec_en_cmd const *param, uint16_t opcode);
extern int llm_scan_period_to_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_adv_rep_ind_handler(ke_msg_id_t const msgid, struct lld_adv_rep_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_sync_start_req_handler(ke_msg_id_t const msgid, struct lld_sync_start_req const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_per_adv_rep_ind_handler(ke_msg_id_t const msgid, struct lld_per_adv_rep_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_per_adv_rx_end_ind_handler(ke_msg_id_t const msgid, struct lld_per_adv_rx_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_scan_end_ind_handler(ke_msg_id_t const msgid, struct lld_scan_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_conless_cte_rx_ind_handler(ke_msg_id_t const msgid, struct lld_conless_cte_rx_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int llm_rpa_renew_to_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int llm_pub_key_gen_ind_handler(ke_msg_id_t const msgid, struct ecc_result_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int llm_dh_key_gen_ind_handler(ke_msg_id_t const msgid, struct ecc_result_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int llm_encrypt_ind_handler(ke_msg_id_t const msgid, struct llm_encrypt_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
#endif

#if (APP_PATCH_LLC_CHMAP)
extern uint8_t llc_loc_ch_map_proc_continue(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t llc_rem_ch_map_proc_continue(uint8_t link_id, uint8_t state, uint8_t status);
extern uint8_t llc_ch_map_up_proc_err_cb(uint8_t link_id, uint8_t error_type, void* param);
extern uint8_t ll_channel_map_ind_handler(uint8_t link_id, struct ll_channel_map_ind *pdu, uint16_t event_cnt);
extern uint8_t ll_min_used_channels_ind_handler(uint8_t link_id, struct ll_min_used_channels_ind *pdu, uint16_t event_cnt);
extern int hci_le_rd_chnl_map_cmd_handler(uint8_t link_id, struct hci_basic_conhdl_cmd const *param, uint16_t opcode);
extern int llc_op_ch_map_upd_ind_handler(ke_msg_id_t const msgid, struct llc_op_ch_map_upd_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int llm_ch_map_update_ind_handler(ke_msg_id_t const msgid, void *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int lld_ch_map_upd_cfm_handler(ke_msg_id_t const msgid, void *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern void llc_cleanup(uint8_t link_id, bool reset);
extern uint8_t llc_start(uint8_t link_id, struct llc_init_parameters *con_params);
extern uint8_t llc_llcp_trans_timer_set(uint8_t link_id, uint8_t proc_type, bool enable);
extern void llc_init(uint8_t init_type);
extern void llc_stop(uint8_t link_id);
extern void llc_llcp_state_set(uint8_t link_id, uint8_t dir, uint8_t state);
extern void llc_proc_reg(uint8_t link_id, uint8_t proc_type, llc_procedure_t *params);
extern void llc_proc_unreg(uint8_t link_id, uint8_t proc_type);
extern uint8_t llc_proc_id_get(uint8_t link_id, uint8_t proc_type);
extern void llc_proc_state_set(llc_procedure_t *proc, uint8_t link_id, uint8_t proc_state);
extern llc_procedure_t * llc_proc_get(uint8_t link_id, uint8_t proc_type);
extern void llc_proc_err_ind(uint8_t link_id, uint8_t proc_type, uint8_t error_type, void* param);
extern void llc_proc_timer_set(uint8_t link_id, uint8_t proc_type, bool enable);
extern void llc_proc_timer_pause_set(uint8_t link_id, uint8_t proc_type, bool enable);
extern uint8_t llc_proc_collision_check(uint8_t link_id, uint8_t proc_id);
#endif

#if (APP_PATCH_NVDS)
extern bool nvds_is_magic_number_ok(void);
extern uint8_t nvds_walk_tag (uint32_t cur_tag_addr,struct nvds_tag_header *nvds_tag_header_ptr,uint32_t *nxt_tag_addr_ptr);
extern uint8_t nvds_browse_tag (uint8_t tag,struct nvds_tag_header *nvds_tag_header_ptr,uint32_t *tag_address_ptr);
extern uint8_t nvds_purge(uint32_t length, uint8_t* buf);
extern uint8_t nvds_init(uint8_t *base, uint32_t len);
extern uint8_t nvds_get(uint8_t tag, nvds_tag_len_t * lengthPtr, uint8_t *buf);
extern uint8_t nvds_lock(uint8_t tag);
extern uint8_t nvds_put(uint8_t tag, nvds_tag_len_t length, uint8_t *buf);
extern uint8_t nvds_del(uint8_t tag);
extern uint8_t nvds_null_init(void);
extern uint8_t nvds_read(uint32_t address, uint32_t length, uint8_t *buf);
extern uint8_t nvds_write(uint32_t address, uint32_t length, uint8_t *buf);
extern uint8_t nvds_erase(uint32_t address, uint32_t length);
extern uint8_t nvds_init_memory(void);
#endif

#if (APP_PATCH_L2CC)
extern uint8_t l2cc_init(uint8_t init_type);
extern uint8_t l2cc_create(uint8_t conidx);
extern uint8_t l2cc_cleanup(uint8_t conidx, bool reset);
extern uint8_t l2cc_update_state(uint8_t conidx, ke_state_t state, bool busy);
extern uint8_t l2cc_op_complete(uint8_t conidx, uint8_t op_type, uint8_t status);
extern uint8_t l2cc_send_cmp_evt(uint8_t conidx, uint8_t operation, const ke_task_id_t requester, uint8_t status, uint16_t cid,uint16_t credit);
extern uint8_t l2cc_set_operation_ptr(uint8_t conidx, uint8_t op_type, void* op);
extern uint8_t l2cc_data_send(uint8_t conidx, uint8_t nb_buffer);
extern uint8_t l2cc_get_operation(uint8_t conidx, uint8_t op_type);
extern void* l2cc_get_operation_ptr(uint8_t conidx, uint8_t op_type);
extern void l2cc_lecb_free(uint8_t conidx, struct l2cc_lecb_info* lecb, bool disconnect_ind);
extern void l2cc_lecb_send_con_req(uint8_t conidx, uint8_t pkt_id, uint16_t le_psm, uint16_t scid, uint16_t credits,uint16_t mps, uint16_t mtu);
extern void l2cc_lecb_send_con_rsp(uint8_t conidx, uint16_t status, uint8_t pkt_id,uint16_t dcid, uint16_t credits, uint16_t mps, uint16_t mtu);
extern void l2cc_lecb_send_disc_req(uint8_t conidx, uint8_t pkt_id, uint16_t scid, uint16_t dcid);
extern void l2cc_lecb_send_disc_rsp(uint8_t conidx, uint8_t pkt_id, uint16_t dcid, uint16_t scid);
extern void l2cc_lecb_send_credit_add(uint8_t conidx, uint8_t pkt_id, uint16_t cid, uint16_t credits);
extern void l2cc_lecb_init_disconnect(uint8_t conidx, struct l2cc_lecb_info* lecb, uint8_t disc_reason);
extern uint16_t l2cc_lecb_h2l_err(uint8_t h_err);
extern struct l2cc_lecb_info* l2cc_lecb_find(uint8_t conidx, uint8_t field, uint16_t value);
extern uint8_t l2cc_pdu_pack(struct l2cc_pdu *p_pdu, uint16_t *p_offset, uint16_t *p_length, uint8_t *p_buffer, uint8_t *pb_flag);
extern uint8_t l2cc_pdu_unpack(uint8_t conidx, struct l2cc_pdu *p_pdu, uint16_t *p_offset, uint16_t *p_rem_len,const uint8_t *p_buffer, uint16_t pkt_length, uint8_t pb_flag);
extern uint8_t l2cc_lecb_pdu_pack(uint8_t conidx, struct l2cc_sdu *sdu, uint16_t *length, uint8_t *buffer, uint16_t* offset,uint16_t pdu_len, uint8_t pb_flag);
extern uint8_t l2cc_lecb_pdu_unpack(uint8_t conidx, struct l2cc_sdu *sdu, uint8_t *buffer, uint16_t length, uint16_t* offset,uint16_t* pdu_remain_len, uint8_t pb_flag);
extern uint8_t l2cc_pdu_rx_init(uint8_t conidx);
extern uint8_t l2cc_pdu_header_check(uint8_t conidx, uint8_t** pp_buffer, uint16_t* p_rx_length);
extern uint8_t l2cc_sig_send_cmd_reject(uint8_t conidx, uint8_t pkt_id, uint16_t reason,uint16_t opt1, uint16_t opt2);
extern uint8_t l2cc_sig_lecb_l2h_err(uint16_t l_err);
extern int l2c_code_reject_handler(uint8_t conidx, struct l2cc_reject *pdu);
extern int l2c_code_conn_param_upd_req_handler(uint8_t conidx, struct l2cc_conn_param_upd_req *pdu);
extern int l2c_code_conn_param_upd_resp_handler(uint8_t conidx, struct l2cc_conn_param_upd_resp *pdu);
extern int l2c_code_le_cb_conn_req_handler(uint8_t conidx, struct l2cc_le_cb_conn_req *pdu);
extern int l2c_code_le_cb_conn_resp_handler(uint8_t conidx, struct l2cc_le_cb_conn_resp *pdu);
extern int l2c_code_disconnection_req_handler(uint8_t conidx, struct l2cc_disconnection_req *pdu);
extern int l2c_code_disconnection_resp_handler(uint8_t conidx, struct l2cc_disconnection_resp *pdu);
extern int l2c_code_le_flow_control_credit_handler(uint8_t conidx, struct l2cc_le_flow_control_credit *pdu);
extern int l2cc_sig_pdu_recv_handler(uint8_t conidx, struct l2cc_pdu *pdu);
extern int l2cc_process_op(uint8_t conidx, uint8_t op_type, void *op_msg, enum l2cc_operation *supp_ops);
extern int l2cc_pdu_send_cmd_handler(ke_msg_id_t const msgid, struct l2cc_pdu_send_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int l2cc_lecb_sdu_send_cmd_handler(ke_msg_id_t const msgid, struct l2cc_lecb_sdu_send_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int l2cc_dbg_pdu_send_cmd_handler(ke_msg_id_t const msgid, struct l2cc_dbg_pdu_send_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int hci_nb_cmp_pkts_evt_handler(ke_msg_id_t const msgid, struct hci_nb_cmp_pkts_evt const *event, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int l2cc_task_hci_acl_data_handler(ke_msg_id_t const msgid, struct hci_acl_data const *param, ke_task_id_t const _dest_id, ke_task_id_t const src_id);
extern int l2cc_lecb_connect_cmd_handler(ke_msg_id_t const msgid, struct l2cc_lecb_connect_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int l2cc_lecb_connect_cfm_handler(ke_msg_id_t const msgid, struct l2cc_lecb_connect_cfm *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int l2cc_lecb_disconnect_cmd_handler(ke_msg_id_t const msgid, struct l2cc_lecb_disconnect_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int l2cc_signaling_trans_to_ind_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int l2cc_lecb_add_cmd_handler(ke_msg_id_t const msgid, struct l2cc_lecb_add_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int l2cc_pdu_recv_ind_handler(ke_msg_id_t const msgid, struct l2cc_pdu_recv_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int l2cc_cmp_evt_handler(ke_msg_id_t const msgid, void *p_paran, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int l2cc_default_msg_handler(ke_msg_id_t const msgid, void *p_paran, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern void *l2cc_pdu_alloc(uint8_t conidx, uint16_t cid, uint8_t code, ke_task_id_t src_id, uint16_t length);
extern uint8_t l2cm_init(uint8_t init_type);
extern uint8_t l2cm_cleanup(uint8_t conidx);
extern uint8_t l2cm_set_link_layer_buff_size(uint16_t pkt_len, uint16_t nb_acl);
extern uint8_t l2cm_tx_status(uint8_t conidx, bool busy);
#endif

#if (APP_PATCH_GAP)
extern bool gapc_smp_check_key_distrib(uint8_t conidx, uint8_t sec_level);
extern bool gapc_smp_check_max_key_size(uint8_t conidx);
extern bool gapc_smp_check_pairing_feat(struct gapc_pairing *pair_feat);
extern bool gapc_smp_is_sec_mode_reached(uint8_t conidx, uint8_t role);
extern int hci_enc_key_refr_evt_handler(ke_msg_id_t const msgid, struct hci_enc_key_ref_cmp_evt const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapc_bond_cfm_handler(ke_msg_id_t const msgid, struct gapc_bond_cfm *cfm, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapc_bond_cmd_handler(ke_msg_id_t const msgid, struct gapc_bond_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapc_encrypt_cfm_handler(ke_msg_id_t const msgid, struct gapc_encrypt_cfm *cfm, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapc_encrypt_cmd_handler(ke_msg_id_t const msgid, struct gapc_encrypt_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapc_gen_dh_key_ind_handler(ke_msg_id_t const msgid, struct gapm_gen_dh_key_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapc_key_press_notification_cmd_handler(ke_msg_id_t const msgid, struct gapc_key_press_notif_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapc_security_cmd_handler(ke_msg_id_t const msgid, struct gapc_security_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapc_sign_cmd_handler(ke_msg_id_t const msgid, struct gapc_sign_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapc_smp_gapm_cmp_evt_handler(ke_msg_id_t const msgid, struct gapm_cmp_evt *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapc_smp_rep_attempts_timer_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapc_smp_timeout_timer_ind_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapm_smpc_pub_key_ind_handler(ke_msg_id_t const msg_id, struct gapm_pub_key_ind *p_param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int hci_enc_chg_evt_handler(ke_msg_id_t const msgid, struct hci_enc_change_evt const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int hci_le_ltk_request_evt_handler(ke_msg_id_t const msgid, struct hci_le_ltk_request_evt const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int hci_le_start_enc_stat_evt_handler(ke_msg_id_t const msgid, struct hci_cmd_stat_event const *event, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapc_smp_l2cc_cmp_evt_handler(ke_msg_id_t const msgid, struct l2cc_cmp_evt *evt, ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int8_t gapc_smp_pairing_irk_exch(uint8_t conidx, struct gap_sec_key *irk, struct gap_bdaddr *identity);
extern uint8_t gapc_smp_aes_res_cb(uint8_t status, const uint8_t *aes_res, uint32_t aes_info);
extern uint8_t gapc_smp_aes_sign_res_cb(uint8_t status, const uint8_t *aes_res, uint32_t aes_info);
extern uint8_t gapc_smp_check_repeated_attempts(uint8_t conidx);
extern uint8_t gapc_smp_cleanup(uint8_t conidx);
extern uint8_t gapc_smp_clear_timeout_timer(uint8_t conidx);
extern uint8_t gapc_smp_comp_cnf_val(uint8_t conidx, uint8_t role, bool local);
extern uint8_t gapc_smp_comp_f4(uint8_t conidx, const uint8_t *u, const uint8_t *v, const uint8_t *x, uint8_t z, uint8_t state);
extern uint8_t gapc_smp_comp_f5(uint8_t conidx, const uint8_t *w, const uint8_t *n1, const uint8_t *n2, const uint8_t *a1, const uint8_t *a2, uint8_t state);
extern uint8_t gapc_smp_comp_f6(uint8_t conidx, const uint8_t *w, const uint8_t *n1, const uint8_t *n2, const uint8_t *r, const uint8_t *iocap, const uint8_t *a1, const uint8_t *a2, uint8_t state);
extern uint8_t gapc_smp_comp_g2(uint8_t conidx, const uint8_t *u, const uint8_t *v, const uint8_t *x, const uint8_t *y, uint8_t state);
extern uint8_t gapc_smp_dhkey_check_send(uint8_t conidx, const uint8_t *dh_key_chk);
extern uint8_t gapc_smp_encrypt_cfm(uint8_t conidx, bool accept, struct gap_sec_key *ltk, uint8_t key_size);
extern uint8_t gapc_smp_encrypt_start_handler(uint8_t conidx, struct gapc_ltk *ltk);
extern uint8_t gapc_smp_encrypt_start(uint8_t conidx, struct gapc_ltk *ltk);
extern uint8_t gapc_smp_generate_rand(uint8_t conidx, uint8_t state);
extern uint8_t gapc_smp_generate_stk(uint8_t conidx, uint8_t role);
extern uint8_t gapc_smp_get_key_sec_prop(uint8_t conidx);
extern uint8_t gapc_smp_get_next_passkey_bit(struct gapc_smp_pair_info *p_pair);
extern uint8_t gapc_smp_handle_dh_key_check_complete(uint8_t conidx, const uint8_t *dh_key);
extern uint8_t gapc_smp_handle_enc_change_evt(uint8_t conidx, uint8_t role, uint8_t status);
extern uint8_t gapc_smp_id_addr_info_send(uint8_t conidx);
extern uint8_t gapc_smp_id_info_send(uint8_t conidx, const uint8_t *irk);
extern uint8_t gapc_smp_init_mac_key_calculation(uint8_t conidx);
extern uint8_t gapc_smp_initiate_dhkey_check(uint8_t conidx);
extern uint8_t gapc_smp_initiate_dhkey_verification(uint8_t conidx);
extern uint8_t gapc_smp_key_press_notification_ind(uint8_t conidx, uint8_t notification_type);
extern uint8_t gapc_smp_launch_rep_att_timer(uint8_t conidx);
extern uint8_t gapc_smp_pair_cfm_send(uint8_t conidx, const uint8_t *conf_val);
extern uint8_t gapc_smp_pair_fail_send(uint8_t conidx, uint8_t status);
extern uint8_t gapc_smp_pair_rand_send(uint8_t conidx, const uint8_t *rand_nb);
extern uint8_t gapc_smp_pairing_csrk_exch(uint8_t conidx, struct gap_sec_key *csrk);
extern uint8_t gapc_smp_pairing_end(uint8_t conidx, uint8_t role, uint8_t status, bool start_ra_timer);
extern uint8_t gapc_smp_pairing_ltk_exch(uint8_t conidx, struct gapc_ltk *ltk);
extern uint8_t gapc_smp_pairing_nc_exch(uint8_t conidx, uint8_t accept);
extern uint8_t gapc_smp_pairing_oob_exch(uint8_t conidx, bool accept, struct gapc_oob *oob);
extern uint8_t gapc_smp_pairing_req_handler(uint8_t conidx, struct gapc_pairing *feat);
extern uint8_t gapc_smp_pairing_rsp(uint8_t conidx, bool accept, struct gapc_pairing *feat);
extern uint8_t gapc_smp_pairing_start(uint8_t conidx, struct gapc_pairing *pairing);
extern uint8_t gapc_smp_pairing_tk_exch(uint8_t conidx, bool accept, struct gap_sec_key *tk);
extern uint8_t gapc_smp_pdu_recv(uint8_t conidx, struct l2cc_pdu *pdu, uint8_t status);
extern uint8_t gapc_smp_pdu_send(uint8_t conidx, struct l2cc_pdu_data_t *pdu);
extern uint8_t gapc_smp_recv_dhkey_check_pdu(uint8_t conidx, struct l2cc_dhkey_check *pdu);
extern uint8_t gapc_smp_recv_enc_info_pdu(uint8_t conidx, struct l2cc_encryption_inf *pdu);
extern uint8_t gapc_smp_recv_id_addr_info_pdu(uint8_t conidx, struct l2cc_id_addr_inf *pdu);
extern uint8_t gapc_smp_recv_id_info_pdu(uint8_t conidx, struct l2cc_identity_inf *pdu);
extern uint8_t gapc_smp_recv_keypress_notification_pdu(uint8_t conidx, struct l2cc_keypress_notification *pdu);
extern uint8_t gapc_smp_recv_mst_id_pdu(uint8_t conidx, struct l2cc_master_id *pdu);
extern uint8_t gapc_smp_recv_pair_cfm_pdu(uint8_t conidx, struct l2cc_pairing_cfm *pdu);
extern uint8_t gapc_smp_recv_pair_fail_pdu(uint8_t conidx, struct l2cc_pairing_failed *pdu);
extern uint8_t gapc_smp_recv_pair_rand_pdu(uint8_t conidx, struct l2cc_pairing_random *pdu);
extern uint8_t gapc_smp_recv_pair_req_pdu(uint8_t conidx, struct l2cc_pairing_req *pdu);
extern uint8_t gapc_smp_recv_pair_rsp_pdu(uint8_t conidx, struct l2cc_pairing_resp *pdu);
extern uint8_t gapc_smp_recv_public_key_pdu(uint8_t conidx, struct l2cc_public_key *pdu);
extern uint8_t gapc_smp_recv_sec_req_pdu(uint8_t conidx, struct l2cc_security_req *pdu);
extern uint8_t gapc_smp_recv_sign_info_pdu(uint8_t conidx, struct l2cc_signing_inf *pdu);
extern uint8_t gapc_smp_security_req_send(uint8_t conidx, uint8_t auth);
extern uint8_t gapc_smp_send_gen_dh_key_cmd(uint8_t conidx, uint8_t *operand_1, uint8_t *operand_2);
extern uint8_t gapc_smp_send_get_pub_key_cmd(uint8_t conidx);
extern uint8_t gapc_smp_send_ltk_req_rsp(uint8_t conidx, bool found, const uint8_t *key);
extern uint8_t gapc_smp_send_pairing_ind(uint8_t conidx, uint8_t ind_type, void *value);
extern uint8_t gapc_smp_send_pairing_req_ind(uint8_t conidx, uint8_t req_type);
extern uint8_t gapc_smp_send_start_enc_cmd(uint8_t conidx, uint8_t key_type, const uint8_t *key, const uint8_t *randnb, uint16_t ediv);
extern uint8_t gapc_smp_sign_command(uint8_t conidx, struct gapc_sign_cmd *param);
extern uint8_t gapc_smp_tkdp_rcp_continue(uint8_t conidx, uint8_t role);
extern uint8_t gapc_smp_tkdp_rcp_start(uint8_t conidx, uint8_t role);
extern uint8_t gapc_smp_tkdp_send_continue(uint8_t conidx, uint8_t role);
extern uint8_t gapc_smp_tkdp_send_start(uint8_t conidx, uint8_t role);
extern int gapm_gen_dh_key_cmd_handler(ke_msg_id_t const msgid, struct gapm_gen_dh_key_cmd * param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapm_gen_rand_addr_cmd_handler(ke_msg_id_t const msgid, struct gapm_gen_rand_addr_cmd *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapm_get_pub_key_cmd_handler(ke_msg_id_t const msgid, struct gapm_get_pub_key_cmd* param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapm_resolv_addr_cmd_handler(ke_msg_id_t const msgid, struct gapm_resolv_addr_cmd *param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int gapm_use_enc_block_cmd_handler(ke_msg_id_t const msgid, struct gapm_use_enc_block_cmd * param,ke_task_id_t const dest_id, ke_task_id_t const src_id);
extern int hci_le_enc_cmd_cmp_evt_handler(uint16_t opcode, struct hci_le_enc_cmd_cmp_evt *param);
extern int hci_le_gen_dhkey_cmp_evt_handler(uint16_t opcode, struct hci_le_gen_dhkey_cmp_evt const *param);
extern int hci_le_gen_dhkey_v2_stat_evt_handler(uint16_t opcode, struct hci_basic_cmd_cmp_evt const *p_event);
extern uint8_t gapm_smp_gen_rand_addr_cb(uint8_t status, const uint8_t* aes_res, uint32_t src_info);
extern uint8_t gapm_smp_gen_rand_nb_cb(uint8_t status, const uint8_t* aes_res, uint32_t src_info);
extern uint8_t gapm_smp_resolv_res_cb(uint8_t index, uint32_t src_info);
extern uint8_t gapm_smp_send_hci_encrypt(const uint8_t *key, const uint8_t* val);
extern uint8_t gapm_smp_use_enc_block_cb(uint8_t status, const uint8_t* aes_res, uint32_t src_info);
#endif


///////////////////////////////////////////P3:     App Patchs     ////////////////////////////////////////////////////

#if APP_PATCH_KE
uint8_t app_patch_ke_init(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_flush(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_ke_sleep_check(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
enum KE_STATUS app_patch_ke_stats_get(uint8_t* max_msg_sent,uint8_t* max_msg_saved,uint8_t* max_timer_used,uint16_t* max_heap_used){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_event_init(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_event_callback_set(uint8_t event_type, void (*p_callback)(void)){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_event_set(uint8_t event_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_event_clear(uint8_t event_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_event_get(uint8_t event_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint32_t app_patch_ke_event_get_all(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_event_flush(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_event_schedule(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_cmp_dest_id(struct co_list_hdr const *msg, uint32_t dest_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_patch_ke_msg_discard(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_patch_ke_msg_save(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
ke_msg_func_t app_patch_ke_handler_search(ke_msg_id_t const msg_id, struct ke_msg_handler const *msg_handler_tab, uint16_t msg_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
ke_msg_func_t app_patch_ke_task_handler_get(ke_msg_id_t const msg_id, ke_task_id_t const task_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
ke_state_t app_patch_ke_state_get(ke_task_id_t const id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
ke_task_id_t app_patch_ke_task_check(ke_task_id_t task){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_task_create(uint8_t task_type, struct ke_task_desc const *p_task_desc){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_task_delete(uint8_t task_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_state_set(ke_task_id_t const id, ke_state_t const state_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_task_init(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_task_msg_flush(ke_task_id_t task){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_task_saved_update(ke_task_id_t const ke_task_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_task_schedule(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t *app_patch_ke_msg_alloc(ke_msg_id_t const id, ke_task_id_t const dest_id, ke_task_id_t const src_id, uint16_t const param_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_msg_send(void const *param_ptr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_msg_send_basic(ke_msg_id_t const id, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_msg_forward(void const *param_ptr, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_msg_forward_new_id(void const *param_ptr, ke_msg_id_t const msg_id, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_msg_free(struct ke_msg const *msg){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
ke_msg_id_t app_patch_ke_msg_dest_id_get(void const *param_ptr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
ke_msg_id_t app_patch_ke_msg_src_id_get(void const *param_ptr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_ke_msg_in_queue(void const *param_ptr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_ke_mem_is_in_heap(uint8_t type, void* mem_ptr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_mem_init(uint8_t type, uint8_t* heap, uint16_t heap_size){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_ke_mem_is_empty(uint8_t type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_ke_check_malloc(uint32_t size, uint8_t type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
void *app_patch_ke_malloc(uint32_t size, uint8_t type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
void app_patch_ke_free(void* mem_ptr){printf("XXX, %s\r\n",__FUNCTION__); }
bool app_patch_ke_is_free(void* mem_ptr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint16_t app_patch_ke_get_mem_usage(uint8_t type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint32_t app_patch_ke_get_max_mem_usage(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
struct co_list_hdr *app_patch_ke_queue_extract(struct co_list * const queue,bool (*func)(struct co_list_hdr const * elmt, uint32_t arg), uint32_t arg){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_queue_insert(struct co_list * const queue, struct co_list_hdr * const element, bool (*cmp)(struct co_list_hdr const *elementA, struct co_list_hdr const *elementB)){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
rwip_time_t app_patch_ke_time(void){printf("XXX, %s\r\n",__FUNCTION__); }
bool app_patch_cmp_abs_time(struct co_list_hdr const * timerA, struct co_list_hdr const * timerB){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_cmp_timer_id(struct co_list_hdr const * timer, uint32_t timer_task){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_ke_time_past(rwip_time_t time){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_ke_timer_active(ke_msg_id_t const timer_id, ke_task_id_t const task_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_timer_adjust_all(uint32_t delay){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_timer_clear(ke_msg_id_t const timer_id, ke_task_id_t const task_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_timer_init(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_timer_schedule(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_ke_timer_set(ke_msg_id_t const timer_id, ke_task_id_t const task_id, uint32_t delay){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
#endif

#if APP_PATCH_CO
bool app_patch_co_list_extract(struct co_list *list, struct co_list_hdr *list_hdr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_co_list_find(struct co_list *list, struct co_list_hdr *list_hdr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
struct co_list_hdr *app_patch_co_list_pop_front(struct co_list *list){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint16_t app_patch_co_list_size(struct co_list *list){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_list_extract_after(struct co_list *list, struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_rem_hdr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_list_extract_sublist(struct co_list *list, struct co_list_hdr *ref_hdr, struct co_list_hdr *last_hdr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_list_init(struct co_list *list){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_list_insert_after(struct co_list *list, struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_add_hdr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_list_insert_before(struct co_list *list, struct co_list_hdr *elt_ref_hdr, struct co_list_hdr *elt_to_add_hdr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_list_merge(struct co_list *list1, struct co_list *list2){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_list_pool_init(struct co_list *list, void *pool, size_t elmt_size, uint32_t elmt_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_list_push_back(struct co_list *list, struct co_list_hdr *list_hdr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_list_push_back_sublist(struct co_list *list, struct co_list_hdr *first_hdr, struct co_list_hdr *last_hdr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_list_push_front(struct co_list *list, struct co_list_hdr *list_hdr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint16_t app_patch_co_util_read_array_size(char **fmt_cursor){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
void app_patch_co_bytes_to_string(char* dest, uint8_t* src, uint8_t nb_bytes){printf("XXX, %s\r\n",__FUNCTION__);}
bool app_patch_co_bdaddr_compare(struct bd_addr const *bd_address1, struct bd_addr const *bd_address2){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_nb_good_le_channels(const struct le_chnl_map* map){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_util_pack(uint8_t* out, uint8_t* in, uint16_t* out_len, uint16_t in_len, const char* format){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_co_util_unpack(uint8_t* out, uint8_t* in, uint16_t* out_len, uint16_t in_len, const char* format){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint16_t app_patch_co_ble_pkt_dur_in_us(uint8_t len, uint8_t rate){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
#endif // APP_PATCH_KE

#if APP_PATCH_AES
struct aes_func_env* app_patch_aes_alloc(uint16_t size, aes_func_continue_cb aes_continue_cb, aes_func_result_cb res_cb, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_aes_encrypt(const uint8_t* key, const uint8_t *val, bool copy, aes_func_result_cb res_cb, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_aes_init(uint8_t init_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_aes_start(struct aes_func_env* env, const uint8_t* key, const uint8_t *val){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
void app_patch_aes_app_encrypt(const uint8_t* key, const uint8_t *val, aes_func_result_cb res_cb, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_aes_app_decrypt(const uint8_t* key, const uint8_t *val, aes_func_result_cb res_cb, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); }
uint8_t app_patch_aes_rand(aes_func_result_cb res_cb, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_aes_result_handler(uint8_t status, uint8_t* result){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_aes_xor_128(uint8_t* result, const uint8_t* a, const uint8_t* b, uint8_t size){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_aes_shift_left_128(const uint8_t* input,uint8_t* output){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_aes_cmac(const uint8_t *key, const uint8_t *message, uint16_t message_len, aes_func_result_cb res_cb, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_aes_cmac_start(struct aes_cmac_env *env, const uint8_t *key, const uint8_t *message, uint16_t message_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_aes_cmac_continue(struct aes_cmac_env *env, uint8_t *aes_res){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
#endif // APP_PATCH_AES

#if APP_PATCH_SCH
uint8_t app_patch_sch_arb_conflict_check(struct sch_arb_elt_tag * evt_a, struct sch_arb_elt_tag * evt_b){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_arb_insert(struct sch_arb_elt_tag *elt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_arb_remove(struct sch_arb_elt_tag *elt, bool not_waiting){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_arb_elt_cancel(struct sch_arb_elt_tag *new_elt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_arb_event_start_isr(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_arb_init(uint8_t init_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_arb_prog_timer(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_arb_sw_isr(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_alarm_prog(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_alarm_init(uint8_t init_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_alarm_timer_isr(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_alarm_set(struct sch_alarm_tag* elt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_alarm_clear(struct sch_alarm_tag* elt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_prog_rx_isr(uint8_t et_idx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_prog_tx_isr(uint8_t et_idx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_prog_skip_isr(uint8_t et_idx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_prog_end_isr(uint8_t et_idx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_prog_init(uint8_t init_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_prog_fifo_isr(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_prog_push(struct sch_prog_params* params){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_slice_compute(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_slice_init(uint8_t init_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_slice_bg_add(uint8_t type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_slice_bg_remove(uint8_t type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_slice_fg_add(uint8_t type, uint32_t end_ts){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_slice_fg_remove(uint8_t type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_slice_per_add(uint8_t type, uint8_t id, uint32_t intv, uint16_t duration, bool retx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_sch_slice_per_remove(uint8_t type, uint8_t id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
#endif //APP_PATCH_SCH

#if APP_PATCH_LLD
bool app_patch_lld_calc_aux_rx(struct lld_calc_aux_rx_out* aux_rx_out, uint8_t index_pkt, uint32_t aux_data){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_lld_rxdesc_check(uint8_t label){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint32_t app_patch_lld_read_clock(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_ch_idx_get(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_local_sca_get(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_channel_assess(uint8_t channel, bool rx_ok, uint32_t timestamp){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_rpa_renew_evt_canceled_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_rpa_renew_evt_start_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_rpa_renew_instant_cbk(struct sch_alarm_tag* elt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_rxdesc_free(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint32_t app_patch_lld_adv_ext_pkt_prepare(uint8_t act_id, uint8_t txdesc_idx,
                                               uint8_t type, uint8_t mode, bool adva, bool targeta, bool adi,bool aux_ptr, bool sync_info, bool tx_power,
                                               uint16_t* ext_header_txbuf_offset, uint16_t* data_txbuf_offset, uint16_t* data_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_lld_adv_pkt_rx(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_end(uint8_t act_id, bool indicate, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_sync_info_set(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_aux_ch_idx_set(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_ext_chain_construct(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_adv_data_set(uint8_t act_id, uint8_t len, uint16_t data, bool release_old_buf){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_scan_rsp_data_set(uint8_t act_id, uint8_t len, uint16_t data, bool release_old_buf){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_evt_start_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_aux_evt_start_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_evt_canceled_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_aux_evt_canceled_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_frm_isr(uint8_t act_id, uint32_t timestamp, bool abort){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_frm_skip_isr(uint8_t act_id, uint32_t timestamp){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_adv_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_lld_scan_sync_accept(uint8_t scan_id, uint8_t index_pkt, uint8_t addr_type, struct bd_addr rx_adva, uint8_t rx_adv_sid){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_patch_lld_scan_sync_info_unpack(struct sync_info* p_syncinfo, uint16_t em_addr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_restart(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_aux_evt_canceled_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_aux_evt_start_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_aux_sched(uint8_t scan_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_end(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_evt_canceled_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_evt_start_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_frm_eof_isr(uint8_t scan_id, uint32_t timestamp, bool abort){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_frm_rx_isr(uint8_t scan_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_frm_skip_isr(uint8_t scan_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_process_pkt_rx(uint8_t scan_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_sched(uint8_t phy_idx, uint32_t timestamp, bool resched){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_scan_trunc_ind(struct lld_scan_evt_params* scan_evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint16_t app_patch_lld_init_compute_winoffset(uint16_t con_intv, uint16_t con_offset, uint8_t txwin_delay, uint32_t timestamp){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_init_connect_req_pack(uint8_t* p_data_pack, struct pdu_con_req_lldata* p_data){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_init_end(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_init_evt_canceled_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_init_evt_start_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_init_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_init_frm_eof_isr(uint8_t env_idx, uint32_t timestamp, bool abort){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_init_frm_skip_isr(uint8_t env_idx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_init_process_pkt_rx(uint8_t env_idx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_init_process_pkt_tx(uint8_t env_idx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_init_sched(uint8_t env_idx, uint32_t timestamp, bool resched){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
void app_patch_lld_con_max_lat_calc(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_tx_len_update(uint8_t link_id, uint8_t tx_rate, uint32_t con_intv){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_evt_time_update(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_cleanup(uint8_t link_id, bool indicate, uint8_t reason){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_instant_proc_end(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_sched(uint8_t link_id, uint32_t clock, bool sync){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_rx(uint8_t link_id, uint32_t timestamp){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_tx(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_tx_prog(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_evt_start_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_evt_canceled_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_frm_isr(uint8_t link_id, uint32_t timestamp, bool abort){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_rx_isr(uint8_t link_id, uint32_t timestamp){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_tx_isr(uint8_t link_id, uint32_t timestamp){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_frm_skip_isr(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); }
void app_patch_lld_con_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type){printf("XXX, %s\r\n",__FUNCTION__); }
uint8_t app_patch_lld_sync_cleanup(uint8_t act_id, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_sync_trunc_ind(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_sync_process_pkt_rx(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_sync_sched(uint8_t act_id, bool resched){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_sync_frm_eof_isr(uint8_t act_id, uint32_t timestamp, bool abort){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_sync_frm_rx_isr(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_sync_frm_skip_isr(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_sync_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_sync_evt_start_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_sync_evt_canceled_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_sync_ant_switch_config(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_per_adv_cleanup(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_per_adv_sched(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_per_adv_chain_construct(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_per_adv_data_set(uint8_t act_id, uint8_t len, uint16_t data, bool release_old_buf){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_per_adv_evt_start_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_per_adv_evt_canceled_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_per_adv_frm_isr(uint8_t act_id, uint32_t timestamp, bool abort){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_per_adv_frm_skip_isr(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_per_adv_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_per_adv_ant_switch_config(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_per_adv_sync_info_get(uint8_t act_id, uint32_t* sync_ind_ts, uint16_t* pa_evt_cnt, struct le_chnl_map *map){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_per_adv_init_info_get(uint8_t act_id, struct access_addr* aa, struct crc_init* crcinit, struct le_chnl_map* chm){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint32_t app_patch_lld_per_adv_ext_pkt_prepare(uint8_t act_id, uint8_t txdesc_idx,
                                                  uint8_t type, uint8_t mode, bool tx_power,
                                                  uint16_t* ext_header_txbuf_offset, uint16_t* data_txbuf_offset,
                                                  uint16_t* data_len, bool force_aux_ptr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_test_freq2chnl(uint8_t freq){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_test_cleanup(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_test_evt_canceled_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_test_evt_start_cbk(struct sch_arb_elt_tag* evt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_test_frm_cbk(uint32_t timestamp, uint32_t dummy, uint8_t irq_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_test_frm_isr(uint32_t timestamp, bool abort){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_patch_lld_test_rx_isr(uint32_t timestamp){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
#endif// APP_PATCH_LLD

#if APP_PATCH_RPA
struct aes_rpa_gen_env;
struct aes_rpa_resolve_env;
uint8_t app_aes_rpa_gen_continue_patch(struct aes_rpa_gen_env *env, uint8_t *aes_res){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_aes_rpa_resolve_continue_patch(struct aes_rpa_resolve_env *env, uint8_t *aes_res){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_aes_rpa_gen_patch(struct irk *irk, aes_func_result_cb res_cb, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_aes_rpa_resolve_patch(uint8_t nb_irk, struct irk *irk, struct bd_addr *addr, aes_rpa_func_result_cb res_cb, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
#endif

#if (APP_PATCH_SCH_PLAN)
uint8_t app_sch_plan_init_patch(uint8_t init_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint32_t app_sch_plan_offset_max_calc_patch(uint32_t interval, uint32_t duration, uint32_t offset_min, uint16_t conhdl){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_sch_plan_offset_req_patch(uint8_t action, bool dur_max, struct sch_plan_req_param *req_param, struct sch_plan_chk_param *chk_param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_sch_plan_interval_req_patch(struct sch_plan_req_param *req_param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_sch_plan_position_range_compute_patch(uint8_t link_id, uint8_t role, uint16_t interval, uint16_t duration, uint16_t *min_offset, uint16_t *max_offset){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_sch_plan_clock_wrap_offset_update_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_sch_plan_set_patch(struct sch_plan_elt_tag *elt_to_add){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_sch_plan_rem_patch(struct sch_plan_elt_tag *elt_to_remove){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_sch_plan_req_patch(struct sch_plan_req_param* req_param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_sch_plan_chk_patch(struct sch_plan_chk_param* chk_param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
#endif

#if APP_PATCH_HCI
uint8_t app_hci_send_2_host_patch(void *param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_send_2_controller_patch(void *param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_hci_evt_mask_check_patch(struct ke_msg *msg){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_init_patch(uint8_t init_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_ble_conhdl_register_patch(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_ble_conhdl_unregister_patch(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_evt_mask_set_patch(struct evt_mask const *evt_msk, uint8_t page){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_evt_filter_add_patch(struct hci_set_evt_filter_cmd const *param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
struct hci_cmd_desc_tag* app_hci_look_for_cmd_desc_patch(uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
struct hci_evt_desc_tag* app_hci_look_for_evt_desc_patch(uint8_t code){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
struct hci_evt_desc_tag* app_hci_look_for_le_evt_desc_patch(uint8_t subcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_pack_bytes_patch(uint8_t** pp_in, uint8_t** pp_out, uint8_t* p_in_end, uint8_t* p_out_end, uint8_t len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_host_nb_cmp_pkts_cmd_pkupk_patch(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_le_set_ext_scan_param_cmd_upk_patch(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_le_ext_create_con_cmd_upk_patch(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_le_set_ext_adv_en_cmd_upk_patch(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_le_adv_report_evt_pkupk_patch(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_le_dir_adv_report_evt_pkupk_patch(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_le_ext_adv_report_evt_pkupk_patch(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_le_conless_iq_report_evt_pkupk_patch(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_le_con_iq_report_evt_pkupk_patch(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_hci_ble_adv_report_filter_check_patch(struct ke_msg * msg){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_ble_adv_report_tx_check_patch(struct ke_msg * msg){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_cmd_reject_patch(const struct hci_cmd_desc_tag* cmd_desc, uint16_t opcode, uint8_t error, uint8_t * payload){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_cmd_received_patch(uint16_t opcode, uint8_t length, uint8_t *payload){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_acl_tx_data_received_patch(uint16_t hdl_flags, uint16_t datalen, uint8_t * payload){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t* app_hci_build_cs_evt_patch(struct ke_msg * msg){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t* app_hci_build_cc_evt_patch(struct ke_msg * msg){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t* app_hci_build_evt_patch(struct ke_msg * msg){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t* app_hci_build_le_evt_patch(struct ke_msg * msg){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t* app_hci_build_acl_data_patch(struct ke_msg * msg){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t* app_hci_acl_tx_data_alloc_patch(uint16_t hdl_flags, uint16_t len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_tx_start_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_tx_done_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_tx_trigger_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_tl_send_patch(struct ke_msg *msg){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_tl_init_patch(uint8_t init_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_hci_cmd_get_max_param_size_patch(uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
#endif

#if APP_PATCH_LLC
uint8_t app_llc_loc_clk_acc_proc_continue_patch(uint8_t link_id, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_clk_acc_req_pdu_send_patch(uint8_t link_id, uint8_t sca){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_clk_acc_rsp_pdu_send_patch(uint8_t link_id, uint8_t sca){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_clk_acc_proc_err_cb_patch(uint8_t link_id, uint8_t error_type, void* param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_clk_acc_req_handler_patch(uint8_t link_id, struct ll_clk_acc_req *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_clk_acc_rsp_handler_patch(uint8_t link_id, struct ll_clk_acc_rsp *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llc_op_clk_acc_ind_handler_patch(ke_msg_id_t const msgid, struct llc_op_clk_acc_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_clk_acc_modify_patch(uint8_t link_id, uint8_t action){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_loc_con_upd_proc_continue_patch(uint8_t link_id, uint8_t state, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_rem_con_upd_proc_continue_patch(uint8_t link_id, uint8_t state, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_pref_param_compute_patch(uint8_t link_id, struct llc_op_con_upd_ind* param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_hci_con_upd_info_send_patch(uint8_t link_id, uint8_t status, struct llc_op_con_upd_ind *param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_llc_con_upd_param_in_range_patch(uint8_t link_id, uint16_t interval_max, uint16_t interval_min, uint16_t latency, uint16_t timeout){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_connection_update_ind_pdu_send_patch(uint8_t link_id, struct llc_op_con_upd_ind *param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_connection_param_req_pdu_send_patch(uint8_t link_id, struct llc_op_con_upd_ind *param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_connection_param_rsp_pdu_send_patch(uint8_t link_id, struct llc_op_con_upd_ind *param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_hci_con_param_req_evt_send_patch(uint8_t link_id, uint16_t con_intv_min, uint16_t con_intv_max, uint16_t con_latency, uint16_t superv_to){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_loc_con_upd_proc_err_cb_patch(uint8_t link_id, uint8_t error_type, void *param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_rem_con_upd_proc_err_cb_patch(uint8_t link_id, uint8_t error_type, void *param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_connection_update_ind_handler_patch(uint8_t link_id, struct ll_connection_update_ind *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_connection_param_req_handler_patch(uint8_t link_id, struct ll_connection_param_req *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_connection_param_rsp_handler_patch(uint8_t link_id, struct ll_connection_param_rsp *param, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_con_upd_cmd_handler_patch(uint8_t link_id, struct hci_le_con_update_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rem_con_param_req_reply_cmd_handler_patch(uint8_t link_id, struct hci_le_rem_con_param_req_reply_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rem_con_param_req_neg_reply_cmd_handler_patch(uint8_t link_id, struct hci_le_rem_con_param_req_neg_reply_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llc_op_con_upd_ind_handler_patch(ke_msg_id_t const msgid, struct llc_op_con_upd_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_con_param_upd_cfm_handler_patch(ke_msg_id_t const msgid, void *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_con_estab_ind_handler_patch(ke_msg_id_t const msgid, struct lld_con_estab_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_con_offset_upd_ind_handler_patch(ke_msg_id_t const msgid, struct lld_con_offset_upd_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_con_move_cbk_patch(uint16_t id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_disconnect_proc_continue_patch(uint8_t link_id, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llc_op_disconnect_ind_handler_patch(ke_msg_id_t const msgid, struct llc_op_disconnect_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_disconnect_cmd_handler_patch(uint8_t link_id, struct hci_disconnect_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_disconnect_end_patch(uint8_t link_id, uint8_t status, uint8_t reason){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_terminate_ind_pdu_send_patch(uint16_t link_id, uint8_t reason){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_disconnect_proc_err_cb_patch(uint8_t link_id, uint8_t error_type, void* param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_terminate_ind_handler_patch(uint8_t link_id, struct ll_terminate_ind *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_disc_ind_handler_patch(ke_msg_id_t const msgid, struct lld_disc_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llc_stopped_ind_handler_patch(ke_msg_id_t const msgid, void *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_disconnect_patch(uint8_t link_id, uint8_t reason, bool immediate){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_init_term_proc_patch(uint8_t link_id, uint8_t reason){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_llcp_send_patch(uint8_t link_id, union llcp_pdu *pdu, llc_llcp_tx_cfm_cb tx_cfm_cb){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_llcp_tx_check_patch(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_reject_ind_pdu_send_patch(uint8_t link_id, uint8_t rej_op_code, uint8_t reason, llc_llcp_tx_cfm_cb cfm_cb){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_llcp_rx_ind_handler_patch(ke_msg_id_t const msgid, struct lld_llcp_rx_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_llcp_tx_cfm_handler_patch(ke_msg_id_t const msgid, void *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_past_sync_info_unpack_patch(struct sync_info* p_syncinfo_struct, uint8_t * p_syncinfo_tab){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_past_rx_patch(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_past_rpa_res_cb_patch(uint8_t index, uint32_t src_info) {printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llc_op_past_ind_handler_patch(ke_msg_id_t const msgid, struct llc_op_past_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_ll_per_sync_ind_handler_patch(uint8_t link_id, struct ll_per_sync_ind *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_per_adv_sync_transf_cmd_handler_patch(uint8_t link_id, struct hci_le_per_adv_sync_transf_cmd const *param, uint16_t opcode) {printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_per_adv_set_info_transf_cmd_handler_patch(uint8_t link_id, struct hci_le_per_adv_set_info_transf_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_per_adv_sync_transf_param_cmd_handler_patch(uint8_t link_id, struct hci_le_set_per_adv_sync_transf_param_cmd const *param, uint16_t opcode) {printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_loc_dl_upd_proc_continue_patch(uint8_t link_id, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_rem_dl_upd_proc_patch(uint8_t link_id, uint16_t max_tx_time, uint16_t max_tx_octets, uint16_t max_rx_time, uint16_t max_rx_octets){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_ll_length_req_handler_patch(uint8_t link_id, struct ll_length_req *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_data_len_cmd_handler_patch(uint8_t link_id, struct hci_le_set_data_len_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_dl_upd_proc_start_patch(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_length_req_pdu_send_patch(uint8_t link_id, uint16_t eff_tx_time, uint16_t eff_tx_octets, uint16_t eff_rx_time, uint16_t eff_rx_octets){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_length_rsp_pdu_send_patch(uint8_t link_id, uint16_t eff_tx_time, uint16_t eff_tx_octets, uint16_t eff_rx_time, uint16_t eff_rx_octets){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_hci_dl_upd_info_send_patch(uint8_t link_id, uint8_t status, uint16_t max_tx_time, uint16_t max_tx_octets, uint16_t max_rx_time, uint16_t max_rx_octets){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_dle_proc_err_cb_patch(uint8_t link_id, uint8_t error_type, void* param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_ll_length_rsp_handler_patch(uint8_t link_id, struct ll_length_rsp *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llc_op_dl_upd_ind_handler_patch(ke_msg_id_t const msgid, struct llc_op_dl_upd_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_loc_encrypt_proc_continue_patch(uint8_t link_id, uint8_t state, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_rem_encrypt_proc_continue_patch(uint8_t link_id, uint8_t state, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_aes_res_cb_patch(uint8_t status, const uint8_t* aes_res, uint32_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_pause_enc_req_pdu_send_patch(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_pause_enc_rsp_pdu_send_patch(uint8_t link_id, llc_llcp_tx_cfm_cb cfm_cb){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_enc_req_pdu_send_patch(uint8_t link_id, uint16_t ediv, uint8_t* rand, uint8_t* skdm, uint8_t* ivm){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_enc_rsp_pdu_send_patch(uint8_t link_id, uint8_t* skds, uint8_t* ivs){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_start_enc_req_pdu_send_patch(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_start_enc_rsp_pdu_send_patch(uint8_t link_id, llc_llcp_tx_cfm_cb cfm_cb){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_pause_enc_rsp_ack_handler_patch(uint8_t link_id, uint8_t op_code){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_start_enc_rsp_ack_handler_patch(uint8_t link_id, uint8_t op_code){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_reject_ind_ack_handler_patch(uint8_t link_id, uint8_t op_code){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_hci_ltk_request_evt_send_patch(uint8_t link_id, uint16_t enc_div, uint8_t* randnb){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_hci_enc_evt_send_patch(uint8_t link_id, uint8_t status, bool re_encrypt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_loc_encrypt_proc_err_cb_patch(uint8_t link_id, uint8_t error_type, void* param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_rem_encrypt_proc_err_cb_patch(uint8_t link_id, uint8_t error_type, void* param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_pause_enc_req_handler_patch(uint8_t link_id, struct ll_pause_enc_req *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_pause_enc_rsp_handler_patch(uint8_t link_id, struct ll_pause_enc_rsp *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_enc_req_handler_patch(uint8_t link_id, struct ll_enc_req *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_enc_rsp_handler_patch(uint8_t link_id, struct ll_enc_rsp *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_start_enc_req_handler_patch(uint8_t link_id, struct ll_start_enc_req *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_start_enc_rsp_handler_patch(uint8_t link_id, struct ll_start_enc_rsp *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_start_enc_cmd_handler_patch(uint8_t link_id, struct hci_le_start_enc_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_ltk_req_reply_cmd_handler_patch(uint8_t link_id, struct hci_le_ltk_req_reply_cmd const *cmd, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_ltk_req_neg_reply_cmd_handler_patch(uint8_t link_id, struct hci_le_ltk_req_neg_reply_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llc_encrypt_ind_handler_patch(ke_msg_id_t const msgid, struct llc_encrypt_ind const *ind,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llc_op_encrypt_ind_handler_patch(ke_msg_id_t const msgid, struct llc_op_encrypt_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_loc_feats_exch_proc_continue_patch(uint8_t link_id, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_feature_req_pdu_send_patch(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ll_feature_rsp_pdu_send_patch(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_hci_feats_info_send_patch(uint8_t link_id, uint8_t status, struct features * feats){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_feats_exch_proc_err_cb_patch(uint8_t link_id, uint8_t error_type, void* param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_feature_req_handler_patch(uint8_t link_id, struct ll_feature_req *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_slave_feature_req_handler_patch(uint8_t link_id, struct ll_slave_feature_req *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_feature_rsp_handler_patch(uint8_t link_id, struct ll_feature_rsp *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_rem_feats_cmd_handler_patch(uint8_t link_id, struct hci_le_rd_rem_feats_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llc_op_feats_exch_ind_handler_patch(ke_msg_id_t const msgid, struct llc_op_feats_exch_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_acl_data_handler_patch(ke_msg_id_t const msgid, struct hci_acl_data *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_acl_rx_ind_handler_patch(ke_msg_id_t const msgid, struct lld_acl_rx_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_hci_nb_cmp_pkts_evt_send_patch(uint8_t link_id, uint8_t nb_of_pkt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_rd_tx_pwr_lvl_cmd_handler_patch(uint8_t link_id, struct hci_rd_tx_pwr_lvl_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_rd_rssi_cmd_handler_patch(uint8_t link_id, struct hci_basic_conhdl_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_vs_set_pref_slave_latency_cmd_handler_patch(uint8_t link_id, struct hci_vs_set_pref_slave_latency_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_vs_set_pref_slave_evt_dur_cmd_handler_patch(uint8_t link_id, struct hci_vs_set_pref_slave_evt_dur_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_command_handler_patch(ke_msg_id_t const msgid, uint16_t const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_loc_phy_upd_proc_continue_patch(uint8_t link_id, uint8_t state, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_rem_phy_upd_proc_continue_patch(uint8_t link_id, uint8_t state, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_dl_chg_check_patch(uint8_t link_id, uint8_t old_tx_phy, uint8_t old_rx_phy){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_loc_phy_upd_proc_err_cb_patch(uint8_t link_id, uint8_t error_type, void* param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_rem_phy_upd_proc_err_cb_patch(uint8_t link_id, uint8_t error_type, void* param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_phy_req_handler_patch(uint8_t link_id, struct ll_phy_req *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_phy_rsp_handler_patch(uint8_t link_id, struct ll_phy_rsp *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_phy_update_ind_handler_patch(uint8_t link_id, struct ll_phy_update_ind *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_phy_cmd_handler_patch(uint8_t link_id, struct hci_le_rd_phy_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_phy_cmd_handler_patch(uint8_t link_id, struct hci_le_set_phy_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llc_op_phy_upd_ind_handler_patch(ke_msg_id_t const msgid, struct llc_op_phy_upd_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_phy_upd_cfm_handler_patch(ke_msg_id_t const msgid, void *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_phy_upd_proc_start_patch(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ver_exch_loc_proc_continue_patch(uint8_t link_id, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llc_op_ver_exch_ind_handler_patch(ke_msg_id_t const msgid, struct llc_op_ver_exch_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_version_ind_handler_patch(uint8_t link_id, struct ll_version_ind *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_rd_rem_ver_info_cmd_handler_patch(uint8_t link_id, struct hci_rd_rem_ver_info_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}

#endif

#if APP_PATCH_LLM
uint8_t app_llm_adv_set_release_patch(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_adv_rpa_gen_cb_patch(uint8_t status, const uint8_t* aes_res, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_adv_con_len_check_patch(struct hci_le_set_ext_adv_param_cmd* ext_param, uint16_t adv_data_len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_adv_set_dft_params_patch(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_adv_hdl_to_id_patch(uint16_t adv_hdl, struct hci_le_set_ext_adv_param_cmd** adv_param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_adv_param_cmd_handler_patch(struct hci_le_set_adv_param_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_adv_data_cmd_handler_patch(struct hci_le_set_adv_data_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_scan_rsp_data_cmd_handler_patch(struct hci_le_set_scan_rsp_data_cmd const *param, uint16_t opcode) {printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_adv_en_cmd_handler_patch(struct hci_le_set_adv_en_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_ext_adv_param_cmd_handler_patch(struct hci_le_set_ext_adv_param_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_adv_set_rand_addr_cmd_handler_patch(struct hci_le_set_adv_set_rand_addr_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_ext_adv_data_cmd_handler_patch(struct hci_le_set_ext_adv_data_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_ext_scan_rsp_data_cmd_handler_patch(struct hci_le_set_ext_scan_rsp_data_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_ext_adv_en_cmd_handler_patch(struct hci_le_set_ext_adv_en_cmd *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_per_adv_param_cmd_handler_patch(struct hci_le_set_per_adv_param_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_per_adv_data_cmd_handler_patch(struct hci_le_set_per_adv_data_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_per_adv_en_cmd_handler_patch(struct hci_le_set_per_adv_en_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_max_adv_data_len_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_nb_supp_adv_sets_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rmv_adv_set_cmd_handler_patch(struct hci_le_rem_adv_set_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_clear_adv_sets_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_conless_cte_tx_param_cmd_handler_patch(struct hci_le_set_conless_cte_tx_param_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_conless_cte_tx_en_cmd_handler_patch(struct hci_le_set_conless_cte_tx_en_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_adv_act_id_get_patch(uint8_t adv_hdl, uint8_t* ext_adv_id, uint8_t* per_adv_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_adv_set_id_get_patch(uint8_t act_id, uint8_t* sid, uint8_t* atype, struct bd_addr* adva){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_adv_ch_tx_pw_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_scan_req_ind_handler_patch(ke_msg_id_t const msgid, struct lld_scan_req_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_adv_end_ind_handler_patch(ke_msg_id_t const msgid, struct lld_adv_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_per_adv_end_ind_handler_patch(ke_msg_id_t const msgid, struct lld_per_adv_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_llm_is_dev_connected_patch(struct bd_addr const *peer_addr, uint8_t peer_addr_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_llm_is_dev_synced_patch(struct bd_addr const *peer_addr, uint8_t peer_addr_type, uint8_t adv_sid){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_link_disc_patch(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_ch_map_update_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_dev_list_empty_entry_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_dev_list_search_patch(const struct bd_addr *bd_addr, uint8_t bd_addr_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_ral_search_patch(const struct bd_addr *bd_addr, uint8_t bd_addr_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_llm_ral_is_empty_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_init_patch(uint8_t init_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_clk_acc_set_patch(uint8_t act_id, bool clk_acc){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_activity_free_get_patch(uint8_t* act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_activity_free_set_patch(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_llm_is_wl_accessible_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_llm_is_non_con_act_ongoing_check_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_llm_is_rand_addr_in_use_check_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_reset_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_rd_local_ver_info_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_rd_bd_addr_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_rd_local_supp_cmds_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_rd_local_supp_feats_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_set_evt_mask_cmd_handler_patch(struct hci_set_evt_mask_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_set_evt_mask_page_2_cmd_handler_patch(struct hci_set_evt_mask_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_set_ctrl_to_host_flow_ctrl_cmd_handler_patch(struct hci_set_ctrl_to_host_flow_ctrl_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_host_buf_size_cmd_handler_patch(struct hci_host_buf_size_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_host_nb_cmp_pkts_cmd_handler_patch(struct hci_host_nb_cmp_pkts_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_evt_mask_cmd_handler_patch(struct hci_le_set_evt_mask_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_buff_size_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_local_supp_feats_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_rand_addr_cmd_handler_patch(struct hci_le_set_rand_addr_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_host_ch_class_cmd_handler_patch(struct hci_le_set_host_ch_class_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_wl_size_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_clear_wlst_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_add_dev_to_wlst_cmd_handler_patch(struct hci_le_add_dev_to_wlst_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rmv_dev_from_wlst_cmd_handler_patch(struct hci_le_rmv_dev_from_wlst_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_rslv_list_size_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_clear_rslv_list_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_add_dev_to_rslv_list_cmd_handler_patch(struct hci_le_add_dev_to_rslv_list_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rmv_dev_from_rslv_list_cmd_handler_patch(struct hci_le_rmv_dev_from_rslv_list_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_rslv_priv_addr_to_cmd_handler_patch(struct hci_le_set_rslv_priv_addr_to_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_addr_resol_en_cmd_handler_patch(struct hci_le_set_addr_resol_en_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_peer_rslv_addr_cmd_handler_patch(struct hci_le_rd_peer_rslv_addr_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_loc_rslv_addr_cmd_handler_patch(struct hci_le_rd_loc_rslv_addr_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_aes_res_cb_patch(uint8_t status, const uint8_t* aes_res, uint32_t dummy){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_enc_cmd_handler_patch(struct hci_le_enc_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rand_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_supp_states_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_tx_pwr_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_rf_path_comp_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_wr_rf_path_comp_cmd_handler_patch(struct hci_le_wr_rf_path_comp_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_priv_mode_cmd_handler_patch(struct hci_le_set_priv_mode_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_mod_sleep_clk_acc_cmd_handler_patch(struct hci_le_mod_sleep_clk_acc_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_antenna_inf_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_suggted_dft_data_len_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_wr_suggted_dft_data_len_cmd_handler_patch(struct hci_le_wr_suggted_dft_data_len_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_max_data_len_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_local_p256_public_key_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_gen_dhkey_v1_cmd_handler_patch(struct hci_le_gen_dhkey_v1_cmd *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_gen_dhkey_v2_cmd_handler_patch(struct hci_le_gen_dhkey_v2_cmd *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_dft_phy_cmd_handler_patch(struct hci_le_set_dft_phy_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_dft_per_adv_sync_transf_param_cmd_handler_patch(struct hci_le_set_dft_per_adv_sync_transf_param_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llm_hci_command_handler_patch(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_create_con_cmd_handler_patch(struct hci_le_create_con_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_ext_create_con_cmd_handler_patch(struct hci_le_ext_create_con_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_create_con_cancel_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_init_end_ind_handler_patch(ke_msg_id_t const msgid, struct lld_init_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_scan_start_patch(uint8_t act_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_llm_adv_reports_list_check_patch(struct bd_addr const *adv_bd_addr, uint8_t addr_type, uint8_t adv_evt_type, uint8_t sid, uint16_t did, uint16_t per_adv_intv){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_scan_sync_acad_attach_patch(uint8_t sync_act_id, uint8_t ad_type, uint16_t task){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_scan_sync_info_get_patch(uint8_t sync_act_id, uint8_t* sid, uint8_t* atype, struct bd_addr* adva){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llm_per_adv_sync_patch(struct llm_per_adv_sync_params * params){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_scan_param_cmd_handler_patch(struct hci_le_set_scan_param_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_scan_en_cmd_handler_patch(struct hci_le_set_scan_en_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_ext_scan_param_cmd_handler_patch(struct hci_le_set_ext_scan_param_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_ext_scan_en_cmd_handler_patch(struct hci_le_set_ext_scan_en_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_per_adv_create_sync_cmd_handler_patch(struct hci_le_per_adv_create_sync_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_per_adv_create_sync_cancel_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_per_adv_term_sync_cmd_handler_patch(struct hci_le_per_adv_term_sync_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_add_dev_to_per_adv_list_cmd_handler_patch(struct hci_le_add_dev_to_per_adv_list_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rmv_dev_from_per_adv_list_cmd_handler_patch(struct hci_le_rmv_dev_from_per_adv_list_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_clear_per_adv_list_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_per_adv_list_size_cmd_handler_patch(void const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_conless_iq_sampl_en_cmd_handler_patch(struct hci_le_set_conless_iq_sampl_en_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_set_per_adv_rec_en_cmd_handler_patch(struct hci_le_set_per_adv_rec_en_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llm_scan_period_to_handler_patch(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_adv_rep_ind_handler_patch(ke_msg_id_t const msgid, struct lld_adv_rep_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_sync_start_req_handler_patch(ke_msg_id_t const msgid, struct lld_sync_start_req const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_per_adv_rep_ind_handler_patch(ke_msg_id_t const msgid, struct lld_per_adv_rep_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_per_adv_rx_end_ind_handler_patch(ke_msg_id_t const msgid, struct lld_per_adv_rx_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_scan_end_ind_handler_patch(ke_msg_id_t const msgid, struct lld_scan_end_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_conless_cte_rx_ind_handler_patch(ke_msg_id_t const msgid, struct lld_conless_cte_rx_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llm_rpa_renew_to_handler_patch(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llm_pub_key_gen_ind_handler_patch(ke_msg_id_t const msgid, struct ecc_result_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llm_dh_key_gen_ind_handler_patch(ke_msg_id_t const msgid, struct ecc_result_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llm_encrypt_ind_handler_patch(ke_msg_id_t const msgid, struct llm_encrypt_ind const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
#endif

#if APP_PATCH_LLC_CHMAP
uint8_t app_llc_loc_ch_map_proc_continue_patch(uint8_t link_id, uint8_t state, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_rem_ch_map_proc_continue_patch(uint8_t link_id, uint8_t state, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_ch_map_up_proc_err_cb_patch(uint8_t link_id, uint8_t error_type, void* param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_channel_map_ind_handler_patch(uint8_t link_id, struct ll_channel_map_ind *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_ll_min_used_channels_ind_handler_patch(uint8_t link_id, struct ll_min_used_channels_ind *pdu, uint16_t event_cnt){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_rd_chnl_map_cmd_handler_patch(uint8_t link_id, struct hci_basic_conhdl_cmd const *param, uint16_t opcode){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llc_op_ch_map_upd_ind_handler_patch(ke_msg_id_t const msgid, struct llc_op_ch_map_upd_ind *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_llm_ch_map_update_ind_handler_patch(ke_msg_id_t const msgid, void *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_lld_ch_map_upd_cfm_handler_patch(ke_msg_id_t const msgid, void *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_cleanup_patch(uint8_t link_id, bool reset){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_start_patch(uint8_t link_id, struct llc_init_parameters *con_params){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_llcp_trans_timer_set_patch(uint8_t link_id, uint8_t proc_type, bool enable){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_init_patch(uint8_t init_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_stop_patch(uint8_t link_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_llcp_state_set_patch(uint8_t link_id, uint8_t dir, uint8_t state){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_proc_reg_patch(uint8_t link_id, uint8_t proc_type, llc_procedure_t *params){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_proc_unreg_patch(uint8_t link_id, uint8_t proc_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_proc_id_get_patch(uint8_t link_id, uint8_t proc_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_proc_state_set_patch(llc_procedure_t *proc, uint8_t link_id, uint8_t proc_state){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
llc_procedure_t * app_llc_proc_get_patch(uint8_t link_id, uint8_t proc_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_proc_err_ind_patch(uint8_t link_id, uint8_t proc_type, uint8_t error_type, void* param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_proc_timer_set_patch(uint8_t link_id, uint8_t proc_type, bool enable){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_proc_timer_pause_set_patch(uint8_t link_id, uint8_t proc_type, bool enable){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_llc_proc_collision_check_patch(uint8_t link_id, uint8_t proc_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}

#endif

#if APP_PATCH_NVDS
bool app_nvds_is_magic_number_ok_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_walk_tag_patch (uint32_t cur_tag_addr,struct nvds_tag_header *nvds_tag_header_ptr,uint32_t *nxt_tag_addr_ptr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_browse_tag_patch (uint8_t tag,struct nvds_tag_header *nvds_tag_header_ptr,uint32_t *tag_address_ptr){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_purge_patch(uint32_t length, uint8_t* buf){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_init_patch(uint8_t *base, uint32_t len){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_get_patch(uint8_t tag, nvds_tag_len_t * lengthPtr, uint8_t *buf){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_lock_patch(uint8_t tag){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_put_patch(uint8_t tag, nvds_tag_len_t length, uint8_t *buf){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_del_patch(uint8_t tag){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_null_init_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_read_patch(uint32_t address, uint32_t length, uint8_t *buf){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_write_patch(uint32_t address, uint32_t length, uint8_t *buf){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_erase_patch(uint32_t address, uint32_t length){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_nvds_init_memory_patch(void){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
#endif

#if APP_PATCH_L2CC
uint8_t app_l2cc_init_patch(uint8_t init_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_create_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_cleanup_patch(uint8_t conidx, bool reset){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_update_state_patch(uint8_t conidx, ke_state_t state, bool busy){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_op_complete_patch(uint8_t conidx, uint8_t op_type, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_send_cmp_evt_patch(uint8_t conidx, uint8_t operation, const ke_task_id_t requester, uint8_t status, uint16_t cid,uint16_t credit){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_set_operation_ptr_patch(uint8_t conidx, uint8_t op_type, void* op){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_data_send_patch(uint8_t conidx, uint8_t nb_buffer){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_get_operation_patch(uint8_t conidx, uint8_t op_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
void* app_l2cc_get_operation_ptr_patch(uint8_t conidx, uint8_t op_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_lecb_free_patch(uint8_t conidx, struct l2cc_lecb_info* lecb, bool disconnect_ind){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_lecb_send_con_req_patch(uint8_t conidx, uint8_t pkt_id, uint16_t le_psm, uint16_t scid, uint16_t credits,uint16_t mps, uint16_t mtu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_lecb_send_con_rsp_patch(uint8_t conidx, uint16_t status, uint8_t pkt_id,uint16_t dcid, uint16_t credits, uint16_t mps, uint16_t mtu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_lecb_send_disc_req_patch(uint8_t conidx, uint8_t pkt_id, uint16_t scid, uint16_t dcid){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_lecb_send_disc_rsp_patch(uint8_t conidx, uint8_t pkt_id, uint16_t dcid, uint16_t scid){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_lecb_send_credit_add_patch(uint8_t conidx, uint8_t pkt_id, uint16_t cid, uint16_t credits){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_lecb_init_disconnect_patch(uint8_t conidx, struct l2cc_lecb_info* lecb, uint8_t disc_reason){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint16_t app_l2cc_lecb_h2l_err_patch(uint8_t h_err){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
struct l2cc_lecb_info* app_l2cc_lecb_find_patch(uint8_t conidx, uint8_t field, uint16_t value){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_pdu_pack_patch(struct l2cc_pdu *p_pdu, uint16_t *p_offset, uint16_t *p_length, uint8_t *p_buffer, uint8_t *pb_flag){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_pdu_unpack_patch(uint8_t conidx, struct l2cc_pdu *p_pdu, uint16_t *p_offset, uint16_t *p_rem_len,const uint8_t *p_buffer, uint16_t pkt_length, uint8_t pb_flag){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_lecb_pdu_pack_patch(uint8_t conidx, struct l2cc_sdu *sdu, uint16_t *length, uint8_t *buffer, uint16_t* offset,uint16_t pdu_len, uint8_t pb_flag){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_lecb_pdu_unpack_patch(uint8_t conidx, struct l2cc_sdu *sdu, uint8_t *buffer, uint16_t length, uint16_t* offset,uint16_t* pdu_remain_len, uint8_t pb_flag){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_pdu_rx_init_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_pdu_header_check_patch(uint8_t conidx, uint8_t** pp_buffer, uint16_t* p_rx_length){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_sig_send_cmd_reject_patch(uint8_t conidx, uint8_t pkt_id, uint16_t reason,uint16_t opt1, uint16_t opt2){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cc_sig_lecb_l2h_err_patch(uint16_t l_err){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2c_code_reject_handler_patch(uint8_t conidx, struct l2cc_reject *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2c_code_conn_param_upd_req_handler_patch(uint8_t conidx, struct l2cc_conn_param_upd_req *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2c_code_conn_param_upd_resp_handler_patch(uint8_t conidx, struct l2cc_conn_param_upd_resp *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2c_code_le_cb_conn_req_handler_patch(uint8_t conidx, struct l2cc_le_cb_conn_req *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2c_code_le_cb_conn_resp_handler_patch(uint8_t conidx, struct l2cc_le_cb_conn_resp *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2c_code_disconnection_req_handler_patch(uint8_t conidx, struct l2cc_disconnection_req *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2c_code_disconnection_resp_handler_patch(uint8_t conidx, struct l2cc_disconnection_resp *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2c_code_le_flow_control_credit_handler_patch(uint8_t conidx, struct l2cc_le_flow_control_credit *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_sig_pdu_recv_handler_patch(uint8_t conidx, struct l2cc_pdu *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_process_op_patch(uint8_t conidx, uint8_t op_type, void *op_msg, enum l2cc_operation *supp_ops){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_pdu_send_cmd_handler_patch(ke_msg_id_t const msgid, struct l2cc_pdu_send_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_lecb_sdu_send_cmd_handler_patch(ke_msg_id_t const msgid, struct l2cc_lecb_sdu_send_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_dbg_pdu_send_cmd_handler_patch(ke_msg_id_t const msgid, struct l2cc_dbg_pdu_send_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_nb_cmp_pkts_evt_handler_patch(ke_msg_id_t const msgid, struct hci_nb_cmp_pkts_evt const *event, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_task_hci_acl_data_handler_patch(ke_msg_id_t const msgid, struct hci_acl_data const *param, ke_task_id_t const _dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_lecb_connect_cmd_handler_patch(ke_msg_id_t const msgid, struct l2cc_lecb_connect_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_lecb_connect_cfm_handler_patch(ke_msg_id_t const msgid, struct l2cc_lecb_connect_cfm *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_lecb_disconnect_cmd_handler_patch(ke_msg_id_t const msgid, struct l2cc_lecb_disconnect_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_signaling_trans_to_ind_handler_patch(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_lecb_add_cmd_handler_patch(ke_msg_id_t const msgid, struct l2cc_lecb_add_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_pdu_recv_ind_handler_patch(ke_msg_id_t const msgid, struct l2cc_pdu_recv_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_cmp_evt_handler_patch(ke_msg_id_t const msgid, void *p_paran, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_l2cc_default_msg_handler_patch(ke_msg_id_t const msgid, void *p_paran, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
void *app_l2cc_pdu_alloc_patch(uint8_t conidx, uint16_t cid, uint8_t code, ke_task_id_t src_id, uint16_t length){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cm_init_patch(uint8_t init_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cm_cleanup_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cm_set_link_layer_buff_size_patch(uint16_t pkt_len, uint16_t nb_acl){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_l2cm_tx_status_patch(uint8_t conidx, bool busy){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
#endif

#if APP_PATCH_GAP
bool app_gapc_smp_check_key_distrib_patch(uint8_t conidx, uint8_t sec_level){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_gapc_smp_check_max_key_size_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_gapc_smp_check_pairing_feat_patch(struct gapc_pairing *pair_feat){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
bool app_gapc_smp_is_sec_mode_reached_patch(uint8_t conidx, uint8_t role){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_enc_key_refr_evt_handler_patch(ke_msg_id_t const msgid, struct hci_enc_key_ref_cmp_evt const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapc_bond_cfm_handler_patch(ke_msg_id_t const msgid, struct gapc_bond_cfm *cfm, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapc_bond_cmd_handler_patch(ke_msg_id_t const msgid, struct gapc_bond_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapc_encrypt_cfm_handler_patch(ke_msg_id_t const msgid, struct gapc_encrypt_cfm *cfm, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapc_encrypt_cmd_handler_patch(ke_msg_id_t const msgid, struct gapc_encrypt_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapc_gen_dh_key_ind_handler_patch(ke_msg_id_t const msgid, struct gapm_gen_dh_key_ind *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapc_key_press_notification_cmd_handler_patch(ke_msg_id_t const msgid, struct gapc_key_press_notif_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapc_security_cmd_handler_patch(ke_msg_id_t const msgid, struct gapc_security_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapc_sign_cmd_handler_patch(ke_msg_id_t const msgid, struct gapc_sign_cmd *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapc_smp_gapm_cmp_evt_handler_patch(ke_msg_id_t const msgid, struct gapm_cmp_evt *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapc_smp_rep_attempts_timer_handler_patch(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapc_smp_timeout_timer_ind_handler_patch(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapm_smpc_pub_key_ind_handler_patch(ke_msg_id_t const msg_id, struct gapm_pub_key_ind *p_param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_enc_chg_evt_handler_patch(ke_msg_id_t const msgid, struct hci_enc_change_evt const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_ltk_request_evt_handler_patch(ke_msg_id_t const msgid, struct hci_le_ltk_request_evt const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_start_enc_stat_evt_handler_patch(ke_msg_id_t const msgid, struct hci_cmd_stat_event const *event, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapc_smp_l2cc_cmp_evt_handler_patch(ke_msg_id_t const msgid, struct l2cc_cmp_evt *evt, ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int8_t app_gapc_smp_pairing_irk_exch_patch(uint8_t conidx, struct gap_sec_key *irk, struct gap_bdaddr *identity){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_aes_res_cb_patch(uint8_t status, const uint8_t *aes_res, uint32_t aes_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_aes_sign_res_cb_patch(uint8_t status, const uint8_t *aes_res, uint32_t aes_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_check_repeated_attempts_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_cleanup_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_clear_timeout_timer_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_comp_cnf_val_patch(uint8_t conidx, uint8_t role, bool local){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_comp_f4_patch(uint8_t conidx, const uint8_t *u, const uint8_t *v, const uint8_t *x, uint8_t z, uint8_t state){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_comp_f5_patch(uint8_t conidx, const uint8_t *w, const uint8_t *n1, const uint8_t *n2, const uint8_t *a1, const uint8_t *a2, uint8_t state){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_comp_f6_patch(uint8_t conidx, const uint8_t *w, const uint8_t *n1, const uint8_t *n2, const uint8_t *r, const uint8_t *iocap, const uint8_t *a1, const uint8_t *a2, uint8_t state){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_comp_g2_patch(uint8_t conidx, const uint8_t *u, const uint8_t *v, const uint8_t *x, const uint8_t *y, uint8_t state){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_dhkey_check_send_patch(uint8_t conidx, const uint8_t *dh_key_chk){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_encrypt_cfm_patch(uint8_t conidx, bool accept, struct gap_sec_key *ltk, uint8_t key_size){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_encrypt_start_handler_patch(uint8_t conidx, struct gapc_ltk *ltk){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_encrypt_start_patch(uint8_t conidx, struct gapc_ltk *ltk){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_generate_rand_patch(uint8_t conidx, uint8_t state){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_generate_stk_patch(uint8_t conidx, uint8_t role){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_get_key_sec_prop_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_get_next_passkey_bit_patch(struct gapc_smp_pair_info *p_pair){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_handle_dh_key_check_complete_patch(uint8_t conidx, const uint8_t *dh_key){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_handle_enc_change_evt_patch(uint8_t conidx, uint8_t role, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_id_addr_info_send_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_id_info_send_patch(uint8_t conidx, const uint8_t *irk){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_init_mac_key_calculation_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_initiate_dhkey_check_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_initiate_dhkey_verification_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_key_press_notification_ind_patch(uint8_t conidx, uint8_t notification_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_launch_rep_att_timer_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pair_cfm_send_patch(uint8_t conidx, const uint8_t *conf_val){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pair_fail_send_patch(uint8_t conidx, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pair_rand_send_patch(uint8_t conidx, const uint8_t *rand_nb){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pairing_csrk_exch_patch(uint8_t conidx, struct gap_sec_key *csrk){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pairing_end_patch(uint8_t conidx, uint8_t role, uint8_t status, bool start_ra_timer){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pairing_ltk_exch_patch(uint8_t conidx, struct gapc_ltk *ltk){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pairing_nc_exch_patch(uint8_t conidx, uint8_t accept){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pairing_oob_exch_patch(uint8_t conidx, bool accept, struct gapc_oob *oob){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pairing_req_handler_patch(uint8_t conidx, struct gapc_pairing *feat){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pairing_rsp_patch(uint8_t conidx, bool accept, struct gapc_pairing *feat){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pairing_start_patch(uint8_t conidx, struct gapc_pairing *pairing){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pairing_tk_exch_patch(uint8_t conidx, bool accept, struct gap_sec_key *tk){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pdu_recv_patch(uint8_t conidx, struct l2cc_pdu *pdu, uint8_t status){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_pdu_send_patch(uint8_t conidx, struct l2cc_pdu_data_t *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_dhkey_check_pdu_patch(uint8_t conidx, struct l2cc_dhkey_check *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_enc_info_pdu_patch(uint8_t conidx, struct l2cc_encryption_inf *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_id_addr_info_pdu_patch(uint8_t conidx, struct l2cc_id_addr_inf *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_id_info_pdu_patch(uint8_t conidx, struct l2cc_identity_inf *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_keypress_notification_pdu_patch(uint8_t conidx, struct l2cc_keypress_notification *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_mst_id_pdu_patch(uint8_t conidx, struct l2cc_master_id *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_pair_cfm_pdu_patch(uint8_t conidx, struct l2cc_pairing_cfm *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_pair_fail_pdu_patch(uint8_t conidx, struct l2cc_pairing_failed *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_pair_rand_pdu_patch(uint8_t conidx, struct l2cc_pairing_random *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_pair_req_pdu_patch(uint8_t conidx, struct l2cc_pairing_req *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_pair_rsp_pdu_patch(uint8_t conidx, struct l2cc_pairing_resp *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_public_key_pdu_patch(uint8_t conidx, struct l2cc_public_key *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_sec_req_pdu_patch(uint8_t conidx, struct l2cc_security_req *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_recv_sign_info_pdu_patch(uint8_t conidx, struct l2cc_signing_inf *pdu){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_security_req_send_patch(uint8_t conidx, uint8_t auth){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_send_gen_dh_key_cmd_patch(uint8_t conidx, uint8_t *operand_1, uint8_t *operand_2){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_send_get_pub_key_cmd_patch(uint8_t conidx){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_send_ltk_req_rsp_patch(uint8_t conidx, bool found, const uint8_t *key){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_send_pairing_ind_patch(uint8_t conidx, uint8_t ind_type, void *value){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_send_pairing_req_ind_patch(uint8_t conidx, uint8_t req_type){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_send_start_enc_cmd_patch(uint8_t conidx, uint8_t key_type, const uint8_t *key, const uint8_t *randnb, uint16_t ediv){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_sign_command_patch(uint8_t conidx, struct gapc_sign_cmd *param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_tkdp_rcp_continue_patch(uint8_t conidx, uint8_t role){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_tkdp_rcp_start_patch(uint8_t conidx, uint8_t role){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_tkdp_send_continue_patch(uint8_t conidx, uint8_t role){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapc_smp_tkdp_send_start_patch(uint8_t conidx, uint8_t role){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapm_gen_dh_key_cmd_handler_patch(ke_msg_id_t const msgid, struct gapm_gen_dh_key_cmd * param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapm_gen_rand_addr_cmd_handler_patch(ke_msg_id_t const msgid, struct gapm_gen_rand_addr_cmd *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapm_get_pub_key_cmd_handler_patch(ke_msg_id_t const msgid, struct gapm_get_pub_key_cmd* param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapm_resolv_addr_cmd_handler_patch(ke_msg_id_t const msgid, struct gapm_resolv_addr_cmd *param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_gapm_use_enc_block_cmd_handler_patch(ke_msg_id_t const msgid, struct gapm_use_enc_block_cmd * param,ke_task_id_t const dest_id, ke_task_id_t const src_id){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_enc_cmd_cmp_evt_handler_patch(uint16_t opcode, struct hci_le_enc_cmd_cmp_evt *param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_gen_dhkey_cmp_evt_handler_patch(uint16_t opcode, struct hci_le_gen_dhkey_cmp_evt const *param){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
int app_hci_le_gen_dhkey_v2_stat_evt_handler_patch(uint16_t opcode, struct hci_basic_cmd_cmp_evt const *p_event){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapm_smp_gen_rand_addr_cb_patch(uint8_t status, const uint8_t* aes_res, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapm_smp_gen_rand_nb_cb_patch(uint8_t status, const uint8_t* aes_res, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapm_smp_resolv_res_cb_patch(uint8_t index, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapm_smp_send_hci_encrypt_patch(const uint8_t *key, const uint8_t* val){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
uint8_t app_gapm_smp_use_enc_block_cb_patch(uint8_t status, const uint8_t* aes_res, uint32_t src_info){printf("XXX, %s\r\n",__FUNCTION__); return 0;}
#endif



///////////////////////////////////////////P4:    Test Code     ////////////////////////////////////////////////////

void app_patch_test(void)
{
    printf("ZZZ: %s ,------- Start \r\n", __FUNCTION__);
#if APP_PATCH_KE
    pf_patch_ke_init               = app_patch_ke_init;               ke_init();
    pf_patch_ke_flush              = app_patch_ke_flush;              ke_flush();
    pf_patch_ke_sleep_check        = app_patch_ke_sleep_check;        ke_sleep_check();
    pf_patch_ke_stats_get          = app_patch_ke_stats_get;          ke_stats_get(NULL,NULL,NULL,NULL);
    pf_patch_ke_event_init         = app_patch_ke_event_init;         ke_event_init();
    pf_patch_ke_event_callback_set = app_patch_ke_event_callback_set; ke_event_callback_set(0,NULL);
    pf_patch_ke_event_set          = app_patch_ke_event_set;          ke_event_set(0);
    pf_patch_ke_event_clear        = app_patch_ke_event_clear;        ke_event_clear(0);
    pf_patch_ke_event_get          = app_patch_ke_event_get;          ke_event_get(0);
    pf_patch_ke_event_get_all      = app_patch_ke_event_get_all;      ke_event_get_all();
    pf_patch_ke_event_flush        = app_patch_ke_event_flush;        ke_event_flush();
    pf_patch_ke_event_schedule     = app_patch_ke_event_schedule;     ke_event_schedule();
    pf_patch_cmp_dest_id           = app_patch_cmp_dest_id;           cmp_dest_id(NULL,0);
    pf_patch_ke_msg_discard        = app_patch_ke_msg_discard;        ke_msg_discard(0,NULL,0,0);
    pf_patch_ke_msg_save           = app_patch_ke_msg_save;           ke_msg_save(0,NULL,0,0);
    pf_patch_ke_handler_search     = app_patch_ke_handler_search;     ke_handler_search(0,0,0);
    pf_patch_ke_task_handler_get   = app_patch_ke_task_handler_get;   ke_task_handler_get(0,0);
    pf_patch_ke_state_get          = app_patch_ke_state_get;          ke_state_get(0);
    pf_patch_ke_task_check         = app_patch_ke_task_check;         ke_task_check(0);
    pf_patch_ke_task_create        = app_patch_ke_task_create;        ke_task_create(0,0);
    pf_patch_ke_task_delete        = app_patch_ke_task_delete;        ke_task_delete(0);
    pf_patch_ke_state_set          = app_patch_ke_state_set;          ke_state_set(0,0);
    pf_patch_ke_task_init          = app_patch_ke_task_init;          ke_task_init();
    pf_patch_ke_task_msg_flush     = app_patch_ke_task_msg_flush;     ke_task_msg_flush(0);
    pf_patch_ke_task_saved_update  = app_patch_ke_task_saved_update;  ke_task_saved_update(0);
    pf_patch_ke_task_schedule      = app_patch_ke_task_schedule;      ke_task_schedule();
    pf_patch_ke_msg_alloc          = app_patch_ke_msg_alloc;          ke_msg_alloc(0,0,0,0);
    pf_patch_ke_msg_send           = app_patch_ke_msg_send;           ke_msg_send(0);
    pf_patch_ke_msg_send_basic     = app_patch_ke_msg_send_basic;     ke_msg_send_basic(0,0,0);
    pf_patch_ke_msg_forward        = app_patch_ke_msg_forward;        ke_msg_forward(0,0,0);
    pf_patch_ke_msg_forward_new_id = app_patch_ke_msg_forward_new_id; ke_msg_forward_new_id(0,0,0,0);
    pf_patch_ke_msg_free           = app_patch_ke_msg_free;           ke_msg_free(0);
    pf_patch_ke_msg_dest_id_get    = app_patch_ke_msg_dest_id_get;    ke_msg_dest_id_get(0);
    pf_patch_ke_msg_src_id_get     = app_patch_ke_msg_src_id_get;     ke_msg_src_id_get(0);
    pf_patch_ke_msg_in_queue       = app_patch_ke_msg_in_queue;       ke_msg_in_queue(0);
    pf_patch_ke_mem_is_in_heap     = app_patch_ke_mem_is_in_heap;     ke_mem_is_in_heap(0,0);
    pf_patch_ke_mem_init           = app_patch_ke_mem_init;           ke_mem_init(0,0,0);
    pf_patch_ke_mem_is_empty       = app_patch_ke_mem_is_empty;       ke_mem_is_empty(0);
    pf_patch_ke_check_malloc       = app_patch_ke_check_malloc;       ke_check_malloc(0,0);
    pf_patch_ke_malloc             = app_patch_ke_malloc;             ke_malloc(0,0);
    pf_patch_ke_free               = app_patch_ke_free;               ke_free(0);
    pf_patch_ke_is_free            = app_patch_ke_is_free;            ke_is_free(0);
    pf_patch_ke_get_mem_usage      = app_patch_ke_get_mem_usage;      ke_get_mem_usage(0);
    pf_patch_ke_get_max_mem_usage  = app_patch_ke_get_max_mem_usage;  ke_get_max_mem_usage();
    pf_patch_ke_queue_extract      = app_patch_ke_queue_extract;      ke_queue_extract(0,0,0);
    pf_patch_ke_queue_insert       = app_patch_ke_queue_insert;       ke_queue_insert(0,0,0);
    pf_patch_ke_time               = app_patch_ke_time;               ke_time();
    pf_patch_cmp_abs_time          = app_patch_cmp_abs_time;          cmp_abs_time(0,0);
    pf_patch_cmp_timer_id          = app_patch_cmp_timer_id;          cmp_timer_id(0,0);
    pf_patch_ke_time_past          = app_patch_ke_time_past;          ke_time_past(testTimer);
    pf_patch_ke_timer_active       = app_patch_ke_timer_active;       ke_timer_active(0,0);
    pf_patch_ke_timer_adjust_all   = app_patch_ke_timer_adjust_all;   ke_timer_adjust_all(0);
    pf_patch_ke_timer_clear        = app_patch_ke_timer_clear;        ke_timer_clear(0,0);
    pf_patch_ke_timer_init         = app_patch_ke_timer_init;         ke_timer_init();
    pf_patch_ke_timer_schedule     = app_patch_ke_timer_schedule;     ke_timer_schedule();
    pf_patch_ke_timer_set          = app_patch_ke_timer_set;          ke_timer_set(0,0,0);
#endif //APP_PATCH_KE

#if APP_PATCH_CO
    pf_patch_co_list_extract           = app_patch_co_list_extract;           co_list_extract(0,0);
    pf_patch_co_list_find              = app_patch_co_list_find;              co_list_find(0,0);
    pf_patch_co_list_pop_front         = app_patch_co_list_pop_front;         co_list_pop_front(0);
    pf_patch_co_list_size              = app_patch_co_list_size;              co_list_size(0);
    pf_patch_co_list_extract_after     = app_patch_co_list_extract_after;     co_list_extract_after(0,0,0);
    pf_patch_co_list_extract_sublist   = app_patch_co_list_extract_sublist;   co_list_extract_sublist(0,0,0);
    pf_patch_co_list_init              = app_patch_co_list_init;              co_list_init(0);
    pf_patch_co_list_insert_after      = app_patch_co_list_insert_after;      co_list_insert_after(0,0,0);
    pf_patch_co_list_insert_before     = app_patch_co_list_insert_before;     co_list_insert_before(0,0,0);
    pf_patch_co_list_merge             = app_patch_co_list_merge;             co_list_merge(0,0);
    pf_patch_co_list_pool_init         = app_patch_co_list_pool_init;         co_list_pool_init(0,0,0,0);
    pf_patch_co_list_push_back         = app_patch_co_list_push_back;         co_list_push_back(0,0);
    pf_patch_co_list_push_back_sublist = app_patch_co_list_push_back_sublist; co_list_push_back_sublist(0,0,0);
    pf_patch_co_list_push_front        = app_patch_co_list_push_front;        co_list_push_front(0,0);
    pf_patch_co_util_read_array_size   = app_patch_co_util_read_array_size;   co_util_read_array_size(0);
    pf_patch_co_bytes_to_string        = app_patch_co_bytes_to_string;        co_bytes_to_string(0,0,0);
    pf_patch_co_bdaddr_compare         = app_patch_co_bdaddr_compare;         co_bdaddr_compare(0,0);
    pf_patch_co_nb_good_le_channels    = app_patch_co_nb_good_le_channels;    co_nb_good_le_channels(0);
    pf_patch_co_util_pack              = app_patch_co_util_pack;              co_util_pack(0,0,0,0,0);
    pf_patch_co_util_unpack            = app_patch_co_util_unpack;            co_util_unpack(0,0,0,0,0);
    pf_patch_co_ble_pkt_dur_in_us      = app_patch_co_ble_pkt_dur_in_us;      co_ble_pkt_dur_in_us(0,0);
#endif

#if APP_PATCH_AES
    pf_patch_aes_alloc          = app_patch_aes_alloc;          aes_alloc(0,0,0,0);
    pf_patch_aes_encrypt        = app_patch_aes_encrypt;        aes_encrypt(0,0,0,0,0);
    pf_patch_aes_init           = app_patch_aes_init;           aes_init(0);
    pf_patch_aes_start          = app_patch_aes_start;          aes_start(0,0,0);
    pf_patch_aes_app_encrypt    = app_patch_aes_app_encrypt;    aes_app_encrypt(0,0,0,0);
    pf_patch_aes_app_decrypt    = app_patch_aes_app_decrypt;    aes_app_decrypt(0,0,0,0);
    pf_patch_aes_rand           = app_patch_aes_rand;           aes_rand(0,0);
    pf_patch_aes_result_handler = app_patch_aes_result_handler; aes_result_handler(0,0);
    pf_patch_aes_xor_128        = app_patch_aes_xor_128;        aes_xor_128(0,0,0,0);
    pf_patch_aes_shift_left_128 = app_patch_aes_shift_left_128; aes_shift_left_128(0,0);
    pf_patch_aes_cmac           = app_patch_aes_cmac;           aes_cmac(0,0,0,0,0);
    pf_patch_aes_cmac_start     = app_patch_aes_cmac_start;     aes_cmac_start(0,0,0,0);
    pf_patch_aes_cmac_continue  = app_patch_aes_cmac_continue;  aes_cmac_continue(0,0);
#endif

#if APP_PATCH_SCH
    pf_patch_sch_arb_conflict_check  = app_patch_sch_arb_conflict_check; 	sch_arb_conflict_check(0,0);
    pf_patch_sch_arb_insert          = app_patch_sch_arb_insert; 	        sch_arb_insert(0);
    pf_patch_sch_arb_remove          = app_patch_sch_arb_remove; 	        sch_arb_remove(0,0);
    pf_patch_sch_arb_elt_cancel      = app_patch_sch_arb_elt_cancel; 	    sch_arb_elt_cancel(0);
    pf_patch_sch_arb_event_start_isr = app_patch_sch_arb_event_start_isr; 	sch_arb_event_start_isr();
    pf_patch_sch_arb_init            = app_patch_sch_arb_init; 	            sch_arb_init(0);
    pf_patch_sch_arb_prog_timer      = app_patch_sch_arb_prog_timer; 	    sch_arb_prog_timer();
    pf_patch_sch_arb_sw_isr          = app_patch_sch_arb_sw_isr; 	        sch_arb_sw_isr();
    pf_patch_sch_alarm_prog          = app_patch_sch_alarm_prog; 	        sch_alarm_prog();
    pf_patch_sch_alarm_init          = app_patch_sch_alarm_init; 	        sch_alarm_init(0);
    pf_patch_sch_alarm_timer_isr     = app_patch_sch_alarm_timer_isr; 	    sch_alarm_timer_isr();
    pf_patch_sch_alarm_set           = app_patch_sch_alarm_set; 	        sch_alarm_set(0);
    pf_patch_sch_alarm_clear         = app_patch_sch_alarm_clear; 	        sch_alarm_clear(0);
    pf_patch_sch_prog_rx_isr         = app_patch_sch_prog_rx_isr; 	        sch_prog_rx_isr(0);
    pf_patch_sch_prog_tx_isr         = app_patch_sch_prog_tx_isr; 	        sch_prog_tx_isr(0);
    pf_patch_sch_prog_skip_isr       = app_patch_sch_prog_skip_isr; 	    sch_prog_skip_isr(0);
    pf_patch_sch_prog_end_isr        = app_patch_sch_prog_end_isr; 	        sch_prog_end_isr(0);
    pf_patch_sch_prog_init           = app_patch_sch_prog_init; 	        sch_prog_init(0);
    pf_patch_sch_prog_fifo_isr       = app_patch_sch_prog_fifo_isr; 	    sch_prog_fifo_isr();
    pf_patch_sch_prog_push           = app_patch_sch_prog_push; 	        sch_prog_push(0);
    pf_patch_sch_slice_compute       = app_patch_sch_slice_compute; 	    sch_slice_compute();
    pf_patch_sch_slice_init          = app_patch_sch_slice_init; 	        sch_slice_init(0);
    pf_patch_sch_slice_bg_add        = app_patch_sch_slice_bg_add; 	        sch_slice_bg_add(0);
    pf_patch_sch_slice_bg_remove     = app_patch_sch_slice_bg_remove; 	    sch_slice_bg_remove(0);
    pf_patch_sch_slice_fg_add        = app_patch_sch_slice_fg_add; 	        sch_slice_fg_add(0,0);
    pf_patch_sch_slice_fg_remove     = app_patch_sch_slice_fg_remove; 	    sch_slice_fg_remove(0);
    pf_patch_sch_slice_per_add       = app_patch_sch_slice_per_add; 	    sch_slice_per_add(0,0,0,0,0);
    pf_patch_sch_slice_per_remove    = app_patch_sch_slice_per_remove; 	    sch_slice_per_remove(0,0);
#endif

#if (APP_PATCH_LLD)
    pf_patch_lld_calc_aux_rx                = app_patch_lld_calc_aux_rx;		        lld_calc_aux_rx(0,0,0);
    pf_patch_lld_rxdesc_check               = app_patch_lld_rxdesc_check;		        lld_rxdesc_check(0);
    pf_patch_lld_read_clock                 = app_patch_lld_read_clock;		            lld_read_clock();
    pf_patch_lld_ch_idx_get                 = app_patch_lld_ch_idx_get;		            lld_ch_idx_get();
    pf_patch_lld_local_sca_get              = app_patch_lld_local_sca_get;		        lld_local_sca_get();
    pf_patch_lld_channel_assess             = app_patch_lld_channel_assess;		        lld_channel_assess(0,0,0);
    pf_patch_lld_rpa_renew_evt_canceled_cbk = app_patch_lld_rpa_renew_evt_canceled_cbk;	lld_rpa_renew_evt_canceled_cbk(0);
    pf_patch_lld_rpa_renew_evt_start_cbk    = app_patch_lld_rpa_renew_evt_start_cbk;	lld_rpa_renew_evt_start_cbk(0);
    pf_patch_lld_rpa_renew_instant_cbk      = app_patch_lld_rpa_renew_instant_cbk;		lld_rpa_renew_instant_cbk(0);
    pf_patch_lld_rxdesc_free                = app_patch_lld_rxdesc_free;		        lld_rxdesc_free();
    pf_patch_lld_adv_ext_pkt_prepare        = app_patch_lld_adv_ext_pkt_prepare;		lld_adv_ext_pkt_prepare(0,0,0,0,0,0,0,0,0,0,0,0,0);
    pf_patch_lld_adv_pkt_rx                 = app_patch_lld_adv_pkt_rx;		            lld_adv_pkt_rx(0);
    pf_patch_lld_adv_end                    = app_patch_lld_adv_end;		            lld_adv_end(0,0,0);
    pf_patch_lld_adv_sync_info_set          = app_patch_lld_adv_sync_info_set;		    lld_adv_sync_info_set(0);
    pf_patch_lld_adv_aux_ch_idx_set         = app_patch_lld_adv_aux_ch_idx_set;		    lld_adv_aux_ch_idx_set(0);
    pf_patch_lld_adv_ext_chain_construct    = app_patch_lld_adv_ext_chain_construct;	lld_adv_ext_chain_construct(0);
    pf_patch_lld_adv_adv_data_set           = app_patch_lld_adv_adv_data_set;		    lld_adv_adv_data_set(0,0,0,0);
    pf_patch_lld_adv_scan_rsp_data_set      = app_patch_lld_adv_scan_rsp_data_set;		lld_adv_scan_rsp_data_set(0,0,0,0);
    pf_patch_lld_adv_evt_start_cbk          = app_patch_lld_adv_evt_start_cbk;		    lld_adv_evt_start_cbk(0);
    pf_patch_lld_adv_aux_evt_start_cbk      = app_patch_lld_adv_aux_evt_start_cbk;		lld_adv_aux_evt_start_cbk(0);
    pf_patch_lld_adv_evt_canceled_cbk       = app_patch_lld_adv_evt_canceled_cbk;		lld_adv_evt_canceled_cbk(0);
    pf_patch_lld_adv_aux_evt_canceled_cbk   = app_patch_lld_adv_aux_evt_canceled_cbk;	lld_adv_aux_evt_canceled_cbk(0);
    pf_patch_lld_adv_frm_isr                = app_patch_lld_adv_frm_isr;		        lld_adv_frm_isr(0,0,0);
    pf_patch_lld_adv_frm_skip_isr           = app_patch_lld_adv_frm_skip_isr;		    lld_adv_frm_skip_isr(0,0);
    pf_patch_lld_adv_frm_cbk                = app_patch_lld_adv_frm_cbk;		        lld_adv_frm_cbk(0,0,0);
    pf_patch_lld_scan_sync_accept           = app_patch_lld_scan_sync_accept;		    lld_scan_sync_accept(0,0,0,testAddr,0);
    pf_patch_lld_scan_sync_info_unpack      = app_patch_lld_scan_sync_info_unpack;		lld_scan_sync_info_unpack(0,0);
    pf_patch_lld_scan_restart               = app_patch_lld_scan_restart;		        lld_scan_restart();
    pf_patch_lld_scan_aux_evt_canceled_cbk  = app_patch_lld_scan_aux_evt_canceled_cbk;  lld_scan_aux_evt_canceled_cbk(0);
    pf_patch_lld_scan_aux_evt_start_cbk     = app_patch_lld_scan_aux_evt_start_cbk;		lld_scan_aux_evt_start_cbk(0);
    pf_patch_lld_scan_aux_sched             = app_patch_lld_scan_aux_sched;		        lld_scan_aux_sched(0);
    pf_patch_lld_scan_end                   = app_patch_lld_scan_end;		            lld_scan_end();
    pf_patch_lld_scan_evt_canceled_cbk      = app_patch_lld_scan_evt_canceled_cbk;		lld_scan_evt_canceled_cbk(0);
    pf_patch_lld_scan_evt_start_cbk         = app_patch_lld_scan_evt_start_cbk;		    lld_scan_evt_start_cbk(0);
    pf_patch_lld_scan_frm_cbk               = app_patch_lld_scan_frm_cbk;		        lld_scan_frm_cbk(0,0,0);
    pf_patch_lld_scan_frm_eof_isr           = app_patch_lld_scan_frm_eof_isr;		    lld_scan_frm_eof_isr(0,0,0);
    pf_patch_lld_scan_frm_rx_isr            = app_patch_lld_scan_frm_rx_isr;		    lld_scan_frm_rx_isr(0);
    pf_patch_lld_scan_frm_skip_isr          = app_patch_lld_scan_frm_skip_isr;		    lld_scan_frm_skip_isr(0);
    pf_patch_lld_scan_process_pkt_rx        = app_patch_lld_scan_process_pkt_rx;		lld_scan_process_pkt_rx(0);
    pf_patch_lld_scan_sched                 = app_patch_lld_scan_sched;		            lld_scan_sched(0,0,0);
    pf_patch_lld_scan_trunc_ind             = app_patch_lld_scan_trunc_ind;		        lld_scan_trunc_ind(0);
    pf_patch_lld_init_compute_winoffset     = app_patch_lld_init_compute_winoffset;		lld_init_compute_winoffset(0,0,0,0);
    pf_patch_lld_init_connect_req_pack      = app_patch_lld_init_connect_req_pack;		lld_init_connect_req_pack(0,0);
    pf_patch_lld_init_end                   = app_patch_lld_init_end;		            lld_init_end();
    pf_patch_lld_init_evt_canceled_cbk      = app_patch_lld_init_evt_canceled_cbk;		lld_init_evt_canceled_cbk(0);
    pf_patch_lld_init_evt_start_cbk         = app_patch_lld_init_evt_start_cbk;		    lld_init_evt_start_cbk(0);
    pf_patch_lld_init_frm_cbk               = app_patch_lld_init_frm_cbk;		        lld_init_frm_cbk(0,0,0);
    pf_patch_lld_init_frm_eof_isr           = app_patch_lld_init_frm_eof_isr;		    lld_init_frm_eof_isr(0,0,0);
    pf_patch_lld_init_frm_skip_isr          = app_patch_lld_init_frm_skip_isr;		    lld_init_frm_skip_isr(0);
    pf_patch_lld_init_process_pkt_rx        = app_patch_lld_init_process_pkt_rx;		lld_init_process_pkt_rx(0);
    pf_patch_lld_init_process_pkt_tx        = app_patch_lld_init_process_pkt_tx;		lld_init_process_pkt_tx(0);
    pf_patch_lld_init_sched                 = app_patch_lld_init_sched;		            lld_init_sched(0,0,0);
    pf_patch_lld_con_max_lat_calc           = app_patch_lld_con_max_lat_calc;		    lld_con_max_lat_calc(0);
    pf_patch_lld_con_tx_len_update          = app_patch_lld_con_tx_len_update;		    lld_con_tx_len_update(0,0,0);
    pf_patch_lld_con_evt_time_update        = app_patch_lld_con_evt_time_update;		lld_con_evt_time_update(0);
    pf_patch_lld_con_cleanup                = app_patch_lld_con_cleanup;		        lld_con_cleanup(0,0,0);
    pf_patch_lld_instant_proc_end           = app_patch_lld_instant_proc_end;		    lld_instant_proc_end(0);
    pf_patch_lld_con_sched                  = app_patch_lld_con_sched;		            lld_con_sched(0,0,0);
    pf_patch_lld_con_rx                     = app_patch_lld_con_rx;		                lld_con_rx(0,0);
    pf_patch_lld_con_tx                     = app_patch_lld_con_tx;		                lld_con_tx(0);
    pf_patch_lld_con_tx_prog                = app_patch_lld_con_tx_prog;		        lld_con_tx_prog(0);
    pf_patch_lld_con_evt_start_cbk          = app_patch_lld_con_evt_start_cbk;		    lld_con_evt_start_cbk(0);
    pf_patch_lld_con_evt_canceled_cbk       = app_patch_lld_con_evt_canceled_cbk;		lld_con_evt_canceled_cbk(0);
    pf_patch_lld_con_frm_isr                = app_patch_lld_con_frm_isr;		        lld_con_frm_isr(0,0,0);
    pf_patch_lld_con_rx_isr                 = app_patch_lld_con_rx_isr;		            lld_con_rx_isr(0,0);
    pf_patch_lld_con_tx_isr                 = app_patch_lld_con_tx_isr;		            lld_con_tx_isr(0,0);
    pf_patch_lld_con_frm_skip_isr           = app_patch_lld_con_frm_skip_isr;		    lld_con_frm_skip_isr(0);
    pf_patch_lld_con_frm_cbk                = app_patch_lld_con_frm_cbk;		        lld_con_frm_cbk(0,0,0);
    pf_patch_lld_sync_cleanup               = app_patch_lld_sync_cleanup;		        lld_sync_cleanup(0,0);
    pf_patch_lld_sync_trunc_ind             = app_patch_lld_sync_trunc_ind;		        lld_sync_trunc_ind(0);
    pf_patch_lld_sync_process_pkt_rx        = app_patch_lld_sync_process_pkt_rx;		lld_sync_process_pkt_rx(0);
    pf_patch_lld_sync_sched                 = app_patch_lld_sync_sched;	                lld_sync_sched(0,0);
    pf_patch_lld_sync_frm_eof_isr           = app_patch_lld_sync_frm_eof_isr;		    lld_sync_frm_eof_isr(0,0,0);
    pf_patch_lld_sync_frm_rx_isr            = app_patch_lld_sync_frm_rx_isr;		    lld_sync_frm_rx_isr(0);
    pf_patch_lld_sync_frm_skip_isr          = app_patch_lld_sync_frm_skip_isr;		    lld_sync_frm_skip_isr(0);
    pf_patch_lld_sync_frm_cbk               = app_patch_lld_sync_frm_cbk;		        lld_sync_frm_cbk(0,0,0);
    pf_patch_lld_sync_evt_start_cbk         = app_patch_lld_sync_evt_start_cbk;		    lld_sync_evt_start_cbk(0);
    pf_patch_lld_sync_evt_canceled_cbk      = app_patch_lld_sync_evt_canceled_cbk;		lld_sync_evt_canceled_cbk(0);
    pf_patch_lld_sync_ant_switch_config     = app_patch_lld_sync_ant_switch_config;		lld_sync_ant_switch_config(0);
    pf_patch_lld_per_adv_cleanup            = app_patch_lld_per_adv_cleanup;		    lld_per_adv_cleanup(0);
    pf_patch_lld_per_adv_sched              = app_patch_lld_per_adv_sched;		        lld_per_adv_sched(0);
    pf_patch_lld_per_adv_chain_construct    = app_patch_lld_per_adv_chain_construct;    lld_per_adv_chain_construct(0);
    pf_patch_lld_per_adv_data_set           = app_patch_lld_per_adv_data_set;		    lld_per_adv_data_set(0,0,0,0);
    pf_patch_lld_per_adv_evt_start_cbk      = app_patch_lld_per_adv_evt_start_cbk;		lld_per_adv_evt_start_cbk(0);
    pf_patch_lld_per_adv_evt_canceled_cbk   = app_patch_lld_per_adv_evt_canceled_cbk;   lld_per_adv_evt_canceled_cbk(0);
    pf_patch_lld_per_adv_frm_isr            = app_patch_lld_per_adv_frm_isr;		    lld_per_adv_frm_isr(0,0,0);
    pf_patch_lld_per_adv_frm_skip_isr       = app_patch_lld_per_adv_frm_skip_isr;		lld_per_adv_frm_skip_isr(0);
    pf_patch_lld_per_adv_frm_cbk            = app_patch_lld_per_adv_frm_cbk;            lld_per_adv_frm_cbk(0,0,0);
    pf_patch_lld_per_adv_ant_switch_config  = app_patch_lld_per_adv_ant_switch_config;  lld_per_adv_ant_switch_config(0);
    pf_patch_lld_per_adv_sync_info_get      = app_patch_lld_per_adv_sync_info_get;		lld_per_adv_sync_info_get(0,0,0,0);
    pf_patch_lld_per_adv_init_info_get      = app_patch_lld_per_adv_init_info_get;		lld_per_adv_init_info_get(0,0,0,0);
    pf_patch_lld_per_adv_ext_pkt_prepare    = app_patch_lld_per_adv_ext_pkt_prepare;    lld_per_adv_ext_pkt_prepare(0,0,0,0,0,0,0,0,0);
    pf_patch_lld_test_freq2chnl             = app_patch_lld_test_freq2chnl;		        lld_test_freq2chnl(0);
    pf_patch_lld_test_cleanup               = app_patch_lld_test_cleanup;		        lld_test_cleanup();
    pf_patch_lld_test_evt_canceled_cbk      = app_patch_lld_test_evt_canceled_cbk;		lld_test_evt_canceled_cbk(0);
    pf_patch_lld_test_evt_start_cbk         = app_patch_lld_test_evt_start_cbk;		    lld_test_evt_start_cbk(0);
    pf_patch_lld_test_frm_cbk               = app_patch_lld_test_frm_cbk;		        lld_test_frm_cbk(0,0,0);
    pf_patch_lld_test_frm_isr               = app_patch_lld_test_frm_isr;		        lld_test_frm_isr(0,0);
    pf_patch_lld_test_rx_isr                = app_patch_lld_test_rx_isr;		        lld_test_rx_isr(0);
#endif

#if (APP_PATCH_RPA)
    pf_aes_rpa_gen_continue_patch     = app_aes_rpa_gen_continue_patch;		aes_rpa_gen_continue(0,0);
    pf_aes_rpa_resolve_continue_patch = app_aes_rpa_resolve_continue_patch;	aes_rpa_resolve_continue(0,0);
    pf_aes_rpa_gen_patch              = app_aes_rpa_gen_patch;		        aes_rpa_gen(0,0,0);
    pf_aes_rpa_resolve_patch          = app_aes_rpa_resolve_patch;		    aes_rpa_resolve(0,0,0,0,0);
#endif

#if (APP_PATCH_SCH_PLAN)
    pf_sch_plan_init_patch                     = app_sch_plan_init_patch;  	            sch_plan_init(0);
    pf_sch_plan_offset_max_calc_patch          = app_sch_plan_offset_max_calc_patch;  	sch_plan_offset_max_calc(0,0,0,0);
    pf_sch_plan_offset_req_patch               = app_sch_plan_offset_req_patch;  	    sch_plan_offset_req(0,0,0,0);
    pf_sch_plan_interval_req_patch             = app_sch_plan_interval_req_patch;  	    sch_plan_interval_req(0);
    pf_sch_plan_position_range_compute_patch   = app_sch_plan_position_range_compute_patch;  	sch_plan_position_range_compute(0,0,0,0,0,0);
    pf_sch_plan_clock_wrap_offset_update_patch = app_sch_plan_clock_wrap_offset_update_patch;  	sch_plan_clock_wrap_offset_update();
    pf_sch_plan_set_patch                      = app_sch_plan_set_patch;  	            sch_plan_set(0);
    pf_sch_plan_rem_patch                      = app_sch_plan_rem_patch;  	            sch_plan_rem(0);
    pf_sch_plan_req_patch                      = app_sch_plan_req_patch;  	            sch_plan_req(0);
    pf_sch_plan_chk_patch                      = app_sch_plan_chk_patch;  	            sch_plan_chk(0);
#endif

#if (APP_PATCH_HCI)
    pf_hci_send_2_host_patch                    = app_hci_send_2_host_patch;  	                hci_send_2_host(0);
    pf_hci_send_2_controller_patch              = app_hci_send_2_controller_patch;  	        hci_send_2_controller(0);
    pf_hci_evt_mask_check_patch                 = app_hci_evt_mask_check_patch;  	            hci_evt_mask_check(0);
    pf_hci_init_patch                           = app_hci_init_patch;  	                        hci_init(0);
    pf_hci_ble_conhdl_register_patch            = app_hci_ble_conhdl_register_patch;  	        hci_ble_conhdl_register(0);
    pf_hci_ble_conhdl_unregister_patch          = app_hci_ble_conhdl_unregister_patch;  	    hci_ble_conhdl_unregister(0);
    pf_hci_evt_mask_set_patch                   = app_hci_evt_mask_set_patch;  	                hci_evt_mask_set(0,0);
//    pf_hci_evt_filter_add_patch                 = app_hci_evt_filter_add_patch;  	            hci_evt_filter_add(0);
    pf_hci_look_for_cmd_desc_patch              = app_hci_look_for_cmd_desc_patch;  	        hci_look_for_cmd_desc(0);
    pf_hci_look_for_evt_desc_patch              = app_hci_look_for_evt_desc_patch;  	        hci_look_for_evt_desc(0);
    pf_hci_look_for_le_evt_desc_patch           = app_hci_look_for_le_evt_desc_patch;  	        hci_look_for_le_evt_desc(0);
    pf_hci_pack_bytes_patch                     = app_hci_pack_bytes_patch;  	                hci_pack_bytes(0,0,0,0,0);
    pf_hci_host_nb_cmp_pkts_cmd_pkupk_patch     = app_hci_host_nb_cmp_pkts_cmd_pkupk_patch;  	hci_host_nb_cmp_pkts_cmd_pkupk(0,0,0,0);
    pf_hci_le_set_ext_scan_param_cmd_upk_patch  = app_hci_le_set_ext_scan_param_cmd_upk_patch;  hci_le_set_ext_scan_param_cmd_upk(0,0,0,0);
    pf_hci_le_ext_create_con_cmd_upk_patch      = app_hci_le_ext_create_con_cmd_upk_patch;  	hci_le_ext_create_con_cmd_upk(0,0,0,0);
    pf_hci_le_set_ext_adv_en_cmd_upk_patch      = app_hci_le_set_ext_adv_en_cmd_upk_patch;  	hci_le_set_ext_adv_en_cmd_upk(0,0,0,0);
    pf_hci_le_adv_report_evt_pkupk_patch        = app_hci_le_adv_report_evt_pkupk_patch;  	    hci_le_adv_report_evt_pkupk(0,0,0,0);
    pf_hci_le_dir_adv_report_evt_pkupk_patch    = app_hci_le_dir_adv_report_evt_pkupk_patch;  	hci_le_dir_adv_report_evt_pkupk(0,0,0,0);
    pf_hci_le_ext_adv_report_evt_pkupk_patch    = app_hci_le_ext_adv_report_evt_pkupk_patch;  	hci_le_ext_adv_report_evt_pkupk(0,0,0,0);
    pf_hci_le_conless_iq_report_evt_pkupk_patch = app_hci_le_conless_iq_report_evt_pkupk_patch; hci_le_conless_iq_report_evt_pkupk(0,0,0,0);
    pf_hci_le_con_iq_report_evt_pkupk_patch     = app_hci_le_con_iq_report_evt_pkupk_patch;  	hci_le_con_iq_report_evt_pkupk(0,0,0,0);
    pf_hci_ble_adv_report_filter_check_patch    = app_hci_ble_adv_report_filter_check_patch;  	hci_ble_adv_report_filter_check(0);
    pf_hci_ble_adv_report_tx_check_patch        = app_hci_ble_adv_report_tx_check_patch;  	    hci_ble_adv_report_tx_check(0);
    pf_hci_cmd_reject_patch                     = app_hci_cmd_reject_patch;  	                hci_cmd_reject(0,0,0,0);
    pf_hci_cmd_received_patch                   = app_hci_cmd_received_patch;  	                hci_cmd_received(0,0,0);
    pf_hci_acl_tx_data_received_patch           = app_hci_acl_tx_data_received_patch;  	        hci_acl_tx_data_received(0,0,0);
    pf_hci_build_cs_evt_patch                   = app_hci_build_cs_evt_patch;  	                hci_build_cs_evt(0);
    pf_hci_build_cc_evt_patch                   = app_hci_build_cc_evt_patch;  	                hci_build_cc_evt(0);
    pf_hci_build_evt_patch                      = app_hci_build_evt_patch;  	                hci_build_evt(0);
    pf_hci_build_le_evt_patch                   = app_hci_build_le_evt_patch;  	                hci_build_le_evt(0);
    pf_hci_build_acl_data_patch                 = app_hci_build_acl_data_patch;  	            hci_build_acl_data(0);
    pf_hci_acl_tx_data_alloc_patch              = app_hci_acl_tx_data_alloc_patch;  	        hci_acl_tx_data_alloc(0,0);
    pf_hci_tx_start_patch                       = app_hci_tx_start_patch;  	                    hci_tx_start();
    pf_hci_tx_done_patch                        = app_hci_tx_done_patch;  	                    hci_tx_done();
    pf_hci_tx_trigger_patch                     = app_hci_tx_trigger_patch;  	                hci_tx_trigger();
    pf_hci_tl_send_patch                        = app_hci_tl_send_patch;  	                    hci_tl_send(0);
    pf_hci_tl_init_patch                        = app_hci_tl_init_patch;  	                    hci_tl_init(0);
    pf_hci_cmd_get_max_param_size_patch         = app_hci_cmd_get_max_param_size_patch;  	    hci_cmd_get_max_param_size(0);
#endif

#if (APP_PATCH_LLC)
    pf_llc_loc_clk_acc_proc_continue_patch                    = app_llc_loc_clk_acc_proc_continue_patch ; 		    llc_loc_clk_acc_proc_continue(0,0);
    pf_llc_ll_clk_acc_req_pdu_send_patch                      = app_llc_ll_clk_acc_req_pdu_send_patch ; 		    llc_ll_clk_acc_req_pdu_send(0,0);
    pf_llc_ll_clk_acc_rsp_pdu_send_patch                      = app_llc_ll_clk_acc_rsp_pdu_send_patch ; 		    llc_ll_clk_acc_rsp_pdu_send(0,0);
    pf_llc_clk_acc_proc_err_cb_patch                          = app_llc_clk_acc_proc_err_cb_patch ; 		        llc_clk_acc_proc_err_cb(0,0,0);
    pf_ll_clk_acc_req_handler_patch                           = app_ll_clk_acc_req_handler_patch ; 		            ll_clk_acc_req_handler(0,0,0);
    pf_ll_clk_acc_rsp_handler_patch                           = app_ll_clk_acc_rsp_handler_patch ; 		            ll_clk_acc_rsp_handler(0,0,0);
    pf_llc_op_clk_acc_ind_handler_patch                       = app_llc_op_clk_acc_ind_handler_patch ; 		        llc_op_clk_acc_ind_handler(0,0,0,0);
    pf_llc_clk_acc_modify_patch                               = app_llc_clk_acc_modify_patch ; 		                llc_clk_acc_modify(0,0);
    pf_llc_loc_con_upd_proc_continue_patch                    = app_llc_loc_con_upd_proc_continue_patch ; 		    llc_loc_con_upd_proc_continue(0,0,0);
    pf_llc_rem_con_upd_proc_continue_patch                    = app_llc_rem_con_upd_proc_continue_patch ; 		    llc_rem_con_upd_proc_continue(0,0,0);
    pf_llc_pref_param_compute_patch                           = app_llc_pref_param_compute_patch ; 		            llc_pref_param_compute(0,0);
    pf_llc_hci_con_upd_info_send_patch                        = app_llc_hci_con_upd_info_send_patch ; 		        llc_hci_con_upd_info_send(0,0,0);
    pf_llc_con_upd_param_in_range_patch                       = app_llc_con_upd_param_in_range_patch ; 		        llc_con_upd_param_in_range(0,0,0,0,0);
    pf_llc_ll_connection_update_ind_pdu_send_patch            = app_llc_ll_connection_update_ind_pdu_send_patch ;   llc_ll_connection_update_ind_pdu_send(0,0);
    pf_llc_ll_connection_param_req_pdu_send_patch             = app_llc_ll_connection_param_req_pdu_send_patch ;    llc_ll_connection_param_req_pdu_send(0,0);
    pf_llc_ll_connection_param_rsp_pdu_send_patch             = app_llc_ll_connection_param_rsp_pdu_send_patch ;    llc_ll_connection_param_rsp_pdu_send(0,0);
    pf_llc_hci_con_param_req_evt_send_patch                   = app_llc_hci_con_param_req_evt_send_patch ; 		    llc_hci_con_param_req_evt_send(0,0,0,0,0);
    pf_llc_loc_con_upd_proc_err_cb_patch                      = app_llc_loc_con_upd_proc_err_cb_patch ; 		    llc_loc_con_upd_proc_err_cb(0,0,0);
    pf_llc_rem_con_upd_proc_err_cb_patch                      = app_llc_rem_con_upd_proc_err_cb_patch ; 		    llc_rem_con_upd_proc_err_cb(0,0,0);
    pf_ll_connection_update_ind_handler_patch                 = app_ll_connection_update_ind_handler_patch ; 		ll_connection_update_ind_handler(0,0,0);
    pf_ll_connection_param_req_handler_patch                  = app_ll_connection_param_req_handler_patch ; 		ll_connection_param_req_handler(0,0,0);
    pf_ll_connection_param_rsp_handler_patch                  = app_ll_connection_param_rsp_handler_patch ; 		ll_connection_param_rsp_handler(0,0,0);
    pf_hci_le_con_upd_cmd_handler_patch                       = app_hci_le_con_upd_cmd_handler_patch ; 		        hci_le_con_upd_cmd_handler(0,0,0);
    pf_hci_le_rem_con_param_req_reply_cmd_handler_patch       = app_hci_le_rem_con_param_req_reply_cmd_handler_patch ; 		    hci_le_rem_con_param_req_reply_cmd_handler(0,0,0);
    pf_hci_le_rem_con_param_req_neg_reply_cmd_handler_patch   = app_hci_le_rem_con_param_req_neg_reply_cmd_handler_patch ; 		hci_le_rem_con_param_req_neg_reply_cmd_handler(0,0,0);
    pf_llc_op_con_upd_ind_handler_patch                       = app_llc_op_con_upd_ind_handler_patch ; 		    llc_op_con_upd_ind_handler(0,0,0,0);
    pf_lld_con_param_upd_cfm_handler_patch                    = app_lld_con_param_upd_cfm_handler_patch ; 		lld_con_param_upd_cfm_handler(0,0,0,0);
    pf_lld_con_estab_ind_handler_patch                        = app_lld_con_estab_ind_handler_patch ; 		    lld_con_estab_ind_handler(0,0,0,0);
    pf_lld_con_offset_upd_ind_handler_patch                   = app_lld_con_offset_upd_ind_handler_patch ; 		lld_con_offset_upd_ind_handler(0,0,0,0);
    pf_llc_con_move_cbk_patch                                 = app_llc_con_move_cbk_patch ; 		            llc_con_move_cbk(0);
    pf_llc_disconnect_proc_continue_patch                     = app_llc_disconnect_proc_continue_patch ; 		llc_disconnect_proc_continue(0,0);
    pf_llc_op_disconnect_ind_handler_patch                    = app_llc_op_disconnect_ind_handler_patch ; 		llc_op_disconnect_ind_handler(0,0,0,0);
    pf_hci_disconnect_cmd_handler_patch                       = app_hci_disconnect_cmd_handler_patch ; 		    hci_disconnect_cmd_handler(0,0,0);
    pf_llc_disconnect_end_patch                               = app_llc_disconnect_end_patch ; 		            llc_disconnect_end(0,0,0);
    pf_llc_ll_terminate_ind_pdu_send_patch                    = app_llc_ll_terminate_ind_pdu_send_patch ; 		llc_ll_terminate_ind_pdu_send(0,0);
    pf_llc_disconnect_proc_err_cb_patch                       = app_llc_disconnect_proc_err_cb_patch ; 		    llc_disconnect_proc_err_cb(0,0,0);
    pf_ll_terminate_ind_handler_patch                         = app_ll_terminate_ind_handler_patch ; 		    ll_terminate_ind_handler(0,0,0);
    pf_lld_disc_ind_handler_patch                             = app_lld_disc_ind_handler_patch ; 		        lld_disc_ind_handler(0,0,0,0);
    pf_llc_stopped_ind_handler_patch                          = app_llc_stopped_ind_handler_patch ; 		    llc_stopped_ind_handler(0,0,0,0);
    pf_llc_disconnect_patch                                   = app_llc_disconnect_patch ; 		                llc_disconnect(0,0,0);
    pf_llc_init_term_proc_patch                               = app_llc_init_term_proc_patch ; 		            llc_init_term_proc(0,0);
    pf_llc_llcp_send_patch                                    = app_llc_llcp_send_patch ; 		                llc_llcp_send(0,0,0);
    pf_llc_llcp_tx_check_patch                                = app_llc_llcp_tx_check_patch ; 		            llc_llcp_tx_check(0);
    pf_llc_ll_reject_ind_pdu_send_patch                       = app_llc_ll_reject_ind_pdu_send_patch ; 		    llc_ll_reject_ind_pdu_send(0,0,0,0);
    pf_lld_llcp_rx_ind_handler_patch                          = app_lld_llcp_rx_ind_handler_patch ; 		    lld_llcp_rx_ind_handler(0,0,0,0);
    pf_lld_llcp_tx_cfm_handler_patch                          = app_lld_llcp_tx_cfm_handler_patch ; 		    lld_llcp_tx_cfm_handler(0,0,0,0);
    pf_llc_past_sync_info_unpack_patch                        = app_llc_past_sync_info_unpack_patch ; 		    llc_past_sync_info_unpack(0,0);
    pf_llc_past_rx_patch                                      = app_llc_past_rx_patch ; 		                llc_past_rx(0);
    pf_llc_past_rpa_res_cb_patch                              = app_llc_past_rpa_res_cb_patch ; 		        llc_past_rpa_res_cb(0,0);
    pf_llc_op_past_ind_handler_patch                          = app_llc_op_past_ind_handler_patch ; 		    llc_op_past_ind_handler(0,0,0,0);
    pf_ll_per_sync_ind_handler_patch                          = app_ll_per_sync_ind_handler_patch ; 		    ll_per_sync_ind_handler(0,0,0);
    pf_hci_le_per_adv_sync_transf_cmd_handler_patch           = app_hci_le_per_adv_sync_transf_cmd_handler_patch ; 		        hci_le_per_adv_sync_transf_cmd_handler(0,0,0);
    pf_hci_le_per_adv_set_info_transf_cmd_handler_patch       = app_hci_le_per_adv_set_info_transf_cmd_handler_patch ; 		    hci_le_per_adv_set_info_transf_cmd_handler(0,0,0);
    pf_hci_le_set_per_adv_sync_transf_param_cmd_handler_patch = app_hci_le_set_per_adv_sync_transf_param_cmd_handler_patch ;    hci_le_set_per_adv_sync_transf_param_cmd_handler(0,0,0);
    pf_llc_loc_dl_upd_proc_continue_patch                     = app_llc_loc_dl_upd_proc_continue_patch ; 		llc_loc_dl_upd_proc_continue(0,0);
    pf_llc_rem_dl_upd_proc_patch                              = app_llc_rem_dl_upd_proc_patch ; 		        llc_rem_dl_upd_proc(0,0,0,0,0);
    pf_ll_length_req_handler_patch                            = app_ll_length_req_handler_patch ; 		        ll_length_req_handler(0,0,0);
    pf_hci_le_set_data_len_cmd_handler_patch                  = app_hci_le_set_data_len_cmd_handler_patch ;     hci_le_set_data_len_cmd_handler(0,0,0);
    pf_dl_upd_proc_start_patch                                = app_dl_upd_proc_start_patch ; 		            dl_upd_proc_start(0);
    pf_llc_ll_length_req_pdu_send_patch                       = app_llc_ll_length_req_pdu_send_patch ; 		    llc_ll_length_req_pdu_send(0,0,0,0,0);
    pf_llc_ll_length_rsp_pdu_send_patch                       = app_llc_ll_length_rsp_pdu_send_patch ; 		    llc_ll_length_rsp_pdu_send(0,0,0,0,0);
    pf_llc_hci_dl_upd_info_send_patch                         = app_llc_hci_dl_upd_info_send_patch ; 		    llc_hci_dl_upd_info_send(0,0,0,0,0,0);
    pf_llc_dle_proc_err_cb_patch                              = app_llc_dle_proc_err_cb_patch ; 		        llc_dle_proc_err_cb(0,0,0);
    pf_ll_length_rsp_handler_patch                            = app_ll_length_rsp_handler_patch ; 		        ll_length_rsp_handler(0,0,0);
    pf_llc_op_dl_upd_ind_handler_patch                        = app_llc_op_dl_upd_ind_handler_patch ; 		    llc_op_dl_upd_ind_handler(0,0,0,0);
    pf_llc_loc_encrypt_proc_continue_patch                    = app_llc_loc_encrypt_proc_continue_patch ; 		llc_loc_encrypt_proc_continue(0,0,0);
    pf_llc_rem_encrypt_proc_continue_patch                    = app_llc_rem_encrypt_proc_continue_patch ; 		llc_rem_encrypt_proc_continue(0,0,0);
    pf_llc_aes_res_cb_patch                                   = app_llc_aes_res_cb_patch ; 		                llc_aes_res_cb(0,0,0);
    pf_llc_ll_pause_enc_req_pdu_send_patch                    = app_llc_ll_pause_enc_req_pdu_send_patch ; 		llc_ll_pause_enc_req_pdu_send(0);
    pf_llc_ll_pause_enc_rsp_pdu_send_patch                    = app_llc_ll_pause_enc_rsp_pdu_send_patch ; 		llc_ll_pause_enc_rsp_pdu_send(0,0);
    pf_llc_ll_enc_req_pdu_send_patch                          = app_llc_ll_enc_req_pdu_send_patch ; 		    llc_ll_enc_req_pdu_send(0,0,0,0,0);
    pf_llc_ll_enc_rsp_pdu_send_patch                          = app_llc_ll_enc_rsp_pdu_send_patch ; 		    llc_ll_enc_rsp_pdu_send(0,0,0);
    pf_llc_ll_start_enc_req_pdu_send_patch                    = app_llc_ll_start_enc_req_pdu_send_patch ; 		llc_ll_start_enc_req_pdu_send(0);
    pf_llc_ll_start_enc_rsp_pdu_send_patch                    = app_llc_ll_start_enc_rsp_pdu_send_patch ; 		llc_ll_start_enc_rsp_pdu_send(0,0);
    pf_llc_ll_pause_enc_rsp_ack_handler_patch                 = app_llc_ll_pause_enc_rsp_ack_handler_patch ;    llc_ll_pause_enc_rsp_ack_handler(0,0);
    pf_llc_ll_start_enc_rsp_ack_handler_patch                 = app_llc_ll_start_enc_rsp_ack_handler_patch ;    llc_ll_start_enc_rsp_ack_handler(0,0);
    pf_llc_ll_reject_ind_ack_handler_patch                    = app_llc_ll_reject_ind_ack_handler_patch ; 		llc_ll_reject_ind_ack_handler(0,0);
    pf_llc_hci_ltk_request_evt_send_patch                     = app_llc_hci_ltk_request_evt_send_patch ; 		llc_hci_ltk_request_evt_send(0,0,0);
    pf_llc_hci_enc_evt_send_patch                             = app_llc_hci_enc_evt_send_patch ; 		        llc_hci_enc_evt_send(0,0,0);
    pf_llc_loc_encrypt_proc_err_cb_patch                      = app_llc_loc_encrypt_proc_err_cb_patch ; 		llc_loc_encrypt_proc_err_cb(0,0,0);
    pf_llc_rem_encrypt_proc_err_cb_patch                      = app_llc_rem_encrypt_proc_err_cb_patch ; 		llc_rem_encrypt_proc_err_cb(0,0,0);
    pf_ll_pause_enc_req_handler_patch                         = app_ll_pause_enc_req_handler_patch ; 		    ll_pause_enc_req_handler(0,0,0);
    pf_ll_pause_enc_rsp_handler_patch                         = app_ll_pause_enc_rsp_handler_patch ; 		    ll_pause_enc_rsp_handler(0,0,0);
    pf_ll_enc_req_handler_patch                               = app_ll_enc_req_handler_patch ; 		            ll_enc_req_handler(0,0,0);
    pf_ll_enc_rsp_handler_patch                               = app_ll_enc_rsp_handler_patch ; 		            ll_enc_rsp_handler(0,0,0);
    pf_ll_start_enc_req_handler_patch                         = app_ll_start_enc_req_handler_patch ; 		    ll_start_enc_req_handler(0,0,0);
    pf_ll_start_enc_rsp_handler_patch                         = app_ll_start_enc_rsp_handler_patch ; 		    ll_start_enc_rsp_handler(0,0,0);
    pf_hci_le_start_enc_cmd_handler_patch                     = app_hci_le_start_enc_cmd_handler_patch ; 		hci_le_start_enc_cmd_handler(0,0,0);
    pf_hci_le_ltk_req_reply_cmd_handler_patch                 = app_hci_le_ltk_req_reply_cmd_handler_patch ;    hci_le_ltk_req_reply_cmd_handler(0,0,0);
    pf_hci_le_ltk_req_neg_reply_cmd_handler_patch             = app_hci_le_ltk_req_neg_reply_cmd_handler_patch ;hci_le_ltk_req_neg_reply_cmd_handler(0,0,0);
    pf_llc_encrypt_ind_handler_patch                          = app_llc_encrypt_ind_handler_patch ; 		    llc_encrypt_ind_handler(0,0,0,0);
    pf_llc_op_encrypt_ind_handler_patch                       = app_llc_op_encrypt_ind_handler_patch ; 		    llc_op_encrypt_ind_handler(0,0,0,0);
    pf_llc_loc_feats_exch_proc_continue_patch                 = app_llc_loc_feats_exch_proc_continue_patch ;    llc_loc_feats_exch_proc_continue(0,0);
    pf_llc_ll_feature_req_pdu_send_patch                      = app_llc_ll_feature_req_pdu_send_patch ; 		llc_ll_feature_req_pdu_send(0);
    pf_llc_ll_feature_rsp_pdu_send_patch                      = app_llc_ll_feature_rsp_pdu_send_patch ; 		llc_ll_feature_rsp_pdu_send(0);
    pf_llc_hci_feats_info_send_patch                          = app_llc_hci_feats_info_send_patch ; 		    llc_hci_feats_info_send(0,0,0);
    pf_llc_feats_exch_proc_err_cb_patch                       = app_llc_feats_exch_proc_err_cb_patch ; 		    llc_feats_exch_proc_err_cb(0,0,0);
    pf_ll_feature_req_handler_patch                           = app_ll_feature_req_handler_patch ; 		        ll_feature_req_handler(0,0,0);
    pf_ll_slave_feature_req_handler_patch                     = app_ll_slave_feature_req_handler_patch ; 		ll_slave_feature_req_handler(0,0,0);
    pf_ll_feature_rsp_handler_patch                           = app_ll_feature_rsp_handler_patch ; 		        ll_feature_rsp_handler(0,0,0);
    pf_hci_le_rd_rem_feats_cmd_handler_patch                  = app_hci_le_rd_rem_feats_cmd_handler_patch ;     hci_le_rd_rem_feats_cmd_handler(0,0,0);
    pf_llc_op_feats_exch_ind_handler_patch                    = app_llc_op_feats_exch_ind_handler_patch ; 		llc_op_feats_exch_ind_handler(0,0,0,0);
    pf_hci_acl_data_handler_patch                             = app_hci_acl_data_handler_patch ; 		        hci_acl_data_handler(0,0,0,0);
    pf_lld_acl_rx_ind_handler_patch                           = app_lld_acl_rx_ind_handler_patch ; 		        lld_acl_rx_ind_handler(0,0,0,0);
    pf_llc_hci_nb_cmp_pkts_evt_send_patch                     = app_llc_hci_nb_cmp_pkts_evt_send_patch ; 		llc_hci_nb_cmp_pkts_evt_send(0,0);
    pf_hci_rd_tx_pwr_lvl_cmd_handler_patch                    = app_hci_rd_tx_pwr_lvl_cmd_handler_patch ; 		hci_rd_tx_pwr_lvl_cmd_handler(0,0,0);
    pf_hci_rd_rssi_cmd_handler_patch                          = app_hci_rd_rssi_cmd_handler_patch ; 		    hci_rd_rssi_cmd_handler(0,0,0);
    pf_hci_vs_set_pref_slave_latency_cmd_handler_patch        = app_hci_vs_set_pref_slave_latency_cmd_handler_patch ; 		hci_vs_set_pref_slave_latency_cmd_handler(0,0,0);
    pf_hci_vs_set_pref_slave_evt_dur_cmd_handler_patch        = app_hci_vs_set_pref_slave_evt_dur_cmd_handler_patch ; 		hci_vs_set_pref_slave_evt_dur_cmd_handler(0,0,0);
    pf_hci_command_handler_patch                              = app_hci_command_handler_patch ; 		        hci_command_handler(0,0,0,0);
    pf_llc_loc_phy_upd_proc_continue_patch                    = app_llc_loc_phy_upd_proc_continue_patch ; 		llc_loc_phy_upd_proc_continue(0,0,0);
    pf_llc_rem_phy_upd_proc_continue_patch                    = app_llc_rem_phy_upd_proc_continue_patch ; 		llc_rem_phy_upd_proc_continue(0,0,0);
    pf_llc_dl_chg_check_patch                                 = app_llc_dl_chg_check_patch ; 		            llc_dl_chg_check(0,0,0);
    pf_llc_loc_phy_upd_proc_err_cb_patch                      = app_llc_loc_phy_upd_proc_err_cb_patch ; 		llc_loc_phy_upd_proc_err_cb(0,0,0);
    pf_llc_rem_phy_upd_proc_err_cb_patch                      = app_llc_rem_phy_upd_proc_err_cb_patch ; 		llc_rem_phy_upd_proc_err_cb(0,0,0);
    pf_ll_phy_req_handler_patch                               = app_ll_phy_req_handler_patch ; 		            ll_phy_req_handler(0,0,0);
    pf_ll_phy_rsp_handler_patch                               = app_ll_phy_rsp_handler_patch ; 		            ll_phy_rsp_handler(0,0,0);
    pf_ll_phy_update_ind_handler_patch                        = app_ll_phy_update_ind_handler_patch ; 		    ll_phy_update_ind_handler(0,0,0);
    pf_hci_le_rd_phy_cmd_handler_patch                        = app_hci_le_rd_phy_cmd_handler_patch ; 		    hci_le_rd_phy_cmd_handler(0,0,0);
    pf_hci_le_set_phy_cmd_handler_patch                       = app_hci_le_set_phy_cmd_handler_patch ; 		    hci_le_set_phy_cmd_handler(0,0,0);
    pf_llc_op_phy_upd_ind_handler_patch                       = app_llc_op_phy_upd_ind_handler_patch ; 		    llc_op_phy_upd_ind_handler(0,0,0,0);
    pf_lld_phy_upd_cfm_handler_patch                          = app_lld_phy_upd_cfm_handler_patch ; 		    lld_phy_upd_cfm_handler(0,0,0,0);
    pf_phy_upd_proc_start_patch                               = app_phy_upd_proc_start_patch ; 		            phy_upd_proc_start(0);
    pf_llc_ver_exch_loc_proc_continue_patch                   = app_llc_ver_exch_loc_proc_continue_patch ; 		llc_ver_exch_loc_proc_continue(0,0);
    pf_llc_op_ver_exch_ind_handler_patch                      = app_llc_op_ver_exch_ind_handler_patch ; 		llc_op_ver_exch_ind_handler(0,0,0,0);
    pf_ll_version_ind_handler_patch                           = app_ll_version_ind_handler_patch ; 		        ll_version_ind_handler(0,0,0);
    pf_hci_rd_rem_ver_info_cmd_handler_patch                  = app_hci_rd_rem_ver_info_cmd_handler_patch ; 	hci_rd_rem_ver_info_cmd_handler(0,0,0);
#endif

#if (APP_PATCH_LLM)
    pf_llm_adv_set_release_patch                                  = app_llm_adv_set_release_patch;		            llm_adv_set_release(0);
    pf_llm_adv_rpa_gen_cb_patch                                   = app_llm_adv_rpa_gen_cb_patch;		            llm_adv_rpa_gen_cb(0,0,0);
    pf_llm_adv_con_len_check_patch                                = app_llm_adv_con_len_check_patch;		        llm_adv_con_len_check(0,0);
    pf_llm_adv_set_dft_params_patch                               = app_llm_adv_set_dft_params_patch;		        llm_adv_set_dft_params(0);
    pf_llm_adv_hdl_to_id_patch                                    = app_llm_adv_hdl_to_id_patch;		            llm_adv_hdl_to_id(0,0);
    pf_hci_le_set_adv_param_cmd_handler_patch                     = app_hci_le_set_adv_param_cmd_handler_patch;		hci_le_set_adv_param_cmd_handler(0,0);
    pf_hci_le_set_adv_data_cmd_handler_patch                      = app_hci_le_set_adv_data_cmd_handler_patch;		hci_le_set_adv_data_cmd_handler(0,0);
    pf_hci_le_set_scan_rsp_data_cmd_handler_patch                 = app_hci_le_set_scan_rsp_data_cmd_handler_patch; hci_le_set_scan_rsp_data_cmd_handler(0,0);
    pf_hci_le_set_adv_en_cmd_handler_patch                        = app_hci_le_set_adv_en_cmd_handler_patch;		hci_le_set_adv_en_cmd_handler(0,0);
    pf_hci_le_set_ext_adv_param_cmd_handler_patch                 = app_hci_le_set_ext_adv_param_cmd_handler_patch; hci_le_set_ext_adv_param_cmd_handler(0,0);
    pf_hci_le_set_adv_set_rand_addr_cmd_handler_patch             = app_hci_le_set_adv_set_rand_addr_cmd_handler_patch;		hci_le_set_adv_set_rand_addr_cmd_handler(0,0);
    pf_hci_le_set_ext_adv_data_cmd_handler_patch                  = app_hci_le_set_ext_adv_data_cmd_handler_patch;          hci_le_set_ext_adv_data_cmd_handler(0,0);
    pf_hci_le_set_ext_scan_rsp_data_cmd_handler_patch             = app_hci_le_set_ext_scan_rsp_data_cmd_handler_patch;		hci_le_set_ext_scan_rsp_data_cmd_handler(0,0);
    pf_hci_le_set_ext_adv_en_cmd_handler_patch                    = app_hci_le_set_ext_adv_en_cmd_handler_patch;		    hci_le_set_ext_adv_en_cmd_handler(0,0);
    pf_hci_le_set_per_adv_param_cmd_handler_patch                 = app_hci_le_set_per_adv_param_cmd_handler_patch;		    hci_le_set_per_adv_param_cmd_handler(0,0);
    pf_hci_le_set_per_adv_data_cmd_handler_patch                  = app_hci_le_set_per_adv_data_cmd_handler_patch;		    hci_le_set_per_adv_data_cmd_handler(0,0);
    pf_hci_le_set_per_adv_en_cmd_handler_patch                    = app_hci_le_set_per_adv_en_cmd_handler_patch;		    hci_le_set_per_adv_en_cmd_handler(0,0);
    pf_hci_le_rd_max_adv_data_len_cmd_handler_patch               = app_hci_le_rd_max_adv_data_len_cmd_handler_patch;		hci_le_rd_max_adv_data_len_cmd_handler(0,0);
    pf_hci_le_rd_nb_supp_adv_sets_cmd_handler_patch               = app_hci_le_rd_nb_supp_adv_sets_cmd_handler_patch;		hci_le_rd_nb_supp_adv_sets_cmd_handler(0,0);
    pf_hci_le_rmv_adv_set_cmd_handler_patch                       = app_hci_le_rmv_adv_set_cmd_handler_patch;		        hci_le_rmv_adv_set_cmd_handler(0,0);
    pf_hci_le_clear_adv_sets_cmd_handler_patch                    = app_hci_le_clear_adv_sets_cmd_handler_patch;		    hci_le_clear_adv_sets_cmd_handler(0,0);
    pf_hci_le_set_conless_cte_tx_param_cmd_handler_patch          = app_hci_le_set_conless_cte_tx_param_cmd_handler_patch;  hci_le_set_conless_cte_tx_param_cmd_handler(0,0);
    pf_hci_le_set_conless_cte_tx_en_cmd_handler_patch             = app_hci_le_set_conless_cte_tx_en_cmd_handler_patch;		hci_le_set_conless_cte_tx_en_cmd_handler(0,0);
    pf_llm_adv_act_id_get_patch                                   = app_llm_adv_act_id_get_patch;		                    llm_adv_act_id_get(0,0,0);
    pf_llm_adv_set_id_get_patch                                   = app_llm_adv_set_id_get_patch;		                    llm_adv_set_id_get(0,0,0,0);
    pf_hci_le_rd_adv_ch_tx_pw_cmd_handler_patch                   = app_hci_le_rd_adv_ch_tx_pw_cmd_handler_patch;		    hci_le_rd_adv_ch_tx_pw_cmd_handler(0,0);
    pf_lld_scan_req_ind_handler_patch                             = app_lld_scan_req_ind_handler_patch;		                lld_scan_req_ind_handler(0,0,0,0);
    pf_lld_adv_end_ind_handler_patch                              = app_lld_adv_end_ind_handler_patch;		                lld_adv_end_ind_handler(0,0,0,0);
    pf_lld_per_adv_end_ind_handler_patch                          = app_lld_per_adv_end_ind_handler_patch;		            lld_per_adv_end_ind_handler(0,0,0,0);
    pf_llm_is_dev_connected_patch                                 = app_llm_is_dev_connected_patch;		                    llm_is_dev_connected(0,0);
    pf_llm_is_dev_synced_patch                                    = app_llm_is_dev_synced_patch;		                    llm_is_dev_synced(0,0,0);
    pf_llm_link_disc_patch                                        = app_llm_link_disc_patch;		                        llm_link_disc(0);
    pf_llm_ch_map_update_patch                                    = app_llm_ch_map_update_patch;		                    llm_ch_map_update();
    pf_llm_dev_list_empty_entry_patch                             = app_llm_dev_list_empty_entry_patch;		                llm_dev_list_empty_entry();
    pf_llm_dev_list_search_patch                                  = app_llm_dev_list_search_patch;		                    llm_dev_list_search(0,0);
    pf_llm_ral_search_patch                                       = app_llm_ral_search_patch;		                        llm_ral_search(0,0);
    pf_llm_ral_is_empty_patch                                     = app_llm_ral_is_empty_patch;		                        llm_ral_is_empty();
    pf_llm_init_patch                                             = app_llm_init_patch;		                                llm_init(0);
    pf_llm_clk_acc_set_patch                                      = app_llm_clk_acc_set_patch;		                        llm_clk_acc_set(0,0);
    pf_llm_activity_free_get_patch                                = app_llm_activity_free_get_patch;		                llm_activity_free_get(0);
    pf_llm_activity_free_set_patch                                = app_llm_activity_free_set_patch;		                llm_activity_free_set(0);
    pf_llm_is_wl_accessible_patch                                 = app_llm_is_wl_accessible_patch;		                    llm_is_wl_accessible();
    pf_llm_is_non_con_act_ongoing_check_patch                     = app_llm_is_non_con_act_ongoing_check_patch;		        llm_is_non_con_act_ongoing_check();
    pf_llm_is_rand_addr_in_use_check_patch                        = app_llm_is_rand_addr_in_use_check_patch;		        llm_is_rand_addr_in_use_check();
    pf_hci_reset_cmd_handler_patch                                = app_hci_reset_cmd_handler_patch;		                hci_reset_cmd_handler(0,0);
    pf_hci_rd_local_ver_info_cmd_handler_patch                    = app_hci_rd_local_ver_info_cmd_handler_patch;		    hci_rd_local_ver_info_cmd_handler(0,0);
    pf_hci_rd_bd_addr_cmd_handler_patch                           = app_hci_rd_bd_addr_cmd_handler_patch;		            hci_rd_bd_addr_cmd_handler(0,0);
    pf_hci_rd_local_supp_cmds_cmd_handler_patch                   = app_hci_rd_local_supp_cmds_cmd_handler_patch;		    hci_rd_local_supp_cmds_cmd_handler(0,0);
    pf_hci_rd_local_supp_feats_cmd_handler_patch                  = app_hci_rd_local_supp_feats_cmd_handler_patch;		    hci_rd_local_supp_feats_cmd_handler(0,0);
    pf_hci_set_evt_mask_cmd_handler_patch                         = app_hci_set_evt_mask_cmd_handler_patch;		            hci_set_evt_mask_cmd_handler(0,0);
    pf_hci_set_evt_mask_page_2_cmd_handler_patch                  = app_hci_set_evt_mask_page_2_cmd_handler_patch;		    hci_set_evt_mask_page_2_cmd_handler(0,0);
    pf_hci_set_ctrl_to_host_flow_ctrl_cmd_handler_patch           = app_hci_set_ctrl_to_host_flow_ctrl_cmd_handler_patch;   hci_set_ctrl_to_host_flow_ctrl_cmd_handler(0,0);
    pf_hci_host_buf_size_cmd_handler_patch                        = app_hci_host_buf_size_cmd_handler_patch;		        hci_host_buf_size_cmd_handler(0,0);
    pf_hci_host_nb_cmp_pkts_cmd_handler_patch                     = app_hci_host_nb_cmp_pkts_cmd_handler_patch;		        hci_host_nb_cmp_pkts_cmd_handler(0,0);
    pf_hci_le_set_evt_mask_cmd_handler_patch                      = app_hci_le_set_evt_mask_cmd_handler_patch;		        hci_le_set_evt_mask_cmd_handler(0,0);
    pf_hci_le_rd_buff_size_cmd_handler_patch                      = app_hci_le_rd_buff_size_cmd_handler_patch;		        hci_le_rd_buff_size_cmd_handler(0,0);
    pf_hci_le_rd_local_supp_feats_cmd_handler_patch               = app_hci_le_rd_local_supp_feats_cmd_handler_patch;		hci_le_rd_local_supp_feats_cmd_handler(0,0);
    pf_hci_le_set_rand_addr_cmd_handler_patch                     = app_hci_le_set_rand_addr_cmd_handler_patch;		        hci_le_set_rand_addr_cmd_handler(0,0);
    pf_hci_le_set_host_ch_class_cmd_handler_patch                 = app_hci_le_set_host_ch_class_cmd_handler_patch;		    hci_le_set_host_ch_class_cmd_handler(0,0);
    pf_hci_le_rd_wl_size_cmd_handler_patch                        = app_hci_le_rd_wl_size_cmd_handler_patch;		        hci_le_rd_wl_size_cmd_handler(0,0);
    pf_hci_le_clear_wlst_cmd_handler_patch                        = app_hci_le_clear_wlst_cmd_handler_patch;		        hci_le_clear_wlst_cmd_handler(0,0);
    pf_hci_le_add_dev_to_wlst_cmd_handler_patch                   = app_hci_le_add_dev_to_wlst_cmd_handler_patch;		    hci_le_add_dev_to_wlst_cmd_handler(0,0);
    pf_hci_le_rmv_dev_from_wlst_cmd_handler_patch                 = app_hci_le_rmv_dev_from_wlst_cmd_handler_patch;		    hci_le_rmv_dev_from_wlst_cmd_handler(0,0);
    pf_hci_le_rd_rslv_list_size_cmd_handler_patch                 = app_hci_le_rd_rslv_list_size_cmd_handler_patch;		    hci_le_rd_rslv_list_size_cmd_handler(0,0);
    pf_hci_le_clear_rslv_list_cmd_handler_patch                   = app_hci_le_clear_rslv_list_cmd_handler_patch;		    hci_le_clear_rslv_list_cmd_handler(0,0);
    pf_hci_le_add_dev_to_rslv_list_cmd_handler_patch              = app_hci_le_add_dev_to_rslv_list_cmd_handler_patch;		hci_le_add_dev_to_rslv_list_cmd_handler(0,0);
    pf_hci_le_rmv_dev_from_rslv_list_cmd_handler_patch            = app_hci_le_rmv_dev_from_rslv_list_cmd_handler_patch;    hci_le_rmv_dev_from_rslv_list_cmd_handler(0,0);
    pf_hci_le_set_rslv_priv_addr_to_cmd_handler_patch             = app_hci_le_set_rslv_priv_addr_to_cmd_handler_patch;		hci_le_set_rslv_priv_addr_to_cmd_handler(0,0);
    pf_hci_le_set_addr_resol_en_cmd_handler_patch                 = app_hci_le_set_addr_resol_en_cmd_handler_patch;		    hci_le_set_addr_resol_en_cmd_handler(0,0);
    pf_hci_le_rd_peer_rslv_addr_cmd_handler_patch                 = app_hci_le_rd_peer_rslv_addr_cmd_handler_patch;		    hci_le_rd_peer_rslv_addr_cmd_handler(0,0);
    pf_hci_le_rd_loc_rslv_addr_cmd_handler_patch                  = app_hci_le_rd_loc_rslv_addr_cmd_handler_patch;		    hci_le_rd_loc_rslv_addr_cmd_handler(0,0);
    pf_llm_aes_res_cb_patch                                       = app_llm_aes_res_cb_patch;		                        llm_aes_res_cb(0,0,0);
    pf_hci_le_enc_cmd_handler_patch                               = app_hci_le_enc_cmd_handler_patch;		                hci_le_enc_cmd_handler(0,0);
    pf_hci_le_rand_cmd_handler_patch                              = app_hci_le_rand_cmd_handler_patch;		                hci_le_rand_cmd_handler(0,0);
    pf_hci_le_rd_supp_states_cmd_handler_patch                    = app_hci_le_rd_supp_states_cmd_handler_patch;		    hci_le_rd_supp_states_cmd_handler(0,0);
    pf_hci_le_rd_tx_pwr_cmd_handler_patch                         = app_hci_le_rd_tx_pwr_cmd_handler_patch;		            hci_le_rd_tx_pwr_cmd_handler(0,0);
    pf_hci_le_rd_rf_path_comp_cmd_handler_patch                   = app_hci_le_rd_rf_path_comp_cmd_handler_patch;		    hci_le_rd_rf_path_comp_cmd_handler(0,0);
    pf_hci_le_wr_rf_path_comp_cmd_handler_patch                   = app_hci_le_wr_rf_path_comp_cmd_handler_patch;		    hci_le_wr_rf_path_comp_cmd_handler(0,0);
    pf_hci_le_set_priv_mode_cmd_handler_patch                     = app_hci_le_set_priv_mode_cmd_handler_patch;		        hci_le_set_priv_mode_cmd_handler(0,0);
    pf_hci_le_mod_sleep_clk_acc_cmd_handler_patch                 = app_hci_le_mod_sleep_clk_acc_cmd_handler_patch;	        hci_le_mod_sleep_clk_acc_cmd_handler(0,0);
    pf_hci_le_rd_antenna_inf_cmd_handler_patch                    = app_hci_le_rd_antenna_inf_cmd_handler_patch;	        hci_le_rd_antenna_inf_cmd_handler(0,0);
    pf_hci_le_rd_suggted_dft_data_len_cmd_handler_patch           = app_hci_le_rd_suggted_dft_data_len_cmd_handler_patch;	hci_le_rd_suggted_dft_data_len_cmd_handler(0,0);
    pf_hci_le_wr_suggted_dft_data_len_cmd_handler_patch           = app_hci_le_wr_suggted_dft_data_len_cmd_handler_patch;	hci_le_wr_suggted_dft_data_len_cmd_handler(0,0);
    pf_hci_le_rd_max_data_len_cmd_handler_patch                   = app_hci_le_rd_max_data_len_cmd_handler_patch;		    hci_le_rd_max_data_len_cmd_handler(0,0);
    pf_hci_le_rd_local_p256_public_key_cmd_handler_patch          = app_hci_le_rd_local_p256_public_key_cmd_handler_patch;  hci_le_rd_local_p256_public_key_cmd_handler(0,0);
    pf_hci_le_gen_dhkey_v1_cmd_handler_patch                      = app_hci_le_gen_dhkey_v1_cmd_handler_patch;		        hci_le_gen_dhkey_v1_cmd_handler(0,0);
    pf_hci_le_gen_dhkey_v2_cmd_handler_patch                      = app_hci_le_gen_dhkey_v2_cmd_handler_patch;		        hci_le_gen_dhkey_v2_cmd_handler(0,0);
    pf_hci_le_set_dft_phy_cmd_handler_patch                       = app_hci_le_set_dft_phy_cmd_handler_patch;		        hci_le_set_dft_phy_cmd_handler(0,0);
    pf_hci_le_set_dft_per_adv_sync_transf_param_cmd_handler_patch = app_hci_le_set_dft_per_adv_sync_transf_param_cmd_handler_patch;		hci_le_set_dft_per_adv_sync_transf_param_cmd_handler(0,0);
    pf_llm_hci_command_handler_patch                              = app_llm_hci_command_handler_patch;		                llm_hci_command_handler(0,0,0,0);
    pf_hci_le_create_con_cmd_handler_patch                        = app_hci_le_create_con_cmd_handler_patch;		        hci_le_create_con_cmd_handler(0,0);
    pf_hci_le_ext_create_con_cmd_handler_patch                    = app_hci_le_ext_create_con_cmd_handler_patch;		    hci_le_ext_create_con_cmd_handler(0,0);
    pf_hci_le_create_con_cancel_cmd_handler_patch                 = app_hci_le_create_con_cancel_cmd_handler_patch;		    hci_le_create_con_cancel_cmd_handler(0,0);
    pf_lld_init_end_ind_handler_patch                             = app_lld_init_end_ind_handler_patch;		                lld_init_end_ind_handler(0,0,0,0);
    pf_llm_scan_start_patch                                       = app_llm_scan_start_patch;		                        llm_scan_start(0);
    pf_llm_adv_reports_list_check_patch                           = app_llm_adv_reports_list_check_patch;		            llm_adv_reports_list_check(0,0,0,0,0,0);
    pf_llm_scan_sync_acad_attach_patch                            = app_llm_scan_sync_acad_attach_patch;		            llm_scan_sync_acad_attach(0,0,0);
    pf_llm_scan_sync_info_get_patch                               = app_llm_scan_sync_info_get_patch;		                llm_scan_sync_info_get(0,0,0,0);
    pf_llm_per_adv_sync_patch                                     = app_llm_per_adv_sync_patch;		                        llm_per_adv_sync(0);
    pf_hci_le_set_scan_param_cmd_handler_patch                    = app_hci_le_set_scan_param_cmd_handler_patch;		    hci_le_set_scan_param_cmd_handler(0,0);
    pf_hci_le_set_scan_en_cmd_handler_patch                       = app_hci_le_set_scan_en_cmd_handler_patch;		        hci_le_set_scan_en_cmd_handler(0,0);
    pf_hci_le_set_ext_scan_param_cmd_handler_patch                = app_hci_le_set_ext_scan_param_cmd_handler_patch;        hci_le_set_ext_scan_param_cmd_handler(0,0);
    pf_hci_le_set_ext_scan_en_cmd_handler_patch                   = app_hci_le_set_ext_scan_en_cmd_handler_patch;		    hci_le_set_ext_scan_en_cmd_handler(0,0);
    pf_hci_le_per_adv_create_sync_cmd_handler_patch               = app_hci_le_per_adv_create_sync_cmd_handler_patch;       hci_le_per_adv_create_sync_cmd_handler(0,0);
    pf_hci_le_per_adv_create_sync_cancel_cmd_handler_patch        = app_hci_le_per_adv_create_sync_cancel_cmd_handler_patch;hci_le_per_adv_create_sync_cancel_cmd_handler(0,0);
    pf_hci_le_per_adv_term_sync_cmd_handler_patch                 = app_hci_le_per_adv_term_sync_cmd_handler_patch;		    hci_le_per_adv_term_sync_cmd_handler(0,0);
    pf_hci_le_add_dev_to_per_adv_list_cmd_handler_patch           = app_hci_le_add_dev_to_per_adv_list_cmd_handler_patch;	hci_le_add_dev_to_per_adv_list_cmd_handler(0,0);
    pf_hci_le_rmv_dev_from_per_adv_list_cmd_handler_patch         = app_hci_le_rmv_dev_from_per_adv_list_cmd_handler_patch;	hci_le_rmv_dev_from_per_adv_list_cmd_handler(0,0);
    pf_hci_le_clear_per_adv_list_cmd_handler_patch                = app_hci_le_clear_per_adv_list_cmd_handler_patch;		hci_le_clear_per_adv_list_cmd_handler(0,0);
    pf_hci_le_rd_per_adv_list_size_cmd_handler_patch              = app_hci_le_rd_per_adv_list_size_cmd_handler_patch;		hci_le_rd_per_adv_list_size_cmd_handler(0,0);
    pf_hci_le_set_conless_iq_sampl_en_cmd_handler_patch           = app_hci_le_set_conless_iq_sampl_en_cmd_handler_patch;   hci_le_set_conless_iq_sampl_en_cmd_handler(0,0);
    pf_hci_le_set_per_adv_rec_en_cmd_handler_patch                = app_hci_le_set_per_adv_rec_en_cmd_handler_patch;		hci_le_set_per_adv_rec_en_cmd_handler(0,0);
    pf_llm_scan_period_to_handler_patch                           = app_llm_scan_period_to_handler_patch;		            llm_scan_period_to_handler(0,0,0,0);
    pf_lld_adv_rep_ind_handler_patch                              = app_lld_adv_rep_ind_handler_patch;		                lld_adv_rep_ind_handler(0,0,0,0);
    pf_lld_sync_start_req_handler_patch                           = app_lld_sync_start_req_handler_patch;		            lld_sync_start_req_handler(0,0,0,0);
    pf_lld_per_adv_rep_ind_handler_patch                          = app_lld_per_adv_rep_ind_handler_patch;		            lld_per_adv_rep_ind_handler(0,0,0,0);
    pf_lld_per_adv_rx_end_ind_handler_patch                       = app_lld_per_adv_rx_end_ind_handler_patch;		        lld_per_adv_rx_end_ind_handler(0,0,0,0);
    pf_lld_scan_end_ind_handler_patch                             = app_lld_scan_end_ind_handler_patch;		                lld_scan_end_ind_handler(0,0,0,0);
    pf_lld_conless_cte_rx_ind_handler_patch                       = app_lld_conless_cte_rx_ind_handler_patch;		        lld_conless_cte_rx_ind_handler(0,0,0,0);
    pf_llm_rpa_renew_to_handler_patch                             = app_llm_rpa_renew_to_handler_patch;		                llm_rpa_renew_to_handler(0,0,0,0);
    pf_llm_pub_key_gen_ind_handler_patch                          = app_llm_pub_key_gen_ind_handler_patch;		            llm_pub_key_gen_ind_handler(0,0,0,0);
    pf_llm_dh_key_gen_ind_handler_patch                           = app_llm_dh_key_gen_ind_handler_patch;		            llm_dh_key_gen_ind_handler(0,0,0,0);
    pf_llm_encrypt_ind_handler_patch                              = app_llm_encrypt_ind_handler_patch;		                llm_encrypt_ind_handler(0,0,0,0);
#endif

#if (APP_PATCH_LLC_CHMAP)
    pf_llc_loc_ch_map_proc_continue_patch     = app_llc_loc_ch_map_proc_continue_patch;		    llc_loc_ch_map_proc_continue(0,0,0);
    pf_llc_rem_ch_map_proc_continue_patch     = app_llc_rem_ch_map_proc_continue_patch;		    llc_rem_ch_map_proc_continue(0,0,0);
    pf_llc_ch_map_up_proc_err_cb_patch        = app_llc_ch_map_up_proc_err_cb_patch;		    llc_ch_map_up_proc_err_cb(0,0,0);
    pf_ll_channel_map_ind_handler_patch       = app_ll_channel_map_ind_handler_patch;		    ll_channel_map_ind_handler(0,0,0);
    pf_ll_min_used_channels_ind_handler_patch = app_ll_min_used_channels_ind_handler_patch;		ll_min_used_channels_ind_handler(0,0,0);
    pf_hci_le_rd_chnl_map_cmd_handler_patch   = app_hci_le_rd_chnl_map_cmd_handler_patch;		hci_le_rd_chnl_map_cmd_handler(0,0,0);
    pf_llc_op_ch_map_upd_ind_handler_patch    = app_llc_op_ch_map_upd_ind_handler_patch;		llc_op_ch_map_upd_ind_handler(0,0,0,0);
    pf_llm_ch_map_update_ind_handler_patch    = app_llm_ch_map_update_ind_handler_patch;		llm_ch_map_update_ind_handler(0,0,0,0);
    pf_lld_ch_map_upd_cfm_handler_patch       = app_lld_ch_map_upd_cfm_handler_patch;		    lld_ch_map_upd_cfm_handler(0,0,0,0);
    pf_llc_cleanup_patch                      = app_llc_cleanup_patch;		                    llc_cleanup(0,0);
    pf_llc_start_patch                        = app_llc_start_patch;		                    llc_start(0,0);
    pf_llc_llcp_trans_timer_set_patch         = app_llc_llcp_trans_timer_set_patch;		        llc_llcp_trans_timer_set(0,0,0);
    pf_llc_init_patch                         = app_llc_init_patch;		                        llc_init(0);
    pf_llc_stop_patch                         = app_llc_stop_patch;		                        llc_stop(0);
    pf_llc_llcp_state_set_patch               = app_llc_llcp_state_set_patch;		            llc_llcp_state_set(0,0,0);
    pf_llc_proc_reg_patch                     = app_llc_proc_reg_patch;		                    llc_proc_reg(0,0,0);
    pf_llc_proc_unreg_patch                   = app_llc_proc_unreg_patch;		                llc_proc_unreg(0,0);
    pf_llc_proc_id_get_patch                  = app_llc_proc_id_get_patch;		                llc_proc_id_get(0,0);
    pf_llc_proc_state_set_patch               = app_llc_proc_state_set_patch;		            llc_proc_state_set(0,0,0);
    pf_llc_proc_get_patch                     = app_llc_proc_get_patch;		                    llc_proc_get(0,0);
    pf_llc_proc_err_ind_patch                 = app_llc_proc_err_ind_patch;		                llc_proc_err_ind(0,0,0,0);
    pf_llc_proc_timer_set_patch               = app_llc_proc_timer_set_patch;		            llc_proc_timer_set(0,0,0);
    pf_llc_proc_timer_pause_set_patch         = app_llc_proc_timer_pause_set_patch;		        llc_proc_timer_pause_set(0,0,0);
    pf_llc_proc_collision_check_patch         = app_llc_proc_collision_check_patch;		        llc_proc_collision_check(0,0);
#endif

#if (APP_PATCH_NVDS)
    pf_nvds_is_magic_number_ok_patch = app_nvds_is_magic_number_ok_patch;	nvds_is_magic_number_ok();
    pf_nvds_walk_tag_patch           = app_nvds_walk_tag_patch;	            nvds_walk_tag(0,0,0);
    pf_nvds_browse_tag_patch         = app_nvds_browse_tag_patch;	        nvds_browse_tag(0,0,0);
    pf_nvds_purge_patch              = app_nvds_purge_patch;	            nvds_purge(0,0);
    pf_nvds_init_patch               = app_nvds_init_patch;	                nvds_init(0,0);
    pf_nvds_get_patch                = app_nvds_get_patch;	                nvds_get(0,0,0);
    pf_nvds_lock_patch               = app_nvds_lock_patch;	                nvds_lock(0);
    pf_nvds_put_patch                = app_nvds_put_patch;	                nvds_put(0,0,0);
    pf_nvds_del_patch                = app_nvds_del_patch;	                nvds_del(0);
    pf_nvds_null_init_patch          = app_nvds_null_init_patch;	        nvds_null_init();
    pf_nvds_read_patch               = app_nvds_read_patch;	                nvds_read(0,0,0);
    pf_nvds_write_patch              = app_nvds_write_patch;	            nvds_write(0,0,0);
    pf_nvds_erase_patch              = app_nvds_erase_patch;	            nvds_erase(0,0);
    pf_nvds_init_memory_patch        = app_nvds_init_memory_patch;	        nvds_init_memory();
#endif

#if (APP_PATCH_L2CC)
    pf_l2cc_init_patch                               = app_l2cc_init_patch;		                l2cc_init(0);
    pf_l2cc_create_patch                             = app_l2cc_create_patch;		            l2cc_create(0);
    pf_l2cc_cleanup_patch                            = app_l2cc_cleanup_patch;		            l2cc_cleanup(0,0);
    pf_l2cc_update_state_patch                       = app_l2cc_update_state_patch;		        l2cc_update_state(0,0,0);
    pf_l2cc_op_complete_patch                        = app_l2cc_op_complete_patch;		        l2cc_op_complete(0,0,0);
    pf_l2cc_send_cmp_evt_patch                       = app_l2cc_send_cmp_evt_patch;		        l2cc_send_cmp_evt(0,0,0,0,0,0);
    pf_l2cc_set_operation_ptr_patch                  = app_l2cc_set_operation_ptr_patch;		l2cc_set_operation_ptr(0,0,0);
    pf_l2cc_data_send_patch                          = app_l2cc_data_send_patch;		        l2cc_data_send(0,0);
    pf_l2cc_get_operation_patch                      = app_l2cc_get_operation_patch;		    l2cc_get_operation(0,0);
    pf_l2cc_get_operation_ptr_patch                  = app_l2cc_get_operation_ptr_patch;		l2cc_get_operation_ptr(0,0);
    pf_l2cc_lecb_free_patch                          = app_l2cc_lecb_free_patch;		        l2cc_lecb_free(0,0,0);
    pf_l2cc_lecb_send_con_req_patch                  = app_l2cc_lecb_send_con_req_patch;		l2cc_lecb_send_con_req(0,0,0,0,0,0,0);
    pf_l2cc_lecb_send_con_rsp_patch                  = app_l2cc_lecb_send_con_rsp_patch;		l2cc_lecb_send_con_rsp(0,0,0,0,0,0,0);
    pf_l2cc_lecb_send_disc_req_patch                 = app_l2cc_lecb_send_disc_req_patch;		l2cc_lecb_send_disc_req(0,0,0,0);
    pf_l2cc_lecb_send_disc_rsp_patch                 = app_l2cc_lecb_send_disc_rsp_patch;		l2cc_lecb_send_disc_rsp(0,0,0,0);
    pf_l2cc_lecb_send_credit_add_patch               = app_l2cc_lecb_send_credit_add_patch;		l2cc_lecb_send_credit_add(0,0,0,0);
    pf_l2cc_lecb_init_disconnect_patch               = app_l2cc_lecb_init_disconnect_patch;		l2cc_lecb_init_disconnect(0,0,0);
    pf_l2cc_lecb_h2l_err_patch                       = app_l2cc_lecb_h2l_err_patch;		        l2cc_lecb_h2l_err(0);
    pf_l2cc_lecb_find_patch                          = app_l2cc_lecb_find_patch;		        l2cc_lecb_find(0,0,0);
    pf_l2cc_pdu_pack_patch                           = app_l2cc_pdu_pack_patch;		            l2cc_pdu_pack(0,0,0,0,0);
    pf_l2cc_pdu_unpack_patch                         = app_l2cc_pdu_unpack_patch;		        l2cc_pdu_unpack(0,0,0,0,0,0,0);
    pf_l2cc_lecb_pdu_pack_patch                      = app_l2cc_lecb_pdu_pack_patch;		    l2cc_lecb_pdu_pack(0,0,0,0,0,0,0);
    pf_l2cc_lecb_pdu_unpack_patch                    = app_l2cc_lecb_pdu_unpack_patch;		    l2cc_lecb_pdu_unpack(0,0,0,0,0,0,0);
    pf_l2cc_pdu_rx_init_patch                        = app_l2cc_pdu_rx_init_patch;		        l2cc_pdu_rx_init(0);
    pf_l2cc_pdu_header_check_patch                   = app_l2cc_pdu_header_check_patch;		    l2cc_pdu_header_check(0,0,0);
    pf_l2cc_sig_send_cmd_reject_patch                = app_l2cc_sig_send_cmd_reject_patch;		l2cc_sig_send_cmd_reject(0,0,0,0,0);
    pf_l2cc_sig_lecb_l2h_err_patch                   = app_l2cc_sig_lecb_l2h_err_patch;		    l2cc_sig_lecb_l2h_err(0);
    pf_l2c_code_reject_handler_patch                 = app_l2c_code_reject_handler_patch;		l2c_code_reject_handler(0,0);
    pf_l2c_code_conn_param_upd_req_handler_patch     = app_l2c_code_conn_param_upd_req_handler_patch;		l2c_code_conn_param_upd_req_handler(0,0);
    pf_l2c_code_conn_param_upd_resp_handler_patch    = app_l2c_code_conn_param_upd_resp_handler_patch;		l2c_code_conn_param_upd_resp_handler(0,0);
    pf_l2c_code_le_cb_conn_req_handler_patch         = app_l2c_code_le_cb_conn_req_handler_patch;		    l2c_code_le_cb_conn_req_handler(0,0);
    pf_l2c_code_le_cb_conn_resp_handler_patch        = app_l2c_code_le_cb_conn_resp_handler_patch;		    l2c_code_le_cb_conn_resp_handler(0,0);
    pf_l2c_code_disconnection_req_handler_patch      = app_l2c_code_disconnection_req_handler_patch;		l2c_code_disconnection_req_handler(0,0);
    pf_l2c_code_disconnection_resp_handler_patch     = app_l2c_code_disconnection_resp_handler_patch;		l2c_code_disconnection_resp_handler(0,0);
    pf_l2c_code_le_flow_control_credit_handler_patch = app_l2c_code_le_flow_control_credit_handler_patch;   l2c_code_le_flow_control_credit_handler(0,0);
    pf_l2cc_sig_pdu_recv_handler_patch               = app_l2cc_sig_pdu_recv_handler_patch;		            l2cc_sig_pdu_recv_handler(0,0);
    pf_l2cc_process_op_patch                         = app_l2cc_process_op_patch;		                    l2cc_process_op(0,0,0,0);
    pf_l2cc_pdu_send_cmd_handler_patch               = app_l2cc_pdu_send_cmd_handler_patch;		            l2cc_pdu_send_cmd_handler(0,0,0,0);
    pf_l2cc_lecb_sdu_send_cmd_handler_patch          = app_l2cc_lecb_sdu_send_cmd_handler_patch;	        l2cc_lecb_sdu_send_cmd_handler(0,0,0,0);
    pf_l2cc_dbg_pdu_send_cmd_handler_patch           = app_l2cc_dbg_pdu_send_cmd_handler_patch;		        l2cc_dbg_pdu_send_cmd_handler(0,0,0,0);
    pf_hci_nb_cmp_pkts_evt_handler_patch             = app_hci_nb_cmp_pkts_evt_handler_patch;		        hci_nb_cmp_pkts_evt_handler(0,0,0,0);
//    pf_l2cc_task_hci_acl_data_handler_patch          = app_l2cc_task_hci_acl_data_handler_patch;            l2cc_task_hci_acl_data_handler(0,0,0,0);
    pf_l2cc_lecb_connect_cmd_handler_patch           = app_l2cc_lecb_connect_cmd_handler_patch;		        l2cc_lecb_connect_cmd_handler(0,0,0,0);
    pf_l2cc_lecb_connect_cfm_handler_patch           = app_l2cc_lecb_connect_cfm_handler_patch;		        l2cc_lecb_connect_cfm_handler(0,0,0,0);
    pf_l2cc_lecb_disconnect_cmd_handler_patch        = app_l2cc_lecb_disconnect_cmd_handler_patch;		    l2cc_lecb_disconnect_cmd_handler(0,0,0,0);
    pf_l2cc_signaling_trans_to_ind_handler_patch     = app_l2cc_signaling_trans_to_ind_handler_patch;		l2cc_signaling_trans_to_ind_handler(0,0,0,0);
    pf_l2cc_lecb_add_cmd_handler_patch               = app_l2cc_lecb_add_cmd_handler_patch;		            l2cc_lecb_add_cmd_handler(0,0,0,0);
    pf_l2cc_pdu_recv_ind_handler_patch               = app_l2cc_pdu_recv_ind_handler_patch;		            l2cc_pdu_recv_ind_handler(0,0,0,0);
//    pf_l2cc_cmp_evt_handler_patch                    = app_l2cc_cmp_evt_handler_patch;		                l2cc_cmp_evt_handler(0,0,0,0);
    pf_l2cc_default_msg_handler_patch                = app_l2cc_default_msg_handler_patch;		            l2cc_default_msg_handler(0,0,0,0);
    pf_l2cc_pdu_alloc_patch                          = app_l2cc_pdu_alloc_patch;		                    l2cc_pdu_alloc(0,0,0,0,0);
    pf_l2cm_init_patch                               = app_l2cm_init_patch;		                            l2cm_init(0);
    pf_l2cm_cleanup_patch                            = app_l2cm_cleanup_patch;		                        l2cm_cleanup(0);
    pf_l2cm_set_link_layer_buff_size_patch           = app_l2cm_set_link_layer_buff_size_patch;		        l2cm_set_link_layer_buff_size(0,0);
    pf_l2cm_tx_status_patch                          = app_l2cm_tx_status_patch;		                    l2cm_tx_status(0,0);
#endif

#if (APP_PATCH_GAP)
    pf_gapc_smp_check_key_distrib_patch              = app_gapc_smp_check_key_distrib_patch;		        gapc_smp_check_key_distrib(0,0);
    pf_gapc_smp_check_max_key_size_patch             = app_gapc_smp_check_max_key_size_patch;		        gapc_smp_check_max_key_size(0);
    pf_gapc_smp_check_pairing_feat_patch             = app_gapc_smp_check_pairing_feat_patch;		        gapc_smp_check_pairing_feat(0);
    pf_gapc_smp_is_sec_mode_reached_patch            = app_gapc_smp_is_sec_mode_reached_patch;		        gapc_smp_is_sec_mode_reached(0,0);
    pf_hci_enc_key_refr_evt_handler_patch            = app_hci_enc_key_refr_evt_handler_patch;		        hci_enc_key_refr_evt_handler(0,0,0,0);
    pf_gapc_bond_cfm_handler_patch                   = app_gapc_bond_cfm_handler_patch;		                gapc_bond_cfm_handler(0,0,0,0);
    pf_gapc_bond_cmd_handler_patch                   = app_gapc_bond_cmd_handler_patch;		                gapc_bond_cmd_handler(0,0,0,0);
    pf_gapc_encrypt_cfm_handler_patch                = app_gapc_encrypt_cfm_handler_patch;		            gapc_encrypt_cfm_handler(0,0,0,0);
    pf_gapc_encrypt_cmd_handler_patch                = app_gapc_encrypt_cmd_handler_patch;		            gapc_encrypt_cmd_handler(0,0,0,0);
    pf_gapc_gen_dh_key_ind_handler_patch             = app_gapc_gen_dh_key_ind_handler_patch;		        gapc_gen_dh_key_ind_handler(0,0,0,0);
    pf_gapc_key_press_notification_cmd_handler_patch = app_gapc_key_press_notification_cmd_handler_patch;   gapc_key_press_notification_cmd_handler(0,0,0,0);
    pf_gapc_security_cmd_handler_patch               = app_gapc_security_cmd_handler_patch;		            gapc_security_cmd_handler(0,0,0,0);
    pf_gapc_sign_cmd_handler_patch                   = app_gapc_sign_cmd_handler_patch;		                gapc_sign_cmd_handler(0,0,0,0);
    pf_gapc_smp_gapm_cmp_evt_handler_patch           = app_gapc_smp_gapm_cmp_evt_handler_patch;		        gapc_smp_gapm_cmp_evt_handler(0,0,0,0);
    pf_gapc_smp_rep_attempts_timer_handler_patch     = app_gapc_smp_rep_attempts_timer_handler_patch;		gapc_smp_rep_attempts_timer_handler(0,0,0,0);
    pf_gapc_smp_timeout_timer_ind_handler_patch      = app_gapc_smp_timeout_timer_ind_handler_patch;		gapc_smp_timeout_timer_ind_handler(0,0,0,0);
    pf_gapm_smpc_pub_key_ind_handler_patch           = app_gapm_smpc_pub_key_ind_handler_patch;		        gapm_smpc_pub_key_ind_handler(0,0,0,0);
    pf_hci_enc_chg_evt_handler_patch                 = app_hci_enc_chg_evt_handler_patch;		            hci_enc_chg_evt_handler(0,0,0,0);
    pf_hci_le_ltk_request_evt_handler_patch          = app_hci_le_ltk_request_evt_handler_patch;		    hci_le_ltk_request_evt_handler(0,0,0,0);
    pf_hci_le_start_enc_stat_evt_handler_patch       = app_hci_le_start_enc_stat_evt_handler_patch;		    hci_le_start_enc_stat_evt_handler(0,0,0,0);
//    pf_gapc_smp_l2cc_cmp_evt_handler_patch           = app_gapc_smp_l2cc_cmp_evt_handler_patch;		        gapc_smp_l2cc_cmp_evt_handler(0,0,0,0);
    pf_gapc_smp_pairing_irk_exch_patch               = app_gapc_smp_pairing_irk_exch_patch;		            gapc_smp_pairing_irk_exch(0,0,0);
    pf_gapc_smp_aes_res_cb_patch                     = app_gapc_smp_aes_res_cb_patch;		                gapc_smp_aes_res_cb(0,0,0);
    pf_gapc_smp_aes_sign_res_cb_patch                = app_gapc_smp_aes_sign_res_cb_patch;		            gapc_smp_aes_sign_res_cb(0,0,0);
    pf_gapc_smp_check_repeated_attempts_patch        = app_gapc_smp_check_repeated_attempts_patch;		    gapc_smp_check_repeated_attempts(0);
    pf_gapc_smp_cleanup_patch                        = app_gapc_smp_cleanup_patch;		                    gapc_smp_cleanup(0);
    pf_gapc_smp_clear_timeout_timer_patch            = app_gapc_smp_clear_timeout_timer_patch;		        gapc_smp_clear_timeout_timer(0);
    pf_gapc_smp_comp_cnf_val_patch                   = app_gapc_smp_comp_cnf_val_patch;		                gapc_smp_comp_cnf_val(0,0,0);
    pf_gapc_smp_comp_f4_patch                        = app_gapc_smp_comp_f4_patch;		                    gapc_smp_comp_f4(0,0,0,0,0,0);
    pf_gapc_smp_comp_f5_patch                        = app_gapc_smp_comp_f5_patch;		                    gapc_smp_comp_f5(0,0,0,0,0,0,0);
    pf_gapc_smp_comp_f6_patch                        = app_gapc_smp_comp_f6_patch;		                    gapc_smp_comp_f6(0,0,0,0,0,0,0,0,0);
    pf_gapc_smp_comp_g2_patch                        = app_gapc_smp_comp_g2_patch;		                    gapc_smp_comp_g2(0,0,0,0,0,0);
    pf_gapc_smp_dhkey_check_send_patch               = app_gapc_smp_dhkey_check_send_patch;		            gapc_smp_dhkey_check_send(0,0);
    pf_gapc_smp_encrypt_cfm_patch                    = app_gapc_smp_encrypt_cfm_patch;		                gapc_smp_encrypt_cfm(0,0,0,0);
    pf_gapc_smp_encrypt_start_handler_patch          = app_gapc_smp_encrypt_start_handler_patch;		    gapc_smp_encrypt_start_handler(0,0);
    pf_gapc_smp_encrypt_start_patch                  = app_gapc_smp_encrypt_start_patch;		            gapc_smp_encrypt_start(0,0);
    pf_gapc_smp_generate_rand_patch                  = app_gapc_smp_generate_rand_patch;		            gapc_smp_generate_rand(0,0);
    pf_gapc_smp_generate_stk_patch                   = app_gapc_smp_generate_stk_patch;		                gapc_smp_generate_stk(0,0);
    pf_gapc_smp_get_key_sec_prop_patch               = app_gapc_smp_get_key_sec_prop_patch;		            gapc_smp_get_key_sec_prop(0);
    pf_gapc_smp_get_next_passkey_bit_patch           = app_gapc_smp_get_next_passkey_bit_patch;		        gapc_smp_get_next_passkey_bit(0);
    pf_gapc_smp_handle_dh_key_check_complete_patch   = app_gapc_smp_handle_dh_key_check_complete_patch;		gapc_smp_handle_dh_key_check_complete(0,0);
    pf_gapc_smp_handle_enc_change_evt_patch          = app_gapc_smp_handle_enc_change_evt_patch;		    gapc_smp_handle_enc_change_evt(0,0,0);
    pf_gapc_smp_id_addr_info_send_patch              = app_gapc_smp_id_addr_info_send_patch;		        gapc_smp_id_addr_info_send(0);
    pf_gapc_smp_id_info_send_patch                   = app_gapc_smp_id_info_send_patch;		                gapc_smp_id_info_send(0,0);
    pf_gapc_smp_init_mac_key_calculation_patch       = app_gapc_smp_init_mac_key_calculation_patch;		    gapc_smp_init_mac_key_calculation(0);
    pf_gapc_smp_initiate_dhkey_check_patch           = app_gapc_smp_initiate_dhkey_check_patch;		        gapc_smp_initiate_dhkey_check(0);
    pf_gapc_smp_initiate_dhkey_verification_patch    = app_gapc_smp_initiate_dhkey_verification_patch;		gapc_smp_initiate_dhkey_verification(0);
    pf_gapc_smp_key_press_notification_ind_patch     = app_gapc_smp_key_press_notification_ind_patch;		gapc_smp_key_press_notification_ind(0,0);
    pf_gapc_smp_launch_rep_att_timer_patch           = app_gapc_smp_launch_rep_att_timer_patch;		        gapc_smp_launch_rep_att_timer(0);
    pf_gapc_smp_pair_cfm_send_patch                  = app_gapc_smp_pair_cfm_send_patch;		            gapc_smp_pair_cfm_send(0,0);
    pf_gapc_smp_pair_fail_send_patch                 = app_gapc_smp_pair_fail_send_patch;		            gapc_smp_pair_fail_send(0,0);
    pf_gapc_smp_pair_rand_send_patch                 = app_gapc_smp_pair_rand_send_patch;		            gapc_smp_pair_rand_send(0,0);
    pf_gapc_smp_pairing_csrk_exch_patch              = app_gapc_smp_pairing_csrk_exch_patch;		        gapc_smp_pairing_csrk_exch(0,0);
    pf_gapc_smp_pairing_end_patch                    = app_gapc_smp_pairing_end_patch;		                gapc_smp_pairing_end(0,0,0,0);
    pf_gapc_smp_pairing_ltk_exch_patch               = app_gapc_smp_pairing_ltk_exch_patch;		            gapc_smp_pairing_ltk_exch(0,0);
    pf_gapc_smp_pairing_nc_exch_patch                = app_gapc_smp_pairing_nc_exch_patch;		            gapc_smp_pairing_nc_exch(0,0);
    pf_gapc_smp_pairing_oob_exch_patch               = app_gapc_smp_pairing_oob_exch_patch;		            gapc_smp_pairing_oob_exch(0,0,0);
    pf_gapc_smp_pairing_req_handler_patch            = app_gapc_smp_pairing_req_handler_patch;		        gapc_smp_pairing_req_handler(0,0);
    pf_gapc_smp_pairing_rsp_patch                    = app_gapc_smp_pairing_rsp_patch;		                gapc_smp_pairing_rsp(0,0,0);
    pf_gapc_smp_pairing_start_patch                  = app_gapc_smp_pairing_start_patch;		            gapc_smp_pairing_start(0,0);
    pf_gapc_smp_pairing_tk_exch_patch                = app_gapc_smp_pairing_tk_exch_patch;		            gapc_smp_pairing_tk_exch(0,0,0);
    pf_gapc_smp_pdu_recv_patch                       = app_gapc_smp_pdu_recv_patch;		                    gapc_smp_pdu_recv(0,0,0);
    pf_gapc_smp_pdu_send_patch                       = app_gapc_smp_pdu_send_patch;		                    gapc_smp_pdu_send(0,0);
    pf_gapc_smp_recv_dhkey_check_pdu_patch           = app_gapc_smp_recv_dhkey_check_pdu_patch;		        gapc_smp_recv_dhkey_check_pdu(0,0);
    pf_gapc_smp_recv_enc_info_pdu_patch              = app_gapc_smp_recv_enc_info_pdu_patch;		        gapc_smp_recv_enc_info_pdu(0,0);
    pf_gapc_smp_recv_id_addr_info_pdu_patch          = app_gapc_smp_recv_id_addr_info_pdu_patch;		    gapc_smp_recv_id_addr_info_pdu(0,0);
    pf_gapc_smp_recv_id_info_pdu_patch               = app_gapc_smp_recv_id_info_pdu_patch;		            gapc_smp_recv_id_info_pdu(0,0);
    pf_gapc_smp_recv_keypress_notification_pdu_patch = app_gapc_smp_recv_keypress_notification_pdu_patch;   gapc_smp_recv_keypress_notification_pdu(0,0);
    pf_gapc_smp_recv_mst_id_pdu_patch                = app_gapc_smp_recv_mst_id_pdu_patch;		    gapc_smp_recv_mst_id_pdu(0,0);
    pf_gapc_smp_recv_pair_cfm_pdu_patch              = app_gapc_smp_recv_pair_cfm_pdu_patch;		gapc_smp_recv_pair_cfm_pdu(0,0);
    pf_gapc_smp_recv_pair_fail_pdu_patch             = app_gapc_smp_recv_pair_fail_pdu_patch;		gapc_smp_recv_pair_fail_pdu(0,0);
    pf_gapc_smp_recv_pair_rand_pdu_patch             = app_gapc_smp_recv_pair_rand_pdu_patch;		gapc_smp_recv_pair_rand_pdu(0,0);
    pf_gapc_smp_recv_pair_req_pdu_patch              = app_gapc_smp_recv_pair_req_pdu_patch;		gapc_smp_recv_pair_req_pdu(0,0);
    pf_gapc_smp_recv_pair_rsp_pdu_patch              = app_gapc_smp_recv_pair_rsp_pdu_patch;		gapc_smp_recv_pair_rsp_pdu(0,0);
    pf_gapc_smp_recv_public_key_pdu_patch            = app_gapc_smp_recv_public_key_pdu_patch;		gapc_smp_recv_public_key_pdu(0,0);
    pf_gapc_smp_recv_sec_req_pdu_patch               = app_gapc_smp_recv_sec_req_pdu_patch;		    gapc_smp_recv_sec_req_pdu(0,0);
    pf_gapc_smp_recv_sign_info_pdu_patch             = app_gapc_smp_recv_sign_info_pdu_patch;		gapc_smp_recv_sign_info_pdu(0,0);
    pf_gapc_smp_security_req_send_patch              = app_gapc_smp_security_req_send_patch;		gapc_smp_security_req_send(0,0);
    pf_gapc_smp_send_gen_dh_key_cmd_patch            = app_gapc_smp_send_gen_dh_key_cmd_patch;		gapc_smp_send_gen_dh_key_cmd(0,0,0);
    pf_gapc_smp_send_get_pub_key_cmd_patch           = app_gapc_smp_send_get_pub_key_cmd_patch;		gapc_smp_send_get_pub_key_cmd(0);
    pf_gapc_smp_send_ltk_req_rsp_patch               = app_gapc_smp_send_ltk_req_rsp_patch;		    gapc_smp_send_ltk_req_rsp(0,0,0);
    pf_gapc_smp_send_pairing_ind_patch               = app_gapc_smp_send_pairing_ind_patch;		    gapc_smp_send_pairing_ind(0,0,0);
    pf_gapc_smp_send_pairing_req_ind_patch           = app_gapc_smp_send_pairing_req_ind_patch;		gapc_smp_send_pairing_req_ind(0,0);
    pf_gapc_smp_send_start_enc_cmd_patch             = app_gapc_smp_send_start_enc_cmd_patch;		gapc_smp_send_start_enc_cmd(0,0,0,0,0);
    pf_gapc_smp_sign_command_patch                   = app_gapc_smp_sign_command_patch;		        gapc_smp_sign_command(0,0);
    pf_gapc_smp_tkdp_rcp_continue_patch              = app_gapc_smp_tkdp_rcp_continue_patch;		gapc_smp_tkdp_rcp_continue(0,0);
    pf_gapc_smp_tkdp_rcp_start_patch                 = app_gapc_smp_tkdp_rcp_start_patch;		    gapc_smp_tkdp_rcp_start(0,0);
    pf_gapc_smp_tkdp_send_continue_patch             = app_gapc_smp_tkdp_send_continue_patch;		gapc_smp_tkdp_send_continue(0,0);
    pf_gapc_smp_tkdp_send_start_patch                = app_gapc_smp_tkdp_send_start_patch;		    gapc_smp_tkdp_send_start(0,0);
    pf_gapm_gen_dh_key_cmd_handler_patch             = app_gapm_gen_dh_key_cmd_handler_patch;		gapm_gen_dh_key_cmd_handler(0,0,0,0);
    pf_gapm_gen_rand_addr_cmd_handler_patch          = app_gapm_gen_rand_addr_cmd_handler_patch;    gapm_gen_rand_addr_cmd_handler(0,0,0,0);
    pf_gapm_get_pub_key_cmd_handler_patch            = app_gapm_get_pub_key_cmd_handler_patch;		gapm_get_pub_key_cmd_handler(0,0,0,0);
    pf_gapm_resolv_addr_cmd_handler_patch            = app_gapm_resolv_addr_cmd_handler_patch;		gapm_resolv_addr_cmd_handler(0,0,0,0);
    pf_gapm_use_enc_block_cmd_handler_patch          = app_gapm_use_enc_block_cmd_handler_patch;	gapm_use_enc_block_cmd_handler(0,0,0,0);
//    pf_hci_le_enc_cmd_cmp_evt_handler_patch          = app_hci_le_enc_cmd_cmp_evt_handler_patch;	hci_le_enc_cmd_cmp_evt_handler(0,0);
    pf_hci_le_gen_dhkey_cmp_evt_handler_patch        = app_hci_le_gen_dhkey_cmp_evt_handler_patch;	hci_le_gen_dhkey_cmp_evt_handler(0,0);
    pf_hci_le_gen_dhkey_v2_stat_evt_handler_patch    = app_hci_le_gen_dhkey_v2_stat_evt_handler_patch;		hci_le_gen_dhkey_v2_stat_evt_handler(0,0);
    pf_gapm_smp_gen_rand_addr_cb_patch               = app_gapm_smp_gen_rand_addr_cb_patch;		            gapm_smp_gen_rand_addr_cb(0,0,0);
    pf_gapm_smp_gen_rand_nb_cb_patch                 = app_gapm_smp_gen_rand_nb_cb_patch;		            gapm_smp_gen_rand_nb_cb(0,0,0);
    pf_gapm_smp_resolv_res_cb_patch                  = app_gapm_smp_resolv_res_cb_patch;		            gapm_smp_resolv_res_cb(0,0);
//    pf_gapm_smp_send_hci_encrypt_patch               = app_gapm_smp_send_hci_encrypt_patch;		            gapm_smp_send_hci_encrypt(0,0);
    pf_gapm_smp_use_enc_block_cb_patch               = app_gapm_smp_use_enc_block_cb_patch;		            gapm_smp_use_enc_block_cb(0,0,0);

#endif

    printf("ZZZ: %s ,------- End \r\n", __FUNCTION__);
}



