#ifndef __MUSB_REG_H
#define __MUSB_REG_H
#include <stdint.h>

#define	MUSB_FDRC_AHB

#define USB_DPLL_DIVISION                 (2)

typedef volatile unsigned char *VUINT8_PTR;
typedef volatile unsigned long *VUINT32_PTR;

#define USB_BASE_ADDR           (0x00804000)
#define REG_USB_BASE_ADDR       USB_BASE_ADDR

#define REG_USB_TESTMODE        (*((VUINT8_PTR) (USB_BASE_ADDR + 0x0F)))
#define VREG_USB_DEVCTL         (*((VUINT8_PTR) (USB_BASE_ADDR + 0x60)))

#define REG_AHB2_USB_OTG_CFG              (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x80)))
#define REG_AHB2_USB_DMA_ENDP             (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x84)))
#define REG_AHB2_USB_VTH                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x88)))
#define REG_AHB2_USB_GEN                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x8C)))
#define REG_AHB2_USB_STAT                 (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x90)))
#define REG_AHB2_USB_INT                  (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x94)))
#define REG_AHB2_USB_RESET                (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x98)))
#define REG_AHB2_USB_DEV_CFG              (*((volatile unsigned char *) (REG_USB_BASE_ADDR + 0x9C)))

#define VREG_USB_RAM_PROTECT   (*((VUINT32_PTR) (USB_BASE_ADDR + 0x300)))
#define USB_RAM_PROTECT_BIT                    (1 << 0)

#define VREG_USB_DIG_MODE      (*((VUINT32_PTR) (USB_BASE_ADDR + 0x304)))
#define USB_DIGITAL_MODE_BIT                   (1 << 0)

#define VREG_USB_BYPASS        (*((VUINT32_PTR) (USB_BASE_ADDR + 0x308)))
#define BYPASS_OTG_DISABLE0_BIT                     (1 << 0)
#define BYPASS_DM_DATA0_BIT                         (1 << 1)
#define BYPASS_DM_EN0_BIT                           (1 << 2)
#define BYPASS_SEL0_BIT                             (1 << 3)

#define VREG_USB_TEST_MODE     (*((VUINT32_PTR) (USB_BASE_ADDR + 0x30C)))
#define USB_TEST_MODE_BIT                   (1 << 1)
#define USB_COMMON_ONN_BIT                  (1 << 0)

#define VREG_USB_PHY_REG0      (*((VUINT32_PTR) (USB_BASE_ADDR + 0x310)))
#define USB_PHY_REG0_DEFAULT_VALUE          (0xE0078518)

#define VREG_USB_PHY_REG1      (*((VUINT32_PTR) (USB_BASE_ADDR + 0x314)))
#define USB_PHY_REG1_DEFAULT_VALUE          (0x020002a7)

#define VREG_USB_PHY_REG2      (*((VUINT32_PTR) (USB_BASE_ADDR + 0x318)))
#define USB_PHY_REG2_DEFAULT_VALUE          (0x45755556)

#define VREG_USB_PHY_REG3      (*((VUINT32_PTR) (USB_BASE_ADDR + 0x31C)))
#define USB_PHY_REG3_DEFAULT_VALUE          (0x68c00005)

#define VREG_USB_PHY_REG4      (*((VUINT32_PTR) (USB_BASE_ADDR + 0x320)))
#define USB_PHY_REG4_DEFAULT_VALUE          (0x420)

#define VREG_USB_PHY_REG5      (*((VUINT32_PTR) (USB_BASE_ADDR + 0x324)))
#define USB_PHY_REG5_DEFAULT_VALUE          (0x0)

#define VREG_USB_TEST_CTRL     (*((VUINT32_PTR) (USB_BASE_ADDR + 0x328))

typedef volatile uint8_t m_reg_t;
typedef volatile uint32_t m_reg32_t;

typedef struct 
{
    m_reg_t suspend_enable:1;           /* D0 Set by the CPU to enable entry 
                                              into Suspend mode when Suspend 
                                              signaling is received on the bus.
                                              Note: This bit is only valid when 
                                              the MUSBFDRC is in Peripheral 
                                              mode. */
    m_reg_t suspend_mode:1;             /* D1 In Peripheral mode, this bit is 
                                              set by the USB when Suspend mode 
                                              is entered. In Host mode, this bit
                                              is set by the CPU when Suspend 
                                              mode is to be entered. The bit is 
                                              cleared when the CPU reads the 
                                              Interrupt register or sets the 
                                              Resume bit (above) or leaves Host 
                                              mode. */
    m_reg_t resume:1;                   /* D2 Set by the CPU to generate Resume 
                                              signaling when the device is in 
                                              Suspend mode. In Peripheral mode, 
                                              the CPU should clear this bit 
                                              after 10 ms (a maximum of 15 ms), 
                                              to end Resume signaling. In Host 
                                              mode, the CPU should clear this 
                                              bit after 20 ms. */
    m_reg_t reset:1;                    /* D3 In Peripheral mode, this read-only 
                                              bit is set while Reset signaling 
                                              is present on the bus. In Host 
                                              mode, this bit is set by the CPU 
                                              for 20 ms, to generate Reset 
                                              signaling on the bus. It is 
                                              automatically set when Host mode 
                                              is entered following Host 
                                              Negotiation (see Section 13): the 
                                              CPU should then clear it after 20 
                                              ms. */
    m_reg_t vbus_lo:1;                  /* D4 This bit indicates the state of 
                                              the VBUSLO input signal to the 
                                              MUSBFDRC. 1  VBus above the 
                                              Session End threshold, 0  VBus 
                                              below this threshold. */
    m_reg_t vbus_sess:1;                /* D5 This bit indicates the state of 
                                              the VBUSSES input signal to the 
                                              MUSBFDRC. 1  VBus above the 
                                              Session Valid threshold for a ‘B’ 
                                              device, 0  VBus below this 
                                              threshold. */
    m_reg_t vbus_val:1;                 /* D6 This bit indicates the state of 
                                              the VBUSVAL input signal to the 
                                              MUSBFDRC. 1  VBus above the VBus 
                                              Valid threshold, 0  VBus below 
                                              this threshold. */
    m_reg_t iso_update:1;               /* D7 Set by the CPU to cause the 
                                              MUSBFDRC to wait for a SOF token 
                                              after TxPktRdy has been set before 
                                              sending the packet. If an IN token 
                                              is received before a SOF token, a 
                                              zero length data packet will be 
                                              sent. This bit is only valid when 
                                              the MUSBFDRC is in Peripheral mode 
                                              and then only affects endpoints 
                                              performing Isochronous 
                                              transfers. */

} mihd01t;/* musb_instance offset 0x01 */

typedef struct 
{
    m_reg_t suspend:1;                  /* D0 Set when Suspend signaling is 
                                              detected on the bus. Only valid 
                                              in Peripheral mode. */
    m_reg_t resume:1;                   /* D1 Set when Resume signaling is 
                                              detected on the bus while the 
                                              MUSBFDRC is in Suspend mode. */
    m_reg_t babble_reset:1;             /* D2 Set in Peripheral mode when Reset 
                                              signaling is detected D2 on the 
                                              bus.
                                              or
                                              Set in Host mode when babble is 
                                              detected. */
    m_reg_t SOF:1;                      /* D3 Set when a new frame starts. */
    m_reg_t connect:1;                  /* D4 Set when a device connection is 
                                              detected. Only valid in Host 
                                              mode. */
    m_reg_t disconnect:1;               /* D5 Set in Host mode when a device 
                                              disconnect is detected. Set in 
                                              Peripheral mode when a session 
                                              ends. */
    m_reg_t session_request:1;          /* D6 Set when Session Request signaling 
                                              has been detected. Only valid when 
                                              MUSBFDRC is ‘A’ device. */
    m_reg_t vbus_error:1;               /* D7 Set when VBus drops below the VBus 
                                              Valid threshold during a session. 
                                              Only valid when MUSBFDRC is ‘A’ 
                                              device. */

} mihd06t;/* musb_instance offset 0x06 */
typedef mihd06t mihd0Bt;
typedef struct 
{
    m_reg_t session:1;                  /* D0 When operating as an ‘A’ device, 
                                              this bit is set or cleared by the 
                                              CPU to start or end a session.
                                              When operating as a ‘B’ device, 
                                              this bit is set/cleared by the 
                                              MUSBFDRC when a session 
                                              starts/ends. It can also be set by 
                                              the CPU to initiate the Session 
                                              Request Protocol, and cleared to 
                                              execute a soft disconnect from the 
                                              host (which then ends the 
                                              session). Note: When the Session 
                                              bit is cleared, CLKOUT is stopped 
                                              so that the PHY is put into 
                                              Suspend mode and power is saved 
                                              while no session is in 
                                              progress. */
    m_reg_t host_req:1;                 /* D1 When set, the MUSBFDRC will 
                                              initiate the Host Negotiation when 
                                              Suspend mode is entered. It is 
                                              cleared when Host Negotiation is 
                                              completed. See Section 13. 
                                              (‘B’ device only) */
    m_reg_t host_mode:1;                /* D2 This Read-only bit is set when the 
                                              MUSBFDRC is acting as a Host. */
    m_reg_t PDCON:1;                    /* D3 This Read-only bit becomes set 
                                              prior to operation as a host to 
                                              indicate that a pull-down resistor 
                                              is required on the USB D+ line 
                                              (alongside a similar pull-down 
                                              resistor on the USB D- line. The 
                                              selection of these resistors will 
                                              normally be handled automatically 
                                              by external circuitry using the 
                                              PDCON output (which will also have 
                                              gone high). */
    m_reg_t PUCON:1;                    /* D4 This Read-only bit becomes set 
                                              prior to operation as a peripheral 
                                              to indicate that a pull-up 
                                              resistor is required on the USB 
                                              D+ line. The selection of this 
                                              resistor will normally be handled 
                                              automatically by external 
                                              circuitry using the PUCON output 
                                              (which will also have gone 
                                              high). */
    m_reg_t ls_device:1;                /* D5 This bit is set when a low-speed 
                                              device has been detected being 
                                              connected to the port. Only valid 
                                              in Host mode. */
    m_reg_t fs_device:1;                /* D6 This bit is set when a full-speed 
                                              device has been detected being 
                                              connected to the port. Only valid 
                                              in Host mode. */
    m_reg_t CID:1;                      /* D7 This bit indicates the state of 
                                              the CID input signal to the 
                                              MUSBFDRC. It should be used to 
                                              indicate the ID pin of the 
                                              mini-A/B connector. 1  B-type, 0  
                                              A-type. */
} mihd0Ft;/* musb_instance offset 0x0F */

typedef union
{
    struct
    {
        m_reg_t rx_pkt_rdy:1;           /* D0 This bit is set when a data packet 
                                              has been received. An interrupt is 
                                              generated when this bit is set. 
                                              The CPU clears this bit by setting 
                                              the ServicedRxPktRdy bit. */
        m_reg_t tx_pkt_rdy:1;           /* D1 The CPU sets this bit after 
                                              loading a data packet into the 
                                              FIFO. It is cleared automatically 
                                              when the data packet has been 
                                              transmitted. An interrupt is 
                                              generated when the bit is 
                                              cleared. */
        m_reg_t sent_stall:1;           /* D2 This bit is set when a STALL 
                                              handshake is transmitted. The CPU 
                                              should clear this bit. */
        m_reg_t data_end:1;             /* D3 The CPU sets this bit:
                                               1. When setting TxPktRdy for the 
                                                  last data packet.
                                               2. When clearing RxPktRdy after 
                                                  unloading the last data 
                                                  packet.
                                               3. When setting TxPktRdy for a 
                                                  zero length data packet. 
                                              It is cleared automatically. */
        m_reg_t setup_end:1;            /* D4 This bit will be set when a 
                                              control transaction ends before 
                                              the DataEnd bit has been set.
                                              An interrupt will be generated and 
                                              the FIFO flushed at this time. The 
                                              bit is cleared by the CPU writing 
                                              a 1 to the ServicedSetupEnd 
                                              bit. */
        m_reg_t send_stall:1;           /* D5 The CPU writes a 1 to this bit to 
                                              terminate the current transaction. 
                                              The STALL handshake will be 
                                              transmitted and then this bit will 
                                              be cleared automatically. */
        m_reg_t serviced_rx_pkt_rdy:1;  /* D6 The CPU writes a 1 to this bit to 
                                              clear the RxPktRdy bit. It is 
                                              cleared automatically. */
        m_reg_t serviced_setup_end:1;   /* D7 The CPU writes a 1 to this bit to 
                                              clear the SetupEnd bit. It is 
                                              cleared automatically. */
    } csr0;
    struct
    {
        m_reg_t tx_pkt_rdy:1;           /* D0 The CPU sets this bit after 
                                              loading a data packet into the 
                                              FIFO. It is cleared automatically 
                                              when a data packet has been 
                                              transmitted. An interrupt is 
                                              generated (if enabled) when the 
                                              bit is cleared. */
        m_reg_t fifo_not_empty:1;       /* D1 The USB sets this bit when there 
                                              is at least 1 packet in the Tx 
                                              FIFO. */
        m_reg_t under_run:1;            /* D2 The USB sets this bit if an IN 
                                              token is received when the 
                                              tx_pkt_rdy bit not set. The CPU 
                                              should clear this bit. */
        m_reg_t flush_fifo:1;           /* D3 The CPU writes a 1 to this bit to 
                                              flush the next packet to be 
                                              transmitted from the endpoint Tx 
                                              FIFO. The FIFO pointer is reset 
                                              and the TxPktRdy bit (below) is 
                                              cleared. Note: FlushFIFO has no 
                                              effect unless tx_pkt_rdy is set. 
                                              Also note that, if the FIFO is 
                                              double-buffered, flush_fifo may 
                                              need to be set twice to completely 
                                              clear the FIFO. */
        m_reg_t send_stall:1;           /* D4 The CPU writes a 1 to this bit to 
                                              issue a STALL handshake to an IN 
                                              token. The CPU clears this bit to 
                                              terminate the stall condition. 
                                              Note: This bit has no effect where 
                                              the endpoint is being used for 
                                              Isochronous transfers. */
        m_reg_t sent_stall:1;           /* D5 This bit is set when a STALL 
                                              handshake is transmitted. The FIFO 
                                              is flushed and the TxPktRdy bit is 
                                              cleared (see below). The CPU 
                                              should clear this bit. */
        m_reg_t clr_data_tog:1;         /* D6 The CPU writes a 1 to this bit to 
                                              reset the endpoint data toggle to 
                                              0. */
        m_reg_t reserved:1;             /* D7 Unused. Returns zero when read. */
    } tx_csr1;

} mi_d11t;/* musb_instance offset 0x11 */

typedef union
{
    struct
    {
        m_reg_t flush_fifo:1;           /* D0 The CPU writes a 1 to this bit to 
                                              flush the next packet to be 
                                              transmitted/read from the Endpoint 
                                              0 FIFO. The FIFO pointer is reset 
                                              and the tx_pkt_rdy/rx_pkt_rdy bit 
                                              (above) is cleared. Note: 
                                              flush_fifo has no effect unless 
                                              tx_pkt_rdy/rx_pkt_rdy is set. */
        m_reg_t reserved:7;           
    } csr02;
    struct
    {
        m_reg_t reserved:2;             /* D0-D1 Unused, always returns 0. */
        m_reg_t dma_mode:1;             /* D2 Two modes of DMA operation are 
                                              supported. The CPU sets this bit 
                                              to select DMA Mode 1 (in which a 
                                              DMA request (but no interrupt) is 
                                              automatically generated for 
                                              packets of size TxMaxP bytes) and 
                                              clears this bit to select DMA Mode 
                                              0 (in which a DMA request and an 
                                              interrupt are generated for all 
                                              packets). */
        m_reg_t frc_data_tog:1;         /* D3 The CPU sets this bit to force the 
                                              endpoint data toggle to switch and 
                                              the data packet to be cleared from 
                                              the FIFO, regardless of whether an 
                                              ACK was received. This can be used 
                                              by Interrupt endpoints that are 
                                              used to communicate rate feedback 
                                              for Isochronous endpoints. */
        m_reg_t dma_enable:1;           /* D4 The CPU sets this bit to enable 
                                              the DMA request for the Tx 
                                              endpoint. */
        m_reg_t mode:1;                 /* D5 The CPU sets this bit to enable 
                                              the endpoint direction as Tx, and 
                                              clears it to enable the endpoint 
                                              direction as Rx. Note: This bit 
                                              only has any effect where the same 
                                              endpoint FIFO is used for both Tx 
                                              and Rx transactions. */
        m_reg_t ISO:1;                  /* D6 The CPU sets this bit to enable 
                                              the Tx endpoint for Isochronous 
                                              transfers, and clears it to enable 
                                              the Tx endpoint for Bulk or 
                                              Interrupt transfers. Note: This 
                                              bit only has any effect in 
                                              Peripheral mode. In Host mode, it 
                                              always returns zero. */
        m_reg_t auto_set:1;             /* D7 If the CPU sets this bit, 
                                              tx_pkt_rdy will be automatically 
                                              set when data of the maximum 
                                              packet size (value in tx_max_p) is 
                                              loaded into the Tx FIFO. If a 
                                              packet of less than the maximum 
                                              packet size is loaded, then 
                                              tx_pkt_rdy will have to be set 
                                              manually. */
    } tx_csr2;

} mihd12t;/* musb_instance offset 0x12 */

typedef struct
{
    m_reg_t rx_pkt_rdy:1;               /* D0 This bit is set when a data packet 
                                              has been received. The CPU should 
                                              clear this bit when the packet has 
                                              been unloaded from the Rx FIFO. An 
                                              interrupt is generated when the 
                                              bit is set. */
    m_reg_t fifo_full:1;                /* D1 This bit is set when no more 
                                              packets can be loaded into the Rx 
                                              FIFO. */
    m_reg_t over_run:1;                 /* D2 This bit is set if an OUT packet 
                                              cannot be loaded into the Rx FIFO. 
                                              The CPU should clear this bit.
                                              Note: This bit is only valid when 
                                              the endpoint is operating in ISO 
                                              mode. In Bulk mode, it always 
                                              returns zero. */
    m_reg_t data_error:1;               /* D3 This bit is set when rx_pkt_rdy is 
                                              set if the data packet has a CRC 
                                              or bit-stuff error. It is cleared
                                              when rx_pkt_rdy is cleared. Note: 
                                              This bit is only valid when the 
                                              endpoint is operating in ISO mode. 
                                              In Bulk mode, it always returns 
                                              zero. */
    m_reg_t flush_fifo:1;               /* D4 The CPU writes a 1 to this bit to 
                                              flush the next packet to be read 
                                              from the endpoint Rx FIFO. The 
                                              FIFO pointer is reset and the 
                                              rx_pkt_rdy bit (below) is cleared. 
                                              Note: flush_fifo has no effect 
                                              unless rx_pkt_rdy is set. 
                                              Also note that, if the FIFO is 
                                              double-buffered, flush_fifo may 
                                              need to be set twice to completely 
                                              clear the FIFO. */
    m_reg_t send_stall:1;               /* D5 The CPU writes a 1 to this bit to 
                                              issue a STALL handshake. The CPU 
                                              clears this bit to terminate the 
                                              stall condition. Note: This bit 
                                              has no effect where the endpoint 
                                              is being used for Isochronous 
                                              transfers. */
    m_reg_t sent_stall:1;               /* D6 This bit is set when a STALL 
                                              handshake is transmitted. The CPU 
                                              should clear this bit. */
    m_reg_t clr_data_tog:1;             /* D7 The CPU writes a 1 to this bit to 
                                              reset the endpoint data toggle to 
                                              0. */

} mi_d14t;/* musb_instance offset 0x14 */

typedef struct
{
    m_reg_t reserved:4;                 /* D0-D4 Unused, always return 0. */
    m_reg_t dma_mode:1;                 /* D4 Two modes of DMA operation are 
                                              supported: DMA Mode 0 in which a 
                                              DMA request is generated for all 
                                              received packets, together with an 
                                              interrupt (if enabled); and DMA 
                                              Mode 1 in which a DMA request (but 
                                              no interrupt) is generated for Rx 
                                              packets of size RxMaxP bytes and 
                                              an interrupt (but no DMA request) 
                                              is generated for Rx packets of any 
                                              other size. The CPU sets this bit 
                                              to select DMA Mode 1 and clears 
                                              this bit to select DMA Mode 0. */
    m_reg_t dma_enable:1;               /* D5 The CPU sets this bit to enable 
                                              the DMA request for the Rx 
                                              endpoint. */
    m_reg_t ISO_auto_req:1;             /* D6 In Peripheral mode, the CPU sets 
                                              this bit to enable the Rx endpoint 
                                              for Isochronous transfers, and 
                                              clears it to enable the Rx 
                                              endpoint for Bulk or Interrupt 
                                              transfers. If the CPU sets this 
                                              bit in Host mode, the ReqPkt bit 
                                              will be automatically set when the 
                                              rx_pkt_rdy bit is cleared. */
    m_reg_t auto_clear:1;               /* D7 If the CPU sets this bit then the 
                                              rx_pkt_rdy bit will be 
                                              automatically cleared when a 
                                              packet of rx_max_p bytes has been 
                                              unloaded from the Rx FIFO. When 
                                              packets of less than the maximum
                                              packet size are unloaded, 
                                              rx_pkt_rdy will have to be cleared 
                                              manually. */

} mihd15t;/* musb_instance offset 0x15 */

typedef union
{
    struct
    {
        m_reg_t rx_pkt_rdy:1;           /* D0 This bit is set when a data packet 
                                              has been received. An interrupt is 
                                              generated when this bit is set. 
                                              The CPU should clear this bit when 
                                              the packet has been read from the 
                                              FIFO. */
        m_reg_t tx_pkt_rdy:1;           /* D1 The CPU sets this bit after 
                                              loading a data packet into the 
                                              FIFO. It is cleared automatically
                                              when the data packet has been 
                                              transmitted. An interrupt is 
                                              generated when the bit is 
                                              cleared. */
        m_reg_t rx_stall:1;             /* D2 This bit is set when a STALL 
                                              handshake is received. The CPU 
                                              should clear this bit. */
        m_reg_t setup_pkt:1;            /* D3 The CPU sets this bit, at the same 
                                              time as the TxPktRdy bit is set, 
                                              to send a SETUP token instead of 
                                              an OUT token for the 
                                              transaction. */
        m_reg_t Error:1;                /* D4 This bit will be set when three 
                                              attempts have been made to perform 
                                              a transaction with no response 
                                              from the peripheral. The CPU 
                                              should clear this bit. An 
                                              interrupt is generated when this 
                                              bit is set. */
        m_reg_t req_pkt:1;              /* D5 The CPU sets this bit to request 
                                              an IN transaction. It is cleared 
                                              when rx_pkt_rdy is set. */
        m_reg_t status_pkt:1;           /* D6 The CPU sets this bit at the same 
                                              time as the tx_pkt_rdy or req_pkt 
                                              bit is set, to perform a status 
                                              stage transaction. Setting this 
                                              bit ensures that the data toggle 
                                              is set to 1 so that a DATA1 packet 
                                              is used for the Status Stage 
                                              transaction. */
        m_reg_t nak_timeout:1;          /* D7 This bit will be set when Endpoint 
                                              0 is halted following the receipt 
                                              of NAK responses for longer than 
                                              the time set by the NAKLimit0 
                                              register. The CPU should clear 
                                              this bit to allow the endpoint to 
                                              continue. */
    } csr0;
    struct
    {
        m_reg_t tx_pkt_rdy:1;           /* D0 The CPU sets this bit after 
                                              loading a data packet into the 
                                              FIFO. It is cleared automatically 
                                              when a data packet has been 
                                              transmitted. An interrupt is 
                                              generated (if enabled) when the 
                                              bit is cleared. */
        m_reg_t fifo_not_empty:1;       /* D1 The USB sets this bit when there 
                                              is at least 1 packet in the Tx 
                                              FIFO. */
        m_reg_t error:1;                /* D2 The USB sets this bit when 3 
                                              attempts have been made to send a 
                                              packet and no handshake packet has 
                                              been received. The CPU should 
                                              clear this bit. An interrupt is 
                                              generated when the bit is set. 
                                              Valid only when the endpoint is 
                                              operating in Bulk or Interrupt 
                                              mode. */
        m_reg_t flush_fifo:1;           /* D3 The CPU writes a 1 to this bit to 
                                              flush the next packet to be 
                                              transmitted from the endpoint Tx
                                              FIFO. The FIFO pointer is reset 
                                              and the tx_pkt_rdy bit (below) is 
                                              cleared. Note: flush_fifo has no
                                              effect unless tx_pkt_rdy is set. 
                                              Also note that, if the FIFO is 
                                              double-buffered, flush_fifo may 
                                              need to be set twice to completely 
                                              clear the FIFO. */
        m_reg_t reserved:1;             /* D4 Unused. Returns zero when read. */
        m_reg_t rx_stall:1;             /* D5 This bit is set when a STALL 
                                              handshake is received. The FIFO is 
                                              flushed and the TxPktRdy bit is
                                              cleared (see below). The CPU 
                                              should clear this bit. */
        m_reg_t clr_data_tog:1;         /* D6 The CPU writes a 1 to this bit to 
                                              reset the endpoint data toggle to 
                                              0. */
        m_reg_t nak_timeout:1;          /* D7 This bit will be set when the Tx 
                                              endpoint is halted following the 
                                              receipt of NAK responses for 
                                              longer than the time set as the 
                                              NAK Limit by the TxInterval 
                                              register. The CPU should clear 
                                              this bit to allow the endpoint to 
                                              continue. Note: Valid only for 
                                              Bulk endpoints. */
    } tx_csr1;

} mih_11t;/* musb_instance offset 0x11 */

typedef struct
{
    m_reg_t rx_pkt_rdy:1;               /* D0 This bit is set when a data packet 
                                              has been received. The CPU should 
                                              clear this bit when the packet has 
                                              been unloaded from the Rx FIFO. An 
                                              interrupt is generated when the 
                                              bit is set. */
    m_reg_t fifo_full:1;                /* D1 This bit is set when no more 
                                              packets can be loaded into the Rx 
                                              FIFO. */
    m_reg_t error:1;                    /* D2 The USB sets this bit when 3 
                                              attempts have been made to receive 
                                              a packet and no data packet has 
                                              been received. The CPU should 
                                              clear this bit. An interrupt is 
                                              generated when the bit is set. 
                                              Note: This bit is only valid when 
                                              the IN endpoint is operating in 
                                              Bulk or Interrupt mode. In ISO 
                                              mode, it always returns zero. */
    m_reg_t data_error_nak_timeout:1;   /* D3 When operating in ISO mode, this 
                                              bit is set when RxPktRdy is set if 
                                              the data packet has a CRC or 
                                              bit-stuff error and cleared when 
                                              rx_pkt_rdy is cleared. In Bulk 
                                              mode, this bit will be set when 
                                              the Rx endpoint is halted 
                                              following the receipt of NAK 
                                              responses for longer than the time 
                                              set as the NAK Limit by the 
                                              rx_interval register. The CPU 
                                              should clear this bit to allow the 
                                              endpoint to continue. */
    m_reg_t flush_fifo:1;               /* D4 The CPU writes a 1 to this bit to 
                                              flush the next packet to be read 
                                              from the endpoint Rx FIFO. The 
                                              FIFO pointer is reset and the 
                                              rx_pkt_rdy bit (below) is cleared. 
                                              Note: flush_fifo has no effect 
                                              unless rx_pkt_rdy is set. 
                                              Also note that, if the FIFO is 
                                              double-buffered, flush_fifo may 
                                              need to be set twice to completely 
                                              clear the FIFO. */
    m_reg_t req_pkt:1;                  /* D5 The CPU writes a 1 to this bit to 
                                              request an IN transaction. It is 
                                              cleared when rx_pkt_rdy is set. 
                                              For double packet buffering the 
                                              req_pkt is cleared immediately 
                                              after being set if there is no 
                                              request pending. If there is a 
                                              request pending then the req_pkt 
                                              is cleared when the rx_pkt_rdy for 
                                              the pending request is cleared. 
                                              Writing a 0 to the req_pkt will 
                                              clear all outstanding requests. */
    m_reg_t rx_stall:1;                 /* D6 When a STALL handshake is 
                                              received, this bit is set and an 
                                              interrupt is generated. The CPU 
                                              should clear this bit. */
    m_reg_t clr_data_tog:1;             /* D7 The CPU writes a 1 to this bit to 
                                              reset the endpoint data toggle to 
                                              0. */

} mih_14t;/* musb_instance offset 0x14 */

typedef struct
{
    m_reg_t target_endpoint_number:4;   /* D0-D3 The CPU should set this value 
                                                 to the endpoint number 
                                                 contained in the OUT/IN 
                                                 endpoint descriptor returned to 
                                                 the MUSBFDRC during device 
                                                 enumeration. */
    m_reg_t protocol:2;                 /* D4-D5 The CPU should set this to 
                                                 select the required protocol 
                                                 for the Tx/Rx endpoint:
                                                    00 : Illegal
                                                    01: Isochronous
                                                    10: Bulk
                                                    11: Interrupt */
    m_reg_t reserved:2;                 
} mih_18t;/* musb_instance offset 0x18 */
typedef mih_18t mih_1At;/* musb_instance offset 0x1A */

typedef struct
{
    m_reg_t tx_fifo_size:4;             /* D0-D3 */
    m_reg_t rx_fifo_size:4;             /* D4-D5 */
} mihd1Ft;/* musb_instance offset 0x1F */

struct musb_instance
{
    m_reg_t faddr;                      /* 00 Function address register. */
    mihd01t power;                      /* 01 Power management register. */
    m_reg_t intr_tx_1;                  /* 02 Interrupt register for Endpoint 0 
                                              plus Tx Endpoints 1 to 7. */
    m_reg_t intr_tx_2;                  /* 03 Interrupt register for Tx 
                                              Endpoints 8 to 15. */
    m_reg_t intr_rx_1;                  /* 04 Interrupt register for Rx 
                                              Endpoints 1 to 7. */
    m_reg_t intr_rx_2;                  /* 05 Interrupt register for Rx 
                                              Endpoints 8 to 15. */
    mihd06t intr_usb;                   /* 06 Interrupt register for common USB 
                                              interrupts. */
    m_reg_t intr_tx_1_e;                /* 07 Interrupt enable register for 
                                              intr_tx_1. */
    m_reg_t intr_tx_2_e;                /* 08 Interrupt enable register for 
                                              intr_tx_2. */
    m_reg_t intr_rx_1_e;                /* 09 Interrupt enable register for 
                                              intr_rx_1. */
    m_reg_t intr_rx_2_e;                /* 0A Interrupt enable register for 
                                              intr_rx_2. */
    mihd0Bt intr_usb_e;                 /* 0B Interrupt enable register for 
                                              intr_usb. */
    m_reg_t frame_1;                    /* 0C Frame number bits 0 to 7. */
    m_reg_t frame_2;                    /* 0D Frame number bits 8 to 10. */
    m_reg_t index;                      /* 0E Index register for selecting the 
                                              endpoint status and control 
                                              registers. */
    mihd0Ft dev_ctl;                    /* 0F USB device control register. */
    union {
        struct
        {
            m_reg_t tx_max_p;           /* 10 Maximum packet size for peripheral
                                              IN endpoint. (Index register set 
                                              to select Endpoints 1 – 15 
                                              only) units of 8 bytes */
            mi_d11t csr0_tx_csr1;       /* 11 Control Status register for 
                                              Endpoint 0. (Index register set 
                                              to select 11 Endpoint 0)
                                              or
                                              Control Status register 1 for 
                                              peripheral IN endpoint. (Index 
                                              register set to select
                                              Endpoints 1 – 15) */
            mihd12t csr02_tx_csr2;      /* 12 Subsidiary Control Status register
                                              for Endpoint 0, containing 
                                              FlushFIFO bit. (Index register set
                                              to select Endpoint 0)
                                              or
                                              Control Status register 2 for 
                                              peripheral IN endpoint. (Index 
                                              register set to select Endpoints 
                                              1 – 15 only) */
            m_reg_t rx_max_p;           /* 13 Maximum packet size for peripheral
                                              OUT endpoint. (Index register set 
                                              to select Endpoints 1 – 15 
                                              only) units of 8 bytes */
            mi_d14t rx_csr_1;           /* 14 Control Status register 1 for 
                                              peripheral OUT endpoint. (Index 
                                              register set to select Endpoints 
                                              1 – 15 only) */
            mihd15t rx_csr_2;           /* 15 Control Status register 2 for 
                                              peripheral OUT endpoint. (Index 
                                              register set to select Endpoints 
                                              1 – 15 only) */
            m_reg_t count0_rx_count1;   /* 16 Number of received bytes in 
                                              Endpoint 0 FIFO. (Index register 
                                              set to select Endpoint 0) */
            m_reg_t rx_count2;          /* 17 Number of bytes in peripheral OUT 
                                              endpoint FIFO (upper byte). (Index
                                              register set to select Endpoints 
                                              1 – 15 only) 
                                              or
                                              Number of bytes in peripheral OUT 
                                              endpoint FIFO (lower byte). (Index 
                                              register set to select Endpoints 
                                              1 – 15)*/
            m_reg_t reserved1[4];       /* 18-1B Reserved. Value returned 
                                              affected by use in Host mode (see 
                                              overleaf). */
            m_reg_t reserved2[3];       /* 1C–1E Only used if Dynamic FIFO 
                                              sizing option is selected. 
                                              Otherwise always return zero. */
        } peripheral;
        struct
        {
            m_reg_t tx_max_p;           /* 10 Maximum packet size for host OUT 
                                              endpoint. (Index register set to 
                                              select Endpoints 1 – 15 only) */
            mih_11t csr0_tx_csr1;       /* 11 Control Status register for 
                                              Endpoint 0. (Index register set to 
                                              select 11 Endpoint 0)
                                              or
                                              Control Status register 1 for 
                                              host OUT endpoint. (Index register
                                              set to select Endpoints 1 – 15) */
            mihd12t csr02_tx_csr2;      /* 12 Subsidiary Control Status register
                                              for Endpoint 0, containing  
                                              FlushFIFO bit. (Index register set
                                              to select Endpoint 0)
                                              or
                                              Control Status register 2 for host 
                                              OUT endpoint. (Index register set 
                                              to select Endpoints 1 – 15 
                                              only) */
            m_reg_t rx_max_p;           /* 13 Maximum packet size for host IN 
                                              endpoint. (Index register set to 
                                              select Endpoints 1 – 15 only) */
            mih_14t rx_csr_1;           /* 14 Control Status register 1 for host 
                                              IN endpoint. (Index register set 
                                              to select Endpoints 1 – 15 
                                              only) */
            mihd15t rx_csr_2;           /* 15 Control Status register 2 for host 
                                              IN endpoint. (Index register set 
                                              to select Endpoints 1 – 15 
                                              only) */
            m_reg_t count0_rx_count1;   /* 16 Number of received bytes in 
                                              Endpoint 0 FIFO. (Index register 
                                              set to select Endpoint 0) 
                                              or
                                              Number of bytes in host IN 
                                              endpoint FIFO (lower byte). (Index 
                                              register set to select Endpoints 
                                              1 – 15)*/
            m_reg_t rx_count2;          /* 17 Number of bytes in host IN 
                                              endpoint FIFO (upper byte). (Index 
                                              register set to select Endpoints 
                                              1 – 15) */
            mih_18t tx_type;            /* 18 Sets the transaction protocol and 
                                              peripheral endpoint number for the 
                                              host OUT endpoint. (Index register 
                                              set to select Endpoints 1 – 15 
                                              only)*/
            m_reg_t nak_limit0_tx_interval;   
                                        /* 19 Sets the NAK response timeout on 
                                              Endpoint 0. (Index register set to 
                                              select Endpoint 0)
                                              or
                                              Sets the transaction protocol and 
                                              peripheral endpoint number for the 
                                              host IN endpoint. (Index register 
                                              set to select Endpoints 1 – 15 
                                              only) */
            mih_1At rx_type;            /* 1A Sets the transaction protocol and 
                                              peripheral endpoint number for the 
                                              host IN endpoint. (Index register 
                                              set to select Endpoints 1 – 15 
                                              only) */
            m_reg_t rx_interval;        /* 1B Sets the polling interval for an 
                                              IN Interrupt endpoint, in ms. 
                                              (Index register set to select 
                                              Endpoints 1 – 15 only) */
            m_reg_t reserved[3];       /* 1C–1E Only used if Dynamic FIFO 
                                              sizing option is selected. 
                                              Otherwise always return zero. */
        } host;
    } mode;
    /* FIFOSize is an 8-bit Read-Only register that returns the size of the FIFO 
       associated with the selected endpoint (numbers 1 – 15) where set as part 
       of the core configuration. Note: The register value is not valid for 
       Endpoint 0 (always 64 bytes) or where Dynamic FIFO Sizing is used.The 
       lower nibble encodes the size of the selected Tx endpoint FIFO; the upper 
       nibble encodes the size of the selected Rx endpoint FIFO. Values of 
       3 – 11 correspond to a FIFO size of 2n bytes (8 – 2048 bytes). If an 
       endpoint has not been configured, a value of 0 will be displayed. Where 
       the Tx and Rx endpoints share the same FIFO, the Rx FIFO size will be 
       encoded as 0xF. */
    mihd1Ft fifo_size;                  /* 1F Returns the configured size of the 
                                              selected Rx FIFO and Tx FIFOs. 
                                              (Index register set to select 
                                              Endpoints 1 – 15 only) Note: Not 
                                              valid when Dynamic FIFO sizing is 
                                              selected (when register has 
                                              different interpretation.) */
    /* These 16 addresses provide CPU access to the FIFOs for each endpoint. 
       Writing to these addresses loads data into the Tx FIFO for the 
       corresponding endpoint. Reading from these addresses unloads data from 
       the Rx FIFO for the corresponding endpoint. The FIFOs are located on byte 
       boundaries (Endpoint 0 at 20h, Endpoint 1 at 21h ... Endpoint 15 at 2Fh). 
       Note: (i) Using the MUSBFDRC – AHB bridge requires a different address 
       mapping to be used for these FIFOs. Details are given in the separate 
       specification for that bridge included in the musbfdrc/docs directory.
       (ii) Depending on the size of the FIFO and the expected maximum packet 
       size, the FIFOs support either single-packet or doublepacket buffering. 
       However, burst writing of multiple packets is not supported as flags need 
       to be set after each packet is written. */
    
#ifdef MUSB_FDRC_AHB
    m_reg32_t fifo[16];                 /* 20-2F FIFOs for Endpoints 0 – 15. */
#else
    m_reg_t fifo[16];                   /* 20-2F FIFOs for Endpoints 0 – 15. */
#endif
};
typedef struct musb_instance * musb_instance_t;

#define get_musb_instance(addr) ((musb_instance_t)(addr))

#endif
