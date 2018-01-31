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
    std::cerr << "Usage: " << name << " <option(s)>\n"
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

int sc_main(int argc, char **argv)
{
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

    /*
     * CPU.
     */
    ArmCortexM3CPU<32> *cpu = new ArmCortexM3CPU<32>("CPU");

    /*
     * Memories.
     */
    Memory<32> *rom = new Memory<32>("rom");
    Memory<32> *ram = new Memory<32>("ram");

    /*
     * Peripherals.
     */
    APB_UART *uart;
    uart = new APB_UART("uart0");
    PL022 *spi = new PL022("spi");
    APBTimer *timer[2];
    timer[0] = new APBTimer("timer0");
    timer[1] = new APBTimer("timer1");
    APBDualTimer *dualTimer = new APBDualTimer("dualtimer");
    DevNull *devNull = new DevNull("devnull0");
    AHBGPIO *gpio = new AHBGPIO("gpio");
    APBWatchdog *watchdog = new APBWatchdog("watchdog");
    systemtest::systemtest * systest = new systemtest::systemtest("systemtest");

    /*
     * Serial TCP backends.
     */
    TCPSerial *tcp;
    tcp = new TCPSerial("tcp_serial0");

    /*
     * Memory router.
     */
    gs::gp::SimpleBusProtocol<32> *protocol =
                            new gs::gp::SimpleBusProtocol<32>("protocol", 10);
    gs::gp::fixedPriorityScheduler *scheduler =
                            new gs::gp::fixedPriorityScheduler("scheduler");
    gs::gp::GenericRouter<32> *router =
                            new gs::gp::GenericRouter<32>("router");

    protocol->router_port(*router);
    protocol->scheduler_port(*scheduler);
    router->protocol_port(*protocol);

    /*
     * Bind the CPU.
     */
    gs::gt::inLineSync<32> *insync = new gs::gt::inLineSync<32>("insync");
    cpu->master_socket(insync->target_socket);
    insync->init_socket(router->target_socket);

    /*
     * Bind the memories.
     */
    router->init_socket(rom->target_port);
    router->init_socket(ram->target_port);

    /*
     * Bind the peripherals.
     */
    uart->serial_sock(tcp->serial_sock);
    router->init_socket(uart->target_port);
    uart->irq_socket(cpu->irq_socket);
    router->init_socket(spi->target_port);
    spi->irq_socket(cpu->irq_socket);
    for (int i = 0; i < 2; i++)
    {
        router->init_socket(timer[i]->targetPort);
        timer[i]->TIMERINTSocket(cpu->irq_socket);
    }
    router->init_socket(dualTimer->targetPort);
    dualTimer->IRQTIMINT1Socket(cpu->irq_socket);
    dualTimer->IRQTIMINTCSocket(devNull->IRQSocket);
    dualTimer->IRQTIMINT2Socket(cpu->irq_socket);
    router->init_socket(gpio->target_port);
    gpio->irq_socket(cpu->irq_socket);
    router->init_socket(watchdog->targetPort);
    watchdog->WDOGINTSocket(cpu->irq_socket);
    watchdog->WDOGRESSocket(devNull->OUTSocket);
    router->init_socket(systest->target_port);

    gs::cnf::cnf_api *Api = gs::cnf::GCnf_Api::getApiInstance(NULL);
    std::cout << "Sleep 5 seconds so you can connect on TCP port "
        << Api->getValue("tcp_serial0.tcp_port") << " ..." << std::endl;
    sleep(7);

    sc_core::sc_start();

    return systest->get_sim_status();
}

