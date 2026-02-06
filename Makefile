NAME       = minishell

CC         = cc
UNAME := $(shell uname)

CFLAGS     = -g -Wall -Wextra -Werror -Iincludes -I$(LIBFT_DIR)

LIBFT_DIR  = libft
LIBFT_A    = $(LIBFT_DIR)/libft.a
LDFLAGS    = -L$(LIBFT_DIR)
LDLIBS     = -lreadline -lft

ifeq ($(UNAME), Darwin)
  CFLAGS  += -I/opt/homebrew/opt/readline/include
  LDFLAGS += -L/opt/homebrew/opt/readline/lib
endif

ifeq ($(UNAME), Linux)
  LDLIBS  += -lncurses
endif

SRC_DIR = sources/
OBJ_DIR = objects/

SRC_MAIN = \
	main.c

SRC_MINISHELL = \
	minishell/minishell.c \
	minishell/init_shell.c \
	minishell/init_shell_utils.c \
	minishell/signals.c \
	minishell/free_utils.c \
	minishell/print_utils.c

SRC_TOKENIZER = \
	tokenizer/tokenizer.c \
	tokenizer/tokenizer_words.c \
	tokenizer/tokenizer_operators.c \
	tokenizer/tokenizer_operators_utils.c \
	tokenizer/tokenizer_chars.c \
	tokenizer/tokenizer_utils.c \
	tokenizer/tokenizer_helpers.c

SRC_PARSER = \
	parser/parser.c \
	parser/parser_tokens.c \
	parser/parser_init.c \
	parser/parser_helpers.c \
	parser/parser_utils.c

SRC_EXPAND = \
	expand/expand_tokens.c \
	expand/expand_tokens_utils.c \
	expand/expand_utils.c

SRC_HEREDOC = \
	heredoc/heredoc.c \
	heredoc/heredoc_expand.c \
	heredoc/heredoc_helpers.c

SRC_ENVP = \
	envp/envp.c \
	envp/envp_utils.c \
	envp/unset_var.c \
	envp/set_var.c \
	envp/build_envp.c

SRC_EXECUTION = \
	execution/execution.c \
	execution/execution_forking.c \
	execution/execution_forking_helpers.c \
	execution/exec_fork_actions.c \
	execution/exec_command_filter.c \
	execution/exec_builtin_commands.c \
	execution/exec_builtin_cmds_utils.c \
	execution/exec_builtin_env_commands.c \
	execution/exec_builtin_env_cmds_utils.c \
	execution/exec_external_commands.c \
	execution/exec_external_cmds_utils.c \
	execution/exec_fd_related_utils.c \
	execution/exec_close_and_free.c

SRC_FILES = \
	$(SRC_MAIN) \
	$(SRC_MINISHELL) \
	$(SRC_TOKENIZER) \
	$(SRC_PARSER) \
	$(SRC_EXPAND) \
	$(SRC_HEREDOC) \
	$(SRC_ENVP) \
	$(SRC_EXECUTION)

SRCS = $(addprefix $(SRC_DIR) $(SRC_FILES))
OBJS = $(addprefix $(OBJ_DIR),$(SRC_FILES:.c=.o))
DEPS = $(OBJS:.o=.d)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(LIBFT_A) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@
	@echo "✅ Built $(NAME)"

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# libft fetch/build
$(LIBFT_DIR):
	git clone https://github.com/moonsund/libft.git $(LIBFT_DIR)

$(LIBFT_A): | $(LIBFT_DIR)
	$(MAKE) -C $(LIBFT_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@rm -f $(NAME)

re: fclean all

.DELETE_ON_ERROR:
