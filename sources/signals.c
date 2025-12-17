#include "minishell.h"

volatile sig_atomic_t g_sigint = 0;
static void ctrl_c_handler(int signum);


void setup_signals(void)
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = ctrl_c_handler;
    sigaction(SIGINT, &sa, NULL); // ctrl+C

    sa.sa_handler = SIG_IGN;
    sigaction(SIGQUIT, &sa, NULL);
}

static void ctrl_c_handler(int signum)
{
    (void)signum;
    g_sigint = 1;
    write(STDOUT_FILENO, "\n", 1);
    // rl_on_new_line();
    // rl_replace_line("", 0);
    // rl_redisplay();
}