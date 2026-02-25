/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 15:52:53 by lde-merc          #+#    #+#             */
/*   Updated: 2026/02/25 17:04:07 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Application.hpp"

int main(int argc, char **argv) {
	std::cout << "\033[32mHello Ray-caster GPU!\033[0m" << std::endl;
	Application app;
	try {
		app.init(argc, argv);
		app.run();
	} catch (inputError &e) {
		std::cerr << "\033[31mInput error:\033[m" << std::endl << e.what() << std::endl; return 1;
	} catch (glfwError &e) {
		std::cerr << "\033[31mGLFW error:\033[m" << std::endl << e.what() << std::endl; return 1;
	} catch (openGlError &e) {
		std::cerr << "\033[31mOpenGl error:\033[m" << std::endl << e.what() << std::endl; return 1;
	} catch (cudaError &e) {
		std::cerr << "\033[31mcuda error:\033[m" << std::endl << e.what() << std::endl; return 1;
	} catch (std::runtime_error &e) {
		std::cerr << e.what() << std::endl; return 1;
	}
	std::cout << "\033[32mApplication closed successfully... Bye !\033[0m" << std::endl;
	return 0;
}
