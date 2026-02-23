# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/23 15:15:51 by lde-merc          #+#    #+#              #
#    Updated: 2026/02/23 17:18:01 by lde-merc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

COMPOSE  = docker compose
SERVICE  = builder

all: rc pt

# ─── Regles HOST (lancent le docker) ─────────────────────────────────────────

rc: ## Build RC
	$(COMPOSE) run --rm -w /Homeworks/RC $(SERVICE) make all
pt: ## Build PathTracing
	$(COMPOSE) run --rm -w /Homeworks/PathTracing $(SERVICE) make all

clean: ## Clean both
	$(COMPOSE) run --rm -w /Homeworks/RC $(SERVICE) make clean
	$(COMPOSE) run --rm -w /Homeworks/PathTracing $(SERVICE) make clean
	@rm -rf RC/objs PathTracing/objs

fclean: ## Full clean both
	$(COMPOSE) run --rm -w /Homeworks/RC $(SERVICE) make fclean
	$(COMPOSE) run --rm -w /Homeworks/PathTracing $(SERVICE) make fclean
	$(COMPOSE) down --volumes --rmi all

re: fclean all

.PHONY: all rc pt clean fclean re