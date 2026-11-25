# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/11/25 12:54:00 by kong              #+#    #+#              #
#    Updated: 2026/11/25 13:20:00 by kong             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Compiler and Flags
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIR = .
BUILD_DIR = build

# Source Files
SRC_FILES := Serializer.cpp \
             main.cpp
OBJ_FILES := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Header Files
HEADERS := Serializer.hpp Data.hpp

# Executable
NAME = serialize

# Default Target
all: $(NAME)

# Build Executable
$(NAME): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $(NAME)

# Build Object Files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
