#include "pharmacycheck.h"

#include "connectsql.h"
#include "clinicdrugfilter.h"
#include "package.h"

static int icount;
extern ConnectSql sql;
PharmacyCheck::PharmacyCheck(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initUI();
	ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: grey;}");
	ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	
	ui.tableWidget->installEventFilter(this);//注册事件过滤器
	

	list_widget = new QListWidget(this);
	list_widget->setWindowFlags(Qt::FramelessWindowHint);
	list_widget->close();
	installEventFilter(list_widget);

	connect(ui.tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(getItem(int,int)));
}

PharmacyCheck::~PharmacyCheck()
{

}

void PharmacyCheck::initUI()
{
	iRow=0;
	QSqlQuery query(*sql.db);		
	QDateTime current_date_time = QDateTime::currentDateTime();
	ui.dateTimeEdit->setDateTime(current_date_time);
	ui.saveButton->setEnabled(false);
	ui.discardButton->setEnabled(false);
	ui.editButton->setEnabled(false);
	ui.deleteButton->setEnabled(false);
	ui.addRowButton->setEnabled(false);
	ui.deleteRowButton->setEnabled(false);
	//ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("gb18030"));
//	item(i, colunm)->setFlags(Qt::NoItemFlags);
	//ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
	ui.lineEdit_PreProfit->setText("0");
	ui.lineEdit_CurProfit->setText("0");
	ui.lineEdit_AftProfit->setText("0");
	int rows = ui.tableWidget->model()->rowCount();   //行总数
	iRow=rows-1;
}
void PharmacyCheck::edit(QString strNo)
{
	ui.tableWidget->installEventFilter(this);
	SetEdit(false);
	ui.addButton->setEnabled(false);
	ui.editButton->setEnabled(true);
	QSqlQuery query(*sql.db);	
	query.exec("select * from yf_check where sheetno='"+strNo+"'");
	while(query.next())
	{
		ui.lineEdit_CheckNo->setText(query.value(1).toString());
		ui.dateTimeEdit->setDateTime(query.value(2).toDateTime());
		
		ui.lineEdit_Remark->setText(query.value(4).toString());		
	}
	ui.tableWidget->setRowCount(0);
	QSqlQuery query2(*sql.db);	
	query2.exec("select * from yf_check where sheetno='"+strNo+"'");
	int row= 0;
	while(query2.next())
	{
		int Num;
		ui.tableWidget->insertRow(row);
		//ui.tableWidget->setItem(row,1,new QTableWidgetItem(query2.value(4).toString()));
		QString strName=query2.value(6).toString();
		QString strsql= "select * from sys_drugdictionary where name='"+strName+"'";
		query.exec(strsql);
		while(query.next())
		{
			ui.tableWidget->setItem(row,1,new QTableWidgetItem(query.value(1).toString()));
			ui.tableWidget->setItem(row,2,new QTableWidgetItem(query.value(4).toString()));
			ui.tableWidget->setItem(row,3,new QTableWidgetItem(query.value(5).toString()));
			ui.tableWidget->setItem(row,5,new QTableWidgetItem(query.value(6).toString()));
			
			ui.tableWidget->setItem(row,9,new QTableWidgetItem(query.value(15).toString()));//
			//Num = query.value(10).toString().toInt();
		}
		ui.tableWidget->setItem(row,6,new QTableWidgetItem(query2.value(12).toString()));
		ui.tableWidget->setItem(row,7,new QTableWidgetItem(query2.value(13).toString()));//
		//strsql= "select * from yk_inventory where name='"+strName+"'";
		//query.exec(strsql);
		//if(query.next())
		//{
		//	int kucun1 = query2.value(7).toString().toInt();
		//	kucun1 = kucun1 - Num;
		//	QString strTemp = QString::number(kucun1);
		//	ui.tableWidget->setItem(row,7,new QTableWidgetItem(strTemp));  //有问题
		//}
		//ui.tableWidget->setItem(row,6,new QTableWidgetItem(query2.value(13).toString()));
		row++;
	}
}
void PharmacyCheck::SetEdit(bool IsEdit)
{
	if (IsEdit==true)
	{
		ui.lineEdit_CheckNo->setEnabled(true);
		ui.tableWidget->setEnabled(true);
		ui.lineEdit_PreProfit->setEnabled(true);
		ui.lineEdit_CurProfit->setEnabled(true);
		ui.lineEdit_AftProfit->setEnabled(true);
	}
	else
	{
		ui.lineEdit_CheckNo->setEnabled(false);
		ui.tableWidget->setEnabled(false);
		ui.lineEdit_PreProfit->setEnabled(false);
		ui.lineEdit_CurProfit->setEnabled(false);
		ui.lineEdit_AftProfit->setEnabled(false);
	}
}

void PharmacyCheck::on_editButton_clicked()
{
	SetEdit(true);
	ui.deleteButton->setEnabled(true);
	ui.saveButton->setEnabled(true);
	ui.discardButton->setEnabled(true);
	ui.addRowButton->setEnabled(true);
	ui.deleteRowButton->setEnabled(true);
}

void PharmacyCheck::on_deleteButton_clicked()//存在问题
{
	int i=0,amount;
	int rows = ui.tableWidget->model()->rowCount();   //行总数
		
	int ok = QMessageBox::warning(this,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("确认删除本单？"),QMessageBox::Yes,QMessageBox::No);
	if(ok == QMessageBox::Yes)
	{
		QSqlQuery query(*sql.db);		
		QString strSheetNo=ui.lineEdit_CheckNo->text();
		QString strsql = "delete from yf_check where sheetno ='"+strSheetNo+"'"; //where ID > 1
		query.exec(strsql);
		strsql = "delete from yf_check_public_info where sheetno ='"+strSheetNo+"'"; //where ID > 1
		query.exec(strsql);
		for (i=0;i< rows ;i++)
		{
			QString strName = ui.tableWidget->item(i,1)->text();	
			int kucun = ui.tableWidget->item(i,6)->text().toInt();

			strsql= "select * from yf_inventory where name ='"+strName+"'";//;//where AbbrName = '"+strName+"'
			query.exec(strsql);

			if(query.next())
			{
				query.prepare("UPDATE yf_inventory SET count= ? WHERE name = '"+strName+"'");
				query.bindValue(0,kucun);
				query.exec();
			}
		}
		//QSqlQuery query(*sql.db);		
		//QString strSheetNo=ui.lineEdit_CheckNo->text();
		//QString strsql = "delete from yf_check where sheetno ='"+strSheetNo+"'"; //where ID > 1
		//query.exec(strsql);

		//strsql = "delete from yf_check where sheetno ='"+strSheetNo+"'"; //where ID > 1
		//query.exec(strsql);
		ui.lineEdit_CheckNo->setText("");
		ui.lineEdit_PreProfit->setText("0");
		ui.lineEdit_CurProfit->setText("0");
		ui.lineEdit_AftProfit->setText("0");
		ui.tableWidget->clearContents();
	}
}

void PharmacyCheck::on_addButton_clicked()
{
	SetEdit(true);
	ui.saveButton->setEnabled(true);
	ui.discardButton->setEnabled(true);
	ui.editButton->setEnabled(false);
	ui.deleteButton->setEnabled(false);
	ui.addRowButton->setEnabled(true);
	ui.deleteRowButton->setEnabled(true);
	ui.tableWidget->setRowCount(0);
	ui.tableWidget->clearContents();
	ui.tableWidget->insertRow(0);
	iRow = 0;
	icount = 0;
	int rows = ui.tableWidget->model()->rowCount();   //行总数
	for (int i=0;i<rows;i++)
	{
		if(i!=0)
			ui.tableWidget->removeRow(i);
	}
	int isheetcount=PublicInfoSheetNo();
	isheetcount++;
	QString strSheetNo= "YFPD"+QString::number(isheetcount, 10);
	ui.lineEdit_CheckNo->setText(strSheetNo);
	ui.lineEdit_PreProfit->setText("0");
	ui.lineEdit_CurProfit->setText("0");
	ui.lineEdit_AftProfit->setText("0");

}
//void PharmacyCheck::on_checkButton_clicked()
//{
//	int i;
//	QSqlQuery query(*sql.db);	
//	QDateTime time;
//	QStringList list;
//	QString strName;
//	QString strDate1;
//	QString strDate2;
//	QString strsql= "select * from yf_inventory";
//	query.exec(strsql);
//		
//	int rows = ui.tableWidget->model()->rowCount();   //行总数
//	for (int i=0;i<rows+1;i++)
//	{
//		ui.tableWidget->removeRow(rows - i);
//	}
//	 
//	//time = ui.dateTimeEdit_Begin->dateTime();
//	//strDate1 = time.toString("YYYY-MM-DD THH:MM:SS");
//
//	//time = ui.dateTimeEdit_End->dateTime();
//	//strDate2 = time.toString("YYYY-MM-DD THH:MM:SS");
//	while(query.next())
//	{
//		strName = query.value(2).toString();
//		list.append(strName);
//
//	}
//
//	for(i =0;i<list.size();i++)
//	{
//		int rows = ui.tableWidget->model()->rowCount();   //行总数
//		ui.tableWidget->insertRow(rows);
//		iRow=rows;
//		strName = list[i];
//		QSqlQuery query(*sql.db);	
//	
//		QString strsql= "select * from sys_drugdictionary where name='"+strName+"'";
//		query.exec(strsql);
//
//		if(query.next())
//		{
//			ui.tableWidget->setItem(i,0,new QTableWidgetItem(query.value(2).toString()));
//			ui.tableWidget->setItem(i,1,new QTableWidgetItem(query.value(1).toString()));
//			ui.tableWidget->setItem(i,2,new QTableWidgetItem(query.value(4).toString()));
//			ui.tableWidget->setItem(i,3,new QTableWidgetItem(query.value(5).toString()));
//			ui.tableWidget->setItem(i,5,new QTableWidgetItem(query.value(6).toString()));
//		}
//		strsql= "select * from yf_inventory where name='"+strName+"'";//;//where AbbrName = '"+strName+"'
//		query.exec(strsql);
//					
//		if(query.next())
//		{
//			ui.tableWidget->setItem(i,6,new QTableWidgetItem(query.value(7).toString()));
//		}
//
//		strsql= "select * from yf_receipt where name='"+strName+"'";//;//where AbbrName = '"+strName+"'
//		query.exec(strsql);
//					
//		if(query.next())
//		{
//			ui.tableWidget->setItem(i,9,new QTableWidgetItem(query.value(12).toString()));
//		}
//	}
//	
//	//for(i =0;i<list.size();i++)
//	//{	
//	//	int preBooknum =0;
//	//	int preInventorynum =0;
//	//	strName = list[i];
//	//	strsql= "select * from yk_check where name='"+strName+"'and Date =(select max(Date) from yk_check where name='"+strName+"')";
//	//	query.exec(strsql);
//	//	while(query.next())
//	//	{
//	//		preBooknum = query.value(12).toInt();
//	//		preInventorynum = query.value(13).toInt();
//	//	}
//	//	
//	//	//查询该药品的入库信息
//	//	int instorage_num = 0;
//	//	int total_instorage_num = 0;
//
//	//	strName = list[i];
//	//	strsql= "select * from yk_instorage where name='"+strName+"'and Date between '"+strDate1+"' and '"+strDate1+"'";
//	//	query.exec(strsql);
//	//	while(query.next())
//	//	{
//	//		instorage_num = query.value(13).toInt();
//	//		total_instorage_num = total_instorage_num + instorage_num;
//	//	}
//	//	//查询该药品的销售调拨信息
//	//	int saleallot_num = 0;
//	//	int total_saleallot_num = 0;
//
//	//	strName = list[i];
//	//	strsql= "select * from yk_saleallot where name='"+strName+"'and Date between '"+strDate1+"' and '"+strDate1+"'";
//	//	query.exec(strsql);
//	//	while(query.next())
//	//	{
//	//		saleallot_num = query.value(13).toInt();
//	//		total_saleallot_num = total_saleallot_num + saleallot_num;
//	//	}
//
//	//	//查询该药品的仓库调拨信息
//	//	int warehouseallot_num = 0;
//	//	int total_warehouseallot_num = 0;
//
//	//	strName = list[i];
//	//	strsql= "select * from yk_warehouseallot where name='"+strName+"'and Date between '"+strDate1+"' and '"+strDate1+"'";
//	//	query.exec(strsql);
//	//	while(query.next())
//	//	{
//	//		warehouseallot_num = query.value(13).toInt();
//	//		total_warehouseallot_num = warehouseallot_num + warehouseallot_num;
//	//	}
//
//	//	int BookCount =preBooknum + total_instorage_num - total_saleallot_num - total_warehouseallot_num; //盘点数量
//	//	int inventory_num = 0;
//	//	strName = list[i];
//	//	strsql= "select * from yk_inventory where name='"+strName+"'";
//	//	query.exec(strsql);
//	//	while(query.next())
//	//	{
//	//		inventory_num = preInventorynum + query.value(7).toInt() ;                  //账面数量
//	//	}
//
// //
//	//}
//
//}
double PharmacyCheck::TotalPrice()
{
	int i;
	int rows = ui.tableWidget->model()->rowCount();
	double icurrentamount = 0.0;

	for(i=0;i<rows;i++)
	{
		if(ui.tableWidget->item(i,7)==NULL||(ui.tableWidget->item(i,7)&& ui.tableWidget->item(i,7)->text()==tr("")))
			continue;
		//int icount = ui.tableWidget->item(i,6)->text().toInt();
		//double unitprice = ui.tableWidget->item(i,6+2)->text().toDouble();
		//double amount = icount*unitprice;
		//QString stramount = QString::number(amount);
		//if(ui.radioButton_Add->isChecked() && !ui.radioButton_Minus->isChecked())
		double amount = ui.tableWidget->item(i,10)->text().toDouble();
			icurrentamount+=amount;
		//if(!ui.radioButton_Add->isChecked() && ui.radioButton_Minus->isChecked())
			//icurrentamount-=amount;
	}
	return icurrentamount;
}
void PharmacyCheck::on_addRowButton_clicked()
{
	//表格增加一行
	int rows = ui.tableWidget->model()->rowCount();   //行总数
	ui.tableWidget->insertRow(rows);
	iRow=rows;
}
void PharmacyCheck::on_deleteRowButton_clicked()
{
	int rows = ui.tableWidget->currentRow(); //行总数
	if(ui.tableWidget->item(rows,7)==NULL||(ui.tableWidget->item(rows,7)&& ui.tableWidget->item(rows,7)->text()==tr("")))
	{	
		ui.tableWidget->removeRow(rows);
		iRow--;
		return;
	}

	ui.tableWidget->removeRow(rows);
	iRow--;

	double CurProfit = TotalPrice();
//		CurProfit = CurProfit + profit;
	QString strprofit = QString::number(CurProfit);
	ui.lineEdit_CurProfit->setText(strprofit);
		
	double PreProfit = ui.lineEdit_PreProfit->text().toDouble();
	double AftProfit = PreProfit + CurProfit;
	strprofit = QString::number(AftProfit);
	ui.lineEdit_AftProfit->setText(strprofit);


}
void PharmacyCheck::getItem(int row,int column)//计算费用
{
	QString str2;
	QString str = str.fromLocal8Bit("警告");
	if (column==7)
	{
		if (ui.tableWidget->item(row,column)->text()==NULL) return;
	//	if (ui.tableWidget->item(row,column+1)->text()==NULL) return;

		int ibookcount = ui.tableWidget->item(row,column)->text().toInt();
		int iinventorycount = ui.tableWidget->item(row,column-1)->text().toInt();
		if(ibookcount<0)
		{
			str2 = str.fromLocal8Bit("盘点数量不能为负，请核对！");
			QMessageBox::warning(this,str,str2,QMessageBox::Ok);
			QString stramount = QString::number(0);
			ui.tableWidget->setItem(row,column,new QTableWidgetItem(stramount));
			return;
		}
		double unitprice = ui.tableWidget->item(row,column+2)->text().toDouble();
		int ierror = ibookcount - iinventorycount;
		QString strerror = QString::number(ierror);
		ui.tableWidget->setItem(row,column+1,new QTableWidgetItem(strerror));
		double profit = unitprice*ierror;
		QString strprofit = QString::number(profit);
		ui.tableWidget->setItem(row,column+3,new QTableWidgetItem(strprofit));

		double CurProfit = TotalPrice();
	//	CurProfit = CurProfit + profit;
		strprofit = QString::number(CurProfit);
		ui.lineEdit_CurProfit->setText(strprofit);
		
		double PreProfit = ui.lineEdit_PreProfit->text().toDouble();
		double AftProfit = PreProfit + CurProfit;
		strprofit = QString::number(AftProfit);
		ui.lineEdit_AftProfit->setText(strprofit);
	}
	if(column==0)
	{

		list_widget->close();
		QString strText;
		if(ui.tableWidget->item(row,0)==NULL)  return;

		strText =  ui.tableWidget->item(row,0)->text();
		if(strText.at(0)== QChar('1')) return;

		list_widget->setGeometry(103, 160+row*30, 150, 280);
		list_widget->show();
		QSqlQuery query(*sql.db);	
		strText =  ui.tableWidget->item(row,0)->text();
		QString strsql= "select * from sys_drugdictionary where abbr = '"+strText+"'";//;//where AbbrName = '"+strName+"'

		query.exec(strsql);
		QStringList list;
		list_widget->clear();
		while(query.next())
		{
			QString str = query.value(1).toString();
			list.append(str);
		}
		list_widget->addItems(list);
	}



}
void PharmacyCheck::on_saveButton_clicked()
{
	static int iFlag;
	int i=0;
	int amount;
	double unitprice;
	int rows = ui.tableWidget->model()->rowCount();   //行总数
	QString str2;
	QString str = str.fromLocal8Bit("警告");
	char strtemp[255]={0};
	//添加到收费单基本信息表中
	QSqlQuery query(*sql.db);

			
	QString strSheetNo=ui.lineEdit_CheckNo->text();
	QString strsql = "select * from yf_check where sheetno ='"+strSheetNo+"'"; //where ID > 1
	query.exec(strsql);
	if(query.next())
	{
		strsql = "delete from yf_check where sheetno ='"+strSheetNo+"'"; //where ID > 1
		query.exec(strsql);
		strsql = "delete from yf_check_public_info where sheetno ='"+strSheetNo+"'"; //where ID > 1
		query.exec(strsql);
	}


	int isheetcount=SheetNo();
		isheetcount++;
	query.prepare("INSERT INTO yf_check_public_info  VALUES(?, ?, ?, ?, ?)");
	query.bindValue(0, isheetcount);
	query.bindValue(1, ui.lineEdit_CheckNo->text());
	query.bindValue(2, ui.dateTimeEdit->dateTime());
	query.bindValue(4, ui.lineEdit_Remark->text());
	iFlag = query.exec();

	for (i=0;i < rows ;i++)
	{
		isheetcount=SheetNo();
		isheetcount++;
		if(ui.tableWidget->item(i,7)==NULL||(ui.tableWidget->item(i,7)&& ui.tableWidget->item(i,7)->text()==tr("")))
		{
			sprintf(strtemp,"第%d行盘点数量为空，请核对！",i+1);
			str2 = str.fromLocal8Bit(strtemp);		
			QMessageBox::warning(this,str,str2,QMessageBox::Ok);
			return;
		}
		query.prepare("INSERT INTO yf_check  VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		query.bindValue(0, isheetcount);
		query.bindValue(1, ui.lineEdit_CheckNo->text());
		query.bindValue(2, ui.dateTimeEdit->dateTime());
		query.bindValue(5, ui.tableWidget->item(i,0)->text());
		query.bindValue(6, ui.tableWidget->item(i,1)->text());
		query.bindValue(7, ui.tableWidget->item(i,2)->text());
		query.bindValue(8, ui.tableWidget->item(i,3)->text());
		query.bindValue(11, ui.tableWidget->item(i,5)->text());
		int temp;
		temp = ui.tableWidget->item(i,6)->text().toInt();
		query.bindValue(12,temp);
		temp = ui.tableWidget->item(i,7)->text().toInt();
		query.bindValue(13, temp);
		temp = ui.tableWidget->item(i,8)->text().toInt();
		query.bindValue(14, temp);
		double dtemp;
		dtemp = ui.tableWidget->item(i,9)->text().toDouble();
		query.bindValue(15, dtemp);
		dtemp = ui.tableWidget->item(i,10)->text().toDouble();
		query.bindValue(16, dtemp);
		
		iFlag = query.exec();

		QString strName = ui.tableWidget->item(i,1)->text();
		int kucun = ui.tableWidget->item(i,7)->text().toInt();


	/*	QString strTemp1 = QString::number(kucun);
		ui.tableWidget->setItem(i,7,new QTableWidgetItem(strTemp1));*/

		QString strsql= "select * from yf_inventory where name ='"+strName+"'";//;//where AbbrName = '"+strName+"'
		query.exec(strsql);

		if(query.next())
		{
			query.prepare("UPDATE yf_inventory SET count= ? WHERE name = '"+strName+"'");
			query.bindValue(0,kucun);
			iFlag = query.exec();
		}
		else
		{

			query.prepare("INSERT INTO yf_inventory VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
			isheetcount=InventorySheetNo();
			isheetcount ++;
			query.bindValue(0, isheetcount);
			query.bindValue(1, strName);
			query.bindValue(2, strName);
			query.bindValue(3, ui.tableWidget->item(i,2)->text());
			query.bindValue(4, ui.tableWidget->item(i,3)->text());
			query.bindValue(6, ui.tableWidget->item(i,5)->text());
			query.bindValue(7, kucun);
			iFlag = query.exec();
		}
	}
	if(iFlag)
	{
		QString str = str.fromLocal8Bit("提示");
		QString str2 = str.fromLocal8Bit("保存成功！");
		QMessageBox::information(this,str,str2);
		SetEdit(false);
		ui.saveButton->setEnabled(false);
		ui.discardButton->setEnabled(false);
		ui.editButton->setEnabled(true);
		ui.deleteButton->setEnabled(true);
		ui.addRowButton->setEnabled(false);
		ui.deleteRowButton->setEnabled(false);
	}
	else
	{
		QString str = str.fromLocal8Bit("提示");
		QString str2 = str.fromLocal8Bit("保存失败！");
		QMessageBox::information(this,str,str2);
	}
}

int  PharmacyCheck::InventorySheetNo()
{
	QSqlQuery query(*sql.db);		
	query.exec("select * from yf_inventory");
	int isheetcount=0;
	while(query.next())
	{ 
		isheetcount++;
	}
	return isheetcount;
}

void PharmacyCheck::on_discardButton_clicked()
{
	ui.lineEdit_CheckNo->setText("");
	ui.lineEdit_PreProfit->setText("");
	ui.lineEdit_CurProfit->setText("");
	ui.lineEdit_AftProfit->setText("");

	int rows = ui.tableWidget->model()->rowCount();   //行总数
	for (int i=0;i<rows+1;i++)
	{
		ui.tableWidget->removeRow(rows - i);
	}
	
}
void PharmacyCheck::on_printButton_clicked()
{
	QPrinter       printer( QPrinter::HighResolution );
	QPrintDialog   dialog( &printer, this );
	if ( dialog.exec() == QDialog::Accepted ) print( &printer );
}
void PharmacyCheck::on_previewButton_clicked()
{
	filePrintPreview();
}
void PharmacyCheck::filePrintPreview()
{
	// 打印预览对话框
	QPrinter             printer( QPrinter::HighResolution );
	QPrintPreviewDialog  preview( &printer, this );
	preview.setWindowTitle("preview");
	preview.setMinimumSize(1000,900);
	connect( &preview, SIGNAL(paintRequested(QPrinter*)), SLOT(print(QPrinter*)) );
	preview.exec();
}

int  PharmacyCheck::PublicInfoSheetNo()
{
	QSqlQuery query(*sql.db);		
	query.exec("select * from yf_check_public_info");
	int isheetcount=0;
	while(query.next())
	{ 
		isheetcount++;
	}
	return isheetcount;
}
int  PharmacyCheck::SheetNo()
{
	QSqlQuery query(*sql.db);		
	query.exec("select * from yf_check");
	int isheetcount=0;
	while(query.next())
	{ 
		isheetcount++;
	}
	return isheetcount;
}

void PharmacyCheck::print( QPrinter* printer )
{
	//// 创建打印页面的绘制对象
	QPainter painter( printer );
	int      w = printer->pageRect().width();
	int      h = printer->pageRect().height();
	QRect    page( w/5, h/15, w, h );
	QRect    page2( w/4, h/8, w, h );
	QRect    page3( w/4, h/6, w, h );
	QRect    page4( 0, h/10, w, h );
	// 根据页面大小设置适当的字体大小
	QFont    font = painter.font();
	font.setPixelSize( (w+h) / 100 );
	painter.setFont( font );

	// 在页面角落绘制标签
	painter.drawText( page, Qt::AlignTop    | Qt::AlignLeft, QString::fromLocal8Bit("                  三河市燕郊镇卫生院") );
	/*painter.drawText( page2, Qt::AlignTop    | Qt::AlignLeft, QString::fromLocal8Bit("周文军") );
	painter.drawText( page3, Qt::AlignTop    | Qt::AlignLeft, QString::fromLocal8Bit("一般针疗费     次   1  10.00") );
	painter.drawText( page4, Qt::AlignTop | Qt::AlignLeft,  QString::fromLocal8Bit("六十九元九角整"));
	painter.drawText( page, Qt::AlignBottom | Qt::AlignLeft,
		QDateTime::currentDateTime().toString( Qt::DefaultLocaleShortDate ) );*/

	// 绘制模拟数据
	page.adjust( w/20, h/20, -w/20, -h/20 );

	//m_scene->render( &painter, page );
	//表格
	/*	QWidget *myForm=new QWidget(this);
	myForm->setObjectName(QString::fromUtf8("Form"));
	myForm->resize(500, 500);
	QTableWidget *tableWidget;
	tableWidget = new QTableWidget(myForm);
	tableWidget->setColumnCount(3);
	tableWidget->setRowCount(4);
	tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
	tableWidget->setGeometry(QRect(0, 0,500, 500));    
	tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
	"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
	"QHeaderView::section:checked{background-color: red;}");    */       
	QPixmap image;
	image=image.grabWidget(ui.tableWidget,-35,0,900, 1000);
	painter.drawPixmap(page4,image);

	//printTableWidget(ui.tableWidget,"preview",printer);
}
void PharmacyCheck::keyPressEvent(QKeyEvent *e) {
	if (!list_widget->isHidden()) {
		int key = e->key();
		int count = list_widget->model()->rowCount();
		QModelIndex currentIndex = list_widget->currentIndex();

		if (Qt::Key_Down == key) {
			int row = currentIndex.row() + 1;
			if (row >= count) {
				row = 0;
			}

			QModelIndex index = list_widget->model()->index(row, 0);
			list_widget->setCurrentIndex(index);
		} else if (Qt::Key_Up == key) {
			int row = currentIndex.row() - 1;
			if (row < 0) {
				row = count - 1;
			}

			QModelIndex index = list_widget->model()->index(row, 0);
			list_widget->setCurrentIndex(index);
		} else if (Qt::Key_Escape == key) {
			list_widget->hide();
		} else if (Qt::Key_Enter == key || Qt::Key_Return == key) {
			if (currentIndex.isValid()) {
				QString strName = list_widget->currentIndex().data().toString();

				int row = ui.tableWidget->currentRow();
				QSqlQuery query(*sql.db);	

				QString strsql= "select * from sys_drugdictionary where name='"+strName+"'";//;//where AbbrName = '"+strName+"'
				query.exec(strsql);
				while(query.next())
				{
					//ui.tableWidget->setItem(iRow,0,new QTableWidgetItem(query.value(2).toString()));
					ui.tableWidget->setItem(row,1,new QTableWidgetItem(query.value(1).toString()));
					ui.tableWidget->setItem(row,2,new QTableWidgetItem(query.value(4).toString()));
					ui.tableWidget->setItem(row,3,new QTableWidgetItem(query.value(5).toString()));
					ui.tableWidget->setItem(row,5,new QTableWidgetItem(query.value(6).toString()));
					ui.tableWidget->setItem(row,9,new QTableWidgetItem(query.value(15).toString()));
				}
				QString strsql1= "select * from yf_inventory where name='"+strName+"'";//;//where AbbrName = '"+strName+"'
				query.exec(strsql1);

				if(query.next())
				{
					ui.tableWidget->setItem(row,6,new QTableWidgetItem(query.value(7).toString()));
				}
				else 
				{
					QString strTemp = QString::number(0);
					ui.tableWidget->setItem(row,6,new QTableWidgetItem(strTemp));
				}
			}

			list_widget->hide();
		} else {
			list_widget->hide();
			//QLineEdit::keyPressEvent(e);
		}
	} else {
		//QLineEdit::keyPressEvent(e);
	}
}