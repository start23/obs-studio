#include "obs-frontend-api/obs-frontend-api.h"

#include "window-basic-stats.hpp"
#include "window-basic-main.hpp"
#include "platform.hpp"
#include "obs-app.hpp"

#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include <string>

OBSBasicStats::OBSBasicStats(QWidget *parent)
	: QDialog             (parent),
	  cpu_info            (os_cpu_usage_info_start()),
	  timer               (this)
{
	QVBoxLayout *mainLayout = new QVBoxLayout();
	QGridLayout *topLayout = new QGridLayout();
	outputLayout = new QGridLayout();

	int row = 0;

	auto newStat = [&] (const char *strLoc, QWidget *label, int col)
	{
		std::string str = "Basic.Stats.";
		str += strLoc;
		topLayout->addWidget(
				new QLabel(QTStr(str.c_str()), this),
				row, col);
		topLayout->addWidget(label, row++, col + 1);
	};

	/* --------------------------------------------- */

	cpuUsage = new QLabel(this);
	hddSpace = new QLabel(this);
#ifdef _WIN32
	memUsage = new QLabel(this);
#endif

	newStat("CPUUsage", cpuUsage, 0);
	newStat("HDDSpaceAvailable", hddSpace, 0);
#ifdef _WIN32
	newStat("MemoryUsage", memUsage, 0);
#endif

	renderTime = new QLabel(this);
	skippedFrames = new QLabel(this);
	missedFrames = new QLabel(this);
	row = 0;

	newStat("AverageTimeToRender", renderTime, 2);
	newStat("SkipppedFrames", skippedFrames, 2);
	newStat("MissedFrames", missedFrames, 2);

	/* --------------------------------------------- */

	QPushButton *closeButton = new QPushButton(QTStr("Close"));
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(closeButton);

	/* --------------------------------------------- */

	outputLayout->addWidget(
			new QLabel(QTStr("Basic.Settings.Output"), this),
			0, 0);
	outputLayout->addWidget(
			new QLabel(QTStr("Basic.Stats.Status"), this),
			0, 1);
	outputLayout->addWidget(
			new QLabel(QTStr("Basic.Stats.Disconnections"), this),
			0, 2);
	outputLayout->addWidget(
			new QLabel(QTStr("Basic.Stats.DroppedFrames"), this),
			0, 3);
	outputLayout->addWidget(
			new QLabel(QTStr("Basic.Stats.MegabytesSent"), this),
			0, 4);
	outputLayout->addWidget(
			new QLabel(QTStr("Basic.Stats.Bitrate"), this),
			0, 5);

	/* --------------------------------------------- */

	AddOutputLabels(QTStr("Basic.Stats.Output.Stream"));
	AddOutputLabels(QTStr("Basic.Stats.Output.Recording"));

	/* --------------------------------------------- */

	QVBoxLayout *outputContainerLayout = new QVBoxLayout();
	outputContainerLayout->addLayout(outputLayout);
	outputContainerLayout->addStretch();

	QWidget *widget = new QWidget(this);
	widget->setLayout(outputContainerLayout);

	QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea->setWidget(widget);
	scrollArea->setWidgetResizable(true);

	/* --------------------------------------------- */

	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(scrollArea);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);

	/* --------------------------------------------- */

	connect(closeButton, &QPushButton::clicked, [this] () {close();});

	installEventFilter(CreateShortcutFilter());

	resize(800, 200);
	setWindowTitle(QTStr("Basic.Stats"));
	setSizeGripEnabled(true);
	setWindowModality(Qt::NonModal);
	setAttribute(Qt::WA_DeleteOnClose, true);

	QObject::connect(&timer, &QTimer::timeout, this, &OBSBasicStats::Update);
	timer.setInterval(2000);
	timer.start();
	Update();
}

OBSBasicStats::~OBSBasicStats()
{
	os_cpu_usage_info_destroy(cpu_info);
}

void OBSBasicStats::AddOutputLabels(QString name)
{
	OutputLabels ol;
	ol.name = new QLabel(name, this);
	ol.status = new QLabel(this);
	ol.disconnections = new QLabel(this);
	ol.droppedFrames = new QLabel(this);
	ol.metabytesSent = new QLabel(this);
	ol.bitrate = new QLabel(this);

	int col = 0;
	int row = outputLabels.size() + 1;
	outputLayout->addWidget(ol.name, row, col++);
	outputLayout->addWidget(ol.status, row, col++);
	outputLayout->addWidget(ol.disconnections, row, col++);
	outputLayout->addWidget(ol.droppedFrames, row, col++);
	outputLayout->addWidget(ol.metabytesSent, row, col++);
	outputLayout->addWidget(ol.bitrate, row, col++);
	outputLabels.push_back(ol);
}

void OBSBasicStats::Update()
{
	OBSBasic *main = reinterpret_cast<OBSBasic*>(App()->GetMainWindow());

	/* TODO: Un-hardcode */

	OBSOutput strOutput = obs_frontend_get_streaming_output();
	OBSOutput recOutput = obs_frontend_get_recording_output();
	obs_output_release(strOutput);
	obs_output_release(recOutput);

	/* ------------------------------------------- */
	/* general usage                               */

	double usage = os_cpu_usage_info_query(cpu_info);
	QString str = QString::number(usage, 'g', 2) + QStringLiteral("%");
	cpuUsage->setText(str);

	const char *mode = config_get_string(main->Config(), "Output", "Mode");
	const char *path = strcmp(mode, "Advanced") ?
		config_get_string(main->Config(), "SimpleOutput", "FilePath") :
		config_get_string(main->Config(), "AdvOut", "RecFilePath");

#define GBYTE (1024ULL * 1024ULL * 1024ULL)
#define TBYTE (1024ULL * 1024ULL * 1024ULL * 1024ULL)
	uint64_t space_bytes = os_get_free_disk_space(path);
	QString abrv = QStringLiteral(" MB");
	long double space;

	space = (long double)space_bytes / (1024.0l * 1024.0l);
	if (space_bytes > TBYTE) {
		space /= 1024.0l * 1024.0l;
		abrv = QStringLiteral(" TB");
	} else if (space_bytes > GBYTE) {
		space /= 1024.0l;
		abrv = QStringLiteral(" GB");
	}

	str = QString::number(space, 'f', 1) + abrv;
	hddSpace->setText(str);

#ifdef _WIN32
	space = (long double)CurrentMemoryUsage() / (1024.0l * 1024.0l);

	str = QString::number(space, 'f', 1) + QStringLiteral(" MB");
	memUsage->setText(str);
#endif

	/* ------------------------------------------- */
	/* recording stats                             */

}
