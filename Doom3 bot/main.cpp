#include "Proxy.h"
int main()
{
	Proxy proxy;
	proxy.EstablishConnection();
	proxy.StartLoop();
}