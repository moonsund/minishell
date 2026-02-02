NAME       = minishell

CC         = cc
UNAME := $(shell uname)

# Add -Wall -Wextra -Werror when testing done and almost ready to submit
CFLAGS     = -g -Iincludes -I$(LIBFT_DIR) #-fsanitize=address

LIBFT_DIR  = libft
LIBFT_A    = $(LIBFT_DIR)/libft.a
LDFLAGS    = -L$(LIBFT_DIR)
LDLIBS     = -lreadline -lft

ifeq ($(UNAME), Darwin) # macOS (Homebrew readline)
  CFLAGS  += -I/opt/homebrew/opt/readline/include
  LDFLAGS += -L/opt/homebrew/opt/readline/lib
endif

# NB have not been tested on Linux yet
ifeq ($(UNAME), Linux)
  LDLIBS  += -lncurses
endif

SRC_PATH = sources/
OBJ_PATH = objects/

SRC_FILES = \
	main.c \
	init/init_shell.c \
	signals.c \
	utils.c \
	tokenizer/tokenizer.c \
	tokenizer/tokenizer_words.c \
	tokenizer/tokenizer_operators.c \
	tokenizer/tokenizer_chars.c \
	tokenizer/tokenizer_utils.c \
	parser/parser_init.c \
	parser/parser_tokens.c \
	parser/parser_utils.c \
	parser/parser.c \
	expand/expand.c \
	heredoc/heredoc.c \
	envp/envp.c \
	envp/envp_utils.c \
	execution/exec_begins.c \
	execution/exec_command_filter.c \
	execution/exec_builtin_commands.c \
	execution/exec_builtin_commands_env.c \
	execution/exec_builtin_commands_env_utils.c \
	execution/exec_external_commands.c \
	execution/exec_fd_related_utils.c \
	execution/exec_close_and_free.c

SRCS = $(addprefix $(SRC_PATH),$(SRC_FILES))
OBJS = $(addprefix $(OBJ_PATH),$(SRC_FILES:.c=.o))
DEPS      = $(OBJS:.o=.d)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(LIBFT_A) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@
	@echo "✅ Built $(NAME)"

$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# libft fetch/build
$(LIBFT_DIR):
	git clone https://github.com/moonsund/libft.git $(LIBFT_DIR)

$(LIBFT_A): | $(LIBFT_DIR)
	$(MAKE) -C $(LIBFT_DIR)

clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@rm -f $(NAME)

re: fclean all

.DELETE_ON_ERROR:
