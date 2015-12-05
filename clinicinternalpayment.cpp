#include "clinicinternalpayment.h"
#include <QtGui>
#include "connectsql.h"
#include "odbcexcel.h"
#include "capital.h"
extern ConnectSql sql;
ClinicInternalPayment::ClinicInternalPayment(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);//border: 2px solid gray
	ui.tableWidget->setStyleSheet("QTableWidget{;background-color: transparent;	selection-color: white;}");
	ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	
	initUI();
}
void ClinicInternalPayment::initUI()
{
	QDateTime current_date_time = QDateTime::currentDateTime();
	ui.dateTimeEdit->setDateTime(current_date_time);
	QTime time= QTime::fromString("00:00:00", "hh:mm:ss");
	ui.dateTimeEdit->setTime(time);
	ui.dateTimeEdit_2->setDateTime(current_date_time);
	time= QTime::fromString("23:59:59", "hh:mm:ss");
	ui.dateTimeEdit_2->setTime(time);

}
void ClinicInternalPayment::on_printButton_clicked()
{
	QPrinter       printer( QPrinter::HighResolution );
	QPrintDialog   dialog( &printer, this );
	if ( dialog.exec() == QDialog::Accepted ) print( &printer );
}
void ClinicInternalPayment::on_previewButton_clicked()
{
	filePrintPreview();
}
void ClinicInternalPayment::on_excelButton_clicked()
{
	QAbstractItemModel* model=ui.tableWidget->model();
	if (model->rowCount()==0||model==NULL)
	{
		QString str = str.fromLocal8Bit("��ʾ");
		QString str2 = str.fromLocal8Bit("������");
		QMessageBox::information(this,str,str2);
		return;
	}

	QFileDialog dlg;
	dlg.setAcceptMode(QFileDialog::AcceptSave);

	dlg.setDirectory(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
	dlg.setNameFilter("*.xls");
	dlg.selectFile(QDate::currentDate().toString("yyyy-MM-dd.xls"));
	if(dlg.exec()!= QDialog::Accepted)
		return;
	QString filePath=dlg.selectedFiles()[0];
	if(OdbcExcel::saveFromTable(filePath,ui.tableWidget,"")) {
		QString str = str.fromLocal8Bit("��ʾ");
		QString str2 = str.fromLocal8Bit("����ɹ�");
		QMessageBox::information(this,str,str2);
	}
	else{
		QString str = str.fromLocal8Bit("����");
		QString msg=str.fromLocal8Bit("����ʧ�ܣ�\n\r")+OdbcExcel::getError();
		QMessageBox::critical(this,str,msg);
	}
}
void ClinicInternalPayment::on_queryButton_clicked()
{
	QString startDate = ui.dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss");
	QString endDate = ui.dateTimeEdit_2->dateTime().toString("yyyy-MM-dd hh:mm:ss");

	QDateTime current_date_time = QDateTime::currentDateTime();
	QString strtime = current_date_time.toString("yyyy-MM-dd hh:mm:ss");

	QStringList typelist;
	typelist.append(QString::fromLocal8Bit("01-��ҩ��"));
	typelist.append(QString::fromLocal8Bit("02-�г�ҩ��"));
	typelist.append(QString::fromLocal8Bit("03-�Һŷ�A"));
	typelist.append(QString::fromLocal8Bit("04-����D"));
	typelist.append(QString::fromLocal8Bit("05-���Ʒ�E"));
	typelist.append(QString::fromLocal8Bit("06-������F"));
	typelist.append(QString::fromLocal8Bit("07-������G"));
	typelist.append(QString::fromLocal8Bit("08-�����H"));
	typelist.append(QString::fromLocal8Bit("12-���Ϸ�"));
	typelist.append(QString::fromLocal8Bit("13-����C"));
	typelist.append(QString::fromLocal8Bit("14-������I"));
	typelist.append(QString::fromLocal8Bit("15-��λ��B"));
	typelist.append(QString::fromLocal8Bit("16-�������Ʒ�"));
	QSqlQuery query(*sql.db);
	QString strsql;
	double tempfee=0.0;
	for (int i = 0;i<typelist.count();i++)
	{
		QString strType = typelist.at(i);
		strsql ="select * from mz_chargedetail where itemtype='"+strType+"'and date between  TIMESTAMP '"+startDate+"' and TIMESTAMP '"+endDate+"'";
		query.exec(strsql);
		double totalfee=0.0;
		while(query.next())
		{
			totalfee+=query.value(8).toDouble();	
		}
		tempfee+=totalfee;
		ui.tableWidget->setItem(i,1,new QTableWidgetItem(QString::number(totalfee)));
	}

	ui.tableWidget->setItem(13,1,new QTableWidgetItem(QString::number(tempfee)));


	strsql ="select * from mz_chargesheet where  date between  TIMESTAMP '"+startDate+"' and TIMESTAMP '"+endDate+"'";
	query.exec(strsql);
	int icount=0;
	while(query.next())
	{
		icount++;
	}
	QString strhint = QString::fromLocal8Bit("�����˴�:")+QString::number(icount);
	ui.label_5->setText(strhint);
}
void ClinicInternalPayment::filePrintPreview()
{
	// ��ӡԤ���Ի���
	QPrinter             printer( QPrinter::HighResolution );
	QPrintPreviewDialog  preview( &printer, this );
	preview.setWindowTitle(QString::fromLocal8Bit("Ԥ��"));
	connect( &preview, SIGNAL(paintRequested(QPrinter*)), SLOT(print(QPrinter*)) );
	preview.exec();
}
void ClinicInternalPayment::print( QPrinter* printer )
{
	QPainter painter( printer );
	int      w = printer->pageRect().width();
	int      h = printer->pageRect().height();
	QRect    page( 0, h/15, w, h );
	QRect    page2(0, h/10, w, h );
	QRect    page3( w/4, h/6, w, h );
	QRect    page4( 0, h/10, w, h );
	QFont    font = painter.font();
	font.setPixelSize( (w+h) / 100 );
	painter.setFont( font );
	painter.drawText( page, Qt::AlignTop    | Qt::AlignHCenter, QString::fromLocal8Bit("�������ོ������Ժ�����ڲ��ɿ") );
    QString str =QString::fromLocal8Bit("����:")+ ui.dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss")+"-"+ui.dateTimeEdit_2 ->dateTime().toString("yyyy-MM-dd hh:mm:ss")+ui.label_5->text();
	painter.drawText( page2, Qt::AlignTop    | Qt::AlignHCenter, str );

	painter.begin(this);
	painter.setPen(QPen(Qt::black,4,Qt::SolidLine));//���û�����ʽ 
	painter.setBrush(QBrush(Qt::white,Qt::SolidPattern));//���û�ˢ��ʽ 
	int row = ui.tableWidget->rowCount();
	int col = ui.tableWidget->columnCount();
	double cellwidth = 1600;
	double cellheight = 160;
	double leftmargin = (w-cellwidth*col)/2;
	double upmargin = 800;
	QStringList list;
	for (int j =0;j<col;j++)
	{
		list.append(ui.tableWidget->horizontalHeaderItem(j)->text());
	}
	for (int i=0;i<row;i++)
	{
		for (int j=0;j<col;j++)
		{
			if (ui.tableWidget->item(i,j)==NULL)
			{
				list.append("");
				continue;
			}
			list.append(ui.tableWidget->item(i,j)->text());
		}
	}
	for (int i=0;i<row+1;i++)
	{
		for (int j=0;j<col;j++)
		{
			painter.drawRect(leftmargin+j*cellwidth,upmargin+cellheight*(i+1),cellwidth,cellheight);
			QRect rect(leftmargin+j*cellwidth,upmargin+cellheight*(i+1),cellwidth,cellheight);
			painter.drawText( rect, Qt::AlignVCenter    | Qt::AlignLeft, list.at(i*col+j) );//ui.billtableWidget->item(i,j)->text()
		}
	}
	painter.drawRect(leftmargin,upmargin+cellheight*(row+2),cellwidth*col,cellheight);
	QRect rect(leftmargin,upmargin+cellheight*(row+2),cellwidth*col,cellheight);

	Capital*capital = new Capital;
	if (ui.tableWidget->item(row-1,1)==NULL)
	{
		return;
	}
	double sum = ui.tableWidget->item(row-1,1)->text().toDouble();
	
	painter.drawText( rect, Qt::AlignVCenter    | Qt::AlignLeft, QString::fromLocal8Bit("��д��")+capital->NumToChineseStr(sum));//ui.billtableWidget->item(i,j)->text()
	
	QRect rect2(leftmargin,upmargin+cellheight*(row+3),cellwidth*col,cellheight);
	painter.drawText( rect2, Qt::AlignVCenter    | Qt::AlignLeft, QString::fromLocal8Bit("������ܣ�")+"   "+ QString::fromLocal8Bit("�շѸ����ˣ�")+"   "+ QString::fromLocal8Bit("ҩ�������ˣ�"));//ui.billtableWidget->item(i,j)->text()

	painter.end();

	//QPixmap image;
	//image=image.grabWidget(ui.tableWidget,-200,0,900, 1000);
	//painter.drawPixmap(page4,image);
}
ClinicInternalPayment::~ClinicInternalPayment()
{

}