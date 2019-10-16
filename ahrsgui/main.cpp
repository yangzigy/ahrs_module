#include "mainwindow.h"
#include <QApplication>
#include "qserialport.h"
#include "qserialportinfo.h"
#include "main.h"

MainWindow *pw=0;

int main(int argc, char *argv[])
{
	HINSTANCE hUser32=LoadLibrary("user32.dll"); //防止UI自动放大
	if(hUser32)
	{
		typedef BOOL (WINAPI* LPSetProcessDPIAware)(void);
		LPSetProcessDPIAware pSetProcessDPIAware=(LPSetProcessDPIAware)GetProcAddress(hUser32,"SetProcessDPIAware");
		if(pSetProcessDPIAware)
		{
			pSetProcessDPIAware();
		}
		FreeLibrary( hUser32 );
	}
	QApplication a(argc, argv);
	QFont font = qApp->font();
	font.setPixelSize(12);
	qApp->setFont(font);
	MainWindow w;
	w.show();
	pw=&w;
	//初始化ui
	pw->ui_initial();

	return a.exec();
}
void ahrs_rx_pro(int type) //设备接收函数
{
	pw->ahrs_rxpro_signal(type);
}
void ahrs_uart_send(u8 *p,int n)
{
	pw->uart->write((const char*)p,n);
}
