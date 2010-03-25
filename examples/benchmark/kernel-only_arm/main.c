// Emtpy main.c file generated by the wizard
#include <cpu/irq.h>
#include <kern/proc.h>
#include <kern/signal.h>
#include <kern/msg.h>

MsgPort in_port;

static void init(void)
{
	IRQ_ENABLE;
	proc_init();
}

static cpu_stack_t proc1_stack[500];

static void proc1_main(void)
{

}

int main(void)
{
	init();
	// generate code for process
	struct Process *p = proc_new(proc1_main, 0, sizeof(proc1_stack), proc1_stack);
	proc_setPri(p, 5);
	proc_yield();
	// generate code for msg
	Msg msg;
	msg_initPort(&in_port, event_createSignal(p, SIG_USER1));
	msg_put(&in_port, &msg);
	msg_peek(&in_port);
	Msg *msg_re = msg_get(&in_port);
	msg_reply(msg_re);
	// generate code for signals
	sig_send(p, SIG_USER0);
	sig_wait(SIG_USER0);

	return 0;
}

