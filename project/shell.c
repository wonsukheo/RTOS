#include "commandTerminalInterface.h"
#include "uart0.h"
#include "shell.h"
#include "rtos.h"	// For RED_LED_BBADDR

void shell(void)
{
    USER_DATA data;
    putsUart0("a. available program with command is limited to \"shell\" for now\r\n");

    while (true) {
        putsUart0("\r\n> ");	// pc terminal
        getsUart0(data.buffer);
        parseFields(&data);

        if (isCommand(&data, "reboot", 0)) {
            putsUart0("Reboot controller\r\n");
        } 
        else if (isCommand(&data, "ps", 0)) {
            ps();
        }
        else if (isCommand(&data, "ipcs", 0)) {
            ipcs();
        }
        else if (isCommand(&data, "kill", 1)) {
            kill(getFieldInteger(&data, 1));
        }
        else if (isCommand(&data, "pkill", 1)) {
            pkill(getFieldString(&data, 1));
        }
        else if (isCommand(&data, "pi", 1)) {
            bool b = (myStrCmp(getFieldString(&data, 1), "on"));
            pi(b);
        }
        else if (isCommand(&data, "preempt", 1)) {
            bool b = (myStrCmp(getFieldString(&data, 1), "on"));
            preempt(b);
        }
        else if (isCommand(&data, "sched", 1)) {
            bool b = (myStrCmp(getFieldString(&data, 1), "prio"));
            sched(b);
        } 
        else if (isCommand(&data, "pidof", 1)) {
            pidof(getFieldString(&data, 1));
        } 
        else if (isBackground(&data)) {
            // Runs program in background
            // Debug with red LED
            const char* proc_name = getFieldString(&data, 0);
            // available program list = shell for now
            if (myStrCmp(proc_name, "shell")) {
                RED_LED_BBADDR = 1;
                putsUart0("Program launched in background\r\n");
            } else {
                putsUart0("Unknown Program\r\n");
            }      
        } 
        else {
            putsUart0("Undefined Command\r\n");
        }
    }
}

bool isBackground(USER_DATA* data)
{
    const char* arg = getFieldString(data, 1);
    if (arg == 0 || *arg == '\0') {
        return false;
    }

    return myStrCmp(arg, "&");
}

// Display process(thread) status
void ps(void)
{
    putsUart0("PS called\r\n");
}

// Display inter-process communication status
void ipcs(void)
{
    putsUart0("IPCS called\r\n");
}

// Kill process with matching PID 
void kill(uint32_t pid)
{
    putUintUart0(pid);
    putsUart0(" killed\r\n");
}

// Kill process with matching name
void pkill(const char* pname)
{
    putsUart0(pname);
    putsUart0(" killed\r\n");
}

// Display PID of process
void pidof(const char* pname)
{
    putsUart0(pname);
    putsUart0(" launched\r\n");
}

// Turn priority inheritance ON/OFF
void pi(bool onOff)
{
    if (onOff) {
        putsUart0("pi on\r\n");
    } else {
        putsUart0("pi off\r\n");
    }
}

// Turn preemption ON/OFF
void preempt(bool onOff)
{
    if (onOff) {
        putsUart0("preempt on\r\n");
    } else {
        putsUart0("preempt off\r\n");
    }
}

// Select priority or Round-Robin scheduling
// expect happy path only(PRIO or RR)
void sched(bool prioRR)
{
    if (prioRR) {
        putsUart0("sched prio\r\n");
    } else {
        putsUart0("sched rr\r\n");
    }
}