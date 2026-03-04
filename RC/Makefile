# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/23 14:54:56 by lde-merc          #+#    #+#              #
#    Updated: 2026/03/04 11:36:13 by lde-merc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CONDA_PREFIX ?= $(CONDA_PREFIX)

# Compilers
CPP      = g++
NVCC     = $(CONDA_PREFIX)/bin/nvcc
FLAGS    = -MMD -g -std=c++17 -I includes/
NVFLAGS  = -g -std=c++17 -I includes/ --extended-lambda
LDFLAGS  = -lGL -lGLU -lglfw -lGLEW -lcudart \
           -Xlinker -rpath -Xlinker $(CONDA_PREFIX)/lib

# Sources
vpath %.cpp srcs/
vpath %.cu  srcs/
vpath %.c 	srcs/

SRC      = $(wildcard srcs/*.cpp)
CUSRC    = $(wildcard srcs/*.cu)
SRCC     = glad.c

# Object directories
OBJDIR   = objs
OBJ      = $(SRC:%.cpp=$(OBJDIR)/%.o)
CUOBJ    = $(CUSRC:srcs/%.cu=$(OBJDIR)/%.o)
OBJGLAD  = $(OBJDIR)/glad.o
DEP      = $(OBJ:.o=.d)

# Target executable
NAME     = RayCaster

all: $(NAME) ## Compile and link in the container

clean: ## Clean object files in the container
	@rm -rf $(OBJDIR)/* $(OBJDIR)/**/*
	@echo "\033[34mDeleted object files!\033[0m"

fclean: clean ## Clean object files and executable and docker volumes
	@rm -f $(NAME)
	@echo "\033[35mDeleted everything!\033[0m"
	
re: fclean all ## Rebuild
	@echo "\033[33mRebuild done!\033[0m"


val: all ## Run with Valgrind
	valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite \
		./$(NAME) resources/42.obj || true


help: ## Display the help
	@awk 'BEGIN {FS = ":.*##"} \
		/^[a-zA-Z_-]+:.*##/ { \
			printf "\033[34m  %-10s\033[0m \033[32m%s\033[0m\n", $$1, $$2 \
		}' $(MAKEFILE_LIST)

# Link (nvcc pour gérer les symboles CUDA)
$(NAME): $(OBJ) $(CUOBJ) $(OBJGLAD)
	@$(NVCC) $(OBJ) $(CUOBJ) $(OBJGLAD) $(LDFLAGS) -o $@
	@echo "\033[32m$(NAME) created!\033[0m"

# Compile C++ sources
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@$(CPP) $(FLAGS) -c $< -o $@

# Compile CUDA sources
$(OBJDIR)/%.o: %.cu
	@mkdir -p $(@D)
	@$(NVCC) $(NVFLAGS) -c $< -o $@
	
# Compile GLAD (C source)
$(OBJDIR)/glad.o: $(SRCC)
	@mkdir -p $(@D)
	@$(CPP) $(FLAGS) -c $< -o $@
	
# Include dependencies
-include $(DEP)

# Phony targets
.PHONY: all clean fclean re help val