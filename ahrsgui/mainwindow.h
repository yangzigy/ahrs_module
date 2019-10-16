#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qserialport.h"
#include "qserialportinfo.h"
#include <QtCharts>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	void ui_initial();

	QSerialPort *uart;
	int uart_over; //串口超时无数
	int rx_stat; //串口超时状态

	int timerid;
	virtual void timerEvent( QTimerEvent *event);

	QChartView *chartView0;
	QChart *chart0;
	QLineSeries *chart_serial_0;
	QLineSeries *chart_serial_1;
	QLineSeries *chart_serial_2;
	QScatterSeries *chart_cali_m;
	void add2chart(int type,float d); //添加到曲线，type为0不添加，123分别为曲线

	bool cali_m_stat; //是否处于校准状态
signals:
	void ahrs_rxpro_signal(int); //ahrs接收处理
public slots:
	void slot_uart_rx(); //串口接收
	void ahrs_rxpro_slot(int type); //ahrs接收处理

	void slot_curve_sel_change(); //曲线选择变化
	void slot_open();
	void slot_write_cfg();
	void slot_dft_cfg();
	void slot_save_cfg();
	void slot_read_cfg();
	void slot_cali_g();
	void slot_cali_m();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
