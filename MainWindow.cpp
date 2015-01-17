#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QSoundEffect>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QDateTime>
#include <QLineEdit>
#include <QPixmap>
#include <QScreen>
#include <QDesktopWidget>
#include <iostream>
#include <fstream>
#include <chrono>
#ifdef _WIN32
#include "psapi.h"
#endif

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),tradeModel(&galaxy,this),tradeProxyModel(this),eqModel(&galaxy,this),bhModel(&galaxy,this)
{
	ui->setupUi(this);
	addAction(ui->actionSaveReport);

	sound.setSource(QUrl::fromLocalFile("Click1.wav"));
	sound.setVolume(1.0);

	setDockNestingEnabled(true);
	setCentralWidget(0);
	tabifyDockWidget(ui->tradeDockWidget,ui->eqDockWidget);
	ui->tradeDockWidget->raise();
	readSettings();

	tradeProxyModel.setSourceModel(&tradeModel);
	eqProxyModel.setSourceModel(&eqModel);

	HierarchicalHeaderView* hv=new HierarchicalHeaderView(Qt::Horizontal, ui->tradeTableView);
	hv->setHighlightSections(true);
	hv->setSectionsClickable(true);
	ui->tradeTableView->setHorizontalHeader(hv);

	//int tableFontHeight=QFontMetrics(QFont("sans",9)).height();
	int tableFontWidth=QFontMetrics(QFont("sans",9)).charWidth("o",0);

	ui->tradeTableView->setModel(&tradeProxyModel);
	ui->tradeTableView->resizeColumnsToContents();
	//ui->tradeTableView->resizeRowsToContents();
	ui->tradeTableView->setColumnWidth(0,tableFontWidth*14);
	ui->tradeTableView->setColumnWidth(1,tableFontWidth*10);

	ui->equipmentTableView->setModel(&eqProxyModel);
	WidgetHeaderView* whv=new WidgetHeaderView(Qt::Horizontal,ui->equipmentTableView);
	ui->equipmentTableView->setHorizontalHeader(whv);
	ui->equipmentTableView->resizeColumnsToContents();
	ui->equipmentTableView->setColumnWidth(0,tableFontWidth*24);
	ui->equipmentTableView->setColumnWidth(1,tableFontWidth*12);
	ui->equipmentTableView->setColumnWidth(2,tableFontWidth*6);//Size
	ui->equipmentTableView->setColumnWidth(3,tableFontWidth*7);//Made
	ui->equipmentTableView->setColumnWidth(4,tableFontWidth*7);//Cost
	ui->equipmentTableView->setColumnWidth(5,tableFontWidth*5);//TL
	ui->equipmentTableView->setColumnWidth(6,tableFontWidth*14);//L T
	ui->equipmentTableView->setColumnWidth(7,tableFontWidth*24);//Loc
	ui->equipmentTableView->setColumnWidth(8,tableFontWidth*10);//Star
	ui->equipmentTableView->setColumnWidth(9,tableFontWidth*8);//Dist
	ui->equipmentTableView->setColumnWidth(10,tableFontWidth*8);//Owner
	ui->equipmentTableView->setColumnWidth(11,tableFontWidth*8);//Durab
	ui->equipmentTableView->setColumnWidth(12,tableFontWidth*48);//Bonus
	//ui->equipmentTableView->resizeRowsToContents();
	//ui->equipmentTableView->verticalHeader()->setDefaultSectionSize(tableFontHeight);

	ui->bhTableView->setModel(&bhModel);
	ui->bhTableView->resizeColumnsToContents();

	connect(&reloadTimer, SIGNAL(timeout()), this, SLOT(parseDump()));
	reloadTimer.setInterval(2000);
	connect(ui->actionReload, &QAction::toggled, [=](bool on) {
		if(on){
			this->reloadTimer.start();
		}
		else{
			this->reloadTimer.stop();
		}
	}  );

	_filename=rangersDir+"/save/autodump.txt";
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::readSettings()
{
	QSettings settings("p-s team", "SRHDDumpReader");
	shortSleep=settings.value("shortSleep",25).toInt();
	maxGenerationTime=settings.value("maxGenerationTime",120000).toInt();
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(400, 400)).toSize();
	resize(size);
	move(pos);
	restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::writeSettings() const
{
	QSettings settings("p-s team", "SRHDDumpReader");
	settings.setValue("shortSleep", shortSleep);
	settings.setValue("maxGenerationTime", maxGenerationTime);
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.setValue("windowState", saveState());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	/*if (maybeSave()) {
	    writeSettings();
	    event->accept();
	} else {
	    event->ignore();
	}*/
	writeSettings();
	event->accept();
}

bool MainWindow::parseDump()
{
	using namespace std;
	using namespace std::chrono;
	high_resolution_clock::time_point tStart = high_resolution_clock::now();
	QFile file(_filename);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		statusBar()->showMessage(tr("File could not be open: ")+_filename);
		return false;
	}
	if(QFileInfo(_filename).lastModified()==_fileModified)
	{
		//dump was parsed earlier
		statusBar()->showMessage(tr("dump was parsed earlier, skipping")+_filename);
		return false;
	}
	_fileModified=QFileInfo(_filename).lastModified();
	setWindowTitle(QStringLiteral("SRHDDumpReader - ")+QFileInfo(_filename).baseName());

	galaxy.clear();
	QTextStream stream(&file);
	galaxy.parseDump(stream);
	high_resolution_clock::time_point tParseEnd = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>( tParseEnd - tStart ).count();
	cout<<"Parsing took "<<duration/1000.0<<"seconds"<<endl;
	statusBar()->showMessage(tr("Parsed %1 stars, %2 planets, %3 black holes, %4 ships and %5 items").
				 arg(galaxy.starCount()).arg(galaxy.planetCount()).
				 arg(galaxy.blackHoleCount()).
				 arg(galaxy.shipCount()).arg(galaxy.equipmentCount()),
				 5000);

	tradeModel.reload();
	eqModel.reload();
	bhModel.reload();
	ui->tradeTableView->resizeColumnsToContents();
	//ui->tradeTableView->resizeRowsToContents();
	//ui->equipmentTableView->resizeColumnsToContents();
	// //ui->equipmentTableView->resizeRowsToContents();

	//ui->equipmentTableView->resizeRowToContents(0);
	//int sectionSize=ui->equipmentTableView->verticalHeader()->sectionSize(0);
	//ui->equipmentTableView->verticalHeader()->setDefaultSectionSize(sectionSize);
	high_resolution_clock::time_point tEnd = high_resolution_clock::now();
	duration = duration_cast<milliseconds>( tEnd - tParseEnd ).count();
	cout<<"Model update took "<<duration/1000.0<<"seconds"<<endl;
	return true;
}

bool MainWindow::openDump()
{
	QString fileName = QFileDialog::getOpenFileName(this,
							tr("Open Dump File"), rangersDir+"/save/",
							tr("SRHD Dump (*.txt);;All Files (*)"));
	if (fileName.isEmpty())
		return false;
	_filename=fileName;
	return parseDump();
}

void MainWindow::showAbout()
{
	QMessageBox::about(this,"SRHDDumpReader",QString("SRHDDumpReader v. %1\nAuthor: Mykola Dimura\n"
							 "Description: small program to parse and analyse"
							 " the galaxy dumps from \"Space Rangers HD: A War"
							 " Apart\" video game\n"
							 "Tip: press F7 in-game for QuickDump.")
			   .arg(QApplication::applicationVersion()));
}
int MainWindow::saveReport() const
{
	using namespace std;
	if(QFileInfo(_filename+".report").exists()) {
		//QString timestamp=QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
		//QFile::rename(_filename+".report",_filename+timestamp+".report");
		QFile::remove(_filename+".report");
	}
	ofstream ofile;
	string filename=QFile::encodeName(_filename+".report").toStdString();
	ofile.open (filename);
	if(!ofile.good()) {
		statusBar()->showMessage(tr("Could not create the report file ")+QString::fromStdString(filename));
		return -100;
	}

	int hugeIndustrialGall=0;
	//Planets inner 30 dist (name, distance, size, tech level, economy, owner)
	ui->tradeTableView->sortByColumn(2,Qt::AscendingOrder);
	string buf, planetsBuf="Industrial planets:\n",
			basesBuf="Bases inner 30 dist:\nname\tstar\tdistance\n";
	buf="name\tstar\tdistance\tsize\ttech level\teconomy\towner\n";
	for(int row=0; row<tradeProxyModel.rowCount(); row++)
	{
		double dist=tradeProxyModel.data(tradeProxyModel.index(row,2)).toInt();

		if(dist>30.0) {
			break;
		}

		int size=tradeProxyModel.data(tradeProxyModel.index(row,27)).toInt();
		string name=tradeProxyModel.data(tradeProxyModel.index(row,0)).toString().toStdString();
		string star=tradeProxyModel.data(tradeProxyModel.index(row,1)).toString().toStdString();
		if(size==0) {//Base, not planet
			basesBuf+=name+'\t'+star+'\t'+to_string(dist);
			basesBuf+='\n';
			continue;
		}

		int techLvl=tradeProxyModel.data(tradeProxyModel.index(row,28)).toInt();
		string economy=tradeProxyModel.data(tradeProxyModel.index(row,29)).toString().toStdString();
		string owner=tradeProxyModel.data(tradeProxyModel.index(row,30)).toString().toStdString();
		if(techLvl>1 && economy=="Industrial"){//Developed planet
			planetsBuf+=owner+'/'+to_string(size)+'/'+to_string(techLvl)+", ";
			if(owner=="Gaal" && size==100) {
				++hugeIndustrialGall;
			}
		}
		buf+=name+'\t'+star+'\t';
		buf+=to_string(dist)+'\t'+to_string(size)+'\t'+to_string(techLvl)+'\t'+economy+'\t'+owner;
		buf+='\n';
	}
	ofile<<"Huge industrial gaal planets in the radius of 30 from player: "<<
	       hugeIndustrialGall<<"\n"<<planetsBuf;
	ofile<<'\n'<<buf<<'\n';

	ofile<<basesBuf;
	ofile<<'\n';

	ofile<<"Burried treasures inner 45 dist:\n";
	ui->equipmentTableView->sortByColumn(9,Qt::AscendingOrder);
	ofile<<"name\tstar\tdistance\tOwner\tLocation\n";
	for(int row=0; row<eqProxyModel.rowCount(); row++)
	{
		if(eqProxyModel.data(eqProxyModel.index(row,9)).toInt()>45.0) {
			break;
		}
		if(eqProxyModel.data(eqProxyModel.index(row,6)).toString()!="Planet/treasure") {//Not Burried
			continue;
		}
		auto type=eqProxyModel.data(eqProxyModel.index(row,1)).toString();
		if(type.startsWith("Art") || //Artifact
		   type.startsWith("W04") || //Rocket launcher
		   type.startsWith("Nod")) { //Micromodule
			for(int col:{0,8,9,10,7,12})
			{
				ofile<<eqProxyModel.data(eqProxyModel.index(row,col)).toString().toStdString()<<'\t';
			}
			ofile<<'\n';
		}
	}
	statusBar()->showMessage(tr("Report saved: ")+QString::fromStdString(filename));
	return hugeIndustrialGall;
}
#ifdef _WIN32
bool rangersWindowActive()
{
	HWND wnd=GetForegroundWindow();
	DWORD windowProcessId = 0;
	GetWindowThreadProcessId(wnd,&windowProcessId);
	//std::cout<<"active window process: "<<windowProcessId;
	HANDLE PID = OpenProcess(
			     PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			     FALSE,
			     windowProcessId /* This is the PID, you can find one from windows task manager */
			     );
	if (PID)
	{
		TCHAR Buffer[MAX_PATH];
		if (GetModuleFileNameEx(PID, 0, Buffer, MAX_PATH))
		{
			// At this point, buffer contains the full path to the executable
			std::wstring wstr(Buffer);
			std::string str(wstr.begin(),wstr.end());
			if(QFileInfo(QString::fromStdString(str)).baseName()=="Rangers")
			{
				return true;
			}
		}
		else
		{
			// You better call GetLastError() here
		}
		CloseHandle(PID);
	}
	return false;
}
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *  /* result */ )
{
	MSG *msg = reinterpret_cast<MSG *>(message);
	if (msg->message == WM_HOTKEY)
	{
		switch(msg->wParam)
		{
		case 100:
			saveDumpWin();
			break;
		case 101:
			//TODO: debug
			generateGalaxies();
			sound.play();
			break;
		default:
			break;
		}
	}
	return false;
}

bool MainWindow::simulateInput(const std::string& str) const
{
	INPUT inp;
	inp.type = INPUT_KEYBOARD;
	inp.ki.wScan = 0;
	inp.ki.time = 0;
	//inp.ki.dwFlags = 0;
	inp.ki.dwExtraInfo = 0;
	std::vector<INPUT> ip;
	for(char c:str)
	{
		if(std::islower(c))
		{
			if(c=='e'){
				inp.ki.wScan = 0x1C;//Enter
				inp.ki.wVk = 0;
				inp.ki.dwFlags = 0 | KEYEVENTF_SCANCODE; // 0 for key press
				ip.push_back(inp);
				inp.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
				ip.push_back(inp);
			}
			else if (c=='s'){
				inp.ki.wVk = 0x1B;//Esc;
				inp.ki.dwFlags = 0; // 0 for key press
				ip.push_back(inp);
				inp.ki.dwFlags = KEYEVENTF_KEYUP;
				ip.push_back(inp);
			}
			else if (c=='f'){
				inp.ki.wVk = 0x78;//F9
				inp.ki.dwFlags = 0; // 0 for key press
				ip.push_back(inp);
				inp.ki.dwFlags = KEYEVENTF_KEYUP;
				ip.push_back(inp);
			}
			else if (c=='b'){
				inp.ki.wVk = 0;
				inp.ki.wScan = 0x0E;//backspace
				inp.ki.dwFlags = 0 | KEYEVENTF_SCANCODE; // 0 for key press
				ip.push_back(inp);
				inp.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
				ip.push_back(inp);
			}
		}
		else {
			// Press normal key
			inp.ki.wVk = c;
			inp.ki.dwFlags = 0; // 0 for key press
			ip.push_back(inp);
			inp.ki.dwFlags = KEYEVENTF_KEYUP;
			ip.push_back(inp);
		}
	}
	if(!rangersWindowActive()) {
		statusBar()->showMessage(tr("Rangers are not in focus, aborting input simulation."));
		return false;
	}
	SendInput(ip.size(), ip.data(), sizeof(INPUT));
	responsiveSleep(shortSleep);
	QCoreApplication::processEvents();
	return true;
}

void MainWindow::saveDumpWin()
{
	//TODO: debug
	if(!rangersWindowActive()) {
		statusBar()->showMessage(tr("Rangers are not in focus, aborting QuickDump."));
		return;
	}
	_filename=rangersDir+"/save/autodump";

	if(QFileInfo(_filename+".txt").exists()) {
		QFile::remove(_filename+".txt");
	}
	if(QFileInfo(_filename+".sav").exists()) {
		QFile::remove(_filename+".sav");
	}
	_filename+=".txt";

	INPUT inp;
	inp.type = INPUT_KEYBOARD;
	inp.ki.wScan = 0;
	inp.ki.time = 0;
	inp.ki.dwFlags = 0;
	inp.ki.dwExtraInfo = 0;

	std::vector<INPUT> ip(2,inp);

	//sound.play();
	QCoreApplication::processEvents();
	//First imitate Ctrl+Shift+MAKEDUMP
	ip[0].ki.wVk = VK_CONTROL;
	ip[1].ki.wVk = VK_SHIFT;
	SendInput(ip.size(), ip.data(), sizeof(INPUT));
	responsiveSleep(shortSleep);
	QCoreApplication::processEvents();

	if(!simulateInput("MAKEDUMP")) {
		return;
	}
	QCoreApplication::processEvents();

	ip.clear();
	inp.ki.wVk = VK_SHIFT;
	inp.ki.dwFlags = KEYEVENTF_KEYUP ;
	ip.push_back(inp);
	inp.ki.wVk = VK_CONTROL;
	inp.ki.dwFlags = KEYEVENTF_KEYUP;
	ip.push_back(inp);
	SendInput(ip.size(), ip.data(), sizeof(INPUT));

	responsiveSleep(shortSleep*75);//Wait for save window to showup
	//sound.play();
	QCoreApplication::processEvents();

	//Remove proposed save name
	if(!simulateInput("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb")) {
		return;
	}
	QCoreApplication::processEvents();

	//Enter new save name ("autodump")
	if(!simulateInput("AUTODUMP")) {
		return;
	}
	QCoreApplication::processEvents();

	//Press Enter to save
	if(!simulateInput("e")) {
		return;
	}

	//wait till the saving has finished
	const int dt=100;
	int oldSize=0;
	QFileInfo fileInfo(_filename);
	for(int t=0; t<30000; t+=dt)
	{
		responsiveSleep(dt);
		fileInfo.refresh();
		int size=fileInfo.size();
		std::cout<<t/1000.0<<": "<<size<<" exists: "<<fileInfo.exists()<<std::endl;
		if(size && size==oldSize) {
			break;
		}
		oldSize=size;
		QCoreApplication::processEvents();
	}

	parseDump();
}

QImage MainWindow::currentScreen(float kx, float ky, float kw, float kh) const
{
	using namespace std;
	auto screenTaken=QDateTime::currentDateTime();
	if(!simulateInput("f")) {//make new screenshot
		return QImage();
	}
	const QString rangersDir=QStandardPaths::locate(QStandardPaths::DocumentsLocation,"SpaceRangersHD",QStandardPaths::LocateDirectory);
	QDir dir(rangersDir+"/Screenshots");
	QStringList fileList;
	QString screenFilename;
	QDateTime fileModified;

	static int saveLag=1000;
	responsiveSleep(saveLag);
	cout<<"current screenshot saving time: "<<saveLag<<endl;
	int t;
	for(t=saveLag; t<8000; t+=saveLag*0.1)
	{
		responsiveSleep(saveLag*0.1);
		dir.refresh();
		fileList=dir.entryList(QDir::Files,QDir::Time);
		if(fileList.isEmpty()){
			//cerr<<string("no files in a dir: ")+to_string(t)<<endl;
			continue;
		}
		screenFilename=dir.absolutePath()+"/"+fileList.first();
		fileModified=QFileInfo(screenFilename).lastModified();
		if(screenTaken.msecsTo(fileModified)<0) {//old screenshot
			/*cerr<<string("old file: ")+to_string(t)+
		  " "+screenFilename.toStdString()+
		  " delta:"+to_string(screenTaken.msecsTo(fileModified))+
	       " taken: "+screenTaken.toString().toStdString()+
	       " modified: "+fileModified.toString().toStdString()<<endl;
	    screenFilename.clear();*/
			continue;
		}
		else {
			break;
		}
	}
	if(screenFilename.isEmpty()) {
		cerr<<string("Could not take a screenshot at time: ")+to_string(t)<<endl;
		return QImage();
	}
	//wait, till the file is fully writen to disk
	QImage currentImage;
	while(!currentImage.load(screenFilename) && t<8000)
	{
		//cerr<<"load failed:"<<t<<endl;
		responsiveSleep(50);
		t+=50;
	}
	saveLag=t*0.95;
	currentImage=currentImage.copy(currentImage.width()*kx,currentImage.height()*ky,
				       currentImage.width()*kw,currentImage.height()*kh);
	QFile::remove(screenFilename);
	return currentImage;
}

void MainWindow::generateGalaxies()
{
	using namespace std;
	using namespace std::chrono;
	std::string filename=QFile::encodeName(rangersDir+"/SRHDDumpReader_generation.log").toStdString();
	ofstream logfile(filename,std::ofstream::out | std::ofstream::app);
	if(!logfile.good()) {
		statusBar()->showMessage(tr("Could not open the log file ")+QString::fromStdString(filename));
	}
	bool removeBadSaves=QFileInfo("removeBadSaves").exists();
	auto originalMainMenu=currentScreen(0,0.9,0.05,0.1);
	if(originalMainMenu.height()==0) {
		statusBar()->showMessage(tr("could not get reference screenshot for the main menu. Generation stopped."));
		return;
	}
	originalMainMenu.save("originalMainMenu.png");

	int generationTime=20000;
	QImage originalGenerationFinished;
	for(int i=0; i<2000;i++)
	{
		//check, if the state is right
		auto currentMainMenu=currentScreen(0,0.9,0.05,0.1);
		if(currentMainMenu!=originalMainMenu) {
			statusBar()->showMessage(tr("Main menu is expected, but does not match. Generation stopped. "));
			return;
		}

		//New Game
		if(!simulateInput("N")) {
			return;
		}
		responsiveSleep(shortSleep*8);
		//Enter (start generation)
		if(!simulateInput("e")) {
			return;
		}
		//Wait until the galaxy is ready
		high_resolution_clock::time_point tGenerationStarted = high_resolution_clock::now();
		QImage currentGenerationFinished;
		if(i==0) {//first run
			responsiveSleep(maxGenerationTime);
			originalGenerationFinished=currentScreen(0.45,0.925,0.1,0.05);
			originalGenerationFinished.save("originalGenerationFinished.png");
		}
		else {
			responsiveSleep(generationTime);
			const int dt=3000;
			int t;
			std::cout<<"Current galaxy generation time: "<<generationTime<<std::endl;
			for(t=generationTime; t<maxGenerationTime; t+=dt)
			{
				currentGenerationFinished=currentScreen(0.45,0.925,0.1,0.05);
				if(currentGenerationFinished==originalGenerationFinished) {
					break;
				}
				else {
					//currentGenerationFinished.save(QString::number(t)+"_currentGen.png");
					responsiveSleep(dt);
				}
			}
			if(t>=maxGenerationTime) {
				statusBar()->showMessage(tr("Galaxy_generation_finished is expected, but does not match. Generation stopped"));
				return;
			}
			high_resolution_clock::time_point tGenerationFinished = high_resolution_clock::now();
			generationTime=duration_cast<milliseconds>( tGenerationFinished - tGenerationStarted ).count()*0.95;
		}
		//Enter (close Intro screen)
		if(!simulateInput("e")) {
			return;
		}
		responsiveSleep(shortSleep*100);
		saveDumpWin();
		int hugeIndustrGaal=saveReport();//huge industrial gaal planets count
		std::string logoutstr=QDateTime::currentDateTime().toString(Qt::ISODate).toStdString()+
				      ": Iteration "+std::to_string(i)+" finished. Huge gaal industrial planets: "+std::to_string(hugeIndustrGaal);
		logfile<<logoutstr<<endl;
		std::cout<<logoutstr<<endl;
		QString prefix=rangersDir+"/save/autodump";
		if(hugeIndustrGaal<5 && removeBadSaves) {//useless save
			QFile::remove(prefix+".txt");
			QFile::remove(prefix+".sav");
			QFile::remove(prefix+".txt.report");
		}
		else {
			QString timestamp=QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
			QFile::rename(prefix+".txt",prefix+timestamp+".txt");
			QFile::rename(prefix+".sav",prefix+timestamp+".sav_");
			QFile::rename(prefix+".txt.report",prefix+timestamp+".txt.report");
		}

		responsiveSleep(shortSleep*20);
		if(!simulateInput("s")) {//Esc
			return;
		}
		responsiveSleep(shortSleep*40);
		if(!simulateInput("E")) {//E==exit
			return;
		}
		if(!simulateInput("e")) {//Enter (confirm)
			return;
		}
		responsiveSleep(shortSleep*40);
	}
}
#endif