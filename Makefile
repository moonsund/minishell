NAME       = minishell

CC         = cc
CFLAGS     = -g -Wall -Wextra -Werror

INC        = -Iincludes

SRC_PATH = sources/
OBJ_PATH = objects/

LIBFT_DIR  = libft
LIBFT_A    = $(LIBFT_DIR)/libft.a


SRC_FILES = main.c

SRCS = $(addprefix $(SRC_PATH),$(SRC_FILES))
OBJS = $(addprefix $(OBJ_PATH),$(SRC_FILES:.c=.o))

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJ_PATH)%.o: $(SRC_PATH)%.c
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -f $(NAME)

re: fclean all