#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sys/select.h>
#include <signal.h>
#include <linux/input.h>

#include "logd.h"
#include "sys_utils.h"
#include "event_mngr_utils.h"
#include "global_state.h"

int _log_v = 4;

static void sig_handler_func(int sig)
{
	DBG("sig_handler_func : %d\n", sig);
	//g_quit = 1;
}

static void sig_handler_ignore(int sig)
{
	DBG("sig_handler_ignore : %d\n", sig);
}

/* copy from shairport */
void signal_setup(void) {
    // mask off all signals before creating threads.
    // this way we control which thread gets which signals.
    // for now, we don't care which thread gets the following.
    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGINT);
    sigdelset(&set, SIGTERM);
    sigdelset(&set, SIGHUP);
    sigdelset(&set, SIGSTOP);
    //sigdelset(&set, SIGCHLD);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    //setting this to SIG_IGN would prevent signalling any threads.
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &sig_handler_ignore;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sa.sa_sigaction = &sig_handler_func;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    sa.sa_sigaction = &sig_handler_ignore;
    sigaction(SIGHUP, &sa, NULL);
}


int main(int argc, char *argv[])
{
    int i;

    if (argc != 3) {
        printf("error num args\n");
        return -1;
    }

    int event_id = atoi(argv[1]);
    int event_state = atoi(argv[2]);

    printf("argc [1] = %d\n", event_id);
    printf("argc [2] = %d\n", event_state);


	int ipc_socket = connect_event_mngr();
	if (ipc_socket < 0) {
		ERR("connect event mngr err\n");
	}

    unsigned char buf[4096];
    for (i = 0; i < 4096; i++) {
        buf[i] = i%256;
    }

	signal_setup();

	register_thread_name(ipc_socket, "pub_client");

	publish_event_buf(ipc_socket, 0x04, buf, 4096);
    publish_event(ipc_socket, event_id, event_state);

}
