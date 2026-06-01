# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/23 14:54:56 by lde-merc          #+#    #+#              #
#    Updated: 2026/06/01 15:34:57 by lde-merc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# CUDA_PREFIX := $(CONDA_PREFIX)/targets/x86_64-linux
CUDA_PATH = /usr/local/cuda-13.1
# Compilers
CPP      = g++-14
NVCC     = $(CUDA_PATH)/bin/nvcc
FLAGS    = -MMD -MP -g -std=c++17 -fno-PIE -I$(CUDA_PREFIX)/include \
			-Iincludes -I includes/ImGui/ \
			-I$(CUDA_PATH)/targets/x86_64-linux/include
			
NVFLAGS  = -MMD -MP -g -std=c++17 --extended-lambda \
			-Xcompiler -fno-PIE \
			-ccbin g++-14 \
			-I$(CUDA_PREFIX)/include -I includes/ --extended-lambda \
			-I$(CUDA_PATH)/targets/x86_64-linux/include \
			-arch=sm_86
			
LDFLAGS  = -Xcompiler -no-pie \
           -lOpenGL -lGLU -lglfw -lGLEW -lcudart \
		   -L$(CUDA_PATH)/targets/x86_64-linux/lib
		   
NVFLAGS += -diag-suppress 550 -diag-suppress 20012

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
OBJ		 = $(SRC:srcs/%.cpp=$(OBJDIR)/%.o)
CUOBJ 	 = $(CUSRC:srcs/%.cu=$(CUOBJDIR)/%.o)
OBJGLAD	 = $(OBJDIR)/glad.o
DEP		 = $(OBJ:.o=.d)
DEP 	 += $(CUOBJ:.o=.d)

# Target executable
NAME     = PathTracer

all: $(NAME) ## Compile and link

run: ## Run the application with NVIDIA offloading, make run ARGS="your arguments here"
	__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia ./$(NAME) $(ARGS)

clean: ## Clean object files
	@rm -rf $(OBJDIR) $(CUOBJDIR)
	@printf "\033[34mDeleted object files!\n\033[0m"

fclean: clean ## Clean object files and executable
	@rm -f $(NAME)
	@printf "\033[35mDeleted everything!\n\033[0m"
	
re: fclean all ## Rebuild
	@printf "\033[33mRebuild done!\n\033[0m"


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
	@printf "\033[32m$(NAME) created!\n\033[0m"

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