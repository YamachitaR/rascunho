# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ryoshio- <ryoshio-@student.42sp.org.br     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/23 13:45:23 by lucasmar          #+#    #+#              #
#    Updated: 2022/10/24 18:20:55 by ryoshio-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# libs *********************************************************************** #
LIBFT_PATH =	./libs/libft/

LIBFT =			$(LIBFT_PATH)libft.a
HEADERS_PATH=	./includes/

# inputs ********************************************************************* #
NAME=minishell

SRC_PATH=./src
OBJ_PATH=./obj

SRC_FILES=minishell.c\
		  check_input_1.c\
		  check_input_2.c\
		  ft_split_ms.c\
		  command_1.c\
		  command_2.c\
		  select_command.c\
		  redirection_1.c\
		  redirection_2.c\
		  redirection_3.c\
		  redirection_4.c\
		  echo.c\
		  cd_1.c\
		  cd_2.c\
		  pwd.c\
		  env_1.c\
		  env_2.c\
		  export_1.c\
		  export_2.c\
		  unset_1.c\
		  error_1.c\
		  error_2.c\
		  ft_get_line.c\
		  dollar_1.c\
		  dollar_2.c\
		  free.c\
		  ft_strncmp_m.c\
		  signal.c\
		  clean_quote.c\
		  exit.c

SRC=$(addprefix $(SRC_PATH)/, $(SRC_FILES))
OBJ=$(SRC:$(SRC_PATH)/%.c=$(OBJ_PATH)/%.o)

# compilation **************************************************************** #
CC=cc
FLAG=-Wall -Werror -Wextra
# clean ********************************************************************** #
RM=-rm -f
RM_DIR=rm -rf
# rules ********************************************************************** #
all:	$(NAME)

$(OBJ_PATH)/%.o:	$(SRC_PATH)/%.c
	@mkdir -p $(OBJ_PATH)
	@$(CC) $(FLAG) -I $(HEADERS_PATH) -c $< -o $@

$(NAME):$(PRINTF) $(LIBFT) $(OBJ)
		@$(CC) $(FLAG) -I $(HEADERS_PATH) -o $@ $(OBJ) \
		$(LIBFT) -lreadline
# message in terminal ************ #
		@echo "\033[1;32m"
		@echo "	Minishell created ✓"
		@echo "\033[0m"
		@echo "\033[0;33m	Welcome to Minishell by lucasmar and ryoshio-'s 42sp \033[0m"
		@echo "\033[0;39m"
		@echo "	run ./Minishell in the terminal !"
		@echo "	ex: ls -la "
		@echo "\033[0m"

#running
run: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  --suppressions=./includes/readline.h ./minishell

$(LIBFT):
	@cd $(LIBFT_PATH) && $(MAKE)

clean:
		@$(RM_DIR) $(OBJ_PATH)
		@echo "\033[0;31m       ▥ Minishell objects clean ✓ \033[0m"

fclean:		clean
		@$(RM) $(NAME)
		@echo "\033[0;31m       ▥ Minishell clean ✓ \033[0m"

fcleanlib:
	@cd $(LIBFT_PATH) && $(MAKE) fclean
	@echo "\033[0;31m       ▥ libs clean ✓ \033[0m"

fcleall: fcleanlib fclean
		 @echo "\033[0;31m       ▥ libs and Minishell clean ✓ \033[0m"

re:			fclean all

reall:		fcleanlib re

.PHONY: all clean fclean fcleanlib fcleanll re reall
