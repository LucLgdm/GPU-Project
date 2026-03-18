/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 12:00:20 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/18 15:43:06 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Application.hpp"

int main() {
	std::cout << "\033[32mHello Boids GPU!\033[0m" << std::endl;
	Application app;
	try {
		app.init();
		app.run();
	} catch (inputError &e) {
		std::cerr << "\033[31mInput error:" << std::endl <<"	" << e.what() << "\033[0m"<< std::endl; return 1;
	} catch (glfwError &e) {
		std::cerr << "\033[31mGLFW error:" << std::endl <<"	" << e.what() << "\033[0m"<< std::endl; return 1;
	} catch (openGlError &e) {
		std::cerr << "\033[31mOpenGl error:" << std::endl <<"	" << e.what() << "\033[0m"<< std::endl; return 1;
	} catch (cuda_Error &e) {
		std::cerr << "\033[31mCuda error:" << std::endl <<"	" << e.what() << "\033[0m"<< std::endl; return 1;
	} catch (std::runtime_error &e) {
		std::cerr << e.what() << std::endl; return 1;
	}
	std::cout << "\033[32mApplication closed successfully... Bye !\033[0m" << std::endl;
	return 0;
}
