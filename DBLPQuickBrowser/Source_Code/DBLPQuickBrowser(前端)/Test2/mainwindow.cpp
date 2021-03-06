#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qstring.h"
#include <fstream>
#include <string>
#include <set>
#include "qset.h"
#include "qfile.h"
#include "analysedialog.h"
#include "allauthordialog.h"
#include "netmapdialog.h"
using namespace std;
//主窗口-代码定义
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->inputName, SIGNAL(returnPressed()), ui->pushButton,
            SIGNAL(clicked()), Qt::UniqueConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

static string DBLP_URL="G:\\dblp\\src\\";
bool abandonflag;//禁止标志，防止字符串过少
int viewmore_activated;//更多浏览标记，记录流指针指向行数
bool showmore_signal;//更多浏览标志，让用户读取后面的搜索词条

void MainWindow::on_pushButton_clicked()//点击“搜索”按钮，触发事件
{   ui->ShowText->clear();
    ui->listWidget->clear();
    viewmore_activated=0;
    showmore_signal=0;
    QString TempInput=ui->inputName->text();
    if(ui->radioButton->isChecked())this->show_Publication(TempInput.toStdString());
    if(ui->radioButton_2->isChecked())this->Show_CoauthorReader_main(TempInput.toStdString());
    if(ui->radioButton_3->isChecked())this->Show_CoauthorReader_author(TempInput.toStdString());
    if(ui->radioButton_4->isChecked())this->Show_FuzzySearch();
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)//点击列表元素
{

QString jointer = index.data().toString();
if (jointer=="[More Information]")
{
    ui->listWidget->takeItem(index.row());
    if(ui->radioButton_2->isChecked())this->Show_CoauthorReader_main(ui->inputName->text().toStdString());
    if(ui->radioButton_3->isChecked())this->Show_CoauthorReader_author(ui->inputName->text().toStdString());
    if(ui->radioButton_4->isChecked())this->Show_FuzzySearch();

}
ui->ShowText->clear();
ui->ShowText->append(jointer);
}

long long stolongl(string str) //string转long long
{
    long long result;
    stringstream is(str);
    is >> result;
    return result;
}

void transformer_Filename(string &tmp_nickname)//nickname特殊字符变换
{if(tmp_nickname.length()<3){
         QMessageBox::information(NULL, MainWindow::tr("Warning!"), MainWindow::tr("An abortion has been activated.Please make sure you have entered 3 words at least!"));
        //QMessageBox::information(NULL, tr("Warning"), tr("An abortion has been activated!Please make sure you have entered 3 words at least."));
    abandonflag=1;
    }
        if ((_stricmp(tmp_nickname.c_str(), "CON") == 0) || (_stricmp(tmp_nickname.c_str(), "PRN") == 0) ||
        (_stricmp(tmp_nickname.c_str(), "AUX") == 0) || (_stricmp(tmp_nickname.c_str(), "NUL") == 0))
        tmp_nickname += "_a";//特殊命名处理
    if (tmp_nickname.find(".") != string::npos)tmp_nickname.replace(tmp_nickname.find("."), 1, "_b");
    if (tmp_nickname.find("/") != string::npos)tmp_nickname.replace(tmp_nickname.find("/"), 1, "_c");
    if (tmp_nickname.find("\\") != string::npos)tmp_nickname.replace(tmp_nickname.find("\\"), 1, "_d");
    if (tmp_nickname.find("<") != string::npos)tmp_nickname.replace(tmp_nickname.find("<"), 1, "_e");
    if (tmp_nickname.find(">") != string::npos)tmp_nickname.replace(tmp_nickname.find(">"), 1, "_f");
    if (tmp_nickname.find("\"") != string::npos)tmp_nickname.replace(tmp_nickname.find("\""), 1, "_g");
    if (tmp_nickname.find(":") != string::npos)tmp_nickname.replace(tmp_nickname.find(":"), 1, "_h");
    if (tmp_nickname.find("|") != string::npos)tmp_nickname.replace(tmp_nickname.find("|"), 1, "_i");
    if (tmp_nickname.find("*") != string::npos)tmp_nickname.replace(tmp_nickname.find("*"), 1, "_j");
    if (tmp_nickname.find("?") != string::npos)tmp_nickname.replace(tmp_nickname.find("?"), 1, "_k");
    if (tmp_nickname.find(" ") != string::npos)tmp_nickname.replace(tmp_nickname.find(" "), 1, "_l");
    if (tmp_nickname.find("$") != string::npos)tmp_nickname.replace(tmp_nickname.find("$"), 1, "_m");
}

bool check_TailElem(string elemstr)//尾节点判定
{
    if ((elemstr.find("</article>") != string::npos) || (elemstr.find("</inproceedings>") != string::npos) ||
        (elemstr.find("</proceedings>") != string::npos) || (elemstr.find("</book>") != string::npos) ||
        (elemstr.find("</incollection>") != string::npos) || (elemstr.find("</phdthesis>") != string::npos) ||
        (elemstr.find("</mastersthesis>") != string::npos) || (elemstr.find("</www>") != string::npos))
        return true;
    else return false;
}

bool check_PrimaryElemName(string elemstr)//一级节点判定
{
    if ((elemstr.find("article") != string::npos) || (elemstr.find("inproceedings") != string::npos) ||
        (elemstr.find("proceedings") != string::npos) || (elemstr.find("book") != string::npos) ||
        (elemstr.find("incollection") != string::npos) || (elemstr.find("phdthesis") != string::npos) ||
        (elemstr.find("mastersthesis") != string::npos) || (elemstr.find("www") != string::npos))
        return true;
    else return false;
}

void MainWindow::Generate_PublicationList
(vector<string> Publicationinfo,vector<string> Publicationnode)//生成Publication列表
{
    int actioncounter=0;
    vector <string>::iterator PLiter=Publicationinfo.begin();
     vector <string>::iterator PLnodeiter=Publicationnode.begin();
    while (PLiter != Publicationinfo.end())
    {
        QString qstr;
        QString nodestr;
        nodestr=QString::fromStdString(*PLnodeiter);
        qstr = QString::fromStdString(*PLiter);
        if(check_TailElem(nodestr.toStdString()))break;
        ui->listWidget->addItem(new QListWidgetItem(QIcon(":/images/info2"),nodestr));
        ui->listWidget->addItem(new QListWidgetItem("      "+qstr));
        ++PLiter;
        ++PLnodeiter;
       actioncounter++;
    }

     ui->ShowText->append("Total number:"+QString::number(actioncounter));
     ui->ShowText->append("Process finished!");
}

void MainWindow::show_Publication(string search_name)//展示Publication主函数
{
    ui->ShowText->clear();
    long long div_finder=0;
    abandonflag=0;
    string tmp_nickname=search_name.substr(0,3);
    transformer_Filename(tmp_nickname);
    if(abandonflag==1){return;}
    //ifstream infile("G:\\dblp\\src\\publication\\" + tmp_nickname + ".txt", ios::in);
    ifstream infile(DBLP_URL+"publication\\" + tmp_nickname + ".txt", ios::in);

    getline(infile, tempstr);
    while (!infile.eof()) {
        if (tempstr.find(search_name) != string::npos)
        {
            size_t startPos = tempstr.find_last_of("=")+1;//urlpt="中，获取=的位置后+1，即“位置
            size_t endPos = tempstr.find_last_of("\"");//最后的”位置
            div_finder=stolongl(tempstr.substr(startPos + 1, endPos - (startPos + 1)));
            break;
        }
        getline(infile, tempstr);
    }
    infile.close();
    if (div_finder != 0)
    {
        vector<string> Publicationnode;
        vector<string> Publicationinfo;
        //ifstream indblp("G:\\dblp\\src\\dblp.xml", ios::in| ios::binary);
        ifstream indblp(DBLP_URL+"dblp.xml", ios::in| ios::binary);

        indblp.seekg(div_finder,ios::beg);
        getline(indblp, tempstr);
        ui->ShowText->append( QString::fromStdString(tempstr));//article行
        getline(indblp, tempstr);
        while (!check_TailElem(tempstr)) {//测试用
            ui->ShowText->append( QString::fromStdString(tempstr));
            size_t startPos2 = tempstr.find_first_of("<");
            size_t endPos2 = tempstr.find_first_of(">");
            string tmp2=tempstr.substr(startPos2 + 1, endPos2 - (startPos2+ 1));
            if(check_PrimaryElemName(tmp2))continue;
            Publicationnode.push_back(tmp2);

            size_t startPos = tempstr.find_first_of(">");
            size_t endPos = tempstr.find_last_of("<");
            string tmp=tempstr.substr(startPos + 1, endPos - (startPos + 1));
            Publicationinfo.push_back(tmp);
            getline(indblp, tempstr);

        }
        Generate_PublicationList(Publicationinfo,Publicationnode);
    }
}

void MainWindow::Show_CoauthorReader_main(string search_name)//展示作者所有文章信息
{
    div_finder=0;
    abandonflag=0;
    string tmp_nickname = search_name.substr(0, 3);
    transformer_Filename(tmp_nickname);
    if(abandonflag==1){return;}
    //ifstream infile("G:\\dblp\\src\\coauthor\\" + tmp_nickname + ".txt", ios::in);
    ifstream infile(DBLP_URL+"coauthor\\" + tmp_nickname + ".txt", ios::in);
    if(!infile){
        QMessageBox::information(NULL, MainWindow::tr("Warning!"), MainWindow::tr("Cannot find such author."));
        return;
    }
    if(viewmore_activated>0){
        for(int i=0;i<viewmore_activated;i++)getline(infile, tempstr);
    };
    getline(infile, tempstr);
    int counter=0;
    while (!infile.eof()) {
        if (tempstr.find(search_name) != string::npos)
        {
            size_t startPos = tempstr.find_last_of("=") + 1;//urlpt="中，获取=的位置后+1，即“位置
            size_t endPos = tempstr.find_last_of("\"");//最后的”位置
            div_finder = stolongl(tempstr.substr(startPos + 1, endPos - (startPos + 1)));

            size_t pub_startPos = tempstr.find("pname=") + 6;
            size_t pub_endPos = tempstr.find("\" urlpt");
            string pub_name = tempstr.substr(pub_startPos + 1, pub_endPos - (pub_startPos + 1));
            ui->listWidget->addItem(new QListWidgetItem(QIcon(":/images/book"),"publication"));
            ui->listWidget->addItem(new QListWidgetItem("      "+QString::fromStdString(pub_name)));
            counter++;
            if(counter==50){viewmore_activated+=50;showmore_signal=1; break;}
        }
        getline(infile, tempstr);
    }
    if(showmore_signal==1)ui->listWidget->addItem(new QListWidgetItem(QIcon(":/images/more"),"[More Information]"));
    infile.close();
}

void MainWindow::findAuthor_inDBLP()//在DBLP中检索作者
{

    if (div_finder != 0)
    {
        //ifstream indblp("G:\\dblp\\src\\dblp.xml", ios::in | ios::binary);
        ifstream indblp(DBLP_URL+"dblp.xml", ios::in | ios::binary);
        indblp.seekg(div_finder, ios::beg);
        while (!check_TailElem(tempstr)) {//测试用
            getline(indblp, tempstr);
            if (tempstr.find("<author>") != string::npos) {
                size_t startPos = tempstr.find_first_of(">");
                size_t endPos = tempstr.find_last_of("<");
                coauthor_tree.insert(tempstr.substr(startPos + 1, endPos - (startPos + 1)));
            }
        }
    }
}

void MainWindow::Probe_RBTree()//遍历红黑树
{
        set<string>::iterator MJiter = coauthor_tree.begin();//迭代器，用于遍历整颗红黑树
        while (MJiter != coauthor_tree.end())
        {
            ui->listWidget->addItem(new QListWidgetItem(QIcon(":/images/people"),"author"));
            ui->listWidget->addItem(new QListWidgetItem("      "+QString::fromStdString(*MJiter)));
            ++MJiter;

        }
        if(showmore_signal==1)ui->listWidget->addItem(new QListWidgetItem(QIcon(":/images/more"),"[More Information]"));

}

void MainWindow::Show_CoauthorReader_author(string search_name)//展示所有合作作者
{
    div_finder = 0;
    abandonflag=0;
    string tmp_nickname = search_name.substr(0, 3);
    transformer_Filename(tmp_nickname);
    if(abandonflag==1){return;}


    //ifstream infile("G:\\dblp\\src\\coauthor\\" + tmp_nickname + ".txt", ios::in);
    ifstream infile(DBLP_URL+"coauthor\\" + tmp_nickname + ".txt", ios::in);
    if(!infile){
        QMessageBox::information(NULL, MainWindow::tr("Warning!"), MainWindow::tr("Cannot find such author."));
        return;
    }
    int counter=0;
    if(viewmore_activated>0){
        for(int i=0;i<viewmore_activated;i++)getline(infile, tempstr);
        coauthor_tree.clear();
    };
    getline(infile, tempstr);
    while (!infile.eof()) {
        if (tempstr.find(search_name) != string::npos)
        {
            size_t startPos = tempstr.find_last_of("=") + 1;//urlpt="中，获取=的位置后+1，即“位置
            size_t endPos = tempstr.find_last_of("\"");//最后的”位置
            div_finder = stolongl(tempstr.substr(startPos + 1, endPos - (startPos + 1)));
           findAuthor_inDBLP();
           counter++;
           if(counter==50){viewmore_activated+=50;showmore_signal=1; break;}
        }
        getline(infile, tempstr);
    }
    infile.close();
    Probe_RBTree();//遍历作者红黑树
}

string itostr(int temp)//int转string
{
    stringstream a;
    a << temp;
    string zz;
    a >> zz;
    return zz;
}

void MainWindow::Show_FuzzySearch()//模糊搜索
{   showmore_signal=0;
    //--存在bug，无法点More Infomation时由于文档不同，不能通过此方法获取当前位置
    //if(viewmore_activated>0){
    //for(int i=0;i<viewmore_activated;i++)getline(infile, tempstr);}
    int counter=0;
    for(int i=2017;i>=1936;i--)
    { //ifstream infile("G:\\dblp\\src\\year\\"+itostr(i)+".txt", ios::in| ios::binary);
      ifstream infile(DBLP_URL+"year\\"+itostr(i)+".txt", ios::in| ios::binary);
      while(!infile.eof()){
        getline(infile,tempstr);
        if(tempstr.find(ui->inputName->text().toStdString())!=string::npos)
        {ui->listWidget->addItem(new QListWidgetItem(QIcon(":/images/info3"),QString::fromStdString(tempstr)));
         counter++;
         if(counter==50){viewmore_activated+=50;showmore_signal=1; break;}
        }
        if(showmore_signal==1)break;
      }
    }
    if(showmore_signal==1){ui->listWidget->addItem(new QListWidgetItem(QIcon(":/images/more"),"[More Information]"));}
}

void MainWindow::on_pushButton_3_clicked()//选择路径按钮
{
    QString path = QFileDialog::getOpenFileName(this, tr("Select File_path for your DBLP index"), ".", tr("Path(*.txt)"));
          if(path.length() == 0) {
                  QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
          } else {
                  QMessageBox::information(NULL, tr("Path"), tr("You selected ") + path);
                  path.replace("path.txt","");
                  path.replace("/","\\");
                  DBLP_URL=path.toStdString();
          }
}

void MainWindow::on_pushButton_8_clicked()//关闭按钮
{
    this->close();
}

void MainWindow::on_pushButton_5_clicked()//年份词频统计dialog弹出按钮
{
    AnalyseDialog *dexv = new AnalyseDialog(this); //设置界面对象为子界面
    dexv->DBLP_URL3=DBLP_URL;
    dexv->show();//打开子界面
}

void MainWindow::on_pushButton_4_clicked()//前一百名作者dialog弹出按钮
{
   AllauthorDialog *childdig=new AllauthorDialog(this);
   childdig->DBLP_URL2=DBLP_URL;
   childdig->show();
}

void MainWindow::on_pushButton_6_clicked()//网页浏览器dialog弹出按钮
{
    NetMapDialog *childdig2=new NetMapDialog(this);
    childdig2->show();
}
