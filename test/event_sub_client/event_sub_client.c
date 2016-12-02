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

#include "logd.h"
#include "sys_utils.h"
#include "event_mngr_utils.h"
#include "global_state.h"

int _log_v = 4;
int g_quit = 0;
static void sig_handler_func(int sig)
{
	DBG("sig_handler_func : %d\n", sig);
	g_quit = 1;
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

int main()
{
	int ret;
	fd_set rfds;
	int max_fd;
	struct timeval tv;
	char recv_buf[8096];
	struct br_event_t br_event;

	int ipc_socket = connect_event_mngr();
	if (ipc_socket < 0) {
		ERR("connect event mngr err\n");
		return -1;
	}

	//signal_setup();

	register_thread_name(ipc_socket, "sub_client");

	subscribe_event(ipc_socket, BROADCAST_UPGRADE_EVENT_ID);

	while (!g_quit) {
		int i;
		FD_ZERO(&rfds);
		FD_SET(ipc_socket, &rfds);

		tv.tv_sec = 0;
		tv.tv_usec = 30 * 1000;

		max_fd = ipc_socket;

		ret = select(max_fd + 1, &rfds, NULL, NULL, &tv);
		if (ret < 0) {
			usleep(10 * 1000);
		}

		if (FD_ISSET(ipc_socket, &rfds)) {
			int n_r = recv(ipc_socket, recv_buf, sizeof(recv_buf) - 1, 0);
			MSG("!!!!!!!!!!!!!!! n_r = %d\n", n_r);

			int *head = recv_buf;
			printf("data:\n");
			if (*head != 0xdeadbeef) {
				printf("%s\n", recv_buf);
			} else {
				for (i = 0; i < n_r; i++) {
					if ((i+1)%16 == 0)
						printf("\n");
					printf("0x%02x ", (unsigned char )recv_buf[i]);
				}
				printf("\n");
			}
		}
	}

}
