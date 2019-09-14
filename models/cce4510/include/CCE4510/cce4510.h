
#ifndef CCE4510_H
#define CCE4510_H

#include <map>

#include <systemc.h>

#include <greenreg/greenreg.h>
#include <greenreg/greenreg_socket.h>
#include "greensignalsocket/green_signal.h"

#include "greenserialsocket/tlm_serial_bidirectional_socket.h"

class CCE4510:
  public gs::reg::gr_device
{
  public:
  SC_HAS_PROCESS(CCE4510);
  CCE4510(sc_core::sc_module_name name);
  ~CCE4510();

  typedef gs_generic_signal::gs_generic_signal_payload irqPayload;
  gs::reg::greenreg_socket<gs::gp::generic_slave> target_port;

  //  sc_out<sc_lv<8> > out_uart;

  /*
   * uart serial socket
   */
//  tlm_serial::tlm_serial_bidirectional_socket<> serial_sock_uart;
//  void serial_b_transport_uart(tlm_serial::tlm_serial_payload& gp,
//                          sc_core::sc_time& time);

  /*
   * iolink transmission line
   */
  tlm_serial::tlm_serial_bidirectional_socket<> serial_sock_iolink;
  void serial_b_transport_iolink(tlm_serial::tlm_serial_payload& gp,
                          sc_core::sc_time& time);

  void WriteData(gs::reg::transaction_type *&tr, const sc_core::sc_time &delay);
  void ReadData(gs::reg::transaction_type *&tr, const sc_core::sc_time &delay);
  void ReadLevel(gs::reg::transaction_type *&tr, const sc_core::sc_time &delay);
  void WriteSio(gs::reg::transaction_type *&tr, const sc_core::sc_time &delay);

private:
  sc_fifo<uint8_t> rx_fifo_;
  sc_fifo<uint8_t> tx_fifo_;
  uint8_t status_nibble_;
  sc_event event_wakeup;

  void end_of_elaboration();

  enum class Reg {
    MODE1 = 0x20, // Channel 1 – Mode                R/W
    OVLD1 = 0x21, // Channel 1 – Overload Protection R/W
    SHRT1 = 0x22, // Channel 1 – Short Protection    R/W
    SIO1  = 0x23, // Channel 1 – SIO Control         R/W
    UART1 = 0x24, // Channel 1 – UART Data           R/W
    FHC1  = 0x25, // Channel 1 – FH Control          R/W
    OD1   = 0x26, // Channel 1 – On-Request Length   R/W
    MPD1  = 0x27, // Channel 1 – Master PD Length    R/W
    DPD1  = 0x28, // Channel 1 – Device PD Length    R/W
    CYCT1 = 0x29, // Channel 1 – Cycle Time          R/W
    FHD1  = 0x2A, // Channel 1 – FH Data             R/W
    BLVL1 = 0x2B, // Channel 1 – FH Buffer Level     R
    IMSK1 = 0x2C, // Channel 1 – Interrupt Masking   R/W
    LSEQ1 = 0x2D, // Channel 1 – LED Sequence        R/W
    LHLD1 = 0x2E, // Channel 1 – LED Hold Times      R/W
    CFG1  = 0x2F, // Channel 1 – Configuration       R/W
    TRSH1 = 0x30, // Channel 1 – Threshold Level     R/W
    MODE2 = 0x40, // Channel 2 – Mode                R/W
    OVLD2 = 0x41, // Channel 2 – Overload Protection R/W
    SHRT2 = 0x42, // Channel 2 – Short Protection    R/W
    SIO2  = 0x43, // Channel 2 – SIO Control         R/W
    UART2 = 0x44, // Channel 2 – UART Data           R/W
    FHC2  = 0x45, // Channel 2 – FH Control          R/W
    OD2   = 0x46, // Channel 2 – On-Request Length   R/W
    MPDL2 = 0x47, // Channel 2 – Master PD Length    R/W
    DPDL2 = 0x48, // Channel 2 – Device PD Length    R/W
    CYCT2 = 0x49, // Channel 2 – Cycle Time          R/W
    FHD2  = 0x4A, // Channel 2 – FH Data             R/W
    BLVL2 = 0x4B, // Channel 2 – FH Buffer Level     R
    IMSK2 = 0x4C, // Channel 2 – Interrupt Masking   R/W
    LSEQ2 = 0x4D, // Channel 2 – LED Sequence        R/W
    LHLD2 = 0x4E, // Channel 2 – LED Hold Times      R/W
    CFG2  = 0x4F, // Channel 2 – Configuration       R/W
    TRSH2 = 0x50, // Channel 2 – Threshold Level     R/W
    STAT  = 0x60, // IC Status                       R
    SMSK  = 0x61, // Channel Synchronization Masks   R/W
    SYNC  = 0x62, // Synchronization Triggers        W
    PROT  = 0x63, // Channel Protection              R/W
    INT   = 0x64, // Interrupt Register              R/W
    REV   = 0x70, // Revision Code                   R
  };

  enum class Mode
  {
    StandardIO = 0,
    UART = 1,
    FrameHandler = 2,
    reserved = 3,
  };

  Mode getMode();
  void setMode(Mode m);
  bool isMaster();
  bool isSlave();
  void setCom(uint8_t v);
  uint8_t getCom();
  void thread_wakeup();
  void thread_tx();
  void thread_rx();
};


#endif /* !CCE4510_H */
