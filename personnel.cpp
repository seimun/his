#include "personnel.h"
#include "connectsql.h"
#include <QTreeWidgetItem>
#include <QTextCodec>
extern ConnectSql sql;
personnel::personnel(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.treeWidget->setColumnCount(1);

	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: grey;}");
	ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	
	ui.treeWidget->setStyleSheet("QTreeWidget{border: 1px solid gray;color: black;	background-color: white;selection-color: grey;}");
	connect(ui.treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(showTable(QTreeWidgetItem*,int)));
	initUI();
}
void personnel::initUI()
{
	ui.treeWidget->clear();
	ui.tableWidget->setRowCount(0);
	QStringList headers;
	headers << QString::fromLocal8Bit("人事列表");
	ui.treeWidget->setHeaderLabels(headers);

	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

	QString strsql;
	strsql = "select * from sys_personnel order by id";
	QSqlQuery query(*sql.db);	
	query.exec(strsql);
	int row = 0;
	while(query.next())
	{	
		QStringList strlist;
		strlist <<query.value(2).toString();
		QTreeWidgetItem *item = new QTreeWidgetItem(ui.treeWidget, strlist);
		ui.treeWidget->addTopLevelItem(item);
		ui.tableWidget->insertRow(row);
		ui.tableWidget->setItem(row,0,new QTableWidgetItem(query.value(0).toString()));
		ui.tableWidget->setItem(row,1,new QTableWidgetItem(query.value(1).toString()));
		ui.tableWidget->setItem(row,2,new QTableWidgetItem(query.value(2).toString()));
		ui.tableWidget->setItem(row,3,new QTableWidgetItem(query.value(3).toString()));
		ui.tableWidget->setItem(row,4,new QTableWidgetItem(query.value(4).toString()));
		ui.tableWidget->setItem(row,5,new QTableWidgetItem(query.value(5).toString()));
		ui.tableWidget->setItem(row,6,new QTableWidgetItem(query.value(6).toString()));
		ui.tableWidget->setItem(row,7,new QTableWidgetItem(query.value(7).toString()));
		ui.tableWidget->setItem(row,8,new QTableWidgetItem(query.value(8).toString()));
		ui.tableWidget->setItem(row,9,new QTableWidgetItem(query.value(9).toString()));
		ui.tableWidget->setItem(row,10,new QTableWidgetItem(query.value(10).toString()));
		row++;
	}
	ui.treeWidget->expandAll();
	ui.saveButton->setEnabled(false);
	ui.deleteButton->setEnabled(false);
}
void personnel::showTable(QTreeWidgetItem*item, int column)
{
	ui.tableWidget->setRowCount(0);
	QString strText = item->text(0);
	QString strsql;
	strsql = "select * from sys_personnel where name  = '"+strText+"'  order by id";
	QSqlQuery query(*sql.db);	
	query.exec(strsql);
	int row = 0;
	while(query.next())
	{	
		ui.tableWidget->insertRow(row);
		ui.tableWidget->setItem(row,0,new QTableWidgetItem(query.value(0).toString()));
		ui.tableWidget->setItem(row,1,new QTableWidgetItem(query.value(1).toString()));
		ui.tableWidget->setItem(row,2,new QTableWidgetItem(query.value(2).toString()));
		ui.tableWidget->setItem(row,3,new QTableWidgetItem(query.value(3).toString()));
		ui.tableWidget->setItem(row,4,new QTableWidgetItem(query.value(4).toString()));
		ui.tableWidget->setItem(row,5,new QTableWidgetItem(query.value(5).toString()));
		ui.tableWidget->setItem(row,6,new QTableWidgetItem(query.value(6).toString()));
		ui.tableWidget->setItem(row,7,new QTableWidgetItem(query.value(7).toString()));
		ui.tableWidget->setItem(row,8,new QTableWidgetItem(query.value(8).toString()));
		ui.tableWidget->setItem(row,9,new QTableWidgetItem(query.value(9).toString()));
		ui.tableWidget->setItem(row,10,new QTableWidgetItem(query.value(10).toString()));
		row++;
	}
}
void personnel::on_addButton_clicked()
{
	initUI();
	ui.saveButton->setEnabled(true);
	ui.tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
	int row = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(row);
	ui.tableWidget->setItem(row,0,new QTableWidgetItem(QString::number(row+1)));
}
void personnel::on_editButton_clicked()
{
	ui.tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
	ui.saveButton->setEnabled(true);
	ui.editButton->setEnabled(true);
}
void personnel::on_saveButton_clicked()
{

	QSqlQuery query(*sql.db);		
	QString strsql = "delete from sys_personnel"; 
	query.exec(strsql);
	int row = ui.tableWidget->model()->rowCount();
	for (int i= 0;i<row;i++)
	{

		query.prepare("INSERT INTO sys_personnel VALUES (?,?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		query.bindValue(0, ui.tableWidget->item(i,0)->text().toInt());
		for (int j=1;j<7;j++)
		{
			if (ui.tableWidget->item(i,j)==NULL) 
				query.bindValue(j, NULL);
			else
				query.bindValue(j, ui.tableWidget->item(i,j)->text());
		}
		if (ui.tableWidget->item(i,7)==NULL) 
			query.bindValue(7, NULL);
		else
			query.bindValue(7, ui.tableWidget->item(i,7)->text().toInt());
		for (int j=8;j<11;j++)
		{
			if (ui.tableWidget->item(i,j)==NULL) 
				query.bindValue(j, NULL);
			else
				query.bindValue(j, ui.tableWidget->item(i,j)->text());
		}
		query.exec();
	}
	initUI();
	ui.saveButton->setEnabled(false);
	ui.editButton->setEnabled(true);
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
void personnel::on_deleteButton_clicked()
{
	int ok = QMessageBox::warning(this,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("确定删除？"),QMessageBox::Yes,QMessageBox::No);
	if(ok == QMessageBox::Yes)
	{
		QList<QTableWidgetItem*> list =  ui.tableWidget->selectedItems();
		if (list.at(0)==NULL)
		{
			return;
		}
		QString strName = ui.tableWidget->item(list.at(0)->row(),2)->text();
		if (strName==NULL)
		{
			return;
		}
		QSqlQuery query(*sql.db);
		query.exec("delete  from sys_personnel where name = '"+strName+"'");
		initUI();
	}
}
void personnel::on_exitButton_clicked()
{
	int ok = QMessageBox::warning(this,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("是否已保存？"),QMessageBox::Yes,QMessageBox::No);
	if(ok == QMessageBox::Yes)
	{
		this->close();
	}
}
personnel::~personnel()
{

}
