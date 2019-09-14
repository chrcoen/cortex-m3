/*
 * toplevel.cpp
 *
 * Copyright (C) 2015, GreenSocs Ltd.
 *
 * Developed by Guillaume Delbergue <guillaume.delbergue@greensocs.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * Linking GreenSocs code, statically or dynamically with other modules
 * is making a combined work based on GreenSocs code. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders, GreenSocs
 * Ltd, give you permission to combine GreenSocs code with free software
 * programs or libraries that are released under the GNU LGPL, under the
 * OSCI license, under the OCP TLM Kit Research License Agreement or
 * under the OVP evaluation license.You may copy and distribute such a
 * system following the terms of the GNU GPL and the licenses of the
 * other code concerned.
 *
 * Note that people who make modified versions of GreenSocs code are not
 * obligated to grant this special exception for their modified versions;
 * it is their choice whether to do so. The GNU General Public License
 * gives permission to release a modified version without this exception;
 * this exception also makes it possible to release a modified version
 * which carries forward this exception.
 *
 */

#include <string>

#include "SimpleCPU/arm/armCortexM3CPU.h"
#include "SimpleMemory/simpleMemory.h"

#include "APB_UART/apb_uart.h"
#include "TCPSerial/TCPSerial.h"
#include "PL022/PL022.h"
#include "APBTimer/APBTimer.h"
#include "APBDualTimer/APBDualTimer.h"
#include "DevNull/DevNull.h"
#include "AHBGPIO/AHBGPIO.h"
#include "APBWatchdog/APBWatchdog.h"
#include "systemtest/systemtest.h"
#include "CCE4510/cce4510.h"
#include "greenrouter/genericRouter.h"
#include "greenrouter/protocol/SimpleBus/simpleBusProtocol.h"
#include "greenrouter/scheduler/fixedPriorityScheduler.h"
#include "greencontrol/config_api_lua_file_parser.h"
#include "greenthreads/inlinesync.h"

#if (defined(_WIN32) || defined(_WIN64))
    #include <windows.h>
    #define sleep(time) Sleep(time * 1000)
    #define usleep(time) Sleep(time / 1000)
#else
    #include <unistd.h>
#endif


static void showUsage(std::string name)
{
    std::cerr << "Usage: " << name << " <time> <option(s)>\n"
              << "Time: Simulation time in ms, 0 means forever\n"
              << "Options:\n"
              << "\t-h,--help\t\t\tShow this help message\n"
              << "\t-c,--conf CONFIGURATION_FILE\t"
              << "Specify the configuration file path"
              << std::endl;
}

std::string parseArgs(int argc, char **argv)
{
    std::string configurationFilePath;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help"))
        {
            showUsage(argv[0]);
            exit(EXIT_SUCCESS);
        }
        else
        {
            if ((arg == "-c") || (arg == "--conf"))
            {
                if (i + 1 < argc)
                {
                    configurationFilePath = argv[++i];
                }
                else
                {
                    std::cerr << "--conf option requires one argument."
                              << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    return configurationFilePath;
}

struct IolModule
{
  ArmCortexM3CPU<32> *cpu;
  Memory<32> *rom;
  Memory<32> *ram;
  APBTimer *timer;
  CCE4510 *cce4510;
  gs::gp::SimpleBusProtocol<32> *protocol;
  gs::gp::fixedPriorityScheduler *scheduler;
  gs::gp::GenericRouter<32> *router;
  gs::gt::inLineSync<32> *insync;
  IolModule(const std::string &suffix)
  {
    cpu = new ArmCortexM3CPU<32>((std::string("CPU") + suffix).c_str());
    rom = new Memory<32>((std::string("rom") + suffix).c_str());
    ram = new Memory<32>((std::string("ram") + suffix).c_str());
    timer = new APBTimer((std::string("timer") + suffix).c_str());
    cce4510 = new CCE4510((std::string("cce4510") + suffix).c_str());
    protocol = new gs::gp::SimpleBusProtocol<32>((std::string("protocol") + suffix).c_str(), 10);
    scheduler = new gs::gp::fixedPriorityScheduler((std::string("scheduler") + suffix).c_str());
    router = new gs::gp::GenericRouter<32>((std::string("router") + suffix).c_str());
    insync = new gs::gt::inLineSync<32>((std::string("insync") + suffix).c_str());
  }
  void bind() {
     protocol->router_port(*router);
     protocol->scheduler_port(*scheduler);
     router->protocol_port(*protocol);

     cpu->master_socket(insync->target_socket);
     insync->init_socket(router->target_socket);
     timer->TIMERINTSocket(cpu->irq_socket);

     router->init_socket(rom->target_port);
     router->init_socket(ram->target_port);
     router->init_socket(timer->targetPort);
     router->init_socket(cce4510->target_port);
  }
  ~IolModule()
  {
    delete cpu;
    delete rom;
    delete ram;
    delete timer;
    delete cce4510;
    delete protocol;
    delete scheduler;
    delete router;
    delete insync;
  }
};


int sc_main(int argc, char **argv)
{
  if (argc < 2) {
    showUsage(argv[0]);
    exit(EXIT_FAILURE);
  }

  /* GSPARAMS */
  std::string configurationFilePath = parseArgs(argc, argv);
  gs::cnf::LuaFile_Tool luareader("luareader");
  bool openResult;

  if(configurationFilePath.empty()) {
      openResult = luareader.config("config.lua");
  } else {
      openResult = luareader.config(configurationFilePath.c_str());
  }

  if(openResult != 0) {
      showUsage(argv[0]);
      exit(EXIT_FAILURE);
  }

  IolModule master("_master");
  IolModule slave("_slave");

  master.bind();
  slave.bind();

  master.cce4510->serial_sock_iolink(slave.cce4510->serial_sock_iolink);

  sc_core::sc_start();

  return 0;
}




struct SimpleModule
{
  ArmCortexM3CPU<32> *cpu;
  Memory<32> *rom;
  Memory<32> *ram;
  gs::gp::SimpleBusProtocol<32> *protocol;
  gs::gp::fixedPriorityScheduler *scheduler;
  gs::gp::GenericRouter<32> *router;
  gs::gt::inLineSync<32> *insync;
  SimpleModule(const std::string &suffix)
  {
    cpu = new ArmCortexM3CPU<32>((std::string("CPU") + suffix).c_str());
    rom = new Memory<32>((std::string("rom") + suffix).c_str());
    ram = new Memory<32>((std::string("ram") + suffix).c_str());
    protocol = new gs::gp::SimpleBusProtocol<32>((std::string("protocol") + suffix).c_str(), 10);
    scheduler = new gs::gp::fixedPriorityScheduler((std::string("scheduler") + suffix).c_str());
    router = new gs::gp::GenericRouter<32>((std::string("router") + suffix).c_str());
    insync = new gs::gt::inLineSync<32>((std::string("insync") + suffix).c_str());
  }
  void bind() {
     protocol->router_port(*router);
     protocol->scheduler_port(*scheduler);
     router->protocol_port(*protocol);

     cpu->master_socket(insync->target_socket);
     insync->init_socket(router->target_socket);

     router->init_socket(rom->target_port);
     router->init_socket(ram->target_port);
  }
  ~SimpleModule()
  {
    delete cpu;
    delete rom;
    delete ram;
    delete protocol;
    delete scheduler;
    delete router;
    delete insync;
  }
};

