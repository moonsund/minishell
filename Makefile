NAME       = minishell

CC         = cc
UNAME := $(shell uname)

CFLAGS     = -g -Wall -Wextra -Werror -Iincludes

LIBFT_DIR  = libft
LIBFT_A    = $(LIBFT_DIR)/libft.a
LDFLAGS    = -L$(LIBFT_DIR)
LDLIBS     = -lreadline -lft

ifeq ($(UNAME), Darwin) # macOS (Homebrew readline)
  CFLAGS  += -I/opt/homebrew/opt/readline/include
  LDFLAGS += -L/opt/homebrew/opt/readline/lib
endif

# NB have not tested on Linux yet
ifeq ($(UNAME), Linux)
  LDLIBS  += -lncurses
endif


SRC_PATH = sources/
OBJ_PATH = objects/

SRC_FILES = main.c signals.c lexer.c normalize.c utils.c

SRCS = $(addprefix $(SRC_PATH),$(SRC_FILES))
OBJS = $(addprefix $(OBJ_PATH),$(SRC_FILES:.c=.o))
DEPS      = $(OBJS:.o=.d)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(LIBFT_A) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@
	@echo "✅ Built $(NAME)"

$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CFLAGS) -c $< -o $@


# libft fetch/build
$(LIBFT_DIR):
	git clone https://github.com/moonsund/libft.git $(LIBFT_DIR)

$(LIBFT_A): | $(LIBFT_DIR)
	$(MAKE) -C $(LIBFT_DIR)


clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -f $(NAME)

re: fclean all

.DELETE_ON_ERROR: