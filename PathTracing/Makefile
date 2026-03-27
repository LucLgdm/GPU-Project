# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/23 14:54:56 by lde-merc          #+#    #+#              #
#    Updated: 2026/03/27 14:24:50 by lde-merc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CUDA_PREFIX := $(CONDA_PREFIX)/targets/x86_64-linux
# Compilers
CPP      = g++
NVCC     = nvcc
FLAGS    = -MMD -MP -g -std=c++17 -I$(CUDA_PREFIX)/include \
			-Iincludes -I includes/ImGui/
			
NVFLAGS  = -MMD -MP -g -std=c++17 -I$(CUDA_PREFIX)/include -I includes/ --extended-lambda
LDFLAGS  = -L$(CONDA_PREFIX)/lib \
           -L$(CUDA_PREFIX)/lib \
           -lOpenGL -lGLU -lglfw -lGLEW -lcudart
NVFLAGS += -diag-suppress 550

FLAGS   += -I$(CONDA_PREFIX)/include
NVFLAGS += -I$(CONDA_PREFIX)/include
LDFLAGS += -L$(CONDA_PREFIX)/lib

# Sources
SRC = $(wildcard srcs/*.cpp) \
      $(wildcard srcs/app/*.cpp) \
      $(wildcard srcs/cuda/*.cpp) \
      $(wildcard srcs/ImGui/*.cpp) \
      $(wildcard srcs/render/*.cpp) \
      $(wildcard srcs/scene/*.cpp)

CUSRC = $(wildcard srcs/cuda/*.cu)

SRCC = srcs/glad.c

# Object directories
OBJDIR   = objs
CUOBJDIR = objs_cuda
OBJ   = $(SRC:srcs/%.cpp=$(OBJDIR)/%.o)
CUOBJ = $(CUSRC:srcs/%.cu=$(CUOBJDIR)/%.o)
OBJGLAD	= $(OBJDIR)/glad.o
DEP		= $(OBJ:.o=.d)
DEP += $(CUOBJ:.o=.d)

# Target executable
NAME     = PathTracer

all: $(NAME) ## Compile and link in the container

clean: ## Clean object files in the container
	@rm -rf $(OBJDIR) $(CUOBJDIR)
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
$(OBJDIR)/%.o: srcs/%.cpp
	@mkdir -p $(@D)
	@$(CPP) $(FLAGS) -c $< -o $@

# Compile CUDA sources
$(CUOBJDIR)/%.o: srcs/%.cu
	@mkdir -p $(@D)
	@$(NVCC) $(NVFLAGS) -c $< -o $@
	
# Compile GLAD (C source)
$(OBJDIR)/glad.o: srcs/glad.c
	@mkdir -p $(@D)
	@$(CPP) $(FLAGS) -c $< -o $@
	
# Include dependencies
-include $(DEP)

# Phony targets
.PHONY: all clean fclean re help val