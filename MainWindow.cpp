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
#include <QJsonDocument>
#include <QToolButton>
#include <QTextCodec>
#include <QItemSelectionModel>
#include <QClipboard>

#include <iostream>
#include <fstream>
#include <chrono>

#ifdef _WIN32
#include "psapi.h"
#endif

QString tabSeparatedValues(const QAbstractItemModel& model)
{
	QString buf;
	for(int c=0; c<model.columnCount();c++)
	{
		buf+=model.headerData(c,Qt::Horizontal).toString();
		buf+='\t';
	}
	buf+='\n';
	for(int r=0; r<model.rowCount(); r++)
	{
		for(int c=0; c<model.columnCount();c++)
		{
			buf+=model.data(model.index(r,c)).toString();
			buf+='\t';
		}
		buf+='\n';
	}
	return buf;
}
QString bbSeparatedValues(const QItemSelectionModel *selectionModel)
{
	const QAbstractItemModel* model=selectionModel->model();
	QModelIndexList indexes = selectionModel->selectedIndexes();
	if(indexes.size() < 1){
		return "";
	}
	std::sort(indexes.begin(), indexes.end());

	QString selectedText="[table][tr]";
	QModelIndex previous=indexes.takeFirst();
	for(const QModelIndex& index:indexes)
	{
		QString text = "[td]" + model->data(previous).toString() + "[/td]";
		// At this point `text` contains the text in one cell
		selectedText.append(text);
		// If you are at the start of the row the row number of the previous index
		// isn't the same.  Text is followed by a row separator, which is a newline.
		if (index.row() != previous.row())
		{
			selectedText.append("[/tr]\n[tr]");
		}
		previous = index;
	}
	selectedText.append("[td]"+model->data(previous).toString()+"[/td]");
	selectedText+="[/tr][/table]";
	return selectedText;
}
QString tabSeparatedValues(const QItemSelectionModel *selectionModel)
{
	const QAbstractItemModel* model=selectionModel->model();
	QModelIndexList indexes = selectionModel->selectedIndexes();
	if(indexes.size() < 1) {
		return "";
	}
	std::sort(indexes.begin(), indexes.end());

	QString selectedText;
	QModelIndex previous=indexes.takeFirst();
	for(const QModelIndex& index:indexes)
	{
		QString text = model->data(previous).toString();
		// At this point `text` contains the text in one cell
		selectedText.append(text);
		// If you are at the start of the row the row number of the previous index
		// isn't the same.  Text is followed by a row separator, which is a newline.
		if (index.row() != previous.row())
		{
			selectedText.append(QLatin1Char('\n'));
		}
		// Otherwise it's the same row, so append a column separator, which is a tab.
		else
		{
			selectedText.append(QLatin1Char('\t'));
		}
		previous = index;
	}
	selectedText.append(model->data(previous).toString());
	selectedText.append(QLatin1Char('\n'));
	return selectedText;
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),tradeModel(&galaxy,this),tradeProxyModel(this),
	eqModel(&galaxy,this),bhModel(&galaxy,this),planetsModel(&galaxy,this)
{
	ui->setupUi(this);

	ui->planetsTableView->installEventFilter(this);
	ui->bhTableView->installEventFilter(this);
	ui->equipmentTableView->installEventFilter(this);
	ui->tradeTableView->installEventFilter(this);

	connect(&reloadTimer, SIGNAL(timeout()), this, SLOT(parseDump()));
	reloadTimer.setInterval(2000);
	connect(ui->actionAutoReload, &QAction::toggled, [=](bool on) {
		if(on) {
			this->reloadTimer.start();
		}
		else {
			this->reloadTimer.stop();
		}
	} );

	ui->mapImageLabel->setBackgroundRole(QPalette::Base);
	ui->scrollArea->setBackgroundRole(QPalette::Dark);

	reloadMenu.addAction(ui->actionAutoReload);
	QToolButton *reloadButton=static_cast<QToolButton*>(ui->mainToolBar->widgetForAction(ui->actionReload));
	reloadButton->setMenu(&reloadMenu);
	reloadButton->setPopupMode(QToolButton::MenuButtonPopup);

	addAction(ui->actionSaveReport);
	saveReportMenu.addAction(ui->actionAutoSaveReport);
	QToolButton *reportButton=static_cast<QToolButton*>(ui->mainToolBar->widgetForAction(ui->actionSaveReport));
	reportButton->setMenu(&saveReportMenu);
	reportButton->setPopupMode(QToolButton::MenuButtonPopup);

	_mapScaleSpinBox.setPrefix("x");
	_mapScaleSpinBox.setToolTip(tr("Map scale"));
	_mapScaleSpinBox.setWhatsThis(tr("Map scale"));
	ui->mainToolBar->insertWidget(ui->mainToolBar->actions()[3],&_mapScaleSpinBox);
	connect(&_mapScaleSpinBox,SIGNAL(valueChanged(double)),this,SLOT(setMapScale(double)));

	sound.setSource(QUrl::fromLocalFile("Click1.wav"));
	sound.setVolume(1.0);

	setDockNestingEnabled(true);
	setCentralWidget(0);
	tabifyDockWidget(ui->tradeDockWidget,ui->eqDockWidget);
	tabifyDockWidget(ui->eqDockWidget,ui->bhDockWidget);
	tabifyDockWidget(ui->bhDockWidget,ui->tradeDockWidget);
	tabifyDockWidget(ui->tradeDockWidget,ui->imageDockWidget);
	ui->tradeDockWidget->raise();
	readSettings();

	tradeProxyModel.setSourceModel(&tradeModel);
	eqProxyModel.setSourceModel(&eqModel);
	planetsProxyModel.setSourceModel(&planetsModel);

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

	ui->planetsTableView->setModel(&planetsProxyModel);
	planetsHeaderView=
			new FilterHorizontalHeaderView(&planetsProxyModel,ui->planetsTableView);
	ui->planetsTableView->setHorizontalHeader(planetsHeaderView);
	ui->planetsTableView->resizeColumnsToContents();

	ui->equipmentTableView->setModel(&eqProxyModel);
	eqHeaderView=
			new FilterHorizontalHeaderView(&eqProxyModel,ui->equipmentTableView);
	ui->equipmentTableView->setHorizontalHeader(eqHeaderView);
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

	connect(planetsHeaderView,&FilterHorizontalHeaderView::presetSaved,
		[&](const QVariantMap& preset, const QString& name) {
		QString fileName=presetDirPlanets+name+".dr.json";
		savePreset(preset,fileName);
	});

	connect(eqHeaderView,&FilterHorizontalHeaderView::presetSaved,
		[&](const QVariantMap& preset, const QString& name) {
		QString fileName=presetDirEq+name+".dr.json";
		savePreset(preset,fileName);
	});

	_filename=rangersDir+"/save/autodump.txt";
	loadPresets();
	openDump(QCoreApplication::arguments().value(1));
}
void copySelectedText(const QItemSelectionModel* selModel, bool bbcode=false)
{
	QString selected_text;
	if(bbcode) {
		selected_text=bbSeparatedValues(selModel);
	}
	else{
		selected_text=tabSeparatedValues(selModel);
	}


	QApplication::clipboard()->setText(selected_text);
}
bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
	if( (object == ui->planetsTableView || object==ui->equipmentTableView ||
	     object==ui->tradeTableView || object==ui->bhTableView) &&
	    event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->matches(QKeySequence::Copy)) {
			QAbstractItemView* viewObject=static_cast<QAbstractItemView *>(object);
			copySelectedText(viewObject->selectionModel());
			return true;
		}
		else if(keyEvent->modifiers().testFlag(Qt::ShiftModifier) &&
			keyEvent->modifiers().testFlag(Qt::ControlModifier)) {
			QAbstractItemView* viewObject=static_cast<QAbstractItemView *>(object);
			copySelectedText(viewObject->selectionModel(),true);
			return true;
		}
	}
	return false;
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
	mapScale=settings.value("mapScale",7.f).toFloat();
	_mapScaleSpinBox.setValue(mapScale);

	bool autoSaveReport=settings.value("autoSaveReport",false).toBool();
	ui->actionAutoSaveReport->setChecked(autoSaveReport);
	bool autoReload=settings.value("autoReload",false).toBool();
	ui->actionAutoReload->setChecked(autoReload);
	//ui->actionAutoReload->toggle();

	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(400, 400)).toSize();
	resize(size);
	move(pos);
	restoreState(settings.value("windowState").toByteArray());
	std::cout<<"rangersDir:"<<rangersDir.toStdString()<<std::endl;

	QFile file("minRowsPreset.json");
	if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
		QJsonDocument doc=QJsonDocument::fromJson(file.readAll());
		if(!doc.isNull()) {
			std::cout<<"loading minRowsPreset.json"<<std::endl;
			QVariantMap minRowsJson=doc.toVariant().toMap();
			using VarMapCI=QVariantMap::const_iterator;
			for (VarMapCI i = minRowsJson.begin(); i != minRowsJson.end(); ++i)
			{
				minRowsPreset[i.key()]=i.value().toInt();
				std::cout<<i.key().toStdString()<<": "<<i.value().toInt()<<std::endl;
			}
		}
	}
	scorers=readScorers("scorers.json");
}

void MainWindow::writeSettings() const
{
	QSettings settings("p-s team", "SRHDDumpReader");
	settings.setValue("shortSleep", shortSleep);
	settings.setValue("maxGenerationTime", maxGenerationTime);
	settings.setValue("mapScale", (double)mapScale);

	settings.setValue("autoReload",ui->actionAutoReload->isChecked());
	settings.setValue("autoSaveReport",ui->actionAutoSaveReport->isChecked());

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

bool MainWindow::parseDump(const QString& filename)
{
	using namespace std;
	using namespace std::chrono;
	high_resolution_clock::time_point tStart = high_resolution_clock::now();
	if(!filename.isEmpty()) {
		_filename=filename;
	}
	QFile file(_filename);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		showMessage(tr("File could not be open: ")+_filename);
		return false;
	}
	if(filename.isEmpty() && QFileInfo(_filename).lastModified()==_fileModified)
	{
		//dump was parsed earlier
		showMessage(tr("dump was parsed earlier, skipping")+_filename);
		return false;
	}
	_fileModified=QFileInfo(_filename).lastModified();
	setWindowTitle(QStringLiteral("SRHDDumpReader - ")+QFileInfo(_filename).baseName());

	galaxy.clear();
	const QByteArray& allArr=(file.readAll());
	QTextStream stream(allArr);
	QTextCodec::ConverterState state;
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	const QString text = codec->toUnicode(allArr.left(500).constData(), 500, &state);
	if (state.invalidChars > 0) {
		stream.setCodec("Windows-1251");
		std::cout<<"Using Windows-1251 encoding"<<std::endl;
	}
	else {
		std::cout<<"Using UTF-8 encoding"<<std::endl;
	}
	high_resolution_clock::time_point tReadEnd = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>( tReadEnd - tStart ).count();
	string timeTaken="Reading the file took "+to_string(duration/1000.0)+" s. ";
	galaxy.parseDump(stream);
	showMessage(tr("Parsed %1 stars, %2 planets, %3 black holes, %4 ships and %5 items").
				 arg(galaxy.starCount()).arg(galaxy.planetCount()).
				 arg(galaxy.blackHoleCount()).
				 arg(galaxy.shipCount()).arg(galaxy.equipmentCount()),
				 5000);
	high_resolution_clock::time_point tParseEnd = high_resolution_clock::now();
	duration = duration_cast<milliseconds>( tParseEnd - tReadEnd ).count();
	timeTaken+="Parsing - "+to_string(duration/1000.0)+" s. ";

	tradeModel.reload();
	eqModel.reload();
	bhModel.reload();
	planetsModel.reload();
	ui->tradeTableView->resizeColumnsToContents();
	ui->planetsTableView->resizeColumnsToContents();
	//ui->tradeTableView->resizeRowsToContents();
	//ui->equipmentTableView->resizeColumnsToContents();
	// //ui->equipmentTableView->resizeRowsToContents();
	//ui->equipmentTableView->resizeRowToContents(0);
	//int sectionSize=ui->equipmentTableView->verticalHeader()->sectionSize(0);
	//ui->equipmentTableView->verticalHeader()->setDefaultSectionSize(sectionSize);
	high_resolution_clock::time_point tModelUpdateEnd = high_resolution_clock::now();
	duration = duration_cast<milliseconds>( tModelUpdateEnd - tParseEnd ).count();
	timeTaken+="Model update - "+to_string(duration/1000.0)+" s. ";

	updateMap();
	high_resolution_clock::time_point tMapEnd = high_resolution_clock::now();
	duration = duration_cast<milliseconds>( tMapEnd - tModelUpdateEnd ).count();
	timeTaken+="map update - "+to_string(duration/1000.0)+" s. ";

	duration = duration_cast<milliseconds>( high_resolution_clock::now() - tStart ).count();
	timeTaken+="Total: "+to_string(duration/1000.0)+" s.";
	cout<<timeTaken<<endl;

	if(ui->actionAutoSaveReport->isChecked()) {
		saveReport();
	}
	return true;
}

bool MainWindow::openDump()
{
	QString fileName = QFileDialog::getOpenFileName(this,
							tr("Open Dump File"), rangersDir+"/save/",
							tr("SRHD Dump (*.txt);;All Files (*)"));

	return openDump(fileName);
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


void MainWindow::saveReport()
{
	using namespace std;

	saveMap();

	using namespace std::chrono;
	high_resolution_clock::time_point tStart = high_resolution_clock::now();

	QVariantMap oldEqPreset=eqHeaderView->preset();
	QVariantMap oldPlPreset=planetsHeaderView->preset();
	_reportSummary.clear();

	QFileInfo fileInfo(_filename);
	QString filename=fileInfo.path()+'/'+fileInfo.completeBaseName()+".report";
	QFile ofile(filename);
	if(ofile.exists()) {
		//QString timestamp=QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
		//QFile::rename(filename,_filename+timestamp+".report");
		QFile::remove(filename);
	}

	if(!ofile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		showMessage(tr("Could not create the report file ")+filename);
		return;
	}

	//Bases
	ui->tradeTableView->sortByColumn(2,Qt::AscendingOrder);
	QString basesBuf="Bases:\nname\tstar\tdistance\n";
	for(int row=0; row<tradeProxyModel.rowCount(); row++)
	{
		double dist=tradeProxyModel.data(tradeProxyModel.index(row,2)).toInt();
		int size=tradeProxyModel.data(tradeProxyModel.index(row,27)).toInt();
		QString name=tradeProxyModel.data(tradeProxyModel.index(row,0)).toString();
		QString star=tradeProxyModel.data(tradeProxyModel.index(row,1)).toString();
		if(size==0) {//Base, not planet
			basesBuf+=name+'\t'+star+'\t'+QString::number(dist);
			basesBuf+='\n';
			continue;
		}
	}

	QString lastSummaryEntry;
	//planets Presets
	QString planetsBuf;
	for(const QString& fileName: planetsReportPresets)
	{
		planetsHeaderView->setPreset(loadPreset(fileName));
		lastSummaryEntry=QFileInfo(fileName).baseName();
		_reportSummary[lastSummaryEntry]=planetsProxyModel.rowCount();
		lastSummaryEntry+=": "+QString::number(planetsProxyModel.rowCount());
		planetsBuf+=lastSummaryEntry+'\n';
		planetsBuf+=tabSeparatedValues(planetsProxyModel);
		planetsBuf+='\n';
	}

	//eq Presets
	QString eqBuf;
	for(const QString& fileName: eqReportPresets)
	{
		eqHeaderView->setPreset(loadPreset(fileName));
		lastSummaryEntry=QFileInfo(fileName).baseName();
		_reportSummary[lastSummaryEntry]=eqProxyModel.rowCount();
		lastSummaryEntry+=": "+QString::number(eqProxyModel.rowCount());
		eqBuf+=lastSummaryEntry+'\n';
		eqBuf+=tabSeparatedValues(eqProxyModel);
		eqBuf+='\n';
	}
	//restore presets
	eqHeaderView->setPreset(oldEqPreset);
	planetsHeaderView->setPreset(oldPlPreset);

	QTextStream out(&ofile); // we will serialize the data into the file
	out.setCodec("UTF-8");
	out << scoresSummary()+'\n'+planetsBuf+eqBuf+basesBuf+'\n'; // serialize a string

	statusBar()->showMessage(tr("Report saved: ")+filename);
	auto duration = duration_cast<milliseconds>( high_resolution_clock::now() - tStart ).count();
	std::cout<<"Report saved: "+filename.toStdString()+" in "+
		   to_string(duration/1000.0)+" s.\n"+
		   reportSummary().toStdString()<<std::endl;
}

void MainWindow::saveAllReports()
{
	QFileInfo fileInfo(_filename);
	QString filename=fileInfo.path()+"/summary.report";
	QFile ofile(filename);
	if(ofile.exists()) {
		//QString timestamp=QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
		//QFile::rename(filename,_filename+timestamp+".report");
		QFile::remove(filename);
	}

	if(!ofile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		showMessage(tr("Could not create the summary report file ")+filename);
		return;
	}
	QTextStream out(&ofile);
	out.setCodec("UTF-8");
	for(const QString& dumpFileName: dumpFileList)
	{
		parseDump(dumpFileName);
		if(!ui->actionAutoSaveReport->isChecked())
		{
			saveReport();
		}
		out << QFileInfo(dumpFileName).baseName()+"\t"+reportSummary()+'\n';
	}
	parseDump(dumpFileList[currentDumpIndex]);
}

void MainWindow::loadNextDump()
{
	if(currentDumpIndex<0 || currentDumpIndex>=dumpFileList.size()-1) {
		return;
	}
	parseDump(dumpFileList[++currentDumpIndex]);
	updateDumpArrows();
}

void MainWindow::loadPreviousDump()
{
	if(currentDumpIndex<=0) {
		return;
	}
	parseDump(dumpFileList[--currentDumpIndex]);
	updateDumpArrows();
}

bool MainWindow::openDump(const QString &fileName)
{
	if (fileName.isEmpty())
		return false;
	//_filename=fileName;
	QDir currentDir=QFileInfo(fileName).dir();
	currentDir.setNameFilters(QStringList("*.txt"));
	currentDir.setFilter(QDir::Files);
	const QFileInfoList& infoList=currentDir.entryInfoList();
	dumpFileList.clear();
	for(const auto& info:infoList)
	{
		dumpFileList<<info.absoluteFilePath();
	}
	currentDumpIndex=dumpFileList.indexOf(fileName);
	std::cout<<currentDumpIndex<<'\n'<<dumpFileList.join('\n').toStdString()<<std::endl;
	updateDumpArrows();
	return parseDump(fileName);
}

void MainWindow::savePreset(const QVariantMap &preset, const QString &fileName) const
{
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		showMessage(tr("Could not save: ")+fileName);
		return;
	}
	file.write(QJsonDocument::fromVariant(preset).toJson());
	showMessage(tr("Preset saved: ")+fileName);
}

void MainWindow::responsiveSleep(int msec) const
{
	int t;
	QMutex mutex;
	QWaitCondition waitCondition;
	for(t=0; t<msec-shortSleep; t+=shortSleep)
	{
		mutex.lock();
		waitCondition.wait(&mutex, shortSleep);
		mutex.unlock();
		QCoreApplication::processEvents();
	}
	mutex.lock();
	waitCondition.wait(&mutex, msec-t);
	mutex.unlock();
	QCoreApplication::processEvents();
}

void MainWindow::loadPresets()
{
	//presets to show in context menus
	std::cout<<"loading presets for context menus:"<<std::endl;
	QDir plDir(presetDirPlanets);
	std::cout<<plDir.absolutePath().toStdString()<<std::endl;
	QDir eqDir(presetDirEq);
	for(const QString& fileName:plDir.entryList(QDir::Files))
	{
		auto preset=loadPreset(plDir.path()+'/'+fileName);
		planetsHeaderView->addPreset(preset,QFileInfo(fileName).baseName());
		std::cout<<fileName.toLocal8Bit().toStdString()<<std::endl;
	}
	for(const QString& fileName:eqDir.entryList(QDir::Files))
	{
		auto preset=loadPreset(eqDir.path()+'/'+fileName);
		eqHeaderView->addPreset(preset,QFileInfo(fileName).baseName());
		std::cout<<fileName.toLocal8Bit().toStdString()<<std::endl;
	}

	//Presets to include in reports:
	std::cout<<"loading presets for reports:"<<std::endl;
	plDir.setPath(presetDirPlanetsReport);
	planetsReportPresets=plDir.entryList(QDir::Files);
	for(QString& str:planetsReportPresets)
	{
		std::cout<<str.toLocal8Bit().toStdString()<<std::endl;
		str=presetDirPlanetsReport+str;
	}
	eqDir.setPath(presetDirEqReport);
	eqReportPresets=eqDir.entryList(QDir::Files);
	for(QString& str:eqReportPresets)
	{
		std::cout<<str.toLocal8Bit().toStdString()<<std::endl;
		str=presetDirEqReport+str;
	}
}

void MainWindow::updateMap()
{
	galaxyMap=galaxy.map(mapScale);
	ui->mapImageLabel->setPixmap(QPixmap::fromImage(galaxyMap));
	ui->mapImageLabel->resize(galaxyMap.size());
}

void MainWindow::updateDumpArrows()
{
	if(currentDumpIndex<0 || currentDumpIndex>=dumpFileList.size()-1)
	{
		ui->actionNextDump->setDisabled(true);
	}
	else
	{
		ui->actionNextDump->setEnabled(true);
	}
	if(currentDumpIndex<=0)
	{
		ui->actionPreviousDump->setDisabled(true);
	}
	else
	{
		ui->actionPreviousDump->setEnabled(true);
	}
}

void MainWindow::saveMap()
{
	if(mapScale>0.f) {
		QFileInfo fileInfo(_filename);
		QString filename=fileInfo.path()+'/'+fileInfo.completeBaseName()+"_map.png";
		if(QFileInfo(filename).exists()) {
			QFile::remove(filename);
		}
		galaxyMap.save(filename);
	}
}

QVariantMap MainWindow::loadPreset(const QString &fileName) const
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
		showMessage(tr("Unable to open file ")+
					 file.errorString()+" "+file.fileName());
		return QVariantMap();
	}
	QJsonDocument doc=QJsonDocument::fromJson(file.readAll());
	if(doc.isNull()) {
		showMessage(tr("Unable to parse JSON file"));
		return QVariantMap();
	}
	return doc.toVariant().toMap();
}

QMap<QString, MainWindow::Scorer> MainWindow::readScorers(const QString &filename)
{
	QMap<QString, MainWindow::Scorer> scorers;
	QFile scorersFile(filename);
	if (!scorersFile.open(QIODevice::ReadOnly|QIODevice::Text)) {
		return scorers;
	}
	QJsonDocument doc=QJsonDocument::fromJson(scorersFile.readAll());
	if(doc.isNull()) {
		return scorers;
	}
	QVariantMap scorersJson=doc.toVariant().toMap();
	using VarMapCI=QVariantMap::const_iterator;
	for (VarMapCI i = scorersJson.begin(); i != scorersJson.end(); ++i)
	{
		scorers[i.key()].read(i.value().toMap());
	}
	return scorers;
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
		showMessage(tr("Rangers are not in focus, aborting input simulation."));
		return false;
	}
	SendInput(ip.size(), ip.data(), sizeof(INPUT));
	responsiveSleep(shortSleep);
	QCoreApplication::processEvents();
	return true;
}

void MainWindow::saveDumpWin()
{
	//TODO: make function bool, return status
	if(!rangersWindowActive()) {
		showMessage(tr("Rangers are not in focus, aborting QuickDump."));
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

	//First imitate Ctrl+Shift+MAKEDUMP
	ip[0].ki.wVk = VK_CONTROL;
	ip[1].ki.wVk = VK_SHIFT;
	SendInput(ip.size(), ip.data(), sizeof(INPUT));
	responsiveSleep(shortSleep);

	if(!simulateInput("MAKEDUMP")) {
		return;
	}

	ip.clear();
	inp.ki.wVk = VK_SHIFT;
	inp.ki.dwFlags = KEYEVENTF_KEYUP ;
	ip.push_back(inp);
	inp.ki.wVk = VK_CONTROL;
	inp.ki.dwFlags = KEYEVENTF_KEYUP;
	ip.push_back(inp);
	SendInput(ip.size(), ip.data(), sizeof(INPUT));

	responsiveSleep(shortSleep*75);//Wait for save window to showup

	//Remove proposed save name
	if(!simulateInput("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb")) {
		return;
	}

	//Enter new save name ("autodump")
	if(!simulateInput("AUTODUMP")) {
		return;
	}

	//Press Enter to save
	if(!simulateInput("e")) {
		return;
	}

	//wait till the saving has finished
	const int dt=300;
	int oldSize=0;
	QFileInfo fileInfo(_filename);
	for(int t=0; t<30000; t+=dt)
	{
		responsiveSleep(dt);
		fileInfo.refresh();
		int size=fileInfo.size();
		if(size && size==oldSize) {
			parseDump();
			return;
		}
		oldSize=size;
	}
	showMessage(tr("Could not save the dump, timeot reached. Not parsing."));
}

QImage MainWindow::currentScreen(float kx, float ky, float kw, float kh)
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

	responsiveSleep(screenSaveLag);
	int t;
	for(t=screenSaveLag; t<8000; t+=screenSaveLag*0.1)
	{
		responsiveSleep(screenSaveLag*0.1);
		dir.refresh();
		fileList=dir.entryList(QDir::Files,QDir::Time);
		if(fileList.isEmpty()){
			continue;
		}
		screenFilename=dir.absolutePath()+"/"+fileList.first();
		fileModified=QFileInfo(screenFilename).lastModified();
		if(screenTaken.msecsTo(fileModified)<0) {//old screenshot
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
	screenSaveLag=t*0.95;
	currentImage=currentImage.copy(currentImage.width()*kx,currentImage.height()*ky,
				       currentImage.width()*kw,currentImage.height()*kh);
	QFile::remove(screenFilename);
	return currentImage;
}

bool isUseless(const QMap<QString, int>& val, const QMap<QString, int>& min)
{
	using MapStrIntCI=QMap<QString,int>::const_iterator;
	for (MapStrIntCI i = min.begin(); i != min.end(); ++i)
	{
		if(val.value(i.key(),0)<i.value()) {
			std::cout<<i.key().toStdString()<<" < "<<i.value()<<std::endl;
			return true;
		}
	}
	return false;
}

void MainWindow::generateGalaxies()
{
	using namespace std;
	using namespace std::chrono;
	std::string filename=QFile::encodeName(rangersDir+"/SRHDDumpReader_generation.log").toStdString();
	ofstream logfile(filename,std::ofstream::out | std::ofstream::app);
	if(!logfile.good()) {
		showMessage(tr("Could not open the log file ")+QString::fromStdString(filename));
	}

	auto originalMainMenu=currentScreen(0,0.9,0.05,0.1);
	if(originalMainMenu.height()==0) {
		showMessage(tr("could not get reference screenshot for the main menu. Generation stopped."));
		return;
	}
	originalMainMenu.save("originalMainMenu.png");
	responsiveSleep(1000);

	int generationTime=20000;
	//std::vector<unsigned> realGenTimes(10,maxGenerationTime);
	const unsigned historyLength=10;
	std::vector<unsigned> numGenChecks(historyLength,10);
	QImage originalGenerationFinished;
	for(int i=0; ;i++)
	{
		high_resolution_clock::time_point iterationStart = high_resolution_clock::now();

		//check, if the state is right
		auto currentMainMenu=currentScreen(0,0.9,0.05,0.1);
		if(currentMainMenu!=originalMainMenu) {
			currentMainMenu.save("failedMainMenu.png");
			showMessage(tr("Main menu is expected, but does not match. Generation stopped. "));
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
			numGenChecks[i%historyLength]=0;
			for(t=generationTime; t<maxGenerationTime; t+=dt)
			{
				currentGenerationFinished=currentScreen(0.45,0.925,0.1,0.05);
				numGenChecks[i%historyLength]++;
				if(currentGenerationFinished==originalGenerationFinished) {
					break;
				}
				else {
					//currentGenerationFinished.save(QString::number(t)+"_currentGen.png");
					responsiveSleep(dt);
				}
			}
			if(t>=maxGenerationTime) {
				currentGenerationFinished.save("failedGenerationFinished.png");
				showMessage(tr("Galaxy_generation_finished is expected, but does not match. Generation stopped"));
				return;
			}
			high_resolution_clock::time_point tGenerationFinished = high_resolution_clock::now();
			auto curGenerationTime=duration_cast<milliseconds>( tGenerationFinished - tGenerationStarted ).count();
			//realGenTimes[i%10]=generationTime;
			std::cout<<"Last galaxy generation took: "<<curGenerationTime/1000.0<<"s. ";
			int minChecks=*std::min_element(numGenChecks.begin(),numGenChecks.end());
			std::cout<<"Succeeded after "<< numGenChecks[i%historyLength] <<" checks. ";
			std::cout<<"Minimum number of checks: "<<minChecks<<". ";
			if(numGenChecks[i%historyLength]<2) {
				generationTime-=1000;
			} else {
				minChecks=std::max(2,minChecks);
				generationTime+=1000*(minChecks-2);
			}
			std::cout<<"Trying next: "<<generationTime/1000.0<<"s."<<std::endl;
		}
		//Enter (close Intro screen)
		if(!simulateInput("e")) {
			return;
		}
		responsiveSleep(shortSleep*100);
		saveDumpWin();
		if(!ui->actionAutoSaveReport->isChecked()) {
			saveReport();
		}

		QString timestamp=QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");

		QString prefix=rangersDir+"/save/autodump";
		if(isUseless(_reportSummary,minRowsPreset)) {//useless save
			QFile::remove(prefix+".txt");
			QFile::remove(prefix+".sav");
			QFile::remove(prefix+".report");
			QFile::remove(prefix+"_map.png");
		}
		else {
			QFile::rename(prefix+".txt",prefix+timestamp+".txt");
			QFile::rename(prefix+".sav",prefix+timestamp+".sav_");
			QFile::rename(prefix+".report",prefix+timestamp+".report");
			QFile::rename(prefix+"_map.png",prefix+timestamp+"_map.png");
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
		int iterationTime=duration_cast<seconds>( high_resolution_clock::now() - iterationStart ).count();
		std::string logoutstr=timestamp.toStdString()+
				      ": Iteration "+std::to_string(i)+
				      " finished in "+to_string(iterationTime)+
				      " s. ";
		logfile<<logoutstr+reportSummary().toStdString()<<endl;
		std::cout<<logoutstr<<endl;

	}
}
#endif


void MainWindow::Scorer::read(const QVariantMap &map)
{
	presetNames.clear();
	weights.clear();
	areBoolean.clear();
	using VarMapCI=QVariantMap::const_iterator;
	for (VarMapCI i = map.begin(); i != map.end(); ++i)
	{
		QVariantMap score=i.value().toMap();
		const QString& presetName=i.key();
		double weight=score.value("weight").toDouble();
		bool isBool=score.value("isBool").toBool();
		addPreset(presetName,weight,isBool);
	}
}

void MainWindow::Scorer::addPreset(const QString presetName, double weight, bool isBool)
{
	presetNames.push_back(presetName);
	weights.push_back(weight);
	areBoolean.push_back(isBool);
}
