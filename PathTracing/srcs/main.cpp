/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 15:52:53 by lde-merc          #+#    #+#             */
/*   Updated: 2026/04/01 15:39:02 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Application.hpp"

int main(int argc, char **argv) {
	std::cout << "\033[32mHello Path Tracer GPU!\033[0m" << std::endl;
	if (argc != 2) {
		std::cerr << "\033[31mPath Tracer needs a scene...\033[0m" << std::endl;
		return 1;
	}
	Application app;
	try {
		app.init(argv[1]);
		app.run();
	} catch (inputError &e) {
		std::cerr << "\033[31mInput error: " << std::endl << e.what() << "\033[0m"<< std::endl;
		return 1;
	} catch (glfwError &e) {
		std::cerr << "\033[31mGLFW error: " << std::endl << e.what() << "\033[0m"<< std::endl;
		return 1;
	} catch (tinyObjLoader_Error &e) {
		std::cerr << "\033[31mtinyobjloader error: " << std::endl << e.what() << "\033[0m"<< std::endl;
		return 1;
	} catch (openGlError &e) {
		std::cerr << "\033[31mOpenGl error: " << std::endl << e.what() << "\033[0m"<< std::endl;
		return 1;
	} catch (cuda_Error &e) {
		std::cerr << "\033[31mCuda error: " << std::endl << e.what() << "\033[0m"<< std::endl;
		return 1;
	} catch (std::runtime_error &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	std::cout << "\033[32mApplication closed successfully... Bye !\033[0m" << std::endl;
	return 0;
}