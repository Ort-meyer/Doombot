#include "Proxy.h"
int main()
{
	Proxy proxy;
	proxy.EstablishConnection();
	proxy.SetMovement((Movement)(Movement::Forward));// | Movement::Left));
	//proxy.SetMouseMovement(100, 100);
	proxy.StartLoop();
}