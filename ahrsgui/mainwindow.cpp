#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qmessagebox.h"
#include "ahrsgui.h"

const u32 com_baud_tab[]=//串口波特率表
{
	9600,19200,28800,38400,57600,115200,
	230400,460800,500000,576000,921600,
};
const char* curve_name_tab[]=
{
	"欧拉角x",
	"欧拉角y",
	"欧拉角z",
	"陀螺x轴",
	"陀螺y轴",
	"陀螺z轴",
	"加计x轴",
	"加计y轴",
	"加计z轴",
	"罗盘x轴",
	"罗盘y轴",
	"罗盘z轴",
	"无",
};
int para_curve_tab[]= { 0,0,0,0, 0,0,0,0, 0,0,0,0 ,0 };

u32 com_time_getms(void)
{
	auto now=chrono::system_clock::now();
	return chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	uart_over=0;
	cali_m_stat=false;
	timerid=startTimer(100);
	rx_stat=0; //未连接
	foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
	{
		ui->cb_uart->addItem(info.portName());
	}
	for(auto b:com_baud_tab)
	{
		QString s;
		s.sprintf("%d",b);
		ui->cb_baud->addItem(s);
		ui->cb_set_baud->addItem(s);
	}
	ui->cb_baud->setCurrentIndex(5);
	ui->cb_set_baud->setCurrentIndex(5);
	//曲线显示部分
	chart0 = new QChart();
	QMargins tmpmarg(5,0,5,0);
	chart0->setMargins(tmpmarg);
	chartView0 = new QChartView(chart0);
//	chartView0->setRenderHint(QPainter::Antialiasing, true); //
//	chart0->setTheme(QChart::ChartThemeDark); //
//	set_theme(chart0->theme());
	//chart0->setTitle("Line chart0");
	chart_serial_0= new QLineSeries(chart0);
	chart_serial_0->setName("CH0");
	chart_serial_0->setUseOpenGL(true); //使用OpenGL加速显示
	chart0->addSeries(chart_serial_0);
	chart_serial_1= new QLineSeries(chart0);
	chart_serial_1->setName("CH1");
	chart_serial_1->setUseOpenGL(true); //使用OpenGL加速显示
	chart0->addSeries(chart_serial_1);
	chart_serial_2= new QLineSeries(chart0);
	chart_serial_2->setName("CH2");
	chart_serial_2->setUseOpenGL(true); //使用OpenGL加速显示
	chart0->addSeries(chart_serial_2);
	chart_cali_m=new QScatterSeries(chart0);
	chart_cali_m->setName("罗盘值");
	chart_cali_m->setMarkerShape(QScatterSeries::MarkerShapeCircle);
	chart_cali_m->setMarkerSize(3.0);
	chart_cali_m->setUseOpenGL(true); //使用OpenGL加速显示
	chart0->addSeries(chart_cali_m);
	chart0->createDefaultAxes();
	chart0->axisX()->setRange(0, 240);
	chart0->axisY()->setRange(-180, 180);
//	static_cast<QValueAxis *>(chart0->axisX())->setLabelFormat("%d  ");
//	static_cast<QValueAxis *>(chart0->axisY())->setLabelFormat("%d  ");
	chartView0->setRubberBand(QChartView::VerticalRubberBand);
	ui->widget_chart->layout()->addWidget(chartView0);
	ui->widget_chart->layout()->setMargin(0);
	ui->widget_chart->setContentsMargins(tmpmarg);

	//初始化串口
	uart=new QSerialPort();
	QObject::connect(uart, SIGNAL(readyRead()), this, SLOT(slot_uart_rx()));
	QObject::connect(this,SIGNAL(ahrs_rxpro_signal(int)),this,SLOT(ahrs_rxpro_slot(int)));
}
MainWindow::~MainWindow()
{
	delete ui;
}
void MainWindow::ui_initial()
{
	ui->bt_cali_m->setCheckable(true); //添加线路的按钮，是带锁的
	for(auto c:curve_name_tab)
	{
		ui->cb_curve0->addItem(c);
		ui->cb_curve1->addItem(c);
		ui->cb_curve2->addItem(c);
	}
	ui->cb_curve0->setCurrentIndex(0);
	ui->cb_curve1->setCurrentIndex(1);
	ui->cb_curve2->setCurrentIndex(2);
}
void MainWindow::slot_uart_rx() //串口接收
{
	char buf[128];
	int n=1;
	while(n)
	{
		n=uart->read(buf,sizeof(buf));
		for(int i=0;i<n;i++)
		{
			rec_head(buf[i],&ahrsguitask_head);
		}
	}
}
void MainWindow::ahrs_rxpro_slot(int type) //ahrs接收处理
{
	uart_over=10;
	QString s;
	u32 tmp=com_time_getms();
	float f;
	if(type==1)
	{
		static u32 pre_ms=0;
		if(tmp-pre_ms>100)
		{
			f=rxbuf1.gx*0.01f;s.sprintf("%.2f",f); ui->lb_g_x->setText(s);add2chart(para_curve_tab[3],f);
			f=rxbuf1.gy*0.01f;s.sprintf("%.2f",f); ui->lb_g_y->setText(s);add2chart(para_curve_tab[4],f);
			f=rxbuf1.gz*0.01f;s.sprintf("%.2f",f); ui->lb_g_z->setText(s);add2chart(para_curve_tab[5],f);
			f=rxbuf1.ax*0.0001f;s.sprintf("%.2f",f); ui->lb_a_x->setText(s);add2chart(para_curve_tab[6],f);
			f=rxbuf1.ay*0.0001f;s.sprintf("%.2f",f); ui->lb_a_y->setText(s);add2chart(para_curve_tab[7],f);
			f=rxbuf1.az*0.0001f;s.sprintf("%.2f",f); ui->lb_a_z->setText(s);add2chart(para_curve_tab[8],f);
			f=rxbuf1.mx*0.0001f;s.sprintf("%.2f",f); ui->lb_m_x->setText(s);add2chart(para_curve_tab[9],f);
			f=rxbuf1.my*0.0001f;s.sprintf("%.2f",f); ui->lb_m_y->setText(s);add2chart(para_curve_tab[10],f);
			f=rxbuf1.mz*0.0001f;s.sprintf("%.2f",f); ui->lb_m_z->setText(s);add2chart(para_curve_tab[11],f);
			f=rxbuf1.dtime*0.1f;s.sprintf("%.1f",f); ui->lb_delta_time->setText(s);
			pre_ms=tmp;
			//磁力计校准
			if(cali_m_stat)
			{
				static QVector3D pre_m(0,0,0);
				QVector3D m(rxbuf1.mx*0.0001f,rxbuf1.my*0.0001f,rxbuf1.mz*0.0001f);
				if(pre_m.distanceToPoint(m)>0.05f)
				{
					pre_m=m;
					cali_m_data.push_back(m);
					//显示
					chart_cali_m->append(m.x(),m.y());
					chart_cali_m->append(m.x()+2,m.z());
				}
			}
		}
	}
	else if(type==2)
	{
		static u32 pre_ms=0;
		if(tmp-pre_ms>100)
		{
			f=rxbuf2.ux*0.01f;s.sprintf("%.2f",f); ui->lb_eu_x->setText(s);add2chart(para_curve_tab[0],f);
			f=rxbuf2.uy*0.01f;s.sprintf("%.2f",f); ui->lb_eu_y->setText(s);add2chart(para_curve_tab[1],f);
			f=rxbuf2.uz*0.01f;s.sprintf("%.2f",f); ui->lb_eu_z->setText(s);add2chart(para_curve_tab[2],f);
			s.sprintf("%.3f",rxbuf2.q0); ui->lb_quat_w->setText(s);
			s.sprintf("%.3f",rxbuf2.q1); ui->lb_quat_x->setText(s);
			s.sprintf("%.3f",rxbuf2.q2); ui->lb_quat_y->setText(s);
			s.sprintf("%.3f",rxbuf2.q3); ui->lb_quat_z->setText(s);
			pre_ms=tmp;
		}
	}
	else if(type==3)
	{
		s.sprintf("%.2f",rxbuf3.mkx*0.0001f); ui->le_m_kx->setText(s);
		s.sprintf("%.2f",rxbuf3.moffx*0.0001f); ui->le_m_0_x->setText(s);
		s.sprintf("%.2f",rxbuf3.mky*0.0001f); ui->le_m_ky->setText(s);
		s.sprintf("%.2f",rxbuf3.moffy*0.0001f); ui->le_m_0_y->setText(s);
		s.sprintf("%.2f",rxbuf3.mkz*0.0001f); ui->le_m_kz->setText(s);
		s.sprintf("%.2f",rxbuf3.moffz*0.0001f); ui->le_m_0_z->setText(s);
		s.sprintf("%.2f",rxbuf3.g_offset_x*0.001f); ui->le_g_0_x->setText(s);
		s.sprintf("%.2f",rxbuf3.g_offset_y*0.001f); ui->le_g_0_y->setText(s);
		s.sprintf("%.2f",rxbuf3.g_offset_z*0.001f); ui->le_g_0_z->setText(s);
		s.sprintf("%d",rxbuf3.fre1); ui->le_fre_1->setText(s);
		s.sprintf("%d",rxbuf3.fre2); ui->le_fre_2->setText(s);
		ui->cb_set_baud->setCurrentIndex(rxbuf3.baud);
		ui->cbox_9axis->setChecked(rxbuf3.mod==0);
	}
}
void MainWindow::timerEvent(QTimerEvent *event)
{
	if(event->timerId() == timerid)
	{
		if(uart_over) uart_over--;
		if(uart_over && rx_stat==0) //若未超时，且显示未连接
		{
			rx_stat=1;
			ui->lb_state->setStyleSheet("color: rgb(0, 180, 0);");
			ui->lb_state->setText("已连接");
			ui->lb_state->setFont(QFont("Microsoft YaHei", 12));
		}
		else if(uart_over==0 && rx_stat) //若超时，且显示了已连接
		{
			rx_stat=0;
			ui->lb_state->setStyleSheet("color: rgb(255, 0, 0);");
			ui->lb_state->setText("未连接");
		}
	}
}
void MainWindow::add2chart(int type,float d) //添加到曲线，type为0不添加，123分别为曲线
{
	if(type==0 ||  cali_m_stat) return ;
	u32 xa=com_time_getms();
	static u32 d0=0;
	static u32 d1=240;
	if(type==1)
	{
		if(chart_serial_0->count()>240)	chart_serial_0->remove(0);
		chart_serial_0->append(xa,d);
		if(chart_serial_0->at(0).x()>d0) d0=chart_serial_0->at(0).x();
		if(chart_serial_0->at(chart_serial_0->count()-1).x()>d1) d1=chart_serial_0->at(chart_serial_0->count()-1).x();
		//chart0->scroll(chart_serial_0->at(0).x(),0);
	}
	else if(type==2)
	{
		if(chart_serial_1->count()>240)	chart_serial_1->remove(0);
		chart_serial_1->append(xa,d);
		if(chart_serial_1->at(0).x()>d0) d0=chart_serial_1->at(0).x();
		if(chart_serial_1->at(chart_serial_1->count()-1).x()>d1) d1=chart_serial_1->at(chart_serial_1->count()-1).x();
		//chart0->scroll(chart_serial_1->at(0).x(),0);
	}
	else if(type==3)
	{
		if(chart_serial_2->count()>240)	chart_serial_2->remove(0);
		chart_serial_2->append(xa,d);
		if(chart_serial_2->at(0).x()>d0) d0=chart_serial_2->at(0).x();
		if(chart_serial_2->at(chart_serial_2->count()-1).x()>d1) d1=chart_serial_2->at(chart_serial_2->count()-1).x();
		//chart0->scroll(chart_serial_2->at(0).x(),0);
	}
	chart0->axisX()->setRange(d0,(d1-d0)>1000?d1:1000+d0);
}
/////////////////////////////////////////////////////////////
//				界面响应
/////////////////////////////////////////////////////////////
void MainWindow::slot_curve_sel_change() //曲线选择变化
{
	int i1=ui->cb_curve0->currentIndex();
	int i2=ui->cb_curve1->currentIndex();
	int i3=ui->cb_curve2->currentIndex();
	int max=sizeof(curve_name_tab)/sizeof(char*);
	if(i1>=0 && i1<max &&
		i2>=0 && i2<max &&
		i3>=0 && i3<max)
	{
		memset(para_curve_tab,0,sizeof(para_curve_tab));
		para_curve_tab[i1]=1;
		para_curve_tab[i2]=2;
		para_curve_tab[i3]=3;
		chart_serial_0->setName(curve_name_tab[i1]);
		chart_serial_1->setName(curve_name_tab[i2]);
		chart_serial_2->setName(curve_name_tab[i3]);
		if(i1==max-1) chart_serial_0->clear();
		if(i2==max-1) chart_serial_1->clear();
		if(i3==max-1) chart_serial_2->clear();
	}
}
void MainWindow::slot_open()
{
	if(ui->bt_open->text()=="打开串口")
	{
		uart->setPortName(ui->cb_uart->currentText());
		uart->setBaudRate(com_baud_tab[ui->cb_baud->currentIndex()]);
		if(uart->open(QIODevice::ReadWrite))
		{
			ui->bt_open->setText("关闭串口");
		}
	}
	else
	{
		uart->close();
		ui->bt_open->setText("打开串口");
	}
}
void MainWindow::slot_write_cfg()
{
	string s1,s2;
	float d1,d2;
	s1=ui->le_m_kx->text().toStdString();
	s2=ui->le_m_0_x->text().toStdString();
	if(sscanf(s1.c_str(),"%f",&d1)==1 &&
		sscanf(s2.c_str(),"%f",&d2)==1)
	{
		ahrs_tx_cmd(1,d1*10000,d2*10000);
	}
	s1=ui->le_m_ky->text().toStdString();
	s2=ui->le_m_0_y->text().toStdString();
	if(sscanf(s1.c_str(),"%f",&d1)==1 &&
		sscanf(s2.c_str(),"%f",&d2)==1)
	{
		ahrs_tx_cmd(2,d1*10000,d2*10000);
	}
	s1=ui->le_m_kz->text().toStdString();
	s2=ui->le_m_0_z->text().toStdString();
	if(sscanf(s1.c_str(),"%f",&d1)==1 &&
		sscanf(s2.c_str(),"%f",&d2)==1)
	{
		ahrs_tx_cmd(3,d1*10000,d2*10000);
	}
	s1=ui->le_g_0_x->text().toStdString();
	if(sscanf(s1.c_str(),"%f",&d1)==1)
	{
		ahrs_tx_cmd(9,d1*1000,0);
	}
	s1=ui->le_g_0_y->text().toStdString();
	if(sscanf(s1.c_str(),"%f",&d1)==1)
	{
		ahrs_tx_cmd(10,d1*1000,0);
	}
	s1=ui->le_g_0_z->text().toStdString();
	if(sscanf(s1.c_str(),"%f",&d1)==1)
	{
		ahrs_tx_cmd(11,d1*1000,0);
	}
	int d;
	s1=ui->le_fre_1->text().toStdString();
	if(sscanf(s1.c_str(),"%d",&d)==1)
	{
		ahrs_tx_cmd(4,d,0);
	}
	s1=ui->le_fre_2->text().toStdString();
	if(sscanf(s1.c_str(),"%d",&d)==1)
	{
		ahrs_tx_cmd(5,d,0);
	}
	ahrs_tx_cmd(7,ui->cbox_9axis->isChecked()?0:1,0);
	ahrs_tx_cmd(8,ui->cb_set_baud->currentIndex(),0);
}
void MainWindow::slot_dft_cfg()
{
	ahrs_tx_cmd(254,0,0);
}
void MainWindow::slot_save_cfg()
{
	ahrs_tx_cmd(255,0,0);
}
void MainWindow::slot_read_cfg()
{
	ahrs_tx_cmd(0,0,0);
}
void MainWindow::slot_cali_g()
{
	ahrs_tx_cmd(6,2,0);
}
void MainWindow::slot_cali_m() //矫正磁力计
{
	if(!ui->bt_cali_m->isChecked())
	{ //完成校准
		int r=ahrs_cali_m_stop();
		QString s;
		switch(r)
		{
		case 0:
			s.sprintf("%.2f",cali_m_k.x()); ui->le_m_kx->setText(s);
			s.sprintf("%.2f",cali_m_0.x()); ui->le_m_0_x->setText(s);
			s.sprintf("%.2f",cali_m_k.y()); ui->le_m_ky->setText(s);
			s.sprintf("%.2f",cali_m_0.y()); ui->le_m_0_y->setText(s);
			s.sprintf("%.2f",cali_m_k.z()); ui->le_m_kz->setText(s);
			s.sprintf("%.2f",cali_m_0.z()); ui->le_m_0_z->setText(s);
			QMessageBox::information(this,"success","完成校准");
			break;
		case 1:
			QMessageBox::information(this,"error","校准点过少");
			break;
		default:
			break;
		}
		cali_m_stat=false;
		chart0->axisY()->setRange(-180, 180);
	}
	else //开始校准
	{
		chart_serial_0->clear();
		chart_serial_1->clear();
		chart_serial_2->clear();
		chart0->axisX()->setRange(-1,3);
		chart0->axisY()->setRange(-1,1);
		chart_cali_m->clear();
		ahrs_cali_m_start();
		cali_m_stat=true;
	}
}

