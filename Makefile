NAME = philo

SRC =	main.c \
		utils.c

OBJ = ${SRC:%.c=%.o}

FLAGS = -Wall -Wextra -Werror

%.o: %.c
	gcc -c -o $@ $< $(FLAGS)

$(NAME): $(OBJ)
	gcc $(FLAGS) $(OBJ) -o $(NAME)

all: $(NAME)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all