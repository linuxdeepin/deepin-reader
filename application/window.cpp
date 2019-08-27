#include "window.h"


Window::Window(DMainWindow *parent)
    : DMainWindow(parent)
{

}

Window::~Window()
{
    // We don't need clean pointers because application has exit here.
}


void Window::initTitlebar()
{

}
