lb
==

MPI load balance serial applications

The lb utility load balances serial jobs by using MPI to execute each
job.  The lb utility allows you to specify your jobs in a text file
that is then read and executed across the resource you request. This
works on multiple nodes. You can generate the text file using any
scripting language you like.

The command file should contain a single serial command per line.

The following example uses the sh echo command as a serial job. You can
exchage this line with your own executables.  Simple Example The fol-
lowing bash loop with create a file called cmd_lines that calls the
echo command 100 times.

	for i in {1..100}; do echo "echo simulator $i" >> cmd_lines; done

The file should look like the following.

	echo simulator 1
	echo simulator 2
	echo simulator 3
	echo simulator 100

Then to run all the commands using 20 cores you would launch it as

	mpiexec -np 20 lb cmd_lines

This program is a rewrite of https://github.com/mlunacek/load_balance
without using Boost.
