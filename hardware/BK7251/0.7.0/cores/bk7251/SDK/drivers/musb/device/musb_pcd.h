#ifndef __MUSB_PCD_H__
#define __MUSB_PCD_H__
#include <musb_reg.h>
#if defined(__CC_ARM) || defined (__IAR_SYSTEMS_ICC__)
    #define MUSB_WEAK   __weak
#elif defined(__GNUC__) || defined(__ADSPBLACKFIN__)
    #define MUSB_WEAK   __attribute__((weak))
#endif

struct musb_pcd_ep
{
    uint8_t *buffer;
    uint8_t is_iso;
    uint8_t max_packet_size;
    uint32_t transfer_size;
    uint32_t remain_size;
};

struct musb_setup_request
{
    uint8_t  request_type;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
};

struct musb_pcd_handler_type
{
    musb_instance_t hmusb;
    struct musb_pcd_ep in_ep[16];
    struct musb_pcd_ep out_ep[16];
    uint8_t address;
    struct musb_setup_request setup_pkt;
    void *user_data;
};
typedef struct musb_pcd_handler_type *musb_pcd_handler_type_t;

extern void musb_pcd_irq(musb_pcd_handler_type_t hpcd);
extern void musb_pcd_reset_callback(musb_pcd_handler_type_t hpcd);
extern void musb_pcd_disconnect_callback(musb_pcd_handler_type_t hpcd);
extern void musb_pcd_connect_callback(musb_pcd_handler_type_t hpcd);
extern void musb_pcd_sof_callback(musb_pcd_handler_type_t hpcd);
extern void musb_pcd_suspend_callback(musb_pcd_handler_type_t hpcd);
extern void musb_pcd_resume_callback(musb_pcd_handler_type_t hpcd);
extern void musb_pcd_setup_callback(musb_pcd_handler_type_t hpcd, struct musb_setup_request * setup);
extern void musb_pcd_data_out_callback(musb_pcd_handler_type_t hpcd, uint32_t ep_addr);
extern void musb_pcd_data_in_callback(musb_pcd_handler_type_t hpcd, uint32_t ep_addr);

extern uint32_t musb_pcd_ep_read_prepare(musb_pcd_handler_type_t hpcd, uint32_t ep_addr, void *buffer, uint32_t size);
extern uint32_t musb_pcd_ep_write(musb_pcd_handler_type_t hpcd, uint8_t ep_addr, void *buffer, uint32_t size);
extern int musb_pcd_ep_set_stall(musb_pcd_handler_type_t hpcd, uint8_t ep_addr);
extern int musb_pcd_ep_clear_stall(musb_pcd_handler_type_t hpcd, uint8_t ep_addr);
extern int musb_pcd_set_address(musb_pcd_handler_type_t hpcd, uint8_t address);
extern int musb_pcd_ep_open(musb_pcd_handler_type_t hpcd, uint8_t ep_addr, uint8_t max_packet_size, uint8_t is_iso);
extern int musb_pcd_ep_close(musb_pcd_handler_type_t hpcd, uint8_t ep_addr);
extern int musb_ep0_send_status(musb_pcd_handler_type_t hpcd);

#endif
