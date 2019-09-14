


#include "CCE4510/cce4510.h"

#define cce4510_r(name, desc, offset, init, mask) r.create_register(name, desc,   \
                                                    offset,                    \
                                                      gs::reg::STANDARD_REG    \
                                                    | gs::reg::SINGLE_IO       \
                                                    | gs::reg::SINGLE_BUFFER   \
                                                    | gs::reg::FULL_WIDTH,     \
                                                    init,                      \
                                                    mask, 32)

#define MODE   ((0x20-0x20)*4)
#define OVLD   ((0x21-0x20)*4)
#define SHRT   ((0x22-0x20)*4)
#define SIO    ((0x23-0x20)*4)
#define UART   ((0x24-0x20)*4)
#define FHC    ((0x25-0x20)*4)
#define OD     ((0x26-0x20)*4)
#define MPD    ((0x27-0x20)*4)
#define DPD    ((0x28-0x20)*4)
#define CYCT   ((0x29-0x20)*4)
#define FHD    ((0x2A-0x20)*4)
#define BLVL   ((0x2B-0x20)*4)
#define IMSK   ((0x2C-0x20)*4)
#define LSEQ   ((0x2D-0x20)*4)
#define LHLD   ((0x2E-0x20)*4)
#define CFG    ((0x2F-0x20)*4)
#define TRSH   ((0x30-0x20)*4)
#define STAT   ((0x60-0x20)*4)
#define SMSK   ((0x61-0x20)*4)
#define SYNC   ((0x62-0x20)*4)
#define PROT   ((0x63-0x20)*4)
#define INT    ((0x64-0x20)*4)
#define REV    ((0x70-0x20)*4)


CCE4510::CCE4510(sc_module_name nm):
  gr_device(nm, gs::reg::INDEXED_ADDRESS, 0x1000, NULL),
  target_port("target_port", r),
  serial_sock_iolink("serial_socket_iolink"),
  rx_fifo_(48),
  tx_fifo_(48),
  status_nibble_(0)
{
  serial_sock_iolink.register_b_transport(this, &CCE4510::serial_b_transport_iolink);

  SC_THREAD(thread_wakeup);
  SC_THREAD(thread_tx);

  cce4510_r("MODE" ,  "MODE register", MODE, 0xC0, 0xFF);
  cce4510_r("SIO"  ,  "SIO  register", SIO , 0x00, 0xFF);
  cce4510_r("FHC" ,   "FHC  register", FHC, 0x00, 0xFF);
  cce4510_r("OD"   ,  "OD   register", OD  , 0x00, 0xFF);
  cce4510_r("MPD"  ,  "MPD  register", MPD , 0x00, 0xFF);
  cce4510_r("DPD"  ,  "DPD  register", DPD , 0x00, 0xFF);
  cce4510_r("FHD"  ,  "FHD  register", FHD , 0x00, 0xFF);
  cce4510_r("BLVL" ,  "BLVL register", BLVL, 0x00, 0xFF);
  cce4510_r("STAT" ,  "STAT register", STAT, 0x00, 0xFF);

  //  SC_METHOD(txTrigger);
//  sensitive << tx_event;
//  dont_initialize();
}

CCE4510::~CCE4510()
{
}

void CCE4510::end_of_elaboration()
{
  GR_FUNCTION_PARAMS(CCE4510, WriteData);
  GR_SENSITIVE(r[FHD].add_rule(gs::reg::POST_WRITE, "WriteData",
                                gs::reg::NOTIFY));

  GR_FUNCTION_PARAMS(CCE4510, ReadData);
  GR_SENSITIVE(r[FHD].add_rule(gs::reg::PRE_READ, "ReadData",
                                gs::reg::NOTIFY));

  GR_FUNCTION_PARAMS(CCE4510, WriteSio);
  GR_SENSITIVE(r[SIO].add_rule(gs::reg::POST_WRITE, "WriteSio",
                                gs::reg::NOTIFY));

  GR_FUNCTION_PARAMS(CCE4510, ReadLevel);
  GR_SENSITIVE(r[BLVL].add_rule(gs::reg::PRE_READ, "ReadLevel",
                                gs::reg::NOTIFY));
}

void CCE4510::WriteData(gs::reg::transaction_type *&tr, const sc_core::sc_time &delay)
{
  uint8_t d = r[FHD];
  tx_fifo_.write(d);
}

void CCE4510::ReadData(gs::reg::transaction_type *&tr, const sc_core::sc_time &delay)
{
  uint8_t d = 0;
  rx_fifo_.nb_read(d);
  r[FHD] = d;
}

void CCE4510::ReadLevel(gs::reg::transaction_type *&tr, const sc_core::sc_time &delay)
{
  r[BLVL] = rx_fifo_.num_available();
}

void CCE4510::WriteSio(gs::reg::transaction_type *&tr, const sc_core::sc_time &delay)
{
  if (r[SIO] & 0x80) {
    setCom(3);
    tx_fifo_.write(0xA2);
    tx_fifo_.write(0x00);
//          event_wakeup.notify();
  }
}


void CCE4510::serial_b_transport_iolink(tlm_serial::tlm_serial_payload& gp,
                                  sc_core::sc_time& time)
{
  uint16_t d = *(uint16_t *)gp.get_data_ptr();

//  uint8_t wakeup = (d >> 15) & 0x01;
  uint8_t com = (d >> 8) & 0x03;

//  if (wakeup)
//  {
//    setMode(Mode::FrameHandler);
//    if (isSlave())
//    {
//      uint16_t da = 0x8000;
//      sc_core::sc_time t(0, sc_core::SC_NS);
//      tlm_serial::tlm_serial_payload txn;
//      txn.set_command(tlm_serial::SERIAL_SEND_CHAR_COMMAND);
//      txn.set_data_ptr((uint8_t *)&da);
//      txn.set_data_length(2);
//      txn.set_response_status(tlm_serial::SERIAL_OK_RESPONSE);
//      txn.set_num_stop_bits_in_end(1);
//      txn.set_enable_bits(tlm_serial::SERIAL_STOP_BITS_ENABLED);
//      serial_sock_iolink->b_transport(txn, t);
//    }
//  }
//  else
  {
    if (com == getCom())
    {
      rx_fifo_.write((uint8_t) d);
    }
  }
}


CCE4510::Mode CCE4510::getMode() {
  return static_cast<Mode>(r[MODE] & 0x3);
}

void CCE4510::setMode(Mode m) {
  uint8_t mode = r[MODE];
  mode &= 0xFC;
  mode |= static_cast<uint8_t>(m);
  r[MODE] = mode;
}

bool CCE4510::isMaster()
{
  return (r[FHC] & (1u << 2));
}

bool CCE4510::isSlave()
{
  return ! isMaster();
}

void CCE4510::setCom(uint8_t v)
{
  uint8_t mode = r[MODE];
  mode &= 0xF3;
  mode |= (v << 2) & 0x0C;
  r[MODE] = mode;
}

uint8_t CCE4510::getCom()
{
  return (r[MODE] >> 2) & 0x03;
}

void CCE4510::thread_wakeup()
{
  sc_time tren = {500, SC_US};
  sc_time tdmt[3] = {{200, SC_US}, {800, SC_US}, {6300, SC_US}};
  uint8_t com = 0;

  for (;;)
  {
    wait(event_wakeup);

    uint16_t da = 0x8000;
    sc_core::sc_time t(0, sc_core::SC_NS);
    tlm_serial::tlm_serial_payload txn;
    txn.set_command(tlm_serial::SERIAL_SEND_CHAR_COMMAND);
    txn.set_data_ptr((uint8_t *)&da);
    txn.set_data_length(2);
    txn.set_response_status(tlm_serial::SERIAL_OK_RESPONSE);
    txn.set_num_stop_bits_in_end(1);
    txn.set_enable_bits(tlm_serial::SERIAL_STOP_BITS_ENABLED);
    serial_sock_iolink->b_transport(txn, t);

    wait(tren);

    if (getMode() == Mode::FrameHandler)
    {
      // wakeup pulse successfull

      com = 3;
      for (int i = 0; i < 3; i++)
      {
        setCom(com);
        tx_fifo_.write(0xA2);
        tx_fifo_.write(0x00);

        wait(tdmt[i], rx_fifo_.data_written_event());

        if (rx_fifo_.num_available() > 0)
        {
          break;
        }
        com--;
      }

      setCom(com);
    }

    uint8_t sio = r[SIO];
    sio &= ~(0x80);
    r[SIO] = sio;
  }
}


void CCE4510::thread_tx()
{
  for (;;)
  {
//    out_uart = "zzzzzzzz";
    uint8_t d = tx_fifo_.read();
    printf("%10.6f %08X %02X\n", sc_time_stamp().to_seconds(), (uint32_t)(size_t)this, d);

//    out_uart = d;
    uint8_t com = (r[MODE] >> 2) & 0x03;
    sc_time dt(0, SC_SEC);
    switch (com) {
      case 1 : dt = sc_time(1.0 / 4.8e3, SC_SEC); break;
      case 2 : dt = sc_time(1.0 / 38.4e3, SC_SEC); break;
      case 3 : dt = sc_time(1.0 / 230.4e3, SC_SEC); break;
    }
//    wait(10 * dt);
//    out_uart = "zzzzzzzz";
//    wait(1 * dt);
    wait(11 *dt);
    if (com > 0) {
      uint16_t da = (((uint16_t)com << 8) & 0x0300) | (((uint16_t) d) & 0x00ff);
      sc_core::sc_time t(0, sc_core::SC_NS);
      tlm_serial::tlm_serial_payload txn;
      txn.set_command(tlm_serial::SERIAL_SEND_CHAR_COMMAND);
      txn.set_data_ptr((uint8_t *)&da);
      txn.set_data_length(2);
      txn.set_response_status(tlm_serial::SERIAL_OK_RESPONSE);
      txn.set_num_stop_bits_in_end(1);
      txn.set_enable_bits(tlm_serial::SERIAL_STOP_BITS_ENABLED);
      serial_sock_iolink->b_transport(txn, t);
    }
  }
}
