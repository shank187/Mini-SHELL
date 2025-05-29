CC = cc
CFLAGS = -Wall -Wextra -Werror  \
   -I$(HOME)/.local/include
#remove fsanitize
LDFLAGS = -L$(HOME)/.local/lib -lreadline

AR = ar
ARFLAGS = rc
RM = rm -f


exe = minishell



SRC = execution/builtin_tools.c execution/execute.c       execution/mini_errors1.c execution/mini_errors0.c execution/piping.c        execution/redirection.c   execution/shell_builts0.c execution/shell_builts1.c execution/shell_builts2.c execution/shell_builts3.c execution/var_tools_0.c   execution/var_tools_1.c  execution/var_tools_2.c execution/piping_1.c execution/execute_1.c \
		includes/allocs.c         includes/cleaning.c       includes/env_tools.c      includes/handle_signals.c includes/mini_atoi.c      includes/mini_lib0.c      includes/mini_lib1.c      includes/mini_lib2.c  includes/mini_lib3.c    includes/mini_itoa.c      includes/mini_split.c \
		parsing/add_helper1.c       parsing/add_utils1.c        parsing/expand_var.c        parsing/handle_red_utils2.c parsing/if_conditions.c     parsing/parse_helper1.c     parsing/parsing.c           parsing/tokenizer.c         parsing/tokenizer_2utils.c  parsing/tokenizer_fill.c \
		parsing/add_helper2.c       parsing/add_utils2.c        parsing/handle_red_utils1.c  parsing/parse.c             parsing/parse_helper2.c     parsing/readline.c          parsing/tokenizer_1utils.c  parsing/tokenizer_3utils.c  parsing/tokenizer_fill2.c  parsing/tokenizer_fill3.c  parsing/readline_tools.c\
		minishell.c minishell_utils.c     parsing/add_helper3.c  parsing/expand_var_utils1.c   parsing/parse_helper3.c           parsing/add_utils3.c            parsing/add_utils4.c        parsing/add_utils5.c         parsing/add_utils6.c   parsing/handle_red_utils3.c  parsing/tokenizer_4utils.c 



OBJ = $(SRC:.c=.o)




all: $(exe) 

$(exe): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@


execution/%.o: execution/%.c execution/execution.h
	$(CC) $(CFLAGS) -c $< -o $@

includes/%.o: includes/%.c BONUS/minishell.h
	$(CC) $(CFLAGS) -c $< -o $@

parsing/%.o: parsing/%.c parsing/he.h.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(exe)

re: fclean all

.PHONY: clean