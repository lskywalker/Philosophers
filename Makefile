NAME = philo

SRC =	main.c \
		init.c \
		algorithm.c \
		utils.c \
		utils2.c

OBJ = ${SRC:%.c=%.o}

FLAGS = -Wall -Wextra -Werror -pthread

ifeq ($(DEBUG),1)
FLAGS += -g3 -fsanitize=address
endif
ifeq ($(DEBUG),2)
FLAGS += -g3 -fsanitize=thread
endif

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