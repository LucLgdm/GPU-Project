# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/23 15:15:51 by lde-merc          #+#    #+#              #
#    Updated: 2026/03/18 11:50:06 by lde-merc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

all: rc pt bd

# ─── Regles HOST (lancent le docker) ─────────────────────────────────────────

rc: ## Build RC
	@echo "\033[36mRaycaster directory for compilation... \033[0m"
	@make --no-print-directory -C RC all
	
pt: ## Build PathTracing
	@echo "\033[36mPathTracer directory for compilation... \033[0m"
	@make --no-print-directory -C PathTracing all

bd: ## Build Boids
	@echo "\033[36mBoid directory for compilation... \033[0m"
	@make --no-print-directory -C Boid all

clean: ## Clean both
	@echo "\033[36mRaycaster directory for cleaning... \033[0m"
	@make --no-print-directory -C RC clean
	@echo "\033[36mPathTracer directory for cleaning... \033[0m"
	@make --no-print-directory -C PathTracing clean
	@echo "\033[36mBoid directory for cleaning... \033[0m"
	@make --no-print-directory -C Boid clean
	
fclean: clean ## Full clean both
	@make --no-print-directory -C RC fclean
	@make --no-print-directory -C PathTracing fclean
	@make --no-print-directory -C Boid fclean

re: fclean all

.PHONY: all rc pt bd clean fclean re