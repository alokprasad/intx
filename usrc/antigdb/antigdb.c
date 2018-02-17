#include<sys/ptrace.h>
#include<stdio.h>
void anti_debug(void)
{
	if(ptrace(PTRACE_TRACEME,0,0,0))
	{
		printf("Attached to debugger with terminate\n");
		kill(getpid());
		exit(0);

	}

}
main()
{
anti_debug();
printf("Anti GDB Code\n");
}
